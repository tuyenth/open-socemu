#ifndef SP805_H_
#define SP805_H_

/// ARM Watchdog IP

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_SP805_PERIPHID0 = 0xFE0/4,
    REG_SP805_PERIPHID1,
    REG_SP805_PERIPHID2,
    REG_SP805_PERIPHID3,
    REG_SP805_PCELLID0,
    REG_SP805_PCELLID1,
    REG_SP805_PCELLID2,
    REG_SP805_PCELLID3,
    REG_SP805_COUNT
};

struct Sp805 : Peripheral<REG_SP805_COUNT>
{
    /// Constructor
    Sp805(sc_core::sc_module_name name)
    : Peripheral<REG_SP805_COUNT>(name)
    {
        // initialize the registers content
    }

private:
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

#endif /*SP805_H_*/
