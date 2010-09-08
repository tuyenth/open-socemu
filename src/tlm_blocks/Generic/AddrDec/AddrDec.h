#ifndef ADDRDEC_H_
#define ADDRDEC_H_

#include "Generic/BusSlave/BusSlave.h"

#include "tlm_utils/simple_initiator_socket.h"

/// debug level
#define ADDRDEC_DEBUG_LEVEL 0

/// Macro to print debug messages
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define ADDRDEC_TLM_DBG(__l, __f, ...)                                                  \
    do {                                                                                \
        if (ADDRDEC_DEBUG_LEVEL >= __l) {                                               \
            TLM_DBG(__f, __VA_ARGS__);                                                  \
        }                                                                               \
    } while (false)


/** AddrDec model is an address decoder to demux accesses from a single initiator to
 * several slaves
 */
struct AddrDec : BusSlave
{
    /** AddrDec constructor
     * @param[in] name Name of the module
     * @param[in] mask Mask of the address decoding (used to alias slaves on a specific
     *            window)
     */
    AddrDec(sc_core::sc_module_name name, sc_dt::uint64 mask = 0xFFFFFFFFFFFFFFFFLL)
    : BusSlave(name)
    , m_head(NULL)
    , m_mask(mask)
    , m_num_slaves(0)
    {
    }

    /** Bind a slave socket to the next available master socket
     * @param[in, out] slave Slave socket to bind
     * @param[in] start Start address for which transactions are forwarded to this slave
     * @param[in] end First address after start for which transactions are NOT forward to this slave
     * @return True if there was an error, False otherwise
     */
    bool
    bind(tlm::tlm_target_socket<>& slave,
         sc_dt::uint64 start, sc_dt::uint64 end)
    {
        struct range* new_range;

        // check if the specified range does not overlap an existing slave
        if (this->check_range(start, end))
            return true;

        // allocate a new slave structure
        new_range = (struct range*)malloc(sizeof(struct range));
        new_range->start = start;
        new_range->end = end;
        new_range->master_socket = new tlm_utils::simple_initiator_socket_tagged<AddrDec>("addrdec_m_socket");

        // hook the master callbacks
        new_range->master_socket->register_nb_transport_bw(this, &AddrDec::bus_m_nb_transport_bw, (int)new_range);
        new_range->master_socket->register_invalidate_direct_mem_ptr(this, &AddrDec::bus_m_invalidate_direct_mem_ptr, (int)new_range);

        // hook the slave socket
        new_range->master_socket->bind(slave);

        // increment the number of slaves
        m_num_slaves++;

        // add the new range in the chained list
        new_range->next = m_head;
        m_head = new_range;

        return false;
    }

    /** Bind a simple slave to the next available master socket
     * @param[in, out] simpleslave BusSlave instance to bind
     * @param[in] start Start address for which transactions are forwarded to this slave
     * @return True if there was an error, False otherwise
     */
    bool
    bind(BusSlave& slave, sc_dt::uint64 start)
    {
        return this->bind(slave, start, start + slave.get_size());
    }

private:
    /// Array of structures containing the address ranges of the targets
    struct range {
        /// Start address of the range (included in range)
        sc_dt::uint64 start;
        /// End address of the range (not included in range)
        sc_dt::uint64 end;
        /// Pointer to the socket allocated for this range access
        tlm_utils::simple_initiator_socket_tagged<AddrDec>* master_socket;
        /// Pointer to the next slave in the chained list
        struct range* next;
    }* m_head;

    /// Decoder global address mask
    sc_dt::uint64 m_mask;

    /// Number of slave socket connections (equals number of internal master sockets)
    int m_num_slaves;

    /** slave_socket blocking transport method
     * @param[in, out] trans Transaction payload object, allocated by initiator, filled here
     * @param[in, out] delay Time object, allocated by initiator, filled here
     */
    void
    slave_b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {
        // sanity check
        #if BUSSLAVE_DEBUG_LEVEL
        assert(m_free == true);
        #endif

        // forward path
        sc_dt::uint64 address = trans.get_address();
        struct range* match = this->find_range(address & m_mask);

        // check that the address is correct
        if (match != NULL)
        {
            // modify address within transaction
            trans.set_address(address - match->start);

            // mark the bus as busy
            #if BUSSLAVE_DEBUG_LEVEL
            m_free = false;
            #endif

            // forward transaction to appropriate target
            (*match->master_socket)->b_transport(trans, delay);

            // replace original address
            trans.set_address(address);

            // mark the bus as free
            #if BUSSLAVE_DEBUG_LEVEL
            m_free = true;
            #endif
        }
        else
        {
            // address was wrong
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        }
    }

    /** slave_socket direct memory access transport method
     * @param[in, out] trans Transaction payload object, allocated by initiator, filled here
     * @param[in, out] dmi_data Direct Memory Interface object
     */
    bool
    slave_get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
                             tlm::tlm_dmi& dmi_data)
    {
        sc_dt::uint64 address = trans.get_address();
        struct range* match = this->find_range(address & m_mask);

        // check address is correct
        if (unlikely(match == NULL))
            return false;

        trans.set_address(address - match->start);

        bool status = (*match->master_socket)->get_direct_mem_ptr(trans, dmi_data);

        trans.set_address(address);

        // calculate DMI address of target in system address space
        dmi_data.set_start_address(match->start + dmi_data.get_start_address());
        dmi_data.set_end_address(match->start + dmi_data.get_end_address());

        return status;
    }

    /** slave_socket debug transport method
     * @param[in, out] trans Transaction payload object, allocated by initiator, filled here
     * @return The number of bytes read or written
     */
    unsigned int
    slave_dbg_transport(tlm::tlm_generic_payload& trans)
    {
        sc_dt::uint64 init_addr = trans.get_address();
        sc_dt::uint64 curr_addr = init_addr & m_mask;
        uint32_t init_len = trans.get_data_length();
        uint8_t* init_ptr = trans.get_data_ptr();
        uint32_t curr_len = init_len;
        uint8_t* curr_ptr = init_ptr;

        while (curr_len > 0)
        {
            uint32_t read_len;
            struct range* match;

            // find the next range
            match = find_range(curr_addr);

            if (likely(match != NULL))
            {
                // target was found
                trans.set_address(curr_addr - match->start);
                trans.set_data_length(curr_len);
                trans.set_data_ptr(curr_ptr);

                // Forward debug transaction to appropriate target
                read_len = (*match->master_socket)->transport_dbg(trans);
            }
            else
            {
                // matching range was not found
                struct range* closest = this->find_next_closest_range(curr_addr);
                if (closest == NULL)
                {
                    // no next range, then fill with FF until the end
                    read_len = curr_len;
                }
                else
                {
                    // check if the next is closer than remaining bytes to read
                    read_len = (curr_len < (closest->start - curr_addr))?
                                curr_len:(closest->start - curr_addr);
                }
                // set to 0xFF the remaining
                memset(curr_ptr, 0xFF, read_len);
            }
            // increment accordingly
            curr_len -= read_len;
            curr_ptr += read_len;
            curr_addr += read_len;
        }

        // reinitialize the transaction in case of further use
        trans.set_address(init_addr);
        trans.set_data_length(init_len);
        trans.set_data_ptr(init_ptr);

        return init_len;
    }


    /** bus_m_socket tagged non-blocking forward transport method
     * @param[in] id Tag identifier
     * @param[in, out] trans Transaction payload object, allocated here, filled by target
     * @param[in, out] phase Phase payload object, allocated here
     * @param[in, out] delay Time object, allocated here, filled by target
     */
    tlm::tlm_sync_enum
    bus_m_nb_transport_bw(int id, tlm::tlm_generic_payload& trans,
                          tlm::tlm_phase& phase, sc_core::sc_time& delay)
    {
        SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
        // sanity check
        assert((struct range*)id != NULL);

        return tlm::TLM_COMPLETED;
    }

    /** bus_m_socket tagged non-blocking forward transport method
     * @param[in] id Tag identifier
     * @param[in] start_range Start address of the memory invalidate command
     * @param[in] end_range End address of the memory invalidate command
     */
    void
    bus_m_invalidate_direct_mem_ptr(int id, sc_dt::uint64 start_range,
                                    sc_dt::uint64 end_range)
    {
        struct range* match = (struct range*)id;

        // Reconstruct address range in system memory map
        sc_dt::uint64 bw_start_range = match->start + start_range;
        sc_dt::uint64 bw_end_range   = match->start + end_range;

        // propagate call backward to initiator
        slave_socket->invalidate_direct_mem_ptr(bw_start_range, bw_end_range);
    }

    /** Set a target's address range
     * @param[in] id Target identifier
     * @param[in] start Decoding start address (inclusive)
     * @param[in] end Decoding end address (exclusive, first address not to be decoded)
     * @return true if there was an error, false otherwise
     */
    bool
    check_range(sc_dt::uint64 start, sc_dt::uint64 end)
    {
        struct range* rover = m_head;

        // sanity checks:
        //  - start address alignment
        assert((start & 0x3) == 0);
        //  - end address alignment
        assert((end & 0x3) == 0);
        //  - start address greater than end address
        assert(start < end);
        //  - addresses does not go beyond mask
        assert((start & (~m_mask)) == 0);
        assert((end & (~m_mask)) == 0);

        // check that the range setting is not conflicting with others
        while (rover != NULL)
        {
            if (start < rover->start)
            {
                if (end > rover->start)
                {
                   // the requested base conflicts with another already configured
                    return true;
                }
            }
            else
            {
                if (start < rover->end)
                {
                   // the requested base conflicts with another already configured
                    return true;
                }
            }
            // move to the next slave
            rover = rover->next;
        }
        return false;
    }

    /** Find the range to which the input address belongs
     * @param[in] address Input address
     * @return The pointer to the found range or NULL if not found
     */
    inline struct range*
    find_range(sc_dt::uint64 address)
    {
        struct range* rover = m_head;
        struct range* prev = NULL;

        while (rover != NULL)
        {
            // check if this is the concerned target
            if ((address >= rover->start) &&
                (address < rover->end))
            {
                // move this element to head if not already there
                if (unlikely(prev != NULL))
                {
                    prev->next = rover->next;
                    rover->next = m_head;
                    m_head = rover;
                }

                // return element pointer
                return rover;
            }
            // move to the next
            prev = rover;
            rover = rover->next;
        }
        return NULL;
    }

    /** Find next closest range
     * existing range
     * @param[in] address
     * @return The pointer to the next closest range
     */
    struct range*
    find_next_closest_range(sc_dt::uint64 address)
    {
        struct range* rover = m_head;
        struct range* closest = NULL;

        while (rover != NULL)
        {
            if (rover->start > address)
            {
                if (closest == NULL)
                    closest = NULL;
                else
                {
                    if ((closest->start - address) > (rover->start - address))
                    {
                        closest = rover;
                    }
                }
            }
            // check if this is the closest target
            // move to the next
            rover = rover->next;
        }
        return closest;
    }
};


#endif /*ADDRDEC_H_*/
