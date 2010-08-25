#ifndef INTSOURCE_H_
#define INTSOURCE_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

struct IntSource
{
    /// Constructor
    IntSource()
    {

    }



protected:
    /// TLM-2 master socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_initiator_socket<IntSource> master_socket;

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

#endif /* INTSOURCE_H_ */
