#ifndef PMU_H_
#define PMU_H_

#include "Peripheral.h"

/// Registers definition
enum
{
    REG_CR_CLK_DIV_SEL       ,
    REG_CR_CLK_SRC_SEL       ,
    REG_CR_CLK_SWITCH        ,
    REG_CR_XTAL_LPO_DIV      ,
    REG_CR_LPO_SEL           ,
    REG_CR_ERR_EST_CNT       ,
    REG_CR_ERR_RESULT        ,
    REG_CR_ERR_EN            ,
    REG_CR_SLEEP_CNT         ,
    REG_CR_WARMUP_CNT        ,
    REG_CR_SLEEP_EN          ,
    REG_CR_SLEEP_REMAIN      ,
    REG_VERSION              ,
    REG_XTAL_STRAP           ,
    REG_CR_RPU_GATE_CTL      ,
    REG_CR_MEM_STBY_CTL0     ,
    REG_CR_XTAL_1M_INT       ,
    REG_CR_XTAL_1M_FRAC      ,
    REG_CR_I2C_PUMP          ,
    REG_CR_REF_N             ,
    REG_CR_REF_M             ,
    REG_CR_MN_ENA            ,
    REG_CR_WAKE_INT_EN       ,
    REG_CR_PULSE_RESET       ,
    REG_CR_LEVEL_RESET       ,
    REG_CR_MEM_CTL           ,
    REG_CR_PAD_CONFIG0       ,
    REG_CR_PAD_CONFIG1       ,
    REG_CR_PAD_CONFIG2       ,
    REG_CR_PAD_CONFIG3       ,
    REG_CR_PAD_CONFIG4       ,
    REG_CR_PAD_CONFIG5       ,
    REG_CR_PAD_CONFIG6       ,
    REG_CR_PAD_DBG_CTL       ,
    REG_CR_PAD_FCN_CTL0      ,
    REG_CR_PAD_FCN_CTL1      ,
    REG_CR_PAD_FCN_CTL2      ,
    REG_CR_PTU_CLK_EN        ,
    REG_CR_BPL_UPI_GATE_CTL  ,
    REG_CR_PLL_WRMUP_CNT     ,
    REG_CR_SDIO_CLK_SEL      ,
    REG_CR_CLK_FMREF_CTL     ,
    REG_CR_PWR_CTL           ,
    REG_CR_MEM_OVST_CTL      ,
    REG_CR_XTAL_BUF_DLY_BYP  ,
    REG_AUD_MASTER_CTL       ,
    REG_CR_TPORT_CLK_SEL     ,
    REG_CR_PTU_CLK_GATE_ON   ,
    REG_CR_PTU_CLK_GATE_OFF  ,
    REG_SR_PMU_STATUS        ,
    REG_SR_PMU_EN            ,
    REG_SR_PMU_SDIS          ,
    REG_FM_SLEEP_CNT         ,
    REG_FM_WARN_CNT          ,
    REG_FM_WARMUP_CNT        ,
    REG_CR_MTU_CLK_EN        ,
    REG_CR_CLK_REQ_CFG       ,
    REG_CR_PAD_CONFIG7       ,
    REG_FM_SLEEP_EN          ,
    REG_FM_SLEEP_REMAIN      ,
    REG_CR_MEM_TM_CTL        ,
    REG_CR_MEM_STBY_CTL1     ,
    REG_PMU_COUNT
};

struct Pmu : Peripheral<REG_PMU_COUNT>
{
    /// Constructor
    Pmu(sc_core::sc_module_name name)
    : Peripheral<REG_PMU_COUNT>(name)
    {
        // initialize the registers content
        m_reg[REG_CR_MEM_CTL] = 0x1E;
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

#endif /*PMU_H_*/
