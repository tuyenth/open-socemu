#ifndef B2070_CR_H_
#define B2070_CR_H_

#include "SimpleSlave.h"

struct B2070_Cr : SimpleSlave
{
    /// Constructor
    B2070_Cr(sc_core::sc_module_name name);

    /// Registers definition
    enum
    {
        REG_CR_CLK_DIV_SEL  = 0,

        REG_CR_COUNT        = 61
    };

    /// Registers content
    uint32_t m_reg[REG_CR_COUNT];

    /// Override the virtual function
    void slave_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

protected:
    /**
     * Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset);

    /**
     * Register write function
     * @param[in] offset Offset of the register to read
     */
    void
    reg_wr(uint32_t offset, uint32_t value);

};

#endif /*B2070_CR_H_*/
