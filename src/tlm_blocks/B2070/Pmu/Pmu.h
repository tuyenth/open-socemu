#ifndef PMU_H_
#define PMU_H_

#include "Peripheral.h"

/// Registers definition
enum
{
    REG_CR_CLK_DIV_SEL  = 0,

    REG_PMU_COUNT        = 61
};

struct Pmu : Peripheral<REG_PMU_COUNT>
{
    /// Constructor
    Pmu(sc_core::sc_module_name name)
    : Peripheral<REG_PMU_COUNT>(name)
    {
    }

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

#endif /*PMU_H_*/
