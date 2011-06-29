#ifndef PL061_H_
#define PL061_H_

/// ARM GPIO IP

#include "utils.h"
#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_PL061_DATA = 0,
    REG_PL061_DIR = 256,
    REG_PL061_IS,
    REG_PL061_IBE,
    REG_PL061_IEV,
    REG_PL061_IE,
    REG_PL061_RIS,
    REG_PL061_MIS,
    REG_PL061_IC,
    REG_PL061_AFSEL,
    REG_PL061_ITCR = 384,
    REG_PL061_ITIP1,
    REG_PL061_ITIP2,
    REG_PL061_ITOP1,
    REG_PL061_ITOP2,
    REG_PL061_ITOP3,
    REG_PL061_PERIPHID0 = 1016,
    REG_PL061_PERIPHID1,
    REG_PL061_PERIPHID2,
    REG_PL061_PERIPHID3,
    REG_PL061_PCELLID0,
    REG_PL061_PCELLID1,
    REG_PL061_PCELLID2,
    REG_PL061_PCELLID3,
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

private:
    /** Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset)
    {
        uint32_t result;
        // retrieve the required parameters
        uint32_t index = offset/4;

        // sanity check
        assert(index < REG_PL061_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        default:
            // read the register value
            result = m_reg[index];
            break;
        }

        return result;
    }

    /** Register write function
     * @param[in] offset Offset of the register to read
     * @param[in] offset Value to write in the register
     */
    void
    reg_wr(uint32_t offset, uint32_t value)
    {
        // retrieve the required parameters
        uint32_t index = offset/4;

        // sanity check
        assert(index < REG_PL061_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        default:
            m_reg[index] = value;
            break;
        }
    }

};

#endif /*PL061_H_*/
