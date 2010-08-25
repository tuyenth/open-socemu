#ifndef INTSLAVE_H_
#define INTSLAVE_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

template <typename MODULE>
struct IntSlave
{
    /** Constructor
     * @param[in] mod Pointer to the module that contains this object
     * @param[in] cb Pointer to the method of the object handling non blocking accesses
     */
    IntSlave(MODULE* mod,
            void (MODULE::*cb)())
    : target("target")
    {
        target.register_b_transport(mod, cb);
    }

protected:
    /// TLM-2 slave socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_target_socket<MODULE> target;
};

#endif /* INTSLAVE_H_ */
