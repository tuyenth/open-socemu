#ifndef B2070_CR_H_
#define B2070_CR_H_

#include "Peripheral.h"

/// Registers definition
enum
{
    REG_CR_CLK_DIV_SEL  = 0,

    REG_CR_COUNT        = 61
};

struct B2070_Cr : Peripheral<REG_CR_COUNT>
{
    /// Constructor
    B2070_Cr(sc_core::sc_module_name name)
    : Peripheral<REG_CR_COUNT>(name)
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

#endif /*B2070_CR_H_*/
