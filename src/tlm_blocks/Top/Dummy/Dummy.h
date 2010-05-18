#ifndef DUMMY_H_
#define DUMMY_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

// **************************************************************************************
// Dummy module able to handle two pipelined transactions
// **************************************************************************************

struct Dummy : sc_core::sc_module
{
    // TLM-2 socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_initiator_socket<Dummy> init_socket;
    tlm_utils::simple_target_socket<Dummy> targ_socket;
    tlm_utils::simple_initiator_socket<Dummy> int_socket;

    // Not necessary if this module does not have a thread
    SC_HAS_PROCESS(Dummy);

    Dummy(sc_core::sc_module_name name);

    /// Module thread.
    void thread_process();

    // TLM-2 blocking transport method
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// TLM-2 non-blocking transport method.
    virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 non-blocking transport method.
    virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 debug transport method.
    virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans);

    /// Registers definition.
    enum {
        REG_STATUS,
        REG_ACK,
        REG_COMMAND,
        REG_SOURCEADDR,
        REG_DESTADDR,
        REG_LENGTH,
        REG_SIZE        = 256
    };

    /// Registers content.
    uint32_t m_registers[REG_SIZE];

    /// Indicate if busy for sanity check.
    bool m_free;

    /// Generic payload transaction to use for interrupt requests
    tlm::tlm_generic_payload int_pl;
    /// Time object for delay to use for interrupt requests
    sc_core::sc_time int_delay;
};

#endif /*DUMMY_H_*/
