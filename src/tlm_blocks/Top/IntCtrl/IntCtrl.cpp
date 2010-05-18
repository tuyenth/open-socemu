#include "IntCtrl.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_INTCTRL 0

// using this namespace to simplify streaming
using namespace std;

IntCtrl::IntCtrl(sc_core::sc_module_name name)
: reg_socket("reg_socket")
, int_socket("int_socket")
, irq_socket("irq_socket")
, fiq_socket("fiq_socket")
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
    // Register callbacks for incoming interface method calls
    reg_socket.register_b_transport(this, &IntCtrl::reg_b_transport);
    reg_socket.register_nb_transport_fw(this, &IntCtrl::reg_nb_transport_fw);
    reg_socket.register_transport_dbg(  this, &IntCtrl::reg_transport_dbg);
    int_socket.register_b_transport(this, &IntCtrl::int_b_transport);
}

void IntCtrl::int_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_INT_SANITY(trans);

    // retrieve the required parameters
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    if (*ptr == 1)
    {
        m_registers[REG_INTCTRL_IRQST_RAW] |= 1;
        if (m_registers[REG_INTCTRL_IRQST_RAW] != 0)
        {
            TLM_INT_SET(irq_socket, irq_pl, irq_delay);
        }
    }
    else
    {
        m_registers[REG_INTCTRL_IRQST_RAW] &= ~1;
        if (m_registers[REG_INTCTRL_IRQST_RAW] == 0)
        {
            TLM_INT_CLR(irq_socket, irq_pl, irq_delay);
        }
    }

    // there was no error in the processing
    trans.set_response_status( tlm::TLM_OK_RESPONSE );


    return;
}

void IntCtrl::reg_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_WORD_SANITY(trans);

    // retrieve the required parameters
    sc_dt::uint64 index = trans.get_address()/4;
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    // sanity check
    assert(index < REG_SIZE);
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
        case REG_INTCTRL_WFI:
            // WFI
            wait();
            break;
        default:
            m_registers[index] = *ptr;
        }
    }

    // there was no error in the processing
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

    // mark as free
    m_free = true;

    return;
}

tlm::tlm_sync_enum IntCtrl::reg_nb_transport_fw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}


unsigned int IntCtrl::reg_transport_dbg(tlm::tlm_generic_payload& trans)
{
    // sanity check
    TLM_TRANS_SANITY(trans);

    // execute the debug command
    TLM_DBG_EXEC(trans, m_registers, sizeof(m_registers));
}




