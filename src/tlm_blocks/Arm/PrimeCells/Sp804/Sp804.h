#ifndef SP804_H_
#define SP804_H_

/// ARM Dual Timer IP

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_SP804_TIMER1_LOAD,
    REG_SP804_TIMER1_VALUE,
    REG_SP804_TIMER1_CONTROL,
    REG_SP804_TIMER1_INTCLR,
    REG_SP804_TIMER1_RIS,
    REG_SP804_TIMER1_MIS,
    REG_SP804_TIMER1_BGLOAD,
    REG_SP804_TIMER2_LOAD = 0x20/4,
    REG_SP804_TIMER2_VALUE,
    REG_SP804_TIMER2_CONTROL,
    REG_SP804_TIMER2_INTCLR,
    REG_SP804_TIMER2_RIS,
    REG_SP804_TIMER2_MIS,
    REG_SP804_TIMER2_BGLOAD,
    REG_SP804_TIMER_ITCR = 0xF00/4,
    REG_SP804_TIMER_ITOP,
    REG_SP804_PERIPHID0 = 0xFE0/4,
    REG_SP804_PERIPHID1,
    REG_SP804_PERIPHID2,
    REG_SP804_PERIPHID3,
    REG_SP804_PCELLID0,
    REG_SP804_PCELLID1,
    REG_SP804_PCELLID2,
    REG_SP804_PCELLID3,
    REG_SP804_COUNT
};

struct Sp804 : Peripheral<REG_SP804_COUNT>
{
    /// Constructor
    Sp804(sc_core::sc_module_name name)
    : Peripheral<REG_SP804_COUNT>(name)
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

#endif /*SP804_H_*/
