#ifndef RBG_H_
#define RBG_H_

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_RBG_CONTROL,
    REG_RBG_STATUS,
    REG_RBG_RANDOM_NUM,
    REG_RBG_COUNT
};

struct Rbg : Peripheral<REG_RBG_COUNT>
{
    // Module has a thread
    SC_HAS_PROCESS(Rbg);

    /// Constructor
    Rbg(sc_core::sc_module_name name)
    : Peripheral<REG_RBG_COUNT>(name)
    {
        m_last_random = 0;

        // create the module thread
        SC_THREAD(thread_process);
    }

private:
    /// Module thread
    void
    thread_process();

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

    /// Last random number
    uint32_t m_last_random;

    /// Event used to wake up the thread
    sc_core::sc_event m_event;
};

#endif /*RBG_H_*/
