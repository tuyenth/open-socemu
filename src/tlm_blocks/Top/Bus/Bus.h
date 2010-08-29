#ifndef BUS_H_
#define BUS_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_BUS 0

#if DEBUG_BUS
#include "utils.h"
// using this namespace to simplify streaming
using namespace std;
#endif

// ************************************************************************************
// Bus model supports multiple initiators and multiple targets
// Supports b_ and nb_ transport interfaces, although only b_transport is actually used
// It does no arbitration, but routes all transactions from initiators without blocking
// It uses a simple built-in routing algorithm
// ************************************************************************************

template<uint8_t N_INITIATORS, uint8_t N_TARGETS>
struct Bus : sc_core::sc_module
{
    // Tagged sockets allow incoming transactions to be identified
    tlm_utils::simple_target_socket_tagged<Bus>* targ_socket[N_INITIATORS];
    tlm_utils::simple_initiator_socket_tagged<Bus>* init_socket[N_TARGETS];

    /// Bus constructor
    Bus(sc_core::sc_module_name name)
    {
        // initialize the bus and the current trans
        m_free = true;
        m_one_pending = false;

        for (uint8_t i = 0; i < N_INITIATORS; i++)
        {
            char txt[20];
            sprintf(txt, "targ_socket_%d", i);
            targ_socket[i] = new tlm_utils::simple_target_socket_tagged<Bus>(txt);

            targ_socket[i]->register_nb_transport_fw(this, &Bus::nb_transport_fw, i);
            targ_socket[i]->register_b_transport(this, &Bus::b_transport, i);
            targ_socket[i]->register_get_direct_mem_ptr(this, &Bus::get_direct_mem_ptr, i);
            targ_socket[i]->register_transport_dbg(this, &Bus::transport_dbg, i);

            // set the initiator as unused
            m_pending[i].is_pending = false;
        }
        for (uint8_t i = 0; i < N_TARGETS; i++)
        {
            char txt[20];
            sprintf(txt, "init_socket_%d", i);
            init_socket[i] = new tlm_utils::simple_initiator_socket_tagged<Bus>(txt);

            init_socket[i]->register_nb_transport_bw(this, &Bus::nb_transport_bw, i);
            init_socket[i]->register_invalidate_direct_mem_ptr(this, &Bus::invalidate_direct_mem_ptr, i);
            m_targ_range[i].base = 0xFFFFFFFFFFFFFFFFLL;
            m_targ_range[i].mask = 0xFFFFFFFFFFFFFFFFLL;
        }
    }

    /** Set a target's address range
     * @param id Identifier of the target
     * @param base Decoding base address
     * @param mask Decoding mask address
     * @return true if there was an error, false otherwise
     */
    bool
    set_range(uint8_t id, sc_dt::uint64 base, sc_dt::uint64 mask)
    {
        sc_dt::uint64 u64_temp;
        uint8_t i;

        if (id < N_TARGETS)
        {
            // check that the mask has only left bits set to 1
            u64_temp = mask ^ (mask << 1);
            if (u64_temp & (u64_temp - 1))
            {
                return true;
            }

            // check the validity of the base address
            if ((base & mask) != base)
            {
                return true;
            }

            // check that the range setting is not conflicting with others
            for (i = 0; i < N_TARGETS; i++)
            {
                if ((base & m_targ_range[i].mask) == m_targ_range[i].base)
                {
                    // the requested base conflicts with another already configured
                    return true;
                }
            }


            m_targ_range[id].base = base;
            m_targ_range[id].mask = mask;
            return false;
        }
        else
        {
            return true;
        }
    }

private:
    /// Tagged non-blocking transport forward method
    tlm::tlm_sync_enum
    nb_transport_fw(int id, tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay)
    {
        SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
        // sanity check
        if ((id < N_INITIATORS) && (id >= 0))
        {
        }
        return tlm::TLM_COMPLETED;
    }

    /// Tagged non-blocking transport backward method
    tlm::tlm_sync_enum
    nb_transport_bw(int id, tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay)
    {
        SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
        // sanity check
        if ((id < N_TARGETS) && (id >= 0))
        {
        }
        return tlm::TLM_COMPLETED;
    }

    /// Tagged TLM-2 blocking transport method
    void
    b_transport(int id, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {
        if ((id < N_INITIATORS) && (id >= 0))
        {
            // Forward path
            sc_dt::uint64 address = trans.get_address();
            sc_dt::uint64 masked_address;
            uint8_t target_nr = decode_address(address, masked_address);

            // check that the adress is corrct
            if (target_nr < N_TARGETS)
            {
                // Modify address within transaction
                trans.set_address(masked_address);

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

                    // clear m_one_pending if there are not any pending anymore
                    m_one_pending = false;
                    for (int i = 0; i < N_INITIATORS; i++)
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

                // Forward transaction to appropriate target
                (*init_socket[target_nr])->b_transport(trans, delay);

                // Replace original address
                trans.set_address(address);

                // mark the bus as free
                m_free = true;

                // check if there are requests pending
                if (m_one_pending)
                {
                    for (int i = 0; i < N_INITIATORS; i++)
                    {
                        if (m_pending[i].is_pending)
                        {
                            m_pending[i].event.notify();
                            break;
                        }
                    }
                }
            }
            else
            {
                // address was wrong
                trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            }
        }
        else
        {
            SC_REPORT_FATAL("TLM-2", "Invalid tagged socket id in bus");
        }
    }

    /// Tagged TLM-2 forward DMI method
    bool
    get_direct_mem_ptr(int id, tlm::tlm_generic_payload& trans,
            tlm::tlm_dmi& dmi_data)
    {
        sc_dt::uint64 masked_address;
        uint8_t target_nr = decode_address(trans.get_address(), masked_address);

        // check address is correct
        if (target_nr >= N_TARGETS)
            return false;

        trans.set_address(masked_address);

        bool status = (*init_socket[target_nr])->get_direct_mem_ptr(trans, dmi_data);

        // Calculate DMI address of target in system address space
        dmi_data.set_start_address(compose_address(target_nr, dmi_data.get_start_address()));
        dmi_data.set_end_address(compose_address(target_nr, dmi_data.get_end_address()));

        return status;
    }

    /// Tagged debug transaction method
    unsigned int
    transport_dbg(int id, tlm::tlm_generic_payload& trans)
    {
        sc_dt::uint64 masked_address;
        uint8_t target_nr = decode_address(trans.get_address(), masked_address);
        if (target_nr >= N_TARGETS)
            return 0;
        trans.set_address(masked_address);

        // Forward debug transaction to appropriate target
        return (*init_socket[target_nr])->transport_dbg(trans);
    }

    /// Tagged backward DMI method
    void
    invalidate_direct_mem_ptr(int id, sc_dt::uint64 start_range,
            sc_dt::uint64 end_range)
    {
        // Reconstruct address range in system memory map
        sc_dt::uint64 bw_start_range = compose_address(id, start_range);
        sc_dt::uint64 bw_end_range   = compose_address(id, end_range);

        // Propagate call backward to all initiators
        for (uint8_t i = 0; i < N_INITIATORS; i++)
            (*targ_socket[i])->invalidate_direct_mem_ptr(bw_start_range, bw_end_range);
    }

    /** Fixed address decoding
     * Check to which target range the input address belongs and mask the address in order
     * to forward to the target only the offset address being accessed.
     * @param address Input address
     * @param masked_address Masked address within the target range
     * @return The target index or beyond maximum index if not correct
     */
    inline uint8_t
    decode_address(sc_dt::uint64 address, sc_dt::uint64& masked_address)
    {
        uint8_t i;
        for (i = 0; i < N_TARGETS; i++)
        {
            // check if this is the concerned target
            if ((address & m_targ_range[i].mask) == m_targ_range[i].base)
            {
                masked_address = address & (~m_targ_range[i].mask);
                break;
            }
        }
        return i;
    }

    inline sc_dt::uint64
    compose_address(uint8_t target_nr, sc_dt::uint64 address)
    {
        return address + m_targ_range[target_nr].base;
    }

    /// Array of structures containing the address ranges of the targets
    struct {
        sc_dt::uint64 base;
        sc_dt::uint64 mask;
    } m_targ_range[N_TARGETS];

    /// Array of structures containing the pending requests description
    struct {
        /// Indicate that initiator is pending
        bool is_pending;
        /// Event used to schedule pending request
        sc_core::sc_event event;
    } m_pending[N_INITIATORS];

    /// Indicate that initiator is pending
    bool m_one_pending;

    // Indicate that bus is free for a new request.
    bool m_free;
};

#endif /*BUS_H_*/
