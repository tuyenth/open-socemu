#ifndef SIMPLEMASTER_H_
#define SIMPLEMASTER_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

// for the helper macros
#include "utils.h"

/// debug level
#define SIMPLEMASTER_DEBUG 0

/// Macro to print debug messages
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define SIMPLEMASTER_TLM_DBG(__l, __f, ...)                                             \
    do {                                                                                \
        if (SIMPLEMASTER_DEBUG_LEVEL >= __l) {                                          \
            TLM_DBG(__f, __VA_ARGS__);                                                  \
        }                                                                               \
    } while (false)

/// Base class for a slave only device
struct SimpleMaster : sc_core::sc_module
{
    /// TLM-2 master socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_initiator_socket<SimpleMaster> master_socket;

    // Module has a thread
    SC_HAS_PROCESS(SimpleMaster);

    /** SimpleMaster class constructor
     * @param[in] name Name of the module
     */
    SimpleMaster(sc_core::sc_module_name name)
        : master_socket("master_socket")
    {
        // force the default values of the BUS transaction
        master_b_pl.set_streaming_width(4);
        master_b_pl.set_byte_enable_ptr(0);
        master_b_pl.set_dmi_allowed(false);
        // register callbacks for incoming interface method calls
        master_socket.register_nb_transport_bw(this, &SimpleMaster::master_nb_transport_bw);

        // create the module thread
        SC_THREAD(thread_process);
    }

    /// Main module thread
    virtual void
    thread_process()
    {
        // wait forever
        sc_core::wait();
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

    /** Bind a slave socket to the local master socket
     * @param[in, out] slave_socket TLM-2 slave socket to bind to the master socket
     */
    void bind(tlm::tlm_target_socket<32, tlm::tlm_base_protocol_types>* slave_socket)
    {
        // hook the slave socket
        this->master_socket.bind(*slave_socket);
    }

protected:
    /** Generic payload transaction to use for master blocking requests.  This is used
     * to speed up the simulation by not allocating dynamically a payload for
     * each blocking transaction.
     * @warn This can only be used for blocking accesses
     */
    tlm::tlm_generic_payload master_b_pl;
    /** Time object for delay to use for master blocking requests.  This is used
     * to speed up the simulation by not allocating dynamically a time object for
     * each blocking transaction.
     * @warn This can only be used for blocking accesses
     */
    sc_core::sc_time master_b_delay;
};

#endif /*SIMPLEMASTER_H_*/
