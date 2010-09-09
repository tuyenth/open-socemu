#ifndef PTU_H_
#define PTU_H_

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_SR_PTU_STATUS0,
    REG_SR_PTU_STATUS1,
    REG_SR_PTU_STATUS2,
    REG_SR_PTU_STATUS3,
    REG_SR_PTU_STATUS4,
    REG_SR_PTU_STATUS5,
    REG_SR_PTU_STATUS6,
    REG_SR_PTU_STATUS7,
    REG_SR_PTU_STATUS8,
    REG_SR_PTU_EN0,
    REG_SR_PTU_EN1,
    REG_SR_PTU_EN2,
    REG_SR_PTU_EN3,
    REG_SR_PTU_EN4,
    REG_SR_PTU_EN5,
    REG_SR_PTU_EN6,
    REG_SR_PTU_EN7,
    REG_SR_PTU_EN8,
    REG_SR_PTU_DIS0,
    REG_SR_PTU_DIS1,
    REG_SR_PTU_DIS2,
    REG_SR_PTU_DIS3,
    REG_SR_PTU_DIS4,
    REG_SR_PTU_TEST0,
    REG_SR_PTU_TEST1,
    REG_SR_PTU_TEST2,
    REG_SR_PTU_TEST3,
    REG_SR_PTU_ESET4,
    REG_SR_PTU_ECLR4,
    REG_DC_PTU_PWRON2GOOD,
    REG_SR_PTU_STATUS9,
    REG_SR_PTU_EN9,
    REG_SR_PTU_DIS9,
    REG_SR_PTU_TEST9,
    REG_PTU_COUNT
};

struct Ptu : Peripheral<REG_PTU_COUNT>
{
    /// Constructor
    Ptu(sc_core::sc_module_name name)
    : Peripheral<REG_PTU_COUNT>(name)
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

#endif /*PTU_H_*/
