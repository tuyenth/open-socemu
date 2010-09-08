#ifndef GPIO_H_
#define GPIO_H_

// this is a peripheral
#include "Generic/Peripheral/Peripheral.h"

// include the registers definition
#include "reg_gpio.h"

/// Interrupt Controller block model
struct Gpio : Peripheral<REG_GPIO_COUNT>
{
    /// Constructor
    Gpio(sc_core::sc_module_name name)
    : Peripheral<REG_GPIO_COUNT>(name)
    {
        // initialized the register access
        reg_gpio = &(m_reg[0]);
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

#endif /*GPIO_H_*/
