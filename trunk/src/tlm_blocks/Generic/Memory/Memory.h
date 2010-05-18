#ifndef MEMORY_H_
#define MEMORY_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

// **************************************************************************************
// Memory module able to handle two pipelined transactions
// **************************************************************************************

struct Memory : sc_core::sc_module
{
    // TLM-2 socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_target_socket<Memory> socket;

    /** Memory class constructor
     * @param name Name of the module.
     * @param[in] data Pointer to the allocated memory.
     * @param[in] size Size of the memory module in bytes.
     */
    Memory(sc_core::sc_module_name name, uint32_t* data, uint32_t size);

    // TLM-2 blocking transport method
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// TLM-2 non-blocking transport method.
    virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 debug transport method.
    virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans);

    /// Memory content.
    uint32_t* m_data;

    /// Size of the memory.
    uint32_t m_size;

    /// Indicate if busy for sanity check
    bool m_free;
};

#endif /*MEMORY_H_*/
