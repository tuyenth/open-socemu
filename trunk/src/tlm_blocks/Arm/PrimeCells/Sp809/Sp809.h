#ifndef SP809_H_
#define SP809_H_

/// ARM Remap and Pause Controller (RPC) IP

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_SP809_PERIPHID0 = 0xFE0/4,
    REG_SP809_PERIPHID1,
    REG_SP809_PERIPHID2,
    REG_SP809_PERIPHID3,
    REG_SP809_PCELLID0,
    REG_SP809_PCELLID1,
    REG_SP809_PCELLID2,
    REG_SP809_PCELLID3,
    REG_SP809_COUNT
};

struct Sp809 : Peripheral<REG_SP809_COUNT>
{
    /// Constructor
    Sp809(sc_core::sc_module_name name)
    : Peripheral<REG_SP809_COUNT>(name)
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

#endif /*SP809_H_*/
