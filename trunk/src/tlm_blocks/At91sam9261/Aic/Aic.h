#ifndef AIC_H_
#define AIC_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

// include the registers definition
#include "reg_aic.h"

/// Advanced Interrupt Controller block model
struct Aic : sc_core::sc_module
{
    /// TLM-2 slave sockets for interrupt sources (tagged to use only one callback)
    tlm_utils::simple_target_socket_tagged<Aic>* int_s_socket[32];

    /// TLM-2 slave socket to handle bus accesses
    tlm_utils::simple_target_socket<Aic> reg_s_socket;

    /// TLM-2 master socket to set/clear IRQ signal
    tlm_utils::simple_initiator_socket<Aic> irq_m_socket;

    /// TLM-2 master socket to set/clear FIQ signal
    tlm_utils::simple_initiator_socket<Aic> fiq_m_socket;

    // Not necessary if this module does not have a thread
//    SC_HAS_PROCESS(Aic);

    /// Constructor
    Aic(sc_core::sc_module_name name);

    /// TLM-2 socket blocking method
    virtual void reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// TLM-2 socket non blocking path
    virtual tlm::tlm_sync_enum reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 socket debug path
    virtual unsigned int reg_s_transport_dbg(tlm::tlm_generic_payload& trans);

    /// TLM-2 socket blocking path
    virtual void int_s_b_transport( int id, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// Check the pending interrupts
    void check_int();

    /// Registers content
    uint32_t m_registers[REG_AIC_COUNT];

    /// Indicate if busy for sanity check
    bool m_free;
    /// Generic payload transaction to use for FIQ requests
    tlm::tlm_generic_payload fiq_pl;
    /// Time object for delay to use for FIQ requests
    sc_core::sc_time fiq_delay;
    /// Generic payload transaction to use for IRQ requests
    tlm::tlm_generic_payload irq_pl;
    /// Time object for delay to use for IRQ requests
    sc_core::sc_time irq_delay;

};

#endif /*AIC_H_*/
