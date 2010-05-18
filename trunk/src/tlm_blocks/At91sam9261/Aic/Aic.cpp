#include "Aic.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_AIC 0

// Used to make direct register access
uint32_t* reg_aic;

// using this namespace to simplify streaming
using namespace std;

Aic::Aic(sc_core::sc_module_name name)
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
    // initialize the register access
    reg_aic = &(m_registers[0]);

    // clear all the registers
    memset(m_registers, 0, sizeof(m_registers));

    // Register callbacks for incoming interface method calls
    reg_s_socket.register_b_transport(this, &Aic::reg_s_b_transport);
    reg_s_socket.register_nb_transport_fw(this, &Aic::reg_s_nb_transport_fw);
    reg_s_socket.register_transport_dbg(this, &Aic::reg_s_transport_dbg);

    for (uint8_t i = 0; i < 32; i++)
    {
        char txt[20];
        sprintf(txt, "int_s_socket_%d", i);
        int_s_socket[i] = new tlm_utils::simple_target_socket_tagged<Aic>(txt);
        // only blocking method is supported
        int_s_socket[i]->register_b_transport(this, &Aic::int_s_b_transport, i);
    }
}

void Aic::reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_WORD_SANITY(trans);

    // retrieve the required parameters
    sc_dt::uint64 index = trans.get_address()/4;
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    // sanity check
    assert(index < REG_AIC_COUNT);
    assert(m_free);

    // mark as busy
    m_free = false;

    // internal delay
    wait(100, sc_core::SC_NS);

    if (trans.get_command() == tlm::TLM_READ_COMMAND)
    {
        *ptr = m_registers[index];
    }
    else
    {
        switch (index)
        {
        case AIC_IECR_INDEX:
            // when writing to this register
            aic_imr_set(aic_imr_get() | (*ptr));
            break;
        case AIC_IDCR_INDEX:
            aic_imr_set(aic_imr_get() & (~(*ptr)));
            break;
        case AIC_IMR_INDEX:
            // read only register
            break;
        case AIC_FFER_INDEX:
            aic_ffsr_set(aic_ffsr_get() | (*ptr));
            break;
        case AIC_FFDR_INDEX:
            aic_ffsr_set(aic_ffsr_get() & (~(*ptr)));
            break;
        case AIC_FFSR_INDEX:
            // read only register
            break;
        default:
            m_registers[index] = *ptr;
            break;
        }
    }

    // check if interrupt configuration has changed
    check_int();


    // there was no error in the processing
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

    // mark as free
    m_free = true;

    return;
}

tlm::tlm_sync_enum Aic::reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}


unsigned int Aic::reg_s_transport_dbg(tlm::tlm_generic_payload& trans)
{
    // sanity check
    TLM_TRANS_SANITY(trans);

    // execute the debug command
    TLM_DBG_EXEC(trans, m_registers, sizeof(m_registers));
}

void Aic::int_s_b_transport( int id, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_INT_SANITY(trans);

    // retrieve the required parameters
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    if (*ptr == 1)
    {
        // set the register value
        aic_ipr_set(aic_ipr_get() | (1 << id));
    }
    else
    {
        // clear the register value
        aic_ipr_set(aic_ipr_get() & (~(1 << id)));
    }

    // check if interrupt configuration has changed
    check_int();

    // there was no error in the processing
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

    return;
}

void Aic::check_int()
{
    // find the highest interrupt pending
    unsigned i;
    uint32_t mask;

    mask = aic_ipr_get() & aic_imr_get();

    // by default clear all IRQ and FIQ
    aic_cisr_set(0);

    // generate the FIQ
    for (i = 0; i < 32; i++)
    {
        // check if FIQ is pending (special case for PID0 = FIQ)
        if (((i == 0) && (mask & 1)) || (aic_ffsr_get() & mask & (1 << i)))
        {
            // the current interrupt source number
            aic_isr_set(i);
            // FIQ pending
            aic_cisr_set(aic_cisr_get() | NFIQ_BIT);
            // FIQ vector
            aic_fvr_set(aic_svr_get(0));

            break;
        }
    }
    if (aic_cisr_nfiq_getf())
    {
        // set the FIQ
        TLM_INT_SET(fiq_m_socket, fiq_pl, fiq_delay);
    }
    else
    {
        // clear the FIQ
        TLM_INT_CLR(fiq_m_socket, fiq_pl, fiq_delay);
    }

    // generate the IRQ
    for (i = 1; i < 32; i++)
    {
        // check if IRQ is pending (special case for PID0 = FIQ)
        if ((~aic_ffsr_get()) & mask & (1 << i))
        {
            // update the ISR except if there is already an FIQ pending
            if (aic_cisr_nfiq_getf() == 0)
            {
                // the current interrupt source number
                aic_isr_set(i);
            }
            // IRQ pending
            aic_cisr_set(aic_cisr_get() | NIRQ_BIT);
            // FIQ vector
            aic_ivr_set(m_registers[AIC_SVR_INDEX+i]);

            break;
        }
    }
    if (aic_cisr_nirq_getf())
    {
        // set the IRQ
        TLM_INT_SET(irq_m_socket, irq_pl, irq_delay);
    }
    else
    {
        // clear the IRQ
        TLM_INT_CLR(irq_m_socket, irq_pl, irq_delay);
    }
}

