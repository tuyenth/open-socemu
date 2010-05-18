#include "Itc.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_ITC 0

// Used to make direct register access
uint32_t* reg_itc;

// using this namespace to simplify streaming
using namespace std;

Itc::Itc(sc_core::sc_module_name name)
: reg_s_socket("reg_s_socket")
, irq_m_socket("irq_m_socket")
, fiq_m_socket("fiq_m_socket")
, m_free(true)
{
    // force the default values of the FIQ transaction
    fiq_pl.set_streaming_width(4);
    fiq_pl.set_byte_enable_ptr(0);
    fiq_pl.set_dmi_allowed(false);
    // force the default values of the IRQ transaction
    irq_pl.set_streaming_width(4);
    irq_pl.set_byte_enable_ptr(0);
    irq_pl.set_dmi_allowed(false);

    // initialized the register access
    reg_itc = &(m_reg[0]);

    // clear all the registers
    memset(m_reg, 0, sizeof(m_reg));

    // Register callbacks for incoming interface method calls
    reg_s_socket.register_b_transport(this, &Itc::reg_s_b_transport);
    reg_s_socket.register_nb_transport_fw(this, &Itc::reg_s_nb_transport_fw);
    reg_s_socket.register_transport_dbg(this, &Itc::reg_s_transport_dbg);

    for (uint8_t i = 0; i < NUM_INT; i++)
    {
        char txt[20];
        sprintf(txt, "int_s_socket[%d]", i);
        int_s_socket[i] = new tlm_utils::simple_target_socket_tagged<Itc>(txt);
        // only blocking method is supported
        int_s_socket[i]->register_b_transport(this, &Itc::int_s_b_transport, i);
    }
}

void Itc::reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_WORD_SANITY(trans);

    // retrieve the required parameters
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    if (trans.get_command() == tlm::TLM_READ_COMMAND)
    {
        *ptr = reg_rd(trans.get_address());
    }
    else
    {
        reg_wr(trans.get_address(), *ptr);
    }

    // there was no error in the processing
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

    return;
}

tlm::tlm_sync_enum Itc::reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}


unsigned int Itc::reg_s_transport_dbg(tlm::tlm_generic_payload& trans)
{
    // sanity check
    TLM_TRANS_SANITY(trans);

    // execute the debug command
    TLM_DBG_EXEC(trans, m_reg, sizeof(m_reg));
}

uint32_t Itc::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_ITC_COUNT);
    assert(m_free);

    // mark as busy
    m_free = false;

    // internal delay
    wait(100, sc_core::SC_NS);

    switch (index)
    {
    case INTENNUM_INDEX:
    case INTDISNUM_INDEX:
        TLM_ERR("Read to ITC W/O register %02X", offset);
        result = 0;
        break;

    default:
        // read the register value
        result = this->m_reg[index];
        break;
    }

    // mark as free
    m_free = true;

    return result;
}

void Itc::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_ITC_COUNT);
    assert(m_free);

    // mark as busy
    m_free = false;

    // internal delay
    wait(100, sc_core::SC_NS);

    switch (index)
    {
    case NIVECTOR_INDEX:
    case FIVECTOR_INDEX:
    case INTSRC_INDEX:
    case NIPEND_INDEX:
    case FIPEND_INDEX:
        TLM_ERR("Write to ITC R/O register %02X, data = 0x%08X", offset, value);
        break;
    case INTENNUM_INDEX:
        // enable a single interrupt in the enable mask
        intenable_set(intenable_get() | (1 << value));
        break;

    case INTDISNUM_INDEX:
        // disable a single interrupt in the enable mask
        intenable_set(intenable_get() & (~(1 << value)));
        break;

    default:
        m_reg[index] = value;
        break;
    }

    // after every write, check if there is an interrupt to generate or clear
    this->check_int();

    // mark as free
    m_free = true;
}

void Itc::int_s_b_transport( int id, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_INT_SANITY(trans);

    // retrieve the required parameters
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    // sanity check
    assert(id < NUM_INT);

    if (*ptr == 1)
    {
        // set the register value
        intsrc_set(intsrc_get() | (1 << id));
    }
    else
    {
        // clear the register value
        intsrc_set(intsrc_get() & (~(1 << id)));
    }

    // check interrupts
    check_int();

    trans.set_response_status( tlm::TLM_OK_RESPONSE );

    return;
}

void Itc::check_int()
{
    uint32_t pending_fiq;
    uint32_t pending_irq;

    // get the enabled FIQ and IRQ
    pending_fiq = intsrc_get() & intenable_get() &   inttype_get();
    pending_irq = intsrc_get() & intenable_get() & (~inttype_get());

    // mask the normal interrupt below the nimask
    if (nimask_get() < NUM_INT)
    {
        pending_irq &= ~((1 << (nimask_get() + 1) ) - 1);
    }

    // set the pending interrupt registers
    fipend_set(pending_fiq);
    nipend_set(pending_irq);

    // set the vector registers
    if (pending_irq != 0)
    {
        for (uint8_t i=10; i != 0; i--)
        {
            if (pending_irq & (1<<i))
            {
                nivector_set(i);
                break;
            }
        }
    }
    else
    {
        // set the vector to an invalid value
        nivector_set(0xF);
    }

    if (pending_fiq != 0)
    {
        for (uint8_t i=10; i != 0; i--)
        {
            if (pending_fiq & (1<<i))
            {
                fivector_set(i);
                break;
            }
        }
    }
    else
    {
        // set the vector to an invalid value
        fivector_set(0xF);
    }

    // check if there are pending interrupts
    if (fipend_get() != 0)
    {
        TLM_INT_SET(fiq_m_socket, fiq_pl, fiq_delay);
        TLM_INT_CLR(irq_m_socket, irq_pl, irq_delay);
    }
    else
    {
        TLM_INT_CLR(fiq_m_socket, fiq_pl, fiq_delay);

        if (nipend_get() != 0)
        {
            TLM_INT_SET(irq_m_socket, irq_pl, irq_delay);
        }
        else
        {
            TLM_INT_CLR(irq_m_socket, irq_pl, irq_delay);
        }
    }
}

