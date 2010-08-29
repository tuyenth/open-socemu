#ifndef MPA_H_
#define MPA_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_MPA 0

#if DEBUG_MPA
#include "utils.h"
// using this namespace to simplify streaming
using namespace std;
#endif


/// Multi Port Arbiter block
template<uint8_t N_MASTERS>
struct Mpa : sc_core::sc_module
{
    /// TLM-2 slave socket to receive bus accesses
    tlm_utils::simple_target_socket_tagged<Mpa>* bus_s_socket[N_MASTERS];
    /// TLM-2 master socket to forward bus accesses
    tlm_utils::simple_initiator_socket<Mpa> bus_m_socket;

    /// Mpa constructor
    Mpa(sc_core::sc_module_name name)
    : bus_m_socket("bus_m_socket")
    , m_one_pending(false)
    , m_free(true)
    {
        // initialize all the master interfaces
        for (int i = 0; i < N_MASTERS; i++)
        {
            char txt[20];
            sprintf(txt, "bus_s_socket_%d", i);
            bus_s_socket[i] = new tlm_utils::simple_target_socket_tagged<Mpa>(txt);

            bus_s_socket[i]->register_nb_transport_fw(this, &Mpa::bus_s_nb_transport_fw, i);
            bus_s_socket[i]->register_b_transport(this, &Mpa::bus_s_b_transport, i);
            bus_s_socket[i]->register_get_direct_mem_ptr(this, &Mpa::bus_s_get_direct_mem_ptr, i);
            bus_s_socket[i]->register_transport_dbg(this, &Mpa::bus_s_transport_dbg, i);

            // set the initiator as unused
            m_pending[i].is_pending = false;
        }
    }

    /// Tagged non-blocking transport forward method
    tlm::tlm_sync_enum
    bus_s_nb_transport_fw(int id, tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay)
    {
        SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");

        return tlm::TLM_COMPLETED;
    }


    /// Tagged TLM-2 blocking transport method
    void
    bus_s_b_transport(int id, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {
        // sanity check
        assert(id < N_MASTERS);

        // check if bus is free
        if (!m_free)
        {
            // mark that at least one request is pending
            m_one_pending = true;

            // mark the initiator as pending
            m_pending[id].is_pending = true;

            // wait to get re-scheduled
            wait(m_pending[id].event);

            // mark the initiator as not pending anymore
            m_pending[id].is_pending = false;

            // clear m_one_pending if there are no other pending transaction
            m_one_pending = false;
            for (int i = 0; i < N_MASTERS; i++)
            {
                if (m_pending[i].is_pending)
                {
                    // there is still at least one pending
                    m_one_pending = true;
                    break;
                }
            }
        }

        // mark the bus as busy
        m_free = false;

        // Forward transaction to single master
        bus_m_socket->b_transport(trans, delay);

        // check if there are requests pending (schedule the highest priority)
        if (m_one_pending)
        {
            for (int i = 0; i < N_MASTERS; i++)
            {
                if (m_pending[i].is_pending)
                {
                    m_pending[i].event.notify();
                    break;
                }
            }
        }
        else
        {
            // mark the bus as free
            m_free = true;
        }
    }

    /// Tagged TLM-2 forward DMI method
    bool
    bus_s_get_direct_mem_ptr(int id, tlm::tlm_generic_payload& trans,
            tlm::tlm_dmi& dmi_data)
    {
        // sanity check
        assert(id < N_MASTERS);

        // forward as is the request to the unique master side
        return bus_m_socket->get_direct_mem_ptr(trans, dmi_data );
    }

    /// Tagged debug transaction method
    virtual unsigned int
    bus_s_transport_dbg(int id, tlm::tlm_generic_payload& trans)
    {
        // sanity check
        assert(id < N_MASTERS);

        // forward as is the request to the unique master side
        return bus_m_socket->transport_dbg(trans);
    }

    void
    bind(tlm::tlm_target_socket<>* slave)
    {
        // hook the slave socket
        bus_m_socket.bind(*slave);
    }

private:
    /// Array of structures containing the pending requests description
    struct {
        /// Indicate that initiator is pending
        bool is_pending;
        /// Event used to schedule pending request
        sc_core::sc_event event;
    } m_pending[N_MASTERS];

    /// Indicate that initiator is pending
    bool m_one_pending;

    // Indicate that bus is free for a new request.
    bool m_free;
};


#endif /*MPA_H_*/
