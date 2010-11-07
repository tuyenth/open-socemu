#ifndef RPC_H_
#define RPC_H_

/// Remap and Pause Controller (RPC)

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_RPC_PERIPHID0 = 0xFE0/4,
    REG_RPC_PERIPHID1,
    REG_RPC_PERIPHID2,
    REG_RPC_PERIPHID3,
    REG_RPC_PCELLID0,
    REG_RPC_PCELLID1,
    REG_RPC_PCELLID2,
    REG_RPC_PCELLID3,
    REG_RPC_COUNT
};

struct Rpc : Peripheral<REG_RPC_COUNT>
{
    /// Constructor
    Rpc(sc_core::sc_module_name name)
    : Peripheral<REG_RPC_COUNT>(name)
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

#endif /*RPC_H_*/
