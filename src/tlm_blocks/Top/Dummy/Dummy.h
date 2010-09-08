#ifndef DUMMY_H_
#define DUMMY_H_

#include "Generic/Peripheral/Peripheral.h"
#include "Generic/IntMaster/IntMaster.h"

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

/// Registers definition
enum {
    REG_DUMMY_STATUS,
    REG_DUMMY_ACK,
    REG_DUMMY_COMMAND,
    REG_DUMMY_SOURCEADDR,
    REG_DUMMY_DESTADDR,
    REG_DUMMY_LENGTH,
    REG_DUMMY_COUNT        = 256
};

/// "Dummy" Peripheral with an interrupt
struct Dummy : Peripheral<REG_DUMMY_COUNT>
{
    // Module has a thread
    SC_HAS_PROCESS(Dummy);

    /** Dummy default constructor
     * @param[in] name Name of the module
     */
    Dummy(sc_core::sc_module_name name)
    : Peripheral<REG_DUMMY_COUNT>(name)
    {
        // create the module thread
        SC_THREAD(thread_process);
    }

    /// Module thread
    void thread_process()
    {
        while (1)
        {
            // wait for an event
            sc_core::wait();
        }
    }

    /// Define the interrupt source
    IntMaster interrupt;

protected:
    /** Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset);

    /** Register write function
     * @param[in] offset Offset of the register to read
     * @param[in] offset Value to write in the register
     */
    void
    reg_wr(uint32_t offset, uint32_t value);
};

#endif /*DUMMY_H_*/
