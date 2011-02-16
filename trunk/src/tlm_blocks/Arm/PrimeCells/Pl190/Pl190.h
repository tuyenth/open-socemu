#ifndef PL190_H_
#define PL190_H_

/// ARM Interrupt Controller IP

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_PL190_IRQSTATUS,
    REG_PL190_FIQSTATUS,
    REG_PL190_RAWINTR,
    REG_PL190_INTSELECT,
    REG_PL190_INTENABLE,
    REG_PL190_INTENCLEAR,
    REG_PL190_SOFTINT,
    REG_PL190_SOFTINTCLEAR,
    REG_PL190_PROTECTION,
    REG_PL190_VECTADDR = 0x30/4,
    REG_PL190_DEFVECTADDR,
    REG_PL190_ITCR = 0x300/4,
    REG_PL190_ITIP1,
    REG_PL190_ITIP2,
    REG_PL190_ITOP1,
    REG_PL190_ITOP2,
    REG_PL190_PERIPHID0 = 0xFE0/4,
    REG_PL190_PERIPHID1,
    REG_PL190_PERIPHID2,
    REG_PL190_PERIPHID3,
    REG_PL190_PCELLID0,
    REG_PL190_PCELLID1,
    REG_PL190_PCELLID2,
    REG_PL190_PCELLID3,
    REG_PL190_COUNT
};

struct Pl190 : Peripheral<REG_PL190_COUNT>
{
    /// Constructor
    Pl190(sc_core::sc_module_name name)
    : Peripheral<REG_PL190_COUNT>(name)
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

#endif /*PL190_H_*/
