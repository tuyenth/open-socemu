#ifndef DUMMY_H_
#define DUMMY_H_

#include "SimpleMasterSlave.h"

/// debug level
#define DUMMY_DEBUG_LEVEL 0

/// Macro to print debug messages
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define DUMMY_TLM_DBG(__l, __f, ...)                                                    \
    do {                                                                                \
        if (DUMMY_DEBUG_LEVEL >= __l) {                                                 \
            TLM_DBG(__f, __VA_ARGS__);                                                  \
        }                                                                               \
    } while (false)

/// "Dummy" module with a bus slave socket and an interrupt master socket
struct Dummy : SimpleMasterSlave
{
    /** Dummy default constructor
     * @param[in] name Name of the module
     */
    Dummy(sc_core::sc_module_name name)
    : SimpleMasterSlave(name, NULL, 0)
    {
        this->set_data(this->m_registers, sizeof(this->m_registers));
    }

    /// Module thread
    void thread_process()
    {
        while (1)
        {
            // wait for an event
            wait();
        }
    }

    /** slave_socket blocking transport method (default behavior, can be overridden)
     * @param[in, out] trans Transaction payload object, allocated by initiator, filled here
     * @param[in, out] delay Time object, allocated by initiator, filled here
     */
    virtual void
    slave_b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {
        // sanity check
        TLM_WORD_SANITY(trans);

        // retrieve the required parameters
        sc_dt::uint64 index = trans.get_address()/4;
        uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

        // sanity check
        assert(index < REG_SIZE);

        // mark as busy
        #if SIMPLESLAVE_DEBUG
        m_free = false;
        #endif

        // internal delay
        wait(100, sc_core::SC_NS);

        if (trans.get_command() == tlm::TLM_READ_COMMAND)
        {
            *ptr = m_registers[index];
        }
        else
        {
            switch (index)
            {
            case REG_COMMAND:
                TLM_INT_SET(master_socket, master_b_pl, master_b_delay);
                break;
            case REG_ACK:
                TLM_INT_CLR(master_socket, master_b_pl, master_b_delay);
                break;
            }
        }
        trans.set_response_status(tlm::TLM_OK_RESPONSE);

        // mark as free
        #if SIMPLESLAVE_DEBUG
        m_free = true;
        #endif

    }

    /// Registers definition
    enum {
        REG_STATUS,
        REG_ACK,
        REG_COMMAND,
        REG_SOURCEADDR,
        REG_DESTADDR,
        REG_LENGTH,
        REG_SIZE        = 256
    };

    /// Registers content
    uint32_t m_registers[REG_SIZE];
};

#endif /*DUMMY_H_*/
