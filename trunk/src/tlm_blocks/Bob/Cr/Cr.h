#ifndef CR_H_
#define CR_H_

/// Controller Registers (CR)

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_CR_0,
    REG_CR_SR_OP_STATE,
    REG_CR_SR_STATUS,
    REG_CR_PHY_STATUS,
    REG_CR_PHY_STATUS_INT_EN,
    REG_CR_COUNT
};

struct Cr : Peripheral<REG_CR_COUNT>
{
    /// Constructor
    Cr(sc_core::sc_module_name name)
    : Peripheral<REG_CR_COUNT>(name)
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

#endif /*CR_H_*/
