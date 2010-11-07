#ifndef DC_H_
#define DC_H_

/// Device Configuration (DC) block

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_DC_PC_CTX_IDX,
    REG_DC_HOPST,
    REG_DC_PRTY_LO,
    REG_DC_PRTY_HI,
    REG_DC_BTA_LO,
    REG_DC_BTA_HI,
    REG_DC_CLSDEV,
    REG_DC_IPA_LO,
    REG_DC_IPA_HI,
    REG_DC_PG_UAP,
    REG_DC_FHS_PYLD,
    REG_DC_N_IQ,
    REG_DC_IQ_TO,
    REG_DC_IQ_RESPTO,
    REG_DC_N_PG,
    REG_DC_PG_TO,
    REG_DC_PG_RESPTO,
    REG_DC_NEWCON_TO,
    REG_DC_PG_CKOFF,
    REG_DC_N_PCLK_OFT,
    REG_DC_N_CLK_OFT,
    REG_DC_N_CLK_WAKE_CNT,
    REG_DC_NCLK_CTL,
    REG_DC_NBTC_PCLK,
    REG_DC_NBTC_CLK,
    REG_DC_ACODE_DLY,
    REG_DC_CR_OP_ST_EN_CNT,
    REG_DC_RTX_START_CNT,
    REG_DC_PLL_EN_CNT,
    REG_DC_FSM_INTRP_ENABLE,
    REG_DC_SMPL_CTL,
    REG_DC_SLOT_OFFSET,
    REG_DC_WIN_PKT0,
    REG_DC_WIN_PKT1,
    REG_DC_WIN_PKT2,
    REG_DC_WIN_TXCONFX,
    REG_DC_TXCONFX_ST,
    REG_DC_ICOEX,
    REG_DC_ICOEX_FREQ0,
    REG_DC_ICOEX_FREQ1,
    REG_DC_ICOEX_FREQ2,
    REG_DC_ICOEX_FREQ3,
    REG_DC_BT_CLK_OFFSET,
    REG_DC_DIAC_LO,
    REG_DC_DIAC_HI,
    REG_DC_GIAC_LO,
    REG_DC_GIAC_HI,
    REG_DC_DUAL_SYNCH,
    REG_DC_WIN_PKT3,
    REG_DC_2CLB_ECI_LSW,
    REG_DC_2CLB_ECI_MSW,
    REG_DC_ENCRYPTION_KEY0 = 0x160/4,
    REG_DC_ENCRYPTION_KEY1,
    REG_DC_ENCRYPTION_KEY2,
    REG_DC_ENCRYPTION_KEY3,
    REG_DC_COUNT
};

struct Dc : Peripheral<REG_DC_COUNT>
{
    /// Constructor
    Dc(sc_core::sc_module_name name)
    : Peripheral<REG_DC_COUNT>(name)
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

#endif /*DC_H_*/
