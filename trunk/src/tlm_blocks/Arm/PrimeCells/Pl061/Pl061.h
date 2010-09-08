#ifndef PL061_H_
#define PL061_H_

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_GPIODATA = 0,
    REG_GPIODIR = 256,
    REG_GPIOIS,
    REG_GPIOIBE,
    REG_GPIOIEV,
    REG_GPIOIE,
    REG_GPIORIS,
    REG_GPIOMIS,
    REG_GPIOIC,
    REG_GPIOAFSEL,
    REG_GPIOITCR = 384,
    REG_GPIOITIP1,
    REG_GPIOITIP2,
    REG_GPIOITOP1,
    REG_GPIOITOP2,
    REG_GPIOITOP3,
    REG_GPIOPERIPHID0 = 1016,
    REG_GPIOPERIPHID1,
    REG_GPIOPERIPHID2,
    REG_GPIOPERIPHID3,
    REG_GPIOPCELLID0,
    REG_GPIOPCELLID1,
    REG_GPIOPCELLID2,
    REG_GPIOPCELLID3,
    REG_PL061_COUNT
};

struct Pl061 : Peripheral<REG_PL061_COUNT>
{
    /// Constructor
    Pl061(sc_core::sc_module_name name)
    : Peripheral<REG_PL061_COUNT>(name)
    {
        // initialize the registers content
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

#endif /*PL061_H_*/
