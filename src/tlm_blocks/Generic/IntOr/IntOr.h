#ifndef _INTOR_H_
#define _INTOR_H_

#include "Generic/BusMaster/BusMaster.h"

// not so obvious inclusions
#include "tlm_utils/simple_target_socket.h"

/// Class to OR several interrupts into a single one
struct IntOr: BusMaster
{
    /** IntOr class constructor
     * @param[in] name Name of the module
     */
    IntOr(sc_core::sc_module_name name)
    : BusMaster(name)
    , m_or(0)
    , m_master_num(0)
    {
    }

    /** slave_socket blocking transport method (default behavior, can be overridden)
     * @param[in] id Tag identifying the master number
     * @param[in, out] trans Transaction payload object, allocated by initiator, filled here
     * @param[in, out] delay Time object, allocated by initiator, filled here
     */
    void
    int_s_b_transport(int id, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {
        // sanity check
        assert(id < m_master_num);
        TLM_INT_SANITY(trans);

        // fetch the required values
        uint32_t* ptr = reinterpret_cast<uint32_t*>((trans).get_data_ptr());

        // check if it is an interrupt set
        if (*ptr == 1)
        {
            // set the bit in the mask
            m_or |= (1 << id);
        }
        else
        {
            // clear the bit in the mask
            m_or &= ~(1 << id);
        }

        // set or clear the interrupt
        if (m_or != 0)
        {
            TLM_INT_SET(master_socket, master_b_pl, master_b_delay);
        }
        else
        {
            TLM_INT_CLR(master_socket, master_b_pl, master_b_delay);
        }

        // mark the transaction correctly handled
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }

    /** Add a new master socket to OR in the output master_socket of the IntOr
     * @param[in, out] master_socket TLM-2 master socket to OR with other master sockets
     */
    void
    add(tlm::tlm_initiator_socket<32, tlm::tlm_base_protocol_types>* master_socket)
    {
        char txt[20];
        tlm_utils::simple_target_socket_tagged<IntOr>* int_s_socket;

        // sanity check
        assert(m_master_num < 32);

        // create the target socket dynamically
        sprintf(txt, "int_s_socket_%u", m_master_num);
        int_s_socket = new tlm_utils::simple_target_socket_tagged<IntOr>(txt);

        // hook the transport handler
        int_s_socket->register_b_transport(this, &IntOr::int_s_b_transport, m_master_num);

        // hook the master socket
        master_socket->bind(*int_s_socket);

        // increment the number of masters
        m_master_num++;
    }

protected:
    /// Or'ed value of all the interrupt lines
    uint32_t m_or;
    /// Number of masters
    int m_master_num;
};

#endif // _INTOR_H_
