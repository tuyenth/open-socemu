#ifndef SIMPLESLAVE_H_
#define SIMPLESLAVE_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

// for compiler specific directives
#include "compiler.h"

// for the helper macros
#include "utils.h"

/// debug level
#define SIMPLESLAVE_DEBUG_LEVEL 0

/// Macro to print debug messages
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define SIMPLESLAVE_TLM_DBG(__l, __f, ...)                                              \
    do {                                                                                \
        if (SIMPLESLAVE_DEBUG_LEVEL >= __l) {                                           \
            TLM_DBG(__f, __VA_ARGS__);                                                  \
        }                                                                               \
    } while (false)

/// Base class for a slave only device
struct SimpleSlave : sc_core::sc_module
{
    /** SimpleSlave class constructor
     * @param[in] name Name of the module
     * @param[in, out] data Pointer to the device data content
     * @param[in] size Size in bytes of the device data
     */
    SimpleSlave(sc_core::sc_module_name name, uint32_t* data = NULL, uint32_t size = 0)
        : slave_socket("slave_socket")
        #if SIMPLESLAVE_DEBUG
        , m_free(true)
        #endif
    {
        // register callback functions for incoming interface method calls
        slave_socket.register_b_transport(this, &SimpleSlave::slave_b_transport);
        slave_socket.register_nb_transport_fw(this, &SimpleSlave::slave_nb_transport_fw);
        slave_socket.register_get_direct_mem_ptr(this, &SimpleSlave::slave_get_direct_mem_ptr);
        slave_socket.register_transport_dbg(this, &SimpleSlave::slave_dbg_transport);

        // set the data container
        set_data(data, size);

        // set the default delay value
        set_delay(100);
    }

    /** Wait for the configured time
     *
     */
    void
    delay(void)
    {
        // internal delay
        sc_core::wait(this->m_delay, sc_core::SC_NS);

    }

    /** Get the internal delay of the module in nanoseconds
     * @return Number of nanoseconds
     */
    double
    get_delay(void)
    {
        return this->m_delay;
    }

    /** Set the internal delay of the module in nanoseconds
     * @param[in] delay Number of nanoseconds
     */
    void
    set_delay(double delay)
    {
        this->m_delay = delay;
    }

    /** Set the data container of the module
     * @param[in, out] data Pointer to the data container
     * @param[in] size Size of the data
     */
    void
    set_data(uint32_t* data, uint32_t size)
    {
        this->m_data = data;
        this->m_size = size;
    }

    /** Get the memory mapped content of the module
     * @return The pointer to the device memory mapped content, can be NULL
     */
    virtual uint32_t*
    get_data()
    {
        return this->m_data;
    }

    /** Get the size
     * @return The size of the device memory mapped content, can be NULL
     */
    virtual uint32_t
    get_size()
    {
        return this->m_size;
    }

    /** Operator & to return the reference to the slave socket
     * @return The reference to the slave socket
     */
    operator tlm::tlm_target_socket<32, tlm::tlm_base_protocol_types> & ()
    {
        return this->slave_socket;
    }

    /** Get the reference to the slave socket
     * @return The reference to the slave socket
     */
    tlm_utils::simple_target_socket<SimpleSlave>&
    socket()
    {
        return this->slave_socket;
    }

    /** slave_socket blocking transport method (default behavior, can be overridden)
     * @param[in, out] trans Transaction payload object, allocated by initiator, filled here
     * @param[in, out] delay Time object, allocated by initiator, filled here
     */
    virtual void
    slave_b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {
        // sanity check
        #if SIMPLESLAVE_DEBUG
        TLM_TRANS_SANITY(trans);
        #endif

        // retrieve the required parameters
        uint32_t length = trans.get_data_length();
        sc_dt::uint64 addr = trans.get_address();
        sc_dt::uint64 index = addr/4;
        uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());
        uint32_t mask, shift;

        // sanity check
        #if SIMPLESLAVE_DEBUG
        assert(length <= 4);
        assert(length > 0);
        assert(index < m_size/4);
        assert(m_free);
        #endif

        // check that the length does not go beyond the word boundary
        if (unlikely(length > (4 - (addr & 3))))
        {
            SC_REPORT_FATAL("TLM-2", "Access goes beyond word boundary");
        }

        // convert the length into a mask
        switch (length)
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

        #if SIMPLESLAVE_DEBUG
        cout << this->name() << ": b_transport received addr=0x"<< hex << trans.get_address() << " at " << sc_core::sc_time_stamp() << endl;
        #endif

        // mark as busy
        #if SIMPLESLAVE_DEBUG
        m_free = false;
        #endif

        // internal delay
        wait(this->m_delay, sc_core::SC_NS);

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
        #if SIMPLESLAVE_DEBUG
        m_free = true;
        #endif

        return;
    }

    /** slave_socket non-blocking forward transport method (default behavior, can be overridden)
     * @param[in, out] trans Transaction payload object, allocated by initiator, filled here
     * @param[in, out] phase Phase payload object, allocated by initiator
     * @param[in, out] delay Time object, allocated by initiator, filled here
     * @return The base protocol non blocking state
     */
    virtual tlm::tlm_sync_enum
    slave_nb_transport_fw(tlm::tlm_generic_payload& trans,
                          tlm::tlm_phase& phase, sc_core::sc_time& delay)
    {
        SC_REPORT_FATAL("TLM-2", "Non blocking not implemented");
        return tlm::TLM_COMPLETED;
    }

    /** slave_socket direct memory access transport method
     * @param[in, out] trans Transaction payload object, allocated by initiator, filled here
     * @param[in, out] dmi_data Direct Memory Interface object
     */
    virtual bool slave_get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
            tlm::tlm_dmi& dmi_data)
    {
        SC_REPORT_FATAL("TLM-2", "DMI not implemented");
        return false;
    }

    /** slave_socket debug transport method (default behavior, can be overridden)
     * @param[in, out] trans Transaction payload object, allocated by initiator, filled here
     * @return The number of bytes read or written
     */
    virtual unsigned int
    slave_dbg_transport(tlm::tlm_generic_payload& trans)
    {
        // sanity check
        #if SIMPLESLAVE_DEBUG
        TLM_TRANS_SANITY(trans);
        #endif

        // execute the debug command
        TLM_DBG_EXEC(trans, this->m_data, this->m_size);
    }

protected:
    /// TLM-2 slave socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_target_socket<SimpleSlave> slave_socket;

    /// Pointer to the content of the device (as words)
    uint32_t* m_data;

    /// Size of the data of the device
    uint32_t m_size;

    /// Internal delay for each operation
    double m_delay;

    // Indicate that device is free for a new request, used for validation
    #if SIMPLESLAVE_DEBUG
    bool m_free;
    #endif
};

#endif /*SIMPLESLAVE_H_*/
