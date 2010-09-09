#ifndef PL081_H_
#define PL081_H_

/// ARM DMA controller IP

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_PL081_PERIPHID0 = 0xFE0/4,
    REG_PL081_PERIPHID1,
    REG_PL081_PERIPHID2,
    REG_PL081_PERIPHID3,
    REG_PL081_PCELLID0,
    REG_PL081_PCELLID1,
    REG_PL081_PCELLID2,
    REG_PL081_PCELLID3,
    REG_PL081_COUNT
};

struct Pl081 : Peripheral<REG_PL081_COUNT>
{
    /// Constructor
    Pl081(sc_core::sc_module_name name)
    : Peripheral<REG_PL081_COUNT>(name)
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

#endif /*PL081_H_*/
