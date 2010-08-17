#ifndef ADDRDEC_H_
#define ADDRDEC_H_

#include "SimpleSlave.h"

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
template<uint8_t N_TARGETS>
struct AddrDec : SimpleSlave
{
    /// TLM-2 master socket to forward bus accesses
    tlm_utils::simple_initiator_socket_tagged<AddrDec>* bus_m_socket[N_TARGETS];

    /** AddrDec constructor
     * @param[in] name Name of the module
     * @param[in] mask Mask of the address decoding (used to alias slaves on a specific
     *            window)
     */
    AddrDec(sc_core::sc_module_name name, sc_dt::uint64 mask = 0xFFFFFFFFFFFFFFFFLL)
        : SimpleSlave(name)
        , m_mask(mask)
        , m_num_slaves(0)
    {
        for (uint8_t i = 0; i < N_TARGETS; i++)
        {
            char txt[20];
            sprintf(txt, "bus_m_socket_%d", i);
            bus_m_socket[i] = new tlm_utils::simple_initiator_socket_tagged<AddrDec>(txt);

            // hook the master callbacks
            bus_m_socket[i]->register_nb_transport_bw(this, &AddrDec::bus_m_nb_transport_bw, i);
            bus_m_socket[i]->register_invalidate_direct_mem_ptr(this, &AddrDec::bus_m_invalidate_direct_mem_ptr, i);

            // configure the default range
            m_bus_m_range[i].start = 0xFFFFFFFFFFFFFFFFLL;
            m_bus_m_range[i].end = 0xFFFFFFFFFFFFFFFFLL;
        }
    }

    /** Set a target's address range
     * @param[in] id Target identifier
     * @param[in] start Decoding start address (inclusive)
     * @param[in] end Decoding end address (exclusive, first address not to be decoded)
     * @return true if there was an error, false otherwise
     */
    bool set_range(uint8_t id, sc_dt::uint64 start, sc_dt::uint64 end)
    {
        uint8_t i;

        // sanity check
        assert(id < N_TARGETS);

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
        for (i = 0; i < N_TARGETS; i++)
        {
            if (start < m_bus_m_range[i].start)
            {
                if (end > m_bus_m_range[i].start)
                {
                   // the requested base conflicts with another already configured
                    return true;
                }
            }
            else
            {
                if (start < m_bus_m_range[i].end)
                {
                   // the requested base conflicts with another already configured
                    return true;
                }
            }
        }

        m_bus_m_range[id].start = start;
        m_bus_m_range[id].end = end;
        return false;
    }

    /** slave_socket blocking transport method
     * @param[in, out] trans Transaction payload object, allocated by initiator, filled here
     * @param[in, out] delay Time object, allocated by initiator, filled here
     */
    virtual void slave_b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {
        // sanity check
        #if SIMPLESLAVE_DEBUG
        assert(this->m_free == true);
        #endif

        // forward path
        sc_dt::uint64 address = trans.get_address();
        sc_dt::uint64 masked_address;
        uint8_t target_nr = decode_address(address & m_mask, masked_address);

        // check that the address is correct
        if (target_nr < N_TARGETS)
        {
            // modify address within transaction
            trans.set_address(masked_address);

            // mark the bus as busy
            #if SIMPLESLAVE_DEBUG
            this->m_free = false;
            #endif

            // forward transaction to appropriate target
            (*bus_m_socket[target_nr])->b_transport(trans, delay);

            // replace original address
            trans.set_address(address);

            // mark the bus as free
            #if SIMPLESLAVE_DEBUG
            this->m_free = true;
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
    virtual bool slave_get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
            tlm::tlm_dmi& dmi_data)
    {
        sc_dt::uint64 address = trans.get_address();
        sc_dt::uint64 masked_address;
        uint8_t target_nr = decode_address(address & m_mask, masked_address);

        // check address is correct
        if (target_nr >= N_TARGETS)
            return false;

        trans.set_address(masked_address);

        bool status = ( *bus_m_socket[target_nr] )->get_direct_mem_ptr(trans, dmi_data);

        trans.set_address(address);

        // Calculate DMI address of target in system address space
        dmi_data.set_start_address(compose_address(target_nr, dmi_data.get_start_address()));
        dmi_data.set_end_address(compose_address(target_nr, dmi_data.get_end_address()));

        return status;
    }

    /** slave_socket debug transport method
     * @param[in, out] trans Transaction payload object, allocated by initiator, filled here
     * @return The number of bytes read or written
     */
    virtual unsigned int slave_dbg_transport(tlm::tlm_generic_payload& trans)
    {
        sc_dt::uint64 init_addr = trans.get_address();
        sc_dt::uint64 curr_addr = init_addr & m_mask;
        uint32_t init_len = trans.get_data_length();
        uint8_t* init_ptr = trans.get_data_ptr();
        uint32_t curr_len = init_len;
        uint8_t* curr_ptr = init_ptr;

        while (curr_len > 0)
        {
            sc_dt::uint64 mask_addr;
            uint32_t targ_len, read_len;
            uint8_t target_nr;


            // check if there is a slave at the current base address
            target_nr = find_target(curr_addr, mask_addr, targ_len);
            if (target_nr < N_TARGETS)
            {
                // target was found
                trans.set_address(mask_addr);
                trans.set_data_length(curr_len);
                trans.set_data_ptr(curr_ptr);

                // Forward debug transaction to appropriate target
                read_len = (*bus_m_socket[target_nr])->transport_dbg(trans);
            }
            else
            {
                // target was not found
                // check if there is a reachable target within remaining len
                read_len = (curr_len < targ_len)?curr_len:targ_len;

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
    virtual tlm::tlm_sync_enum bus_m_nb_transport_bw(int id,
            tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay)
    {
        SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
        // sanity check
        assert(id < N_TARGETS);
        assert(id >= 0);

        return tlm::TLM_COMPLETED;
    }

    /** bus_m_socket tagged non-blocking forward transport method
     * @param[in] id Tag identifier
     * @param[in] start_range Start address of the memory invalidate command
     * @param[in] end_range End address of the memory invalidate command
     */
    virtual void bus_m_invalidate_direct_mem_ptr(int id,
            sc_dt::uint64 start_range,
            sc_dt::uint64 end_range)
    {
        // Reconstruct address range in system memory map
        sc_dt::uint64 bw_start_range = compose_address(id, start_range);
        sc_dt::uint64 bw_end_range   = compose_address(id, end_range);

        // propagate call backward to initiator
        slave_socket->invalidate_direct_mem_ptr(bw_start_range, bw_end_range);
    }

    /** Fixed address decoding
     * Check to which target range the input address belongs and mask the address in order
     * to forward to the target only the offset address being accessed.
     * @param address Input address
     * @param masked_address Masked address within the target range
     * @return The target index or beyond maximum index if not correct
     */
    inline uint8_t decode_address(sc_dt::uint64 address, sc_dt::uint64& masked_address)
    {
        uint8_t i;
        for (i = 0; i < N_TARGETS; i++)
        {
            // check if this is the concerned target
            if ((address >= m_bus_m_range[i].start) &&
                (address < m_bus_m_range[i].end))
            {
                masked_address = address - m_bus_m_range[i].start;
                break;
            }
        }
        return i;
    }

    /** Address Decoding
     * Check to which target range the input address belongs and mask the address in order
     * to forward to the target only the offset address being accessed
     * @param[in] address Fixed input address within input range
     * @param[out] masked_address Masked address within the target range only
     * @param[out] len If target match, contains the length to its end, otherwise
     *                 contains the length to the next target in range (0xFFFFFFFF if
     *                 address is greater than last supported target)
     * @return The found target index or beyond maximum index if not found
     */
    inline uint8_t find_target(sc_dt::uint64 address, sc_dt::uint64& masked_address, uint32_t& len)
    {
        uint8_t i;
        uint32_t length_to_next = 0xFFFFFFFF;

        for (i = 0; i < N_TARGETS; i++)
        {
            // check if this is the concerned target
            if ((address >= m_bus_m_range[i].start) &&
                (address < m_bus_m_range[i].end))
            {
                masked_address = address - m_bus_m_range[i].start;
                len = m_bus_m_range[i].end - masked_address;
                return i;
            }
            // check if the address is before start of target
            else if (address < m_bus_m_range[i].start)
            {
                // check if this is the closest target
                if (length_to_next > (m_bus_m_range[i].start - address))
                {
                    length_to_next = (m_bus_m_range[i].start - address);
                }
            }
        }
        len = length_to_next;
        return i;
    }

    /** Fixed address recomposing
     * Recompose the address for a given target number
     * @param[in] target_nr Target number
     * @param[in] address Target address
     * @return The initiator's point of view address
     */
    inline sc_dt::uint64 compose_address(uint8_t target_nr, sc_dt::uint64 address)
    {
        return address + m_bus_m_range[target_nr].start;
    }

    /** Bind a slave socket to the next available master socket
     * @param[in, out] slave Slave socket to bind
     * @param[in] start Start address for which transactions are forwarded to this slave
     * @param[in] end First address after start for which transactions are NOT forward to this slave
     * @return True if there was an error, False otherwise
     */
    bool bind(tlm::tlm_target_socket<32, tlm::tlm_base_protocol_types>& slave,
            sc_dt::uint64 start, sc_dt::uint64 end)
    {
        bool status;

        // sanity check
        assert(m_num_slaves < N_TARGETS);

        // hook the slave socket
        bus_m_socket[m_num_slaves]->bind(slave);

        // add the range specification
        status = this->set_range(m_num_slaves, start, end);

        // increment the index
        m_num_slaves++;

        return status;
    }


    /// Array of structures containing the address ranges of the targets
    struct {
        /// Start address of the range (included in range)
        sc_dt::uint64 start;
        /// End address of the range (not included in range)
        sc_dt::uint64 end;
    } m_bus_m_range[N_TARGETS];

    /// Decoder global address mask
    sc_dt::uint64 m_mask;

    /// Number of slave socket connections (equals number of internal master sockets)
    int m_num_slaves;

};


#endif /*ADDRDEC_H_*/
