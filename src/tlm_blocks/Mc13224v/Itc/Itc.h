#ifndef ITC_H_
#define ITC_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

// include the registers definition
#include "reg_itc.h"

/// Interrupt Controller block model
struct Itc : sc_core::sc_module
{
    enum
    {
        NUM_INT = 16
    };
    /// TLM-2 slave sockets for interrupt sources (tagged to use only one callback)
    tlm_utils::simple_target_socket_tagged<Itc>* int_s_socket[NUM_INT];

    /// TLM-2 slave socket to handle bus accesses.
    tlm_utils::simple_target_socket<Itc> reg_s_socket;

    /// TLM-2 master socket to set/clear IRQ signal
    tlm_utils::simple_initiator_socket<Itc> irq_m_socket;

    /// TLM-2 master socket to set/clear FIQ signal
    tlm_utils::simple_initiator_socket<Itc> fiq_m_socket;

    // Not necessary if this module does not have a thread
//    SC_HAS_PROCESS(Itc);

    /// Constructor.
    Itc(sc_core::sc_module_name name);

    /// TLM-2 socket blocking method.
    virtual void reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// TLM-2 socket non blocking path.
    virtual tlm::tlm_sync_enum reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 socket debug path.
    virtual unsigned int reg_s_transport_dbg(tlm::tlm_generic_payload& trans);

    /// TLM-2 socket blocking path.
    virtual void int_s_b_transport( int id, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// Read access to the registers
    uint32_t reg_rd(uint32_t offset);

    /// Write access to the registers
    void reg_wr(uint32_t offset, uint32_t value);

    /// Check that interrupt status.
    void check_int();

    /// Registers content.
    uint32_t m_reg[REG_ITC_COUNT];

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

#endif /*ITC_H_*/
