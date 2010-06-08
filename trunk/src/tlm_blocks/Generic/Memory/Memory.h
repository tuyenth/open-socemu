#ifndef MEMORY_H_
#define MEMORY_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_MEMORY 0

/// Generic Memory TLM module
struct Memory : sc_core::sc_module
{
    // TLM-2 socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_target_socket<Memory> slave_socket;

    /** Memory class constructor
     * @param name Name of the module
     * @param[in] data Pointer to the allocated memory
     * @param[in] size Size of the memory module in bytes
     */
    Memory(sc_core::sc_module_name name, uint32_t* data, uint32_t size)
    : slave_socket("slave_socket")
    , m_size(size)
    , m_free(true)
    {
        // allocate the memory (for test purposes it is not initialized)
        this->m_data = data;

        // register callbacks for incoming interface method calls
        slave_socket.register_b_transport(this, &Memory::slave_b_transport);
        slave_socket.register_nb_transport_fw(this, &Memory::slave_nb_transport_fw);
        slave_socket.register_transport_dbg(this, &Memory::slave_transport_dbg);
    }


    // TLM-2 blocking transport method
    virtual void
    slave_b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {
        // sanity check
        TLM_TRANS_SANITY(trans);

        // retrieve the required parameters
        uint32_t length = trans.get_data_length();
        sc_dt::uint64 addr = trans.get_address();
        sc_dt::uint64 index = addr/4;
        uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());
        uint32_t mask, shift;

        // sanity check
        assert(length <= 4);
        assert(length > 0);
        assert(index < m_size/4);
        assert(m_free);
        // check that the length does not go beyond the word boundary
        assert(length <= (4 - (addr & 3)));
        assert((length == 1) || (length == 2) || (length == 4));

        // convert the length into a mask
        /// Helps converting partial accesses to masks
        switch(length)
        {
        case 1:
            mask = 0xFF;
            break;
        case 2:
            mask = 0xFFFF;
            break;
        case 4:
            mask = 0xFFFFFFFF;
            break;
        default:
            SC_REPORT_FATAL("TLM-2", "Access unsupported length");
            mask = 0;
            break;
        }
        shift = (addr & 3)*8;

        #if DEBUG_MEMORY
        cout << this->name() << ": b_transport received addr=0x"<< hex << trans.get_address() << " at " << sc_core::sc_time_stamp() << endl;
        #endif

        // mark as busy
        m_free = false;

        // internal delay
        wait(100, sc_core::SC_NS);

        if (trans.get_command() == tlm::TLM_READ_COMMAND)
        {
            *ptr = (m_data[index] >> shift) & mask;
        }
        else
        {
            uint32_t data = (m_data[index] & (~(mask << shift)));

            // set the data
            data |= ((*ptr) & mask) << shift;

            m_data[index] = data;
        }

        // there was no error in the processing
        trans.set_response_status(tlm::TLM_OK_RESPONSE);

        // mark as free
        m_free = true;

        return;
    }

    /// TLM-2 non-blocking transport method
    virtual tlm::tlm_sync_enum
    slave_nb_transport_fw(tlm::tlm_generic_payload& trans,
                    tlm::tlm_phase& phase, sc_core::sc_time& delay)
    {
        SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
        return tlm::TLM_COMPLETED;
    }

    /// TLM-2 debug transport method
    virtual unsigned int
    slave_transport_dbg(tlm::tlm_generic_payload& trans)
    {
        // sanity check
        TLM_TRANS_SANITY(trans);

        // execute the debug command
        TLM_DBG_EXEC(trans, m_data, m_size);
    }

    /// Memory content
    uint32_t* m_data;

    /// Size of the memory
    uint32_t m_size;

    /// Indicate if busy for sanity check
    bool m_free;
};

#endif /*MEMORY_H_*/
