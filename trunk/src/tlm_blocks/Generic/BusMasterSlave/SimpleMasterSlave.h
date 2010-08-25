#ifndef SIMPLEMASTERSLAVE_H_
#define SIMPLEMASTERSLAVE_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

// for the helper macros
#include "utils.h"

// main derived class
#include "SimpleSlave.h"

/// debug level
#define SIMPLEMASTERSLAVE_DEBUG_LEVEL 0

/// Macro to print debug messages
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define SIMPLEMASTERSLAVE_TLM_DBG(__l, __f, ...)                                        \
    do {                                                                                \
        if (SIMPLEMASTERSLAVE_DEBUG_LEVEL >= __l) {                                     \
            TLM_DBG(__f, __VA_ARGS__);                                                  \
        }                                                                               \
    } while (false)

/// Base class for a master and slave device (not using multiple inheritance for speed)
struct SimpleMasterSlave : SimpleSlave
{
    /// TLM-2 master socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_initiator_socket<SimpleMasterSlave> master_socket;

    // Module has a thread
    SC_HAS_PROCESS(SimpleMasterSlave);

    /** SimpleMasterSlave class constructor
     * @param name Name of the module
     * @param data Pointer to the device data content
     * @param size Size in bytes of the device data
     */
    SimpleMasterSlave(sc_core::sc_module_name name, uint32_t* data, uint32_t size)
    : SimpleSlave(name, data, size)
    , master_socket("master_socket")
    {
        // force the default values of the BUS transaction
        master_b_pl.set_streaming_width(4);
        master_b_pl.set_byte_enable_ptr(0);
        master_b_pl.set_dmi_allowed(false);
        // register callbacks for incoming interface method calls
        master_socket.register_nb_transport_bw(this, &SimpleMasterSlave::master_nb_transport_bw);
        master_socket.register_invalidate_direct_mem_ptr(this, &SimpleMasterSlave::master_invalidate_direct_mem_ptr);

        // create the module thread
        SC_THREAD(thread_process);
    }

    /// Main module thread
    virtual void
    thread_process()
    {
        // by default, wait forever
    }

    /** slave_socket non-blocking forward transport method (default behavior, can be overridden)
     * @param[in, out] trans Transaction payload object, allocated here, filled by target
     * @param[in, out] phase Phase payload object, allocated here
     * @param[in, out] delay Time object, allocated here, filled by target
     */
    virtual tlm::tlm_sync_enum
    master_nb_transport_bw(tlm::tlm_generic_payload& trans,
                          tlm::tlm_phase& phase, sc_core::sc_time& delay)
    {
        SC_REPORT_FATAL("TLM-2", "Non blocking not implemented");
        return tlm::TLM_COMPLETED;
    }

    /** master_socket tagged non-blocking forward transport method
     * @param[in] start_range Start address of the memory invalidate command
     * @param[in] end_range End address of the memory invalidate command
     */
    virtual void
    master_invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                        sc_dt::uint64 end_range)
    {
        SC_REPORT_FATAL("TLM-2", "DMI not implemented");
    }

    /** Bind a slave socket to the local master socket
     * @param[in, out] slave_socket TLM-2 slave socket to bind to the master socket
     */
    void
    bind(tlm::tlm_target_socket<32, tlm::tlm_base_protocol_types>* slave_socket)
    {
        // hook the slave socket
        this->master_socket.bind(*slave_socket);
    }

protected:
    /** Generic payload transaction to use for master blocking requests.  This is used
     * to speed up the simulation by not allocating dynamically a payload for
     * each blocking transaction.
     * @warn This prevents can only be used for blocking accesses
     */
    tlm::tlm_generic_payload master_b_pl;
    /** Time object for delay to use for master blocking requests.  This is used
     * to speed up the simulation by not allocating dynamically a time object for
     * each blocking transaction.
     * @warn This can only be used for blocking accesses
     */
    sc_core::sc_time master_b_delay;
};

#endif /*SIMPLEMASTERSLAVE_H_*/
