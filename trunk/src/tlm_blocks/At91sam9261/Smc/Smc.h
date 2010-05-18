#ifndef SMC_H_
#define SMC_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

// include the registers definition
#include "reg_smc.h"

/// Static Memory Controller block model
struct Smc : sc_core::sc_module
{
    /// TLM-2 slave socket to access the registers
    tlm_utils::simple_target_socket<Smc> reg_s_socket;

    /// TLM-2 slave socket to receive bus accesses
    tlm_utils::simple_target_socket<Smc> bus_s_socket;

    /// TLM-2 master socket to forward bus accesses
    tlm_utils::simple_initiator_socket<Smc> bus_m_socket;

    // Not necessary if this module does not have a thread
//    SC_HAS_PROCESS(Smc);

    /// Constructor
    Smc(sc_core::sc_module_name name);

    /// TLM-2 socket blocking path
    virtual void reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// TLM-2 socket non blocking path
    virtual tlm::tlm_sync_enum reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 socket debug path
    virtual unsigned int reg_s_transport_dbg(tlm::tlm_generic_payload& trans);

    /// TLM-2 socket blocking path
    virtual void bus_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// TLM-2 socket non blocking path
    virtual tlm::tlm_sync_enum bus_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 socket debug path
    virtual unsigned int bus_s_transport_dbg(tlm::tlm_generic_payload& trans);

    /// Registers content
    uint32_t m_registers[REG_SMC_COUNT];

    /// Indicate if busy for sanity check
    bool m_free;

};

#endif /*SMC_H_*/
