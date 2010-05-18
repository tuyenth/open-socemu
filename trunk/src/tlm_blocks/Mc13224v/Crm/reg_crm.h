#ifndef _REG_CRM_H_
#define _REG_CRM_H_

#include <stdint.h>
#include "compiler.h"
#include "arch.h"
#include "reg_sim.h"

#define REG_CRM_BASE_ADDR 0x80003000

#define REG_CRM_COUNT 21

#define REG_CRM_DECODING_MASK 0x0000007F

/**
 * @brief SYS_CNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  13:08          XTAL_CLKDIV   0x0
 *     05        XTAL32_EXISTS   0
 *     04        JTAG_SECU_OFF   0
 *     03        SPIF_1P8V_SEL   0
 *     02        PADS_1P8V_SEL   0
 *  01:00           PWR_SOURCE   0x0
 * </pre>
 */
#define SYS_CNTL_INDEX  0x00000000

__INLINE uint32_t sys_cntl_get(void)
{
    return sim_reg_crm_rd(SYS_CNTL_INDEX);
}

__INLINE void sys_cntl_set(uint32_t value)
{
    sim_reg_crm_wr(SYS_CNTL_INDEX, value);
}

// field definitions
#define XTAL_CLKDIV_MASK     0x00003F00
#define XTAL_CLKDIV_LSB      8
#define XTAL_CLKDIV_WIDTH    0x00000006
#define XTAL32_EXISTS_BIT    0x00000020
#define XTAL32_EXISTS_POS    5
#define JTAG_SECU_OFF_BIT    0x00000010
#define JTAG_SECU_OFF_POS    4
#define SPIF_1P8V_SEL_BIT    0x00000008
#define SPIF_1P8V_SEL_POS    3
#define PADS_1P8V_SEL_BIT    0x00000004
#define PADS_1P8V_SEL_POS    2
#define PWR_SOURCE_MASK      0x00000003
#define PWR_SOURCE_LSB       0
#define PWR_SOURCE_WIDTH     0x00000002

#define XTAL_CLKDIV_RST      0x0
#define XTAL32_EXISTS_RST    0x0
#define JTAG_SECU_OFF_RST    0x0
#define SPIF_1P8V_SEL_RST    0x0
#define PADS_1P8V_SEL_RST    0x0
#define PWR_SOURCE_RST       0x0

__INLINE void sys_cntl_pack(uint8_t xtalclkdiv, uint8_t xtal32exists, uint8_t jtagsecuoff, uint8_t spif1p8vsel, uint8_t pads1p8vsel, uint8_t pwrsource)
{
    ASSERT_ERR(((xtalclkdiv << 8) & ~0x00003F00) == 0);
    ASSERT_ERR(((xtal32exists << 5) & ~0x00000020) == 0);
    ASSERT_ERR(((jtagsecuoff << 4) & ~0x00000010) == 0);
    ASSERT_ERR(((spif1p8vsel << 3) & ~0x00000008) == 0);
    ASSERT_ERR(((pads1p8vsel << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((pwrsource << 0) & ~0x00000003) == 0);
    sim_reg_crm_wr(SYS_CNTL_INDEX,  (xtalclkdiv << 8) | (xtal32exists << 5) | (jtagsecuoff << 4) | (spif1p8vsel << 3) | (pads1p8vsel << 2) | (pwrsource << 0));
}

__INLINE void sys_cntl_unpack(uint8_t* xtalclkdiv, uint8_t* xtal32exists, uint8_t* jtagsecuoff, uint8_t* spif1p8vsel, uint8_t* pads1p8vsel, uint8_t* pwrsource)
{
    uint32_t localVal = sim_reg_crm_rd(SYS_CNTL_INDEX);

    *xtalclkdiv = (localVal & 0x00003F00) >> 8;
    *xtal32exists = (localVal & 0x00000020) >> 5;
    *jtagsecuoff = (localVal & 0x00000010) >> 4;
    *spif1p8vsel = (localVal & 0x00000008) >> 3;
    *pads1p8vsel = (localVal & 0x00000004) >> 2;
    *pwrsource = (localVal & 0x00000003) >> 0;
}

__INLINE uint8_t xtal_clkdiv_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SYS_CNTL_INDEX);
    return ((localVal & 0x00003F00) >> 8);
}

__INLINE void xtal_clkdiv_setf(uint8_t xtalclkdiv)
{
    ASSERT_ERR(((xtalclkdiv << 8) & ~0x00003F00) == 0);
    sim_reg_crm_wr(SYS_CNTL_INDEX, (sim_reg_crm_rd(SYS_CNTL_INDEX) & ~0x00003F00) | (xtalclkdiv << 8));
}

__INLINE uint8_t xtal32_exists_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SYS_CNTL_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE void xtal32_exists_setf(uint8_t xtal32exists)
{
    ASSERT_ERR(((xtal32exists << 5) & ~0x00000020) == 0);
    sim_reg_crm_wr(SYS_CNTL_INDEX, (sim_reg_crm_rd(SYS_CNTL_INDEX) & ~0x00000020) | (xtal32exists << 5));
}

__INLINE uint8_t jtag_secu_off_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SYS_CNTL_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE void jtag_secu_off_setf(uint8_t jtagsecuoff)
{
    ASSERT_ERR(((jtagsecuoff << 4) & ~0x00000010) == 0);
    sim_reg_crm_wr(SYS_CNTL_INDEX, (sim_reg_crm_rd(SYS_CNTL_INDEX) & ~0x00000010) | (jtagsecuoff << 4));
}

__INLINE uint8_t spif_1p8v_sel_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SYS_CNTL_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE void spif_1p8v_sel_setf(uint8_t spif1p8vsel)
{
    ASSERT_ERR(((spif1p8vsel << 3) & ~0x00000008) == 0);
    sim_reg_crm_wr(SYS_CNTL_INDEX, (sim_reg_crm_rd(SYS_CNTL_INDEX) & ~0x00000008) | (spif1p8vsel << 3));
}

__INLINE uint8_t pads_1p8v_sel_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SYS_CNTL_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void pads_1p8v_sel_setf(uint8_t pads1p8vsel)
{
    ASSERT_ERR(((pads1p8vsel << 2) & ~0x00000004) == 0);
    sim_reg_crm_wr(SYS_CNTL_INDEX, (sim_reg_crm_rd(SYS_CNTL_INDEX) & ~0x00000004) | (pads1p8vsel << 2));
}

__INLINE uint8_t pwr_source_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SYS_CNTL_INDEX);
    return ((localVal & 0x00000003) >> 0);
}

__INLINE void pwr_source_setf(uint8_t pwrsource)
{
    ASSERT_ERR(((pwrsource << 0) & ~0x00000003) == 0);
    sim_reg_crm_wr(SYS_CNTL_INDEX, (sim_reg_crm_rd(SYS_CNTL_INDEX) & ~0x00000003) | (pwrsource << 0));
}

/**
 * @brief WU_CNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:28          EXT_OUT_POL   0x0
 *  23:20           EXT_WU_IEN   0x0
 *     17           RTC_WU_IEN   0
 *     16         TIMER_WU_IEN   0
 *  15:12           EXT_WU_POL   0x0
 *  11:08          EXT_WU_EDGE   0x0
 *  07:04            EXT_WU_EN   0x0
 *     03             AUTO_ADC   0
 *     02            HOST_WAKE   0
 *     01            RTC_WU_EN   0
 *     00          TIMER_WU_EN   0
 * </pre>
 */
#define WU_CNTL_INDEX  0x00000001

__INLINE uint32_t wu_cntl_get(void)
{
    return sim_reg_crm_rd(WU_CNTL_INDEX);
}

__INLINE void wu_cntl_set(uint32_t value)
{
    sim_reg_crm_wr(WU_CNTL_INDEX, value);
}

// field definitions
#define EXT_OUT_POL_MASK    0xF0000000
#define EXT_OUT_POL_LSB     28
#define EXT_OUT_POL_WIDTH   0x00000004
#define EXT_WU_IEN_MASK     0x00F00000
#define EXT_WU_IEN_LSB      20
#define EXT_WU_IEN_WIDTH    0x00000004
#define RTC_WU_IEN_BIT      0x00020000
#define RTC_WU_IEN_POS      17
#define TIMER_WU_IEN_BIT    0x00010000
#define TIMER_WU_IEN_POS    16
#define EXT_WU_POL_MASK     0x0000F000
#define EXT_WU_POL_LSB      12
#define EXT_WU_POL_WIDTH    0x00000004
#define EXT_WU_EDGE_MASK    0x00000F00
#define EXT_WU_EDGE_LSB     8
#define EXT_WU_EDGE_WIDTH   0x00000004
#define EXT_WU_EN_MASK      0x000000F0
#define EXT_WU_EN_LSB       4
#define EXT_WU_EN_WIDTH     0x00000004
#define AUTO_ADC_BIT        0x00000008
#define AUTO_ADC_POS        3
#define HOST_WAKE_BIT       0x00000004
#define HOST_WAKE_POS       2
#define RTC_WU_EN_BIT       0x00000002
#define RTC_WU_EN_POS       1
#define TIMER_WU_EN_BIT     0x00000001
#define TIMER_WU_EN_POS     0

#define EXT_OUT_POL_RST     0x0
#define EXT_WU_IEN_RST      0x0
#define RTC_WU_IEN_RST      0x0
#define TIMER_WU_IEN_RST    0x0
#define EXT_WU_POL_RST      0x0
#define EXT_WU_EDGE_RST     0x0
#define EXT_WU_EN_RST       0x0
#define AUTO_ADC_RST        0x0
#define HOST_WAKE_RST       0x0
#define RTC_WU_EN_RST       0x0
#define TIMER_WU_EN_RST     0x0

__INLINE void wu_cntl_pack(uint8_t extoutpol, uint8_t extwuien, uint8_t rtcwuien, uint8_t timerwuien, uint8_t extwupol, uint8_t extwuedge, uint8_t extwuen, uint8_t autoadc, uint8_t hostwake, uint8_t rtcwuen, uint8_t timerwuen)
{
    ASSERT_ERR(((extoutpol << 28) & ~0xF0000000) == 0);
    ASSERT_ERR(((extwuien << 20) & ~0x00F00000) == 0);
    ASSERT_ERR(((rtcwuien << 17) & ~0x00020000) == 0);
    ASSERT_ERR(((timerwuien << 16) & ~0x00010000) == 0);
    ASSERT_ERR(((extwupol << 12) & ~0x0000F000) == 0);
    ASSERT_ERR(((extwuedge << 8) & ~0x00000F00) == 0);
    ASSERT_ERR(((extwuen << 4) & ~0x000000F0) == 0);
    ASSERT_ERR(((autoadc << 3) & ~0x00000008) == 0);
    ASSERT_ERR(((hostwake << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((rtcwuen << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((timerwuen << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX,  (extoutpol << 28) | (extwuien << 20) | (rtcwuien << 17) | (timerwuien << 16) | (extwupol << 12) | (extwuedge << 8) | (extwuen << 4) | (autoadc << 3) | (hostwake << 2) | (rtcwuen << 1) | (timerwuen << 0));
}

__INLINE void wu_cntl_unpack(uint8_t* extoutpol, uint8_t* extwuien, uint8_t* rtcwuien, uint8_t* timerwuien, uint8_t* extwupol, uint8_t* extwuedge, uint8_t* extwuen, uint8_t* autoadc, uint8_t* hostwake, uint8_t* rtcwuen, uint8_t* timerwuen)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);

    *extoutpol = (localVal & 0xF0000000) >> 28;
    *extwuien = (localVal & 0x00F00000) >> 20;
    *rtcwuien = (localVal & 0x00020000) >> 17;
    *timerwuien = (localVal & 0x00010000) >> 16;
    *extwupol = (localVal & 0x0000F000) >> 12;
    *extwuedge = (localVal & 0x00000F00) >> 8;
    *extwuen = (localVal & 0x000000F0) >> 4;
    *autoadc = (localVal & 0x00000008) >> 3;
    *hostwake = (localVal & 0x00000004) >> 2;
    *rtcwuen = (localVal & 0x00000002) >> 1;
    *timerwuen = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t ext_out_pol_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);
    return ((localVal & 0xF0000000) >> 28);
}

__INLINE void ext_out_pol_setf(uint8_t extoutpol)
{
    ASSERT_ERR(((extoutpol << 28) & ~0xF0000000) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX, (sim_reg_crm_rd(WU_CNTL_INDEX) & ~0xF0000000) | (extoutpol << 28));
}

__INLINE uint8_t ext_wu_ien_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);
    return ((localVal & 0x00F00000) >> 20);
}

__INLINE void ext_wu_ien_setf(uint8_t extwuien)
{
    ASSERT_ERR(((extwuien << 20) & ~0x00F00000) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX, (sim_reg_crm_rd(WU_CNTL_INDEX) & ~0x00F00000) | (extwuien << 20));
}

__INLINE uint8_t rtc_wu_ien_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE void rtc_wu_ien_setf(uint8_t rtcwuien)
{
    ASSERT_ERR(((rtcwuien << 17) & ~0x00020000) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX, (sim_reg_crm_rd(WU_CNTL_INDEX) & ~0x00020000) | (rtcwuien << 17));
}

__INLINE uint8_t timer_wu_ien_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE void timer_wu_ien_setf(uint8_t timerwuien)
{
    ASSERT_ERR(((timerwuien << 16) & ~0x00010000) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX, (sim_reg_crm_rd(WU_CNTL_INDEX) & ~0x00010000) | (timerwuien << 16));
}

__INLINE uint8_t ext_wu_pol_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);
    return ((localVal & 0x0000F000) >> 12);
}

__INLINE void ext_wu_pol_setf(uint8_t extwupol)
{
    ASSERT_ERR(((extwupol << 12) & ~0x0000F000) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX, (sim_reg_crm_rd(WU_CNTL_INDEX) & ~0x0000F000) | (extwupol << 12));
}

__INLINE uint8_t ext_wu_edge_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);
    return ((localVal & 0x00000F00) >> 8);
}

__INLINE void ext_wu_edge_setf(uint8_t extwuedge)
{
    ASSERT_ERR(((extwuedge << 8) & ~0x00000F00) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX, (sim_reg_crm_rd(WU_CNTL_INDEX) & ~0x00000F00) | (extwuedge << 8));
}

__INLINE uint8_t ext_wu_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);
    return ((localVal & 0x000000F0) >> 4);
}

__INLINE void ext_wu_en_setf(uint8_t extwuen)
{
    ASSERT_ERR(((extwuen << 4) & ~0x000000F0) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX, (sim_reg_crm_rd(WU_CNTL_INDEX) & ~0x000000F0) | (extwuen << 4));
}

__INLINE uint8_t auto_adc_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE void auto_adc_setf(uint8_t autoadc)
{
    ASSERT_ERR(((autoadc << 3) & ~0x00000008) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX, (sim_reg_crm_rd(WU_CNTL_INDEX) & ~0x00000008) | (autoadc << 3));
}

__INLINE uint8_t host_wake_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void host_wake_setf(uint8_t hostwake)
{
    ASSERT_ERR(((hostwake << 2) & ~0x00000004) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX, (sim_reg_crm_rd(WU_CNTL_INDEX) & ~0x00000004) | (hostwake << 2));
}

__INLINE uint8_t rtc_wu_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void rtc_wu_en_setf(uint8_t rtcwuen)
{
    ASSERT_ERR(((rtcwuen << 1) & ~0x00000002) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX, (sim_reg_crm_rd(WU_CNTL_INDEX) & ~0x00000002) | (rtcwuen << 1));
}

__INLINE uint8_t timer_wu_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_CNTL_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void timer_wu_en_setf(uint8_t timerwuen)
{
    ASSERT_ERR(((timerwuen << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(WU_CNTL_INDEX, (sim_reg_crm_rd(WU_CNTL_INDEX) & ~0x00000001) | (timerwuen << 0));
}

/**
 * @brief SLEEP_CNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     07           DIG_PAD_EN   0
 *     06              MCU_RET   0
 *  05:04              RAM_RET   0x0
 *     01                 DOZE   0
 *     00                  HIB   0
 * </pre>
 */
#define SLEEP_CNTL_INDEX  0x00000002

__INLINE uint32_t sleep_cntl_get(void)
{
    return sim_reg_crm_rd(SLEEP_CNTL_INDEX);
}

__INLINE void sleep_cntl_set(uint32_t value)
{
    sim_reg_crm_wr(SLEEP_CNTL_INDEX, value);
}

// field definitions
#define DIG_PAD_EN_BIT    0x00000080
#define DIG_PAD_EN_POS    7
#define MCU_RET_BIT       0x00000040
#define MCU_RET_POS       6
#define RAM_RET_MASK      0x00000030
#define RAM_RET_LSB       4
#define RAM_RET_WIDTH     0x00000002
#define DOZE_BIT          0x00000002
#define DOZE_POS          1
#define HIB_BIT           0x00000001
#define HIB_POS           0

#define DIG_PAD_EN_RST    0x0
#define MCU_RET_RST       0x0
#define RAM_RET_RST       0x0
#define DOZE_RST          0x0
#define HIB_RST           0x0

__INLINE void sleep_cntl_pack(uint8_t digpaden, uint8_t mcuret, uint8_t ramret, uint8_t doze, uint8_t hib)
{
    ASSERT_ERR(((digpaden << 7) & ~0x00000080) == 0);
    ASSERT_ERR(((mcuret << 6) & ~0x00000040) == 0);
    ASSERT_ERR(((ramret << 4) & ~0x00000030) == 0);
    ASSERT_ERR(((doze << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((hib << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(SLEEP_CNTL_INDEX,  (digpaden << 7) | (mcuret << 6) | (ramret << 4) | (doze << 1) | (hib << 0));
}

__INLINE void sleep_cntl_unpack(uint8_t* digpaden, uint8_t* mcuret, uint8_t* ramret, uint8_t* doze, uint8_t* hib)
{
    uint32_t localVal = sim_reg_crm_rd(SLEEP_CNTL_INDEX);

    *digpaden = (localVal & 0x00000080) >> 7;
    *mcuret = (localVal & 0x00000040) >> 6;
    *ramret = (localVal & 0x00000030) >> 4;
    *doze = (localVal & 0x00000002) >> 1;
    *hib = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t dig_pad_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SLEEP_CNTL_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE void dig_pad_en_setf(uint8_t digpaden)
{
    ASSERT_ERR(((digpaden << 7) & ~0x00000080) == 0);
    sim_reg_crm_wr(SLEEP_CNTL_INDEX, (sim_reg_crm_rd(SLEEP_CNTL_INDEX) & ~0x00000080) | (digpaden << 7));
}

__INLINE uint8_t mcu_ret_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SLEEP_CNTL_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE void mcu_ret_setf(uint8_t mcuret)
{
    ASSERT_ERR(((mcuret << 6) & ~0x00000040) == 0);
    sim_reg_crm_wr(SLEEP_CNTL_INDEX, (sim_reg_crm_rd(SLEEP_CNTL_INDEX) & ~0x00000040) | (mcuret << 6));
}

__INLINE uint8_t ram_ret_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SLEEP_CNTL_INDEX);
    return ((localVal & 0x00000030) >> 4);
}

__INLINE void ram_ret_setf(uint8_t ramret)
{
    ASSERT_ERR(((ramret << 4) & ~0x00000030) == 0);
    sim_reg_crm_wr(SLEEP_CNTL_INDEX, (sim_reg_crm_rd(SLEEP_CNTL_INDEX) & ~0x00000030) | (ramret << 4));
}

__INLINE uint8_t doze_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SLEEP_CNTL_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void doze_setf(uint8_t doze)
{
    ASSERT_ERR(((doze << 1) & ~0x00000002) == 0);
    sim_reg_crm_wr(SLEEP_CNTL_INDEX, (sim_reg_crm_rd(SLEEP_CNTL_INDEX) & ~0x00000002) | (doze << 1));
}

__INLINE uint8_t hib_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SLEEP_CNTL_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void hib_setf(uint8_t hib)
{
    ASSERT_ERR(((hib << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(SLEEP_CNTL_INDEX, (sim_reg_crm_rd(SLEEP_CNTL_INDEX) & ~0x00000001) | (hib << 0));
}

/**
 * @brief BS_CNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  13:08         ARM_OFF_TIME   0x0
 *     02            BS_MAN_EN   0
 *     01             WAIT4IRQ   0
 *     00                BS_EN   0
 * </pre>
 */
#define BS_CNTL_INDEX  0x00000003

__INLINE uint32_t bs_cntl_get(void)
{
    return sim_reg_crm_rd(BS_CNTL_INDEX);
}

__INLINE void bs_cntl_set(uint32_t value)
{
    sim_reg_crm_wr(BS_CNTL_INDEX, value);
}

// field definitions
#define ARM_OFF_TIME_MASK   0x00003F00
#define ARM_OFF_TIME_LSB    8
#define ARM_OFF_TIME_WIDTH  0x00000006
#define BS_MAN_EN_BIT       0x00000004
#define BS_MAN_EN_POS       2
#define WAIT4IRQ_BIT        0x00000002
#define WAIT4IRQ_POS        1
#define BS_EN_BIT           0x00000001
#define BS_EN_POS           0

#define ARM_OFF_TIME_RST    0x0
#define BS_MAN_EN_RST       0x0
#define WAIT4IRQ_RST        0x0
#define BS_EN_RST           0x0

__INLINE void bs_cntl_pack(uint8_t armofftime, uint8_t bsmanen, uint8_t wait4irq, uint8_t bsen)
{
    ASSERT_ERR(((armofftime << 8) & ~0x00003F00) == 0);
    ASSERT_ERR(((bsmanen << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((wait4irq << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((bsen << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(BS_CNTL_INDEX,  (armofftime << 8) | (bsmanen << 2) | (wait4irq << 1) | (bsen << 0));
}

__INLINE void bs_cntl_unpack(uint8_t* armofftime, uint8_t* bsmanen, uint8_t* wait4irq, uint8_t* bsen)
{
    uint32_t localVal = sim_reg_crm_rd(BS_CNTL_INDEX);

    *armofftime = (localVal & 0x00003F00) >> 8;
    *bsmanen = (localVal & 0x00000004) >> 2;
    *wait4irq = (localVal & 0x00000002) >> 1;
    *bsen = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t arm_off_time_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(BS_CNTL_INDEX);
    return ((localVal & 0x00003F00) >> 8);
}

__INLINE void arm_off_time_setf(uint8_t armofftime)
{
    ASSERT_ERR(((armofftime << 8) & ~0x00003F00) == 0);
    sim_reg_crm_wr(BS_CNTL_INDEX, (sim_reg_crm_rd(BS_CNTL_INDEX) & ~0x00003F00) | (armofftime << 8));
}

__INLINE uint8_t bs_man_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(BS_CNTL_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void bs_man_en_setf(uint8_t bsmanen)
{
    ASSERT_ERR(((bsmanen << 2) & ~0x00000004) == 0);
    sim_reg_crm_wr(BS_CNTL_INDEX, (sim_reg_crm_rd(BS_CNTL_INDEX) & ~0x00000004) | (bsmanen << 2));
}

__INLINE uint8_t wait4irq_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(BS_CNTL_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void wait4irq_setf(uint8_t wait4irq)
{
    ASSERT_ERR(((wait4irq << 1) & ~0x00000002) == 0);
    sim_reg_crm_wr(BS_CNTL_INDEX, (sim_reg_crm_rd(BS_CNTL_INDEX) & ~0x00000002) | (wait4irq << 1));
}

__INLINE uint8_t bs_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(BS_CNTL_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void bs_en_setf(uint8_t bsen)
{
    ASSERT_ERR(((bsen << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(BS_CNTL_INDEX, (sim_reg_crm_rd(BS_CNTL_INDEX) & ~0x00000001) | (bsen << 0));
}

/**
 * @brief COP_CNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  22:16            COP_COUNT   0x0
 *  14:08          COP_TIMEOUT   0x0
 *     02               COP_WP   0
 *     01              COP_OUT   0
 *     00               COP_EN   0
 * </pre>
 */
#define COP_CNTL_INDEX  0x00000004

__INLINE uint32_t cop_cntl_get(void)
{
    return sim_reg_crm_rd(COP_CNTL_INDEX);
}

__INLINE void cop_cntl_set(uint32_t value)
{
    sim_reg_crm_wr(COP_CNTL_INDEX, value);
}

// field definitions
#define COP_COUNT_MASK     0x007F0000
#define COP_COUNT_LSB      16
#define COP_COUNT_WIDTH    0x00000007
#define COP_TIMEOUT_MASK   0x00007F00
#define COP_TIMEOUT_LSB    8
#define COP_TIMEOUT_WIDTH  0x00000007
#define COP_WP_BIT         0x00000004
#define COP_WP_POS         2
#define COP_OUT_BIT        0x00000002
#define COP_OUT_POS        1
#define COP_EN_BIT         0x00000001
#define COP_EN_POS         0

#define COP_COUNT_RST      0x0
#define COP_TIMEOUT_RST    0x0
#define COP_WP_RST         0x0
#define COP_OUT_RST        0x0
#define COP_EN_RST         0x0

__INLINE void cop_cntl_pack(uint8_t copcount, uint8_t coptimeout, uint8_t copwp, uint8_t copout, uint8_t copen)
{
    ASSERT_ERR(((copcount << 16) & ~0x007F0000) == 0);
    ASSERT_ERR(((coptimeout << 8) & ~0x00007F00) == 0);
    ASSERT_ERR(((copwp << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((copout << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((copen << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(COP_CNTL_INDEX,  (copcount << 16) | (coptimeout << 8) | (copwp << 2) | (copout << 1) | (copen << 0));
}

__INLINE void cop_cntl_unpack(uint8_t* copcount, uint8_t* coptimeout, uint8_t* copwp, uint8_t* copout, uint8_t* copen)
{
    uint32_t localVal = sim_reg_crm_rd(COP_CNTL_INDEX);

    *copcount = (localVal & 0x007F0000) >> 16;
    *coptimeout = (localVal & 0x00007F00) >> 8;
    *copwp = (localVal & 0x00000004) >> 2;
    *copout = (localVal & 0x00000002) >> 1;
    *copen = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t cop_count_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(COP_CNTL_INDEX);
    return ((localVal & 0x007F0000) >> 16);
}

__INLINE void cop_count_setf(uint8_t copcount)
{
    ASSERT_ERR(((copcount << 16) & ~0x007F0000) == 0);
    sim_reg_crm_wr(COP_CNTL_INDEX, (sim_reg_crm_rd(COP_CNTL_INDEX) & ~0x007F0000) | (copcount << 16));
}

__INLINE uint8_t cop_timeout_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(COP_CNTL_INDEX);
    return ((localVal & 0x00007F00) >> 8);
}

__INLINE void cop_timeout_setf(uint8_t coptimeout)
{
    ASSERT_ERR(((coptimeout << 8) & ~0x00007F00) == 0);
    sim_reg_crm_wr(COP_CNTL_INDEX, (sim_reg_crm_rd(COP_CNTL_INDEX) & ~0x00007F00) | (coptimeout << 8));
}

__INLINE uint8_t cop_wp_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(COP_CNTL_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void cop_wp_setf(uint8_t copwp)
{
    ASSERT_ERR(((copwp << 2) & ~0x00000004) == 0);
    sim_reg_crm_wr(COP_CNTL_INDEX, (sim_reg_crm_rd(COP_CNTL_INDEX) & ~0x00000004) | (copwp << 2));
}

__INLINE uint8_t cop_out_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(COP_CNTL_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void cop_out_setf(uint8_t copout)
{
    ASSERT_ERR(((copout << 1) & ~0x00000002) == 0);
    sim_reg_crm_wr(COP_CNTL_INDEX, (sim_reg_crm_rd(COP_CNTL_INDEX) & ~0x00000002) | (copout << 1));
}

__INLINE uint8_t cop_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(COP_CNTL_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void cop_en_setf(uint8_t copen)
{
    ASSERT_ERR(((copen << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(COP_CNTL_INDEX, (sim_reg_crm_rd(COP_CNTL_INDEX) & ~0x00000001) | (copen << 0));
}

/**
 * @brief COP_SERVICE register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          COP_SERVICE   0x0
 * </pre>
 */
#define COP_SERVICE_INDEX  0x00000005

__INLINE uint32_t cop_service_get(void)
{
    return sim_reg_crm_rd(COP_SERVICE_INDEX);
}

__INLINE void cop_service_set(uint32_t value)
{
    sim_reg_crm_wr(COP_SERVICE_INDEX, value);
}

// field definitions
#define COP_SERVICE_MASK   0xFFFFFFFF
#define COP_SERVICE_LSB    0
#define COP_SERVICE_WIDTH  0x00000020

#define COP_SERVICE_RST    0x0

__INLINE uint32_t cop_service_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(COP_SERVICE_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

__INLINE void cop_service_setf(uint32_t copservice)
{
    ASSERT_ERR(((copservice << 0) & ~0xFFFFFFFF) == 0);
    sim_reg_crm_wr(COP_SERVICE_INDEX, copservice << 0);
}

/**
 * @brief STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     19        VREG_1P5V_RDY   0
 *     18        VREG_1P8V_RDY   0
 *     17        VREG_BUCK_RDY   0
 *     10              COP_EVT   0
 *     09             CAL_DONE   0
 *  07:04           EXT_WU_EVT   0x0
 *     03           RTC_WU_EVT   0
 *     02          DOZE_WU_EVT   0
 *     01           HIB_WU_EVT   0
 *     00           SLEEP_SYNC   0
 * </pre>
 */
#define STATUS_INDEX  0x00000006

__INLINE uint32_t status_get(void)
{
    return sim_reg_crm_rd(STATUS_INDEX);
}

__INLINE void status_set(uint32_t value)
{
    sim_reg_crm_wr(STATUS_INDEX, value);
}

// field definitions
#define VREG_1P5V_RDY_BIT    0x00080000
#define VREG_1P5V_RDY_POS    19
#define VREG_1P8V_RDY_BIT    0x00040000
#define VREG_1P8V_RDY_POS    18
#define VREG_BUCK_RDY_BIT    0x00020000
#define VREG_BUCK_RDY_POS    17
#define COP_EVT_BIT          0x00000400
#define COP_EVT_POS          10
#define CAL_DONE_BIT         0x00000200
#define CAL_DONE_POS         9
#define EXT_WU_EVT_MASK      0x000000F0
#define EXT_WU_EVT_LSB       4
#define EXT_WU_EVT_WIDTH     0x00000004
#define RTC_WU_EVT_BIT       0x00000008
#define RTC_WU_EVT_POS       3
#define DOZE_WU_EVT_BIT      0x00000004
#define DOZE_WU_EVT_POS      2
#define HIB_WU_EVT_BIT       0x00000002
#define HIB_WU_EVT_POS       1
#define SLEEP_SYNC_BIT       0x00000001
#define SLEEP_SYNC_POS       0

#define VREG_1P5V_RDY_RST    0x0
#define VREG_1P8V_RDY_RST    0x0
#define VREG_BUCK_RDY_RST    0x0
#define COP_EVT_RST          0x0
#define CAL_DONE_RST         0x0
#define EXT_WU_EVT_RST       0x0
#define RTC_WU_EVT_RST       0x0
#define DOZE_WU_EVT_RST      0x0
#define HIB_WU_EVT_RST       0x0
#define SLEEP_SYNC_RST       0x0

__INLINE void status_pack(uint8_t caldone, uint8_t extwuevt, uint8_t rtcwuevt, uint8_t dozewuevt, uint8_t hibwuevt, uint8_t sleepsync)
{
    ASSERT_ERR(((caldone << 9) & ~0x00000200) == 0);
    ASSERT_ERR(((extwuevt << 4) & ~0x000000F0) == 0);
    ASSERT_ERR(((rtcwuevt << 3) & ~0x00000008) == 0);
    ASSERT_ERR(((dozewuevt << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((hibwuevt << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((sleepsync << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(STATUS_INDEX,  (caldone << 9) | (extwuevt << 4) | (rtcwuevt << 3) | (dozewuevt << 2) | (hibwuevt << 1) | (sleepsync << 0));
}

__INLINE void status_unpack(uint8_t* vreg1p5vrdy, uint8_t* vreg1p8vrdy, uint8_t* vregbuckrdy, uint8_t* copevt, uint8_t* caldone, uint8_t* extwuevt, uint8_t* rtcwuevt, uint8_t* dozewuevt, uint8_t* hibwuevt, uint8_t* sleepsync)
{
    uint32_t localVal = sim_reg_crm_rd(STATUS_INDEX);

    *vreg1p5vrdy = (localVal & 0x00080000) >> 19;
    *vreg1p8vrdy = (localVal & 0x00040000) >> 18;
    *vregbuckrdy = (localVal & 0x00020000) >> 17;
    *copevt = (localVal & 0x00000400) >> 10;
    *caldone = (localVal & 0x00000200) >> 9;
    *extwuevt = (localVal & 0x000000F0) >> 4;
    *rtcwuevt = (localVal & 0x00000008) >> 3;
    *dozewuevt = (localVal & 0x00000004) >> 2;
    *hibwuevt = (localVal & 0x00000002) >> 1;
    *sleepsync = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t vreg_1p5v_rdy_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(STATUS_INDEX);
    return ((localVal & 0x00080000) >> 19);
}

__INLINE uint8_t vreg_1p8v_rdy_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(STATUS_INDEX);
    return ((localVal & 0x00040000) >> 18);
}

__INLINE uint8_t vreg_buck_rdy_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(STATUS_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE uint8_t cop_evt_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(STATUS_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE uint8_t cal_done_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(STATUS_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE void cal_done_setf(uint8_t caldone)
{
    ASSERT_ERR(((caldone << 9) & ~0x00000200) == 0);
    sim_reg_crm_wr(STATUS_INDEX, (sim_reg_crm_rd(STATUS_INDEX) & ~0x00000200) | (caldone << 9));
}

__INLINE uint8_t ext_wu_evt_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(STATUS_INDEX);
    return ((localVal & 0x000000F0) >> 4);
}

__INLINE void ext_wu_evt_setf(uint8_t extwuevt)
{
    ASSERT_ERR(((extwuevt << 4) & ~0x000000F0) == 0);
    sim_reg_crm_wr(STATUS_INDEX, (sim_reg_crm_rd(STATUS_INDEX) & ~0x000000F0) | (extwuevt << 4));
}

__INLINE uint8_t rtc_wu_evt_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(STATUS_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE void rtc_wu_evt_setf(uint8_t rtcwuevt)
{
    ASSERT_ERR(((rtcwuevt << 3) & ~0x00000008) == 0);
    sim_reg_crm_wr(STATUS_INDEX, (sim_reg_crm_rd(STATUS_INDEX) & ~0x00000008) | (rtcwuevt << 3));
}

__INLINE uint8_t doze_wu_evt_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(STATUS_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void doze_wu_evt_setf(uint8_t dozewuevt)
{
    ASSERT_ERR(((dozewuevt << 2) & ~0x00000004) == 0);
    sim_reg_crm_wr(STATUS_INDEX, (sim_reg_crm_rd(STATUS_INDEX) & ~0x00000004) | (dozewuevt << 2));
}

__INLINE uint8_t hib_wu_evt_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(STATUS_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void hib_wu_evt_setf(uint8_t hibwuevt)
{
    ASSERT_ERR(((hibwuevt << 1) & ~0x00000002) == 0);
    sim_reg_crm_wr(STATUS_INDEX, (sim_reg_crm_rd(STATUS_INDEX) & ~0x00000002) | (hibwuevt << 1));
}

__INLINE uint8_t sleep_sync_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(STATUS_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void sleep_sync_setf(uint8_t sleepsync)
{
    ASSERT_ERR(((sleepsync << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(STATUS_INDEX, (sim_reg_crm_rd(STATUS_INDEX) & ~0x00000001) | (sleepsync << 0));
}

/**
 * @brief MOD_STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     17               AIM_EN   0
 *     15               NEX_EN   0
 *     14               JTA_EN   0
 *     12               ADC_EN   0
 *     11              SPIF_EN   0
 *     10               SSI_EN   0
 *     09               I2C_EN   0
 *     08               RIF_EN   0
 *     07               TMR_EN   0
 *     06             UART2_EN   0
 *     05             UART1_EN   0
 *     04              GPIO_EN   0
 *     03               SPI_EN   0
 *     02               ASM_EN   0
 *     01              MACA_EN   0
 *     00               ARM_EN   0
 * </pre>
 */
#define MOD_STATUS_INDEX  0x00000007

__INLINE uint32_t mod_status_get(void)
{
    return sim_reg_crm_rd(MOD_STATUS_INDEX);
}

__INLINE void mod_status_set(uint32_t value)
{
    sim_reg_crm_wr(MOD_STATUS_INDEX, value);
}

// field definitions
#define AIM_EN_BIT      0x00020000
#define AIM_EN_POS      17
#define NEX_EN_BIT      0x00008000
#define NEX_EN_POS      15
#define JTA_EN_BIT      0x00004000
#define JTA_EN_POS      14
#define ADC_EN_BIT      0x00001000
#define ADC_EN_POS      12
#define SPIF_EN_BIT     0x00000800
#define SPIF_EN_POS     11
#define SSI_EN_BIT      0x00000400
#define SSI_EN_POS      10
#define I2C_EN_BIT      0x00000200
#define I2C_EN_POS      9
#define RIF_EN_BIT      0x00000100
#define RIF_EN_POS      8
#define TMR_EN_BIT      0x00000080
#define TMR_EN_POS      7
#define UART2_EN_BIT    0x00000040
#define UART2_EN_POS    6
#define UART1_EN_BIT    0x00000020
#define UART1_EN_POS    5
#define GPIO_EN_BIT     0x00000010
#define GPIO_EN_POS     4
#define SPI_EN_BIT      0x00000008
#define SPI_EN_POS      3
#define ASM_EN_BIT      0x00000004
#define ASM_EN_POS      2
#define MACA_EN_BIT     0x00000002
#define MACA_EN_POS     1
#define ARM_EN_BIT      0x00000001
#define ARM_EN_POS      0

#define AIM_EN_RST      0x0
#define NEX_EN_RST      0x0
#define JTA_EN_RST      0x0
#define ADC_EN_RST      0x0
#define SPIF_EN_RST     0x0
#define SSI_EN_RST      0x0
#define I2C_EN_RST      0x0
#define RIF_EN_RST      0x0
#define TMR_EN_RST      0x0
#define UART2_EN_RST    0x0
#define UART1_EN_RST    0x0
#define GPIO_EN_RST     0x0
#define SPI_EN_RST      0x0
#define ASM_EN_RST      0x0
#define MACA_EN_RST     0x0
#define ARM_EN_RST      0x0

__INLINE void mod_status_unpack(uint8_t* aimen, uint8_t* nexen, uint8_t* jtaen, uint8_t* adcen, uint8_t* spifen, uint8_t* ssien, uint8_t* i2cen, uint8_t* rifen, uint8_t* tmren, uint8_t* uart2en, uint8_t* uart1en, uint8_t* gpioen, uint8_t* spien, uint8_t* asmen, uint8_t* macaen, uint8_t* armen)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);

    *aimen = (localVal & 0x00020000) >> 17;
    *nexen = (localVal & 0x00008000) >> 15;
    *jtaen = (localVal & 0x00004000) >> 14;
    *adcen = (localVal & 0x00001000) >> 12;
    *spifen = (localVal & 0x00000800) >> 11;
    *ssien = (localVal & 0x00000400) >> 10;
    *i2cen = (localVal & 0x00000200) >> 9;
    *rifen = (localVal & 0x00000100) >> 8;
    *tmren = (localVal & 0x00000080) >> 7;
    *uart2en = (localVal & 0x00000040) >> 6;
    *uart1en = (localVal & 0x00000020) >> 5;
    *gpioen = (localVal & 0x00000010) >> 4;
    *spien = (localVal & 0x00000008) >> 3;
    *asmen = (localVal & 0x00000004) >> 2;
    *macaen = (localVal & 0x00000002) >> 1;
    *armen = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t aim_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE uint8_t nex_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00008000) >> 15);
}

__INLINE uint8_t jta_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00004000) >> 14);
}

__INLINE uint8_t adc_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00001000) >> 12);
}

__INLINE uint8_t spif_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000800) >> 11);
}

__INLINE uint8_t ssi_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE uint8_t i2c_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE uint8_t rif_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE uint8_t tmr_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE uint8_t uart2_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE uint8_t uart1_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE uint8_t gpio_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE uint8_t spi_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE uint8_t asm_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE uint8_t maca_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE uint8_t arm_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(MOD_STATUS_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

/**
 * @brief WU_COUNT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             WU_COUNT   0x0
 * </pre>
 */
#define WU_COUNT_INDEX  0x00000008

__INLINE uint32_t wu_count_get(void)
{
    return sim_reg_crm_rd(WU_COUNT_INDEX);
}

__INLINE void wu_count_set(uint32_t value)
{
    sim_reg_crm_wr(WU_COUNT_INDEX, value);
}

// field definitions
#define WU_COUNT_MASK   0xFFFFFFFF
#define WU_COUNT_LSB    0
#define WU_COUNT_WIDTH  0x00000020

#define WU_COUNT_RST    0x0

__INLINE uint32_t wu_count_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_COUNT_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

/**
 * @brief WU_TIMEOUT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00           WU_TIMEOUT   0x0
 * </pre>
 */
#define WU_TIMEOUT_INDEX  0x00000009

__INLINE uint32_t wu_timeout_get(void)
{
    return sim_reg_crm_rd(WU_TIMEOUT_INDEX);
}

__INLINE void wu_timeout_set(uint32_t value)
{
    sim_reg_crm_wr(WU_TIMEOUT_INDEX, value);
}

// field definitions
#define WU_TIMEOUT_MASK   0xFFFFFFFF
#define WU_TIMEOUT_LSB    0
#define WU_TIMEOUT_WIDTH  0x00000020

#define WU_TIMEOUT_RST    0x0

__INLINE uint32_t wu_timeout_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(WU_TIMEOUT_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

__INLINE void wu_timeout_setf(uint32_t wutimeout)
{
    ASSERT_ERR(((wutimeout << 0) & ~0xFFFFFFFF) == 0);
    sim_reg_crm_wr(WU_TIMEOUT_INDEX, wutimeout << 0);
}

/**
 * @brief RTC_COUNT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            RTC_COUNT   0x0
 * </pre>
 */
#define RTC_COUNT_INDEX  0x0000000A

__INLINE uint32_t rtc_count_get(void)
{
    return sim_reg_crm_rd(RTC_COUNT_INDEX);
}

__INLINE void rtc_count_set(uint32_t value)
{
    sim_reg_crm_wr(RTC_COUNT_INDEX, value);
}

// field definitions
#define RTC_COUNT_MASK   0xFFFFFFFF
#define RTC_COUNT_LSB    0
#define RTC_COUNT_WIDTH  0x00000020

#define RTC_COUNT_RST    0x0

__INLINE uint32_t rtc_count_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(RTC_COUNT_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

/**
 * @brief RTC_TIMEOUT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          RTC_TIMEOUT   0x0
 * </pre>
 */
#define RTC_TIMEOUT_INDEX  0x0000000B

__INLINE uint32_t rtc_timeout_get(void)
{
    return sim_reg_crm_rd(RTC_TIMEOUT_INDEX);
}

__INLINE void rtc_timeout_set(uint32_t value)
{
    sim_reg_crm_wr(RTC_TIMEOUT_INDEX, value);
}

// field definitions
#define RTC_TIMEOUT_MASK   0xFFFFFFFF
#define RTC_TIMEOUT_LSB    0
#define RTC_TIMEOUT_WIDTH  0x00000020

#define RTC_TIMEOUT_RST    0x0

__INLINE uint32_t rtc_timeout_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(RTC_TIMEOUT_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

__INLINE void rtc_timeout_setf(uint32_t rtctimeout)
{
    ASSERT_ERR(((rtctimeout << 0) & ~0xFFFFFFFF) == 0);
    sim_reg_crm_wr(RTC_TIMEOUT_INDEX, rtctimeout << 0);
}

/**
 * @brief CAL_CNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     17              CAL_IEN   0
 *     16               CAL_EN   0
 *  15:00          CAL_TIMEOUT   0x0
 * </pre>
 */
#define CAL_CNTL_INDEX  0x0000000D

__INLINE uint32_t cal_cntl_get(void)
{
    return sim_reg_crm_rd(CAL_CNTL_INDEX);
}

__INLINE void cal_cntl_set(uint32_t value)
{
    sim_reg_crm_wr(CAL_CNTL_INDEX, value);
}

// field definitions
#define CAL_IEN_BIT        0x00020000
#define CAL_IEN_POS        17
#define CAL_EN_BIT         0x00010000
#define CAL_EN_POS         16
#define CAL_TIMEOUT_MASK   0x0000FFFF
#define CAL_TIMEOUT_LSB    0
#define CAL_TIMEOUT_WIDTH  0x00000010

#define CAL_IEN_RST        0x0
#define CAL_EN_RST         0x0
#define CAL_TIMEOUT_RST    0x0

__INLINE void cal_cntl_pack(uint8_t calien, uint8_t calen, uint16_t caltimeout)
{
    ASSERT_ERR(((calien << 17) & ~0x00020000) == 0);
    ASSERT_ERR(((calen << 16) & ~0x00010000) == 0);
    ASSERT_ERR(((caltimeout << 0) & ~0x0000FFFF) == 0);
    sim_reg_crm_wr(CAL_CNTL_INDEX,  (calien << 17) | (calen << 16) | (caltimeout << 0));
}

__INLINE void cal_cntl_unpack(uint8_t* calien, uint8_t* calen, uint16_t* caltimeout)
{
    uint32_t localVal = sim_reg_crm_rd(CAL_CNTL_INDEX);

    *calien = (localVal & 0x00020000) >> 17;
    *calen = (localVal & 0x00010000) >> 16;
    *caltimeout = (localVal & 0x0000FFFF) >> 0;
}

__INLINE uint8_t cal_ien_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(CAL_CNTL_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE void cal_ien_setf(uint8_t calien)
{
    ASSERT_ERR(((calien << 17) & ~0x00020000) == 0);
    sim_reg_crm_wr(CAL_CNTL_INDEX, (sim_reg_crm_rd(CAL_CNTL_INDEX) & ~0x00020000) | (calien << 17));
}

__INLINE uint8_t cal_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(CAL_CNTL_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE void cal_en_setf(uint8_t calen)
{
    ASSERT_ERR(((calen << 16) & ~0x00010000) == 0);
    sim_reg_crm_wr(CAL_CNTL_INDEX, (sim_reg_crm_rd(CAL_CNTL_INDEX) & ~0x00010000) | (calen << 16));
}

__INLINE uint16_t cal_timeout_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(CAL_CNTL_INDEX);
    return ((localVal & 0x0000FFFF) >> 0);
}

__INLINE void cal_timeout_setf(uint16_t caltimeout)
{
    ASSERT_ERR(((caltimeout << 0) & ~0x0000FFFF) == 0);
    sim_reg_crm_wr(CAL_CNTL_INDEX, (sim_reg_crm_rd(CAL_CNTL_INDEX) & ~0x0000FFFF) | (caltimeout << 0));
}

/**
 * @brief CAL_COUNT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            CAL_COUNT   0x0
 * </pre>
 */
#define CAL_COUNT_INDEX  0x0000000E

__INLINE uint32_t cal_count_get(void)
{
    return sim_reg_crm_rd(CAL_COUNT_INDEX);
}

__INLINE void cal_count_set(uint32_t value)
{
    sim_reg_crm_wr(CAL_COUNT_INDEX, value);
}

// field definitions
#define CAL_COUNT_MASK   0xFFFFFFFF
#define CAL_COUNT_LSB    0
#define CAL_COUNT_WIDTH  0x00000020

#define CAL_COUNT_RST    0x0

__INLINE uint32_t cal_count_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(CAL_COUNT_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

/**
 * @brief RINGOSC_CNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  12:09           ROSC_CTUNE   0x0
 *  08:04           ROSC_FTUNE   0x0
 *     00              ROSC_EN   0
 * </pre>
 */
#define RINGOSC_CNTL_INDEX  0x0000000F

__INLINE uint32_t ringosc_cntl_get(void)
{
    return sim_reg_crm_rd(RINGOSC_CNTL_INDEX);
}

__INLINE void ringosc_cntl_set(uint32_t value)
{
    sim_reg_crm_wr(RINGOSC_CNTL_INDEX, value);
}

// field definitions
#define ROSC_CTUNE_MASK   0x00001E00
#define ROSC_CTUNE_LSB    9
#define ROSC_CTUNE_WIDTH  0x00000004
#define ROSC_FTUNE_MASK   0x000001F0
#define ROSC_FTUNE_LSB    4
#define ROSC_FTUNE_WIDTH  0x00000005
#define ROSC_EN_BIT       0x00000001
#define ROSC_EN_POS       0

#define ROSC_CTUNE_RST    0x0
#define ROSC_FTUNE_RST    0x0
#define ROSC_EN_RST       0x0

__INLINE void ringosc_cntl_pack(uint8_t roscctune, uint8_t roscftune, uint8_t roscen)
{
    ASSERT_ERR(((roscctune << 9) & ~0x00001E00) == 0);
    ASSERT_ERR(((roscftune << 4) & ~0x000001F0) == 0);
    ASSERT_ERR(((roscen << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(RINGOSC_CNTL_INDEX,  (roscctune << 9) | (roscftune << 4) | (roscen << 0));
}

__INLINE void ringosc_cntl_unpack(uint8_t* roscctune, uint8_t* roscftune, uint8_t* roscen)
{
    uint32_t localVal = sim_reg_crm_rd(RINGOSC_CNTL_INDEX);

    *roscctune = (localVal & 0x00001E00) >> 9;
    *roscftune = (localVal & 0x000001F0) >> 4;
    *roscen = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t rosc_ctune_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(RINGOSC_CNTL_INDEX);
    return ((localVal & 0x00001E00) >> 9);
}

__INLINE void rosc_ctune_setf(uint8_t roscctune)
{
    ASSERT_ERR(((roscctune << 9) & ~0x00001E00) == 0);
    sim_reg_crm_wr(RINGOSC_CNTL_INDEX, (sim_reg_crm_rd(RINGOSC_CNTL_INDEX) & ~0x00001E00) | (roscctune << 9));
}

__INLINE uint8_t rosc_ftune_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(RINGOSC_CNTL_INDEX);
    return ((localVal & 0x000001F0) >> 4);
}

__INLINE void rosc_ftune_setf(uint8_t roscftune)
{
    ASSERT_ERR(((roscftune << 4) & ~0x000001F0) == 0);
    sim_reg_crm_wr(RINGOSC_CNTL_INDEX, (sim_reg_crm_rd(RINGOSC_CNTL_INDEX) & ~0x000001F0) | (roscftune << 4));
}

__INLINE uint8_t rosc_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(RINGOSC_CNTL_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void rosc_en_setf(uint8_t roscen)
{
    ASSERT_ERR(((roscen << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(RINGOSC_CNTL_INDEX, (sim_reg_crm_rd(RINGOSC_CNTL_INDEX) & ~0x00000001) | (roscen << 0));
}

/**
 * @brief XTAL_CNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  25:21           XTAL_CTUNE   0x0
 *  20:16           XTAL_FTUNE   0x0
 *  11:08       XTAL_IBIAS_SEL   0x0
 * </pre>
 */
#define XTAL_CNTL_INDEX  0x00000010

__INLINE uint32_t xtal_cntl_get(void)
{
    return sim_reg_crm_rd(XTAL_CNTL_INDEX);
}

__INLINE void xtal_cntl_set(uint32_t value)
{
    sim_reg_crm_wr(XTAL_CNTL_INDEX, value);
}

// field definitions
#define XTAL_CTUNE_MASK       0x03E00000
#define XTAL_CTUNE_LSB        21
#define XTAL_CTUNE_WIDTH      0x00000005
#define XTAL_FTUNE_MASK       0x001F0000
#define XTAL_FTUNE_LSB        16
#define XTAL_FTUNE_WIDTH      0x00000005
#define XTAL_IBIAS_SEL_MASK   0x00000F00
#define XTAL_IBIAS_SEL_LSB    8
#define XTAL_IBIAS_SEL_WIDTH  0x00000004

#define XTAL_CTUNE_RST        0x0
#define XTAL_FTUNE_RST        0x0
#define XTAL_IBIAS_SEL_RST    0x0

__INLINE void xtal_cntl_pack(uint8_t xtalctune, uint8_t xtalftune, uint8_t xtalibiassel)
{
    ASSERT_ERR(((xtalctune << 21) & ~0x03E00000) == 0);
    ASSERT_ERR(((xtalftune << 16) & ~0x001F0000) == 0);
    ASSERT_ERR(((xtalibiassel << 8) & ~0x00000F00) == 0);
    sim_reg_crm_wr(XTAL_CNTL_INDEX,  (xtalctune << 21) | (xtalftune << 16) | (xtalibiassel << 8));
}

__INLINE void xtal_cntl_unpack(uint8_t* xtalctune, uint8_t* xtalftune, uint8_t* xtalibiassel)
{
    uint32_t localVal = sim_reg_crm_rd(XTAL_CNTL_INDEX);

    *xtalctune = (localVal & 0x03E00000) >> 21;
    *xtalftune = (localVal & 0x001F0000) >> 16;
    *xtalibiassel = (localVal & 0x00000F00) >> 8;
}

__INLINE uint8_t xtal_ctune_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(XTAL_CNTL_INDEX);
    return ((localVal & 0x03E00000) >> 21);
}

__INLINE void xtal_ctune_setf(uint8_t xtalctune)
{
    ASSERT_ERR(((xtalctune << 21) & ~0x03E00000) == 0);
    sim_reg_crm_wr(XTAL_CNTL_INDEX, (sim_reg_crm_rd(XTAL_CNTL_INDEX) & ~0x03E00000) | (xtalctune << 21));
}

__INLINE uint8_t xtal_ftune_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(XTAL_CNTL_INDEX);
    return ((localVal & 0x001F0000) >> 16);
}

__INLINE void xtal_ftune_setf(uint8_t xtalftune)
{
    ASSERT_ERR(((xtalftune << 16) & ~0x001F0000) == 0);
    sim_reg_crm_wr(XTAL_CNTL_INDEX, (sim_reg_crm_rd(XTAL_CNTL_INDEX) & ~0x001F0000) | (xtalftune << 16));
}

__INLINE uint8_t xtal_ibias_sel_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(XTAL_CNTL_INDEX);
    return ((localVal & 0x00000F00) >> 8);
}

__INLINE void xtal_ibias_sel_setf(uint8_t xtalibiassel)
{
    ASSERT_ERR(((xtalibiassel << 8) & ~0x00000F00) == 0);
    sim_reg_crm_wr(XTAL_CNTL_INDEX, (sim_reg_crm_rd(XTAL_CNTL_INDEX) & ~0x00000F00) | (xtalibiassel << 8));
}

/**
 * @brief XTAL32_CNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  05:04          XTAL32_GAIN   0x3
 *     00            XTAL32_EN   0
 * </pre>
 */
#define XTAL32_CNTL_INDEX  0x00000011

__INLINE uint32_t xtal32_cntl_get(void)
{
    return sim_reg_crm_rd(XTAL32_CNTL_INDEX);
}

__INLINE void xtal32_cntl_set(uint32_t value)
{
    sim_reg_crm_wr(XTAL32_CNTL_INDEX, value);
}

// field definitions
#define XTAL32_GAIN_MASK   0x00000030
#define XTAL32_GAIN_LSB    4
#define XTAL32_GAIN_WIDTH  0x00000002
#define XTAL32_EN_BIT      0x00000001
#define XTAL32_EN_POS      0

#define XTAL32_GAIN_RST    0x3
#define XTAL32_EN_RST      0x0

__INLINE void xtal32_cntl_pack(uint8_t xtal32gain, uint8_t xtal32en)
{
    ASSERT_ERR(((xtal32gain << 4) & ~0x00000030) == 0);
    ASSERT_ERR(((xtal32en << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(XTAL32_CNTL_INDEX,  (xtal32gain << 4) | (xtal32en << 0));
}

__INLINE void xtal32_cntl_unpack(uint8_t* xtal32gain, uint8_t* xtal32en)
{
    uint32_t localVal = sim_reg_crm_rd(XTAL32_CNTL_INDEX);

    *xtal32gain = (localVal & 0x00000030) >> 4;
    *xtal32en = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t xtal32_gain_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(XTAL32_CNTL_INDEX);
    return ((localVal & 0x00000030) >> 4);
}

__INLINE void xtal32_gain_setf(uint8_t xtal32gain)
{
    ASSERT_ERR(((xtal32gain << 4) & ~0x00000030) == 0);
    sim_reg_crm_wr(XTAL32_CNTL_INDEX, (sim_reg_crm_rd(XTAL32_CNTL_INDEX) & ~0x00000030) | (xtal32gain << 4));
}

__INLINE uint8_t xtal32_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(XTAL32_CNTL_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void xtal32_en_setf(uint8_t xtal32en)
{
    ASSERT_ERR(((xtal32en << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(XTAL32_CNTL_INDEX, (sim_reg_crm_rd(XTAL32_CNTL_INDEX) & ~0x00000001) | (xtal32en << 0));
}

/**
 * @brief VREG_CNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  11:08          BUCK_CLKDIV   0xF
 *     07         VREG_1P8V_EN   0
 *  06:05        VREG_1P5V_SEL   0x1
 *  04:03         VREG_1P5V_EN   0x0
 *     02       BUCK_BYPASS_EN   0
 *     01     BUCK_SYNC_REC_EN   0
 *     00              BUCK_EN   0
 * </pre>
 */
#define VREG_CNTL_INDEX  0x00000012

__INLINE uint32_t vreg_cntl_get(void)
{
    return sim_reg_crm_rd(VREG_CNTL_INDEX);
}

__INLINE void vreg_cntl_set(uint32_t value)
{
    sim_reg_crm_wr(VREG_CNTL_INDEX, value);
}

// field definitions
#define BUCK_CLKDIV_MASK        0x00000F00
#define BUCK_CLKDIV_LSB         8
#define BUCK_CLKDIV_WIDTH       0x00000004
#define VREG_1P8V_EN_BIT        0x00000080
#define VREG_1P8V_EN_POS        7
#define VREG_1P5V_SEL_MASK      0x00000060
#define VREG_1P5V_SEL_LSB       5
#define VREG_1P5V_SEL_WIDTH     0x00000002
#define VREG_1P5V_EN_MASK       0x00000018
#define VREG_1P5V_EN_LSB        3
#define VREG_1P5V_EN_WIDTH      0x00000002
#define BUCK_BYPASS_EN_BIT      0x00000004
#define BUCK_BYPASS_EN_POS      2
#define BUCK_SYNC_REC_EN_BIT    0x00000002
#define BUCK_SYNC_REC_EN_POS    1
#define BUCK_EN_BIT             0x00000001
#define BUCK_EN_POS             0

#define BUCK_CLKDIV_RST         0xF
#define VREG_1P8V_EN_RST        0x0
#define VREG_1P5V_SEL_RST       0x1
#define VREG_1P5V_EN_RST        0x0
#define BUCK_BYPASS_EN_RST      0x0
#define BUCK_SYNC_REC_EN_RST    0x0
#define BUCK_EN_RST             0x0

__INLINE void vreg_cntl_pack(uint8_t buckclkdiv, uint8_t vreg1p8ven, uint8_t vreg1p5vsel, uint8_t vreg1p5ven, uint8_t buckbypassen, uint8_t bucksyncrecen, uint8_t bucken)
{
    ASSERT_ERR(((buckclkdiv << 8) & ~0x00000F00) == 0);
    ASSERT_ERR(((vreg1p8ven << 7) & ~0x00000080) == 0);
    ASSERT_ERR(((vreg1p5vsel << 5) & ~0x00000060) == 0);
    ASSERT_ERR(((vreg1p5ven << 3) & ~0x00000018) == 0);
    ASSERT_ERR(((buckbypassen << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((bucksyncrecen << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((bucken << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(VREG_CNTL_INDEX,  (buckclkdiv << 8) | (vreg1p8ven << 7) | (vreg1p5vsel << 5) | (vreg1p5ven << 3) | (buckbypassen << 2) | (bucksyncrecen << 1) | (bucken << 0));
}

__INLINE void vreg_cntl_unpack(uint8_t* buckclkdiv, uint8_t* vreg1p8ven, uint8_t* vreg1p5vsel, uint8_t* vreg1p5ven, uint8_t* buckbypassen, uint8_t* bucksyncrecen, uint8_t* bucken)
{
    uint32_t localVal = sim_reg_crm_rd(VREG_CNTL_INDEX);

    *buckclkdiv = (localVal & 0x00000F00) >> 8;
    *vreg1p8ven = (localVal & 0x00000080) >> 7;
    *vreg1p5vsel = (localVal & 0x00000060) >> 5;
    *vreg1p5ven = (localVal & 0x00000018) >> 3;
    *buckbypassen = (localVal & 0x00000004) >> 2;
    *bucksyncrecen = (localVal & 0x00000002) >> 1;
    *bucken = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t buck_clkdiv_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(VREG_CNTL_INDEX);
    return ((localVal & 0x00000F00) >> 8);
}

__INLINE void buck_clkdiv_setf(uint8_t buckclkdiv)
{
    ASSERT_ERR(((buckclkdiv << 8) & ~0x00000F00) == 0);
    sim_reg_crm_wr(VREG_CNTL_INDEX, (sim_reg_crm_rd(VREG_CNTL_INDEX) & ~0x00000F00) | (buckclkdiv << 8));
}

__INLINE uint8_t vreg_1p8v_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(VREG_CNTL_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE void vreg_1p8v_en_setf(uint8_t vreg1p8ven)
{
    ASSERT_ERR(((vreg1p8ven << 7) & ~0x00000080) == 0);
    sim_reg_crm_wr(VREG_CNTL_INDEX, (sim_reg_crm_rd(VREG_CNTL_INDEX) & ~0x00000080) | (vreg1p8ven << 7));
}

__INLINE uint8_t vreg_1p5v_sel_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(VREG_CNTL_INDEX);
    return ((localVal & 0x00000060) >> 5);
}

__INLINE void vreg_1p5v_sel_setf(uint8_t vreg1p5vsel)
{
    ASSERT_ERR(((vreg1p5vsel << 5) & ~0x00000060) == 0);
    sim_reg_crm_wr(VREG_CNTL_INDEX, (sim_reg_crm_rd(VREG_CNTL_INDEX) & ~0x00000060) | (vreg1p5vsel << 5));
}

__INLINE uint8_t vreg_1p5v_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(VREG_CNTL_INDEX);
    return ((localVal & 0x00000018) >> 3);
}

__INLINE void vreg_1p5v_en_setf(uint8_t vreg1p5ven)
{
    ASSERT_ERR(((vreg1p5ven << 3) & ~0x00000018) == 0);
    sim_reg_crm_wr(VREG_CNTL_INDEX, (sim_reg_crm_rd(VREG_CNTL_INDEX) & ~0x00000018) | (vreg1p5ven << 3));
}

__INLINE uint8_t buck_bypass_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(VREG_CNTL_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void buck_bypass_en_setf(uint8_t buckbypassen)
{
    ASSERT_ERR(((buckbypassen << 2) & ~0x00000004) == 0);
    sim_reg_crm_wr(VREG_CNTL_INDEX, (sim_reg_crm_rd(VREG_CNTL_INDEX) & ~0x00000004) | (buckbypassen << 2));
}

__INLINE uint8_t buck_sync_rec_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(VREG_CNTL_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void buck_sync_rec_en_setf(uint8_t bucksyncrecen)
{
    ASSERT_ERR(((bucksyncrecen << 1) & ~0x00000002) == 0);
    sim_reg_crm_wr(VREG_CNTL_INDEX, (sim_reg_crm_rd(VREG_CNTL_INDEX) & ~0x00000002) | (bucksyncrecen << 1));
}

__INLINE uint8_t buck_en_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(VREG_CNTL_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void buck_en_setf(uint8_t bucken)
{
    ASSERT_ERR(((bucken << 0) & ~0x00000001) == 0);
    sim_reg_crm_wr(VREG_CNTL_INDEX, (sim_reg_crm_rd(VREG_CNTL_INDEX) & ~0x00000001) | (bucken << 0));
}

/**
 * @brief SW_RST register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00               SW_RST   0x87651234
 * </pre>
 */
#define SW_RST_INDEX  0x00000014

__INLINE uint32_t sw_rst_get(void)
{
    return sim_reg_crm_rd(SW_RST_INDEX);
}

__INLINE void sw_rst_set(uint32_t value)
{
    sim_reg_crm_wr(SW_RST_INDEX, value);
}

// field definitions
#define SW_RST_MASK   0xFFFFFFFF
#define SW_RST_LSB    0
#define SW_RST_WIDTH  0x00000020

#define SW_RST_RST    0x87651234

__INLINE uint32_t sw_rst_getf(void)
{
    uint32_t localVal = sim_reg_crm_rd(SW_RST_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

__INLINE void sw_rst_setf(uint32_t swrst)
{
    ASSERT_ERR(((swrst << 0) & ~0xFFFFFFFF) == 0);
    sim_reg_crm_wr(SW_RST_INDEX, swrst << 0);
}


#endif // _REG_CRM_H_

