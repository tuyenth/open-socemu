#ifndef RMP_H_
#define RMP_H_

/// Remap and Pause block

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_RMP_PAUSE,
    REG_RMP_REMAP,
    REG_RMP_RESETSTATUS,
    REG_RMP_RESETSTATUSCLR,
    REG_RMP_PERIPHID0 = 0xFE0/4,
    REG_RMP_PERIPHID1,
    REG_RMP_PERIPHID2,
    REG_RMP_PERIPHID3,
    REG_RMP_PCELLID0,
    REG_RMP_PCELLID1,
    REG_RMP_PCELLID2,
    REG_RMP_PCELLID3,
    REG_RMP_COUNT
};

struct Rmp : Peripheral<REG_RMP_COUNT>
{
    /// Constructor
    Rmp(sc_core::sc_module_name name)
    : Peripheral<REG_RMP_COUNT>(name)
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

#endif /*RMP_H_*/
