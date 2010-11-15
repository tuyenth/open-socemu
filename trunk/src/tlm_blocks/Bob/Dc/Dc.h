#ifndef DC_H_
#define DC_H_

/// Device Configuration (DC) block

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_DC_PC_CTX_IDX,      // 0x000E8024
    REG_DC_HOPST,           // 0x000E8028
    REG_DC_EMPTY,           // 0x000E802C
    REG_DC_PRTY_LO,         // 0x000E8030
    REG_DC_PRTY_HI,         // 0x000E8034
    REG_DC_BTA_LO,          // 0x000E8038
    REG_DC_BTA_HI,          // 0x000E803C
    REG_DC_CLSDEV,          // 0x000E8040
    REG_DC_IPA_LO,          // 0x000E8044
    REG_DC_IPA_HI,          // 0x000E8048
    REG_DC_PG_UAP,          // 0x000E804C
    REG_DC_FHS_PYLD,        // 0x000E8050
    REG_DC_N_IQ,            // 0x000E8054
    REG_DC_IQ_TO,           // 0x000E8058
    REG_DC_IQ_RESPTO,       // 0x000E805C
    REG_DC_N_PG,            // 0x000E8060
    REG_DC_PG_TO,           // 0x000E8064
    REG_DC_PG_RESPTO,       // 0x000E8068
    REG_DC_NEWCON_TO,       // 0x000E806C
    REG_DC_PG_CKOFF,        // 0x000E8070
    REG_DC_N_PCLK_OFT,      // 0x000E8074
    REG_DC_N_CLK_OFT,       // 0x000E8078
    REG_DC_N_CLK_WAKE_CNT,  // 0x000E807C
    REG_DC_NCLK_CTL,        // 0x000E8080
    REG_DC_NBTC_PCLK,       // 0x000E8084
    REG_DC_NBTC_CLK,        // 0x000E8088
    REG_DC_ACODE_DLY,       // 0x000E808C
    REG_DC_CR_OP_ST_EN_CNT, // 0x000E8090
    REG_DC_RTX_START_CNT,   // 0x000E8094
    REG_DC_PLL_EN_CNT,      // 0x000E8098
    REG_DC_FSM_INTRP_ENABLE,// 0x000E809C
    REG_DC_SMPL_CTL,        // 0x000E80A0
    REG_DC_SLOT_OFFSET,     // 0x000E80A4
    REG_DC_WIN_PKT0,        // 0x000E80A8
    REG_DC_WIN_PKT1,        // 0x000E80AC
    REG_DC_WIN_PKT2,        // 0x000E80B0
    REG_DC_WIN_TXCONFX,     // 0x000E80B4
    REG_DC_TXCONFX_ST,      // 0x000E80B8
    REG_DC_ICOEX,           // 0x000E80BC
    REG_DC_ICOEX_FREQ0,     // 0x000E80C0
    REG_DC_ICOEX_FREQ1,     // 0x000E80C4
    REG_DC_ICOEX_FREQ2,     // 0x000E80C8
    REG_DC_ICOEX_FREQ3,     // 0x000E80CC
    REG_DC_BT_CLK_OFFSET,   // 0x000E80D0
    REG_DC_DIAC_LO,         // 0x000E80D4
    REG_DC_DIAC_HI,         // 0x000E80D8
    REG_DC_GIAC_LO,         // 0x000E80DC
    REG_DC_GIAC_HI,         // 0x000E80E0
    REG_DC_DUAL_SYNCH,      // 0x000E80E4
    REG_DC_WIN_PKT3,        // 0x000E80E8
    REG_DC_2CLB_ECI_LSW,    // 0x000E80EC
    REG_DC_2CLB_ECI_MSW,    // 0x000E80F0
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
