#ifndef RESERVED_H_
#define RESERVED_H_

// this is a peripheral
#include "Peripheral.h"

// include the registers definition
#include "reg_reserved.h"

/// Interrupt Controller block model
struct Reserved : Peripheral<REG_RESERVED_COUNT>
{
    /// Constructor
    Reserved(sc_core::sc_module_name name)
    : Peripheral<REG_RESERVED_COUNT>(name)
    {
        // initialized the register access
        reg_reserved = &(m_reg[0]);
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

#endif /*RESERVED_H_*/
