#ifndef _INTOR_H_
#define _INTOR_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

/// Class to OR several interrupts into a single one
struct IntOr: sc_core::sc_module
{
    /// TLM-2 master socket to transmit interrupt change accesses
    tlm_utils::simple_initiator_socket<IntOr> int_m_socket;

    SC_CTOR(IntOr)
    : int_m_socket("int_m_socket")
    , m_or(0)
    , m_master_num(0)
    {
        // force the default values of the INT transaction
        int_pl.set_streaming_width(4);
        int_pl.set_byte_enable_ptr(0);
        int_pl.set_dmi_allowed(false);
    }

    // Tagged TLM-2 blocking transport method
    virtual void int_s_b_transport(int id, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
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
            TLM_INT_SET(int_m_socket, int_pl, int_delay);
        }
        else
        {
            TLM_INT_CLR(int_m_socket, int_pl, int_delay);
        }

        // mark the transaction correctly handled
        trans.set_response_status( tlm::TLM_OK_RESPONSE );
    }

    void bind(tlm::tlm_target_socket<32, tlm::tlm_base_protocol_types>* slave)
    {
        // hook the slave socket
        int_m_socket.bind(*slave);
    }

    void add(tlm::tlm_initiator_socket<32, tlm::tlm_base_protocol_types>* master)
    {
        char txt[20];
        tlm_utils::simple_target_socket_tagged<IntOr>* int_s_socket;

        // create the target socket dynamically
        sprintf(txt, "int_s_socket_%u", m_master_num);
        int_s_socket = new tlm_utils::simple_target_socket_tagged<IntOr>(txt);

        // hook the transport handler
        int_s_socket->register_b_transport(this, &IntOr::int_s_b_transport, m_master_num);

        // hook the master socket
        master->bind(*int_s_socket);

        // increment the number of masters
        m_master_num++;
    }

    /// Or'ed value of all the interrupt lines.
    uint32_t m_or;
    /// Number of masters.
    int m_master_num;
    /// Generic payload transaction to use for INT requests
    tlm::tlm_generic_payload int_pl;
    /// Time object for delay to use for INT requests
    sc_core::sc_time int_delay;

};




#endif // _INTOR_H_
