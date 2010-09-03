#ifndef INTMASTER_H_
#define INTMASTER_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

// for the helper macros
#include "utils.h"

struct IntMaster
{
    /// Constructor
    IntMaster()
    {
        // force the default values of the BUS transaction
        master_b_pl.set_streaming_width(4);
        master_b_pl.set_byte_enable_ptr(0);
        master_b_pl.set_dmi_allowed(false);
        // register callbacks for incoming interface method calls
        master_socket.register_nb_transport_bw(this, &IntMaster::master_nb_transport_bw);
        master_socket.register_invalidate_direct_mem_ptr(this, &IntMaster::master_invalidate_direct_mem_ptr);
    }

    /// Assert the interrupt
    void
    set()
    {
        TLM_INT_SET(master_socket, master_b_pl, master_b_delay);
    }

    /// De-assert the interrupt
    void
    clear()
    {
        TLM_INT_CLR(master_socket, master_b_pl, master_b_delay);
    }

    /** Bind a slave socket to the local master socket
     * @param[in, out] slave_socket TLM-2 slave socket to bind to the master socket
     */
    void
    bind(tlm::tlm_target_socket<>& slave_socket)
    {
        // hook the slave socket
        this->master_socket.bind(slave_socket);
    }

protected:
    /// TLM-2 master socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_initiator_socket<IntMaster> master_socket;

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

    /** slave_socket non-blocking forward transport method (default behavior, can be overridden)
     * @param[in, out] trans Transaction payload object, allocated here, filled by target
     * @param[in, out] phase Phase payload object, allocated here
     * @param[in, out] delay Time object, allocated here, filled by target
     */
    tlm::tlm_sync_enum
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
    void
    master_invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                        sc_dt::uint64 end_range)
    {
        SC_REPORT_FATAL("TLM-2", "DMI not implemented");
    }
};

#endif /* INTMASTER_H_ */
