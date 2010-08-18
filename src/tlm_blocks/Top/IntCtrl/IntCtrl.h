#ifndef INTCTRL_H_
#define INTCTRL_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

struct IntCtrl : sc_core::sc_module
{
    /// Standard bus socket to read the registers
    tlm_utils::simple_target_socket<IntCtrl> reg_socket;

    /// Interrupt socket (only one interrupt source)
    tlm_utils::simple_target_socket<IntCtrl> int_socket;

    /// IRQ socket to set/clear in IRQ signal
    tlm_utils::simple_initiator_socket<IntCtrl> irq_socket;

    /// IRQ socket to set/clear in FIQ signal
    tlm_utils::simple_initiator_socket<IntCtrl> fiq_socket;

    // Not necessary if this module does not have a thread
//    SC_HAS_PROCESS(IntCtrl);

    /// Constructor
    IntCtrl(sc_core::sc_module_name name);

    // TLM-2 blocking transport method
    virtual void int_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    // TLM-2 blocking transport method
    virtual void reg_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// TLM-2 non-blocking transport method
    virtual tlm::tlm_sync_enum reg_nb_transport_fw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 debug transport method
    virtual unsigned int reg_transport_dbg(tlm::tlm_generic_payload& trans);

    /// Registers definition
    enum
    {
        REG_INTCTRL_IRQEN_SET  = 0,
        REG_INTCTRL_IRQEN_CLR  = 1,
        REG_INTCTRL_IRQST_RAW  = 2,
        REG_INTCTRL_FIQST_RAW  = 3,
        REG_INTCTRL_WFI        = 4,

        REG_SIZE               = 256
    };

    /// Registers content
    uint32_t m_registers[REG_SIZE];

    /// Indicate if busy for sanity check
    bool m_free;

    /// Generic payload transaction to use for interrupt requests
    tlm::tlm_generic_payload fiq_pl;
    /// Time object for delay to use for interrupt requests
    sc_core::sc_time fiq_delay;
    /// Generic payload transaction to use for interrupt requests
    tlm::tlm_generic_payload irq_pl;
    /// Time object for delay to use for interrupt requests
    sc_core::sc_time irq_delay;
};

#endif /*INTCTRL_H_*/
