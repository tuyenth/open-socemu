#ifndef _REG_SPIF_H_
#define _REG_SPIF_H_

#include <stdint.h>
#include "compiler.h"
#include "arch.h"
#include "reg_sim.h"

#define REG_SPIF_BASE_ADDR 0x8000C000

#define REG_SPIF_COUNT 5

#define REG_SPIF_DECODING_MASK 0x0000001F

/**
 * @brief SPIF_TX_DATA register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         SPIF_TX_DATA   0x0
 * </pre>
 */
#define SPIF_TX_DATA_INDEX  0x00000000

__INLINE uint32_t spif_tx_data_get(void)
{
    return sim_reg_spif_rd(SPIF_TX_DATA_INDEX);
}

__INLINE void spif_tx_data_set(uint32_t value)
{
    sim_reg_spif_wr(SPIF_TX_DATA_INDEX, value);
}

// field definitions
#define SPIF_TX_DATA_MASK   0xFFFFFFFF
#define SPIF_TX_DATA_LSB    0
#define SPIF_TX_DATA_WIDTH  0x00000020

#define SPIF_TX_DATA_RST    0x0

__INLINE uint32_t spif_tx_data_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_TX_DATA_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

__INLINE void spif_tx_data_setf(uint32_t spiftxdata)
{
    ASSERT_ERR(((spiftxdata << 0) & ~0xFFFFFFFF) == 0);
    sim_reg_spif_wr(SPIF_TX_DATA_INDEX, spiftxdata << 0);
}

/**
 * @brief SPIF_RX_DATA register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         SPIF_RX_DATA   0x0
 * </pre>
 */
#define SPIF_RX_DATA_INDEX  0x00000001

__INLINE uint32_t spif_rx_data_get(void)
{
    return sim_reg_spif_rd(SPIF_RX_DATA_INDEX);
}

__INLINE void spif_rx_data_set(uint32_t value)
{
    sim_reg_spif_wr(SPIF_RX_DATA_INDEX, value);
}

// field definitions
#define SPIF_RX_DATA_MASK   0xFFFFFFFF
#define SPIF_RX_DATA_LSB    0
#define SPIF_RX_DATA_WIDTH  0x00000020

#define SPIF_RX_DATA_RST    0x0

__INLINE uint32_t spif_rx_data_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_RX_DATA_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

/**
 * @brief SPIF_CLK_CTRL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08       SPIF_SCK_COUNT   0x0
 *     07           SPIF_START   0
 *  06:00     SPIF_DATA_LENGTH   0x0
 * </pre>
 */
#define SPIF_CLK_CTRL_INDEX  0x00000002

__INLINE uint32_t spif_clk_ctrl_get(void)
{
    return sim_reg_spif_rd(SPIF_CLK_CTRL_INDEX);
}

__INLINE void spif_clk_ctrl_set(uint32_t value)
{
    sim_reg_spif_wr(SPIF_CLK_CTRL_INDEX, value);
}

// field definitions
#define SPIF_SCK_COUNT_MASK     0x0000FF00
#define SPIF_SCK_COUNT_LSB      8
#define SPIF_SCK_COUNT_WIDTH    0x00000008
#define SPIF_START_BIT          0x00000080
#define SPIF_START_POS          7
#define SPIF_DATA_LENGTH_MASK   0x0000007F
#define SPIF_DATA_LENGTH_LSB    0
#define SPIF_DATA_LENGTH_WIDTH  0x00000007

#define SPIF_SCK_COUNT_RST      0x0
#define SPIF_START_RST          0x0
#define SPIF_DATA_LENGTH_RST    0x0

__INLINE void spif_clk_ctrl_pack(uint8_t spifsckcount, uint8_t spifstart, uint8_t spifdatalength)
{
    ASSERT_ERR(((spifsckcount << 8) & ~0x0000FF00) == 0);
    ASSERT_ERR(((spifstart << 7) & ~0x00000080) == 0);
    ASSERT_ERR(((spifdatalength << 0) & ~0x0000007F) == 0);
    sim_reg_spif_wr(SPIF_CLK_CTRL_INDEX,  (spifsckcount << 8) | (spifstart << 7) | (spifdatalength << 0));
}

__INLINE void spif_clk_ctrl_unpack(uint8_t* spifsckcount, uint8_t* spifstart, uint8_t* spifdatalength)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_CLK_CTRL_INDEX);

    *spifsckcount = (localVal & 0x0000FF00) >> 8;
    *spifstart = (localVal & 0x00000080) >> 7;
    *spifdatalength = (localVal & 0x0000007F) >> 0;
}

__INLINE uint8_t spif_sck_count_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_CLK_CTRL_INDEX);
    return ((localVal & 0x0000FF00) >> 8);
}

__INLINE void spif_sck_count_setf(uint8_t spifsckcount)
{
    ASSERT_ERR(((spifsckcount << 8) & ~0x0000FF00) == 0);
    sim_reg_spif_wr(SPIF_CLK_CTRL_INDEX, (sim_reg_spif_rd(SPIF_CLK_CTRL_INDEX) & ~0x0000FF00) | (spifsckcount << 8));
}

__INLINE uint8_t spif_start_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_CLK_CTRL_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE void spif_start_setf(uint8_t spifstart)
{
    ASSERT_ERR(((spifstart << 7) & ~0x00000080) == 0);
    sim_reg_spif_wr(SPIF_CLK_CTRL_INDEX, (sim_reg_spif_rd(SPIF_CLK_CTRL_INDEX) & ~0x00000080) | (spifstart << 7));
}

__INLINE uint8_t spif_data_length_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_CLK_CTRL_INDEX);
    return ((localVal & 0x0000007F) >> 0);
}

__INLINE void spif_data_length_setf(uint8_t spifdatalength)
{
    ASSERT_ERR(((spifdatalength << 0) & ~0x0000007F) == 0);
    sim_reg_spif_wr(SPIF_CLK_CTRL_INDEX, (sim_reg_spif_rd(SPIF_CLK_CTRL_INDEX) & ~0x0000007F) | (spifdatalength << 0));
}

/**
 * @brief SPIF_SETUP register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     17           SPIF_3WIRE   0
 *     16            SPIF_MODE   0
 *  14:12        SPIF_SCK_FREQ   0x0
 *     10      SPIF_MISO_PHASE   0
 *     09       SPIF_SCK_PHASE   0
 *     08         SPIF_SCK_POL   0
 *  05:04   SPIF_SDO_INACTIVE_ST   0x0
 *  03:02        SPIF_SS_DELAY   0x0
 *  01:00        SPIF_SS_SETUP   0x0
 * </pre>
 */
#define SPIF_SETUP_INDEX  0x00000003

__INLINE uint32_t spif_setup_get(void)
{
    return sim_reg_spif_rd(SPIF_SETUP_INDEX);
}

__INLINE void spif_setup_set(uint32_t value)
{
    sim_reg_spif_wr(SPIF_SETUP_INDEX, value);
}

// field definitions
#define SPIF_3WIRE_BIT              0x00020000
#define SPIF_3WIRE_POS              17
#define SPIF_MODE_BIT               0x00010000
#define SPIF_MODE_POS               16
#define SPIF_SCK_FREQ_MASK          0x00007000
#define SPIF_SCK_FREQ_LSB           12
#define SPIF_SCK_FREQ_WIDTH         0x00000003
#define SPIF_MISO_PHASE_BIT         0x00000400
#define SPIF_MISO_PHASE_POS         10
#define SPIF_SCK_PHASE_BIT          0x00000200
#define SPIF_SCK_PHASE_POS          9
#define SPIF_SCK_POL_BIT            0x00000100
#define SPIF_SCK_POL_POS            8
#define SPIF_SDO_INACTIVE_ST_MASK   0x00000030
#define SPIF_SDO_INACTIVE_ST_LSB    4
#define SPIF_SDO_INACTIVE_ST_WIDTH  0x00000002
#define SPIF_SS_DELAY_MASK          0x0000000C
#define SPIF_SS_DELAY_LSB           2
#define SPIF_SS_DELAY_WIDTH         0x00000002
#define SPIF_SS_SETUP_MASK          0x00000003
#define SPIF_SS_SETUP_LSB           0
#define SPIF_SS_SETUP_WIDTH         0x00000002

#define SPIF_3WIRE_RST              0x0
#define SPIF_MODE_RST               0x0
#define SPIF_SCK_FREQ_RST           0x0
#define SPIF_MISO_PHASE_RST         0x0
#define SPIF_SCK_PHASE_RST          0x0
#define SPIF_SCK_POL_RST            0x0
#define SPIF_SDO_INACTIVE_ST_RST    0x0
#define SPIF_SS_DELAY_RST           0x0
#define SPIF_SS_SETUP_RST           0x0

__INLINE void spif_setup_pack(uint8_t spif3wire, uint8_t spifmode, uint8_t spifsckfreq, uint8_t spifmisophase, uint8_t spifsckphase, uint8_t spifsckpol, uint8_t spifsdoinactivest, uint8_t spifssdelay, uint8_t spifsssetup)
{
    ASSERT_ERR(((spif3wire << 17) & ~0x00020000) == 0);
    ASSERT_ERR(((spifmode << 16) & ~0x00010000) == 0);
    ASSERT_ERR(((spifsckfreq << 12) & ~0x00007000) == 0);
    ASSERT_ERR(((spifmisophase << 10) & ~0x00000400) == 0);
    ASSERT_ERR(((spifsckphase << 9) & ~0x00000200) == 0);
    ASSERT_ERR(((spifsckpol << 8) & ~0x00000100) == 0);
    ASSERT_ERR(((spifsdoinactivest << 4) & ~0x00000030) == 0);
    ASSERT_ERR(((spifssdelay << 2) & ~0x0000000C) == 0);
    ASSERT_ERR(((spifsssetup << 0) & ~0x00000003) == 0);
    sim_reg_spif_wr(SPIF_SETUP_INDEX,  (spif3wire << 17) | (spifmode << 16) | (spifsckfreq << 12) | (spifmisophase << 10) | (spifsckphase << 9) | (spifsckpol << 8) | (spifsdoinactivest << 4) | (spifssdelay << 2) | (spifsssetup << 0));
}

__INLINE void spif_setup_unpack(uint8_t* spif3wire, uint8_t* spifmode, uint8_t* spifsckfreq, uint8_t* spifmisophase, uint8_t* spifsckphase, uint8_t* spifsckpol, uint8_t* spifsdoinactivest, uint8_t* spifssdelay, uint8_t* spifsssetup)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_SETUP_INDEX);

    *spif3wire = (localVal & 0x00020000) >> 17;
    *spifmode = (localVal & 0x00010000) >> 16;
    *spifsckfreq = (localVal & 0x00007000) >> 12;
    *spifmisophase = (localVal & 0x00000400) >> 10;
    *spifsckphase = (localVal & 0x00000200) >> 9;
    *spifsckpol = (localVal & 0x00000100) >> 8;
    *spifsdoinactivest = (localVal & 0x00000030) >> 4;
    *spifssdelay = (localVal & 0x0000000C) >> 2;
    *spifsssetup = (localVal & 0x00000003) >> 0;
}

__INLINE uint8_t spif_3wire_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_SETUP_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE void spif_3wire_setf(uint8_t spif3wire)
{
    ASSERT_ERR(((spif3wire << 17) & ~0x00020000) == 0);
    sim_reg_spif_wr(SPIF_SETUP_INDEX, (sim_reg_spif_rd(SPIF_SETUP_INDEX) & ~0x00020000) | (spif3wire << 17));
}

__INLINE uint8_t spif_mode_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_SETUP_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE void spif_mode_setf(uint8_t spifmode)
{
    ASSERT_ERR(((spifmode << 16) & ~0x00010000) == 0);
    sim_reg_spif_wr(SPIF_SETUP_INDEX, (sim_reg_spif_rd(SPIF_SETUP_INDEX) & ~0x00010000) | (spifmode << 16));
}

__INLINE uint8_t spif_sck_freq_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_SETUP_INDEX);
    return ((localVal & 0x00007000) >> 12);
}

__INLINE void spif_sck_freq_setf(uint8_t spifsckfreq)
{
    ASSERT_ERR(((spifsckfreq << 12) & ~0x00007000) == 0);
    sim_reg_spif_wr(SPIF_SETUP_INDEX, (sim_reg_spif_rd(SPIF_SETUP_INDEX) & ~0x00007000) | (spifsckfreq << 12));
}

__INLINE uint8_t spif_miso_phase_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_SETUP_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE void spif_miso_phase_setf(uint8_t spifmisophase)
{
    ASSERT_ERR(((spifmisophase << 10) & ~0x00000400) == 0);
    sim_reg_spif_wr(SPIF_SETUP_INDEX, (sim_reg_spif_rd(SPIF_SETUP_INDEX) & ~0x00000400) | (spifmisophase << 10));
}

__INLINE uint8_t spif_sck_phase_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_SETUP_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE void spif_sck_phase_setf(uint8_t spifsckphase)
{
    ASSERT_ERR(((spifsckphase << 9) & ~0x00000200) == 0);
    sim_reg_spif_wr(SPIF_SETUP_INDEX, (sim_reg_spif_rd(SPIF_SETUP_INDEX) & ~0x00000200) | (spifsckphase << 9));
}

__INLINE uint8_t spif_sck_pol_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_SETUP_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE void spif_sck_pol_setf(uint8_t spifsckpol)
{
    ASSERT_ERR(((spifsckpol << 8) & ~0x00000100) == 0);
    sim_reg_spif_wr(SPIF_SETUP_INDEX, (sim_reg_spif_rd(SPIF_SETUP_INDEX) & ~0x00000100) | (spifsckpol << 8));
}

__INLINE uint8_t spif_sdo_inactive_st_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_SETUP_INDEX);
    return ((localVal & 0x00000030) >> 4);
}

__INLINE void spif_sdo_inactive_st_setf(uint8_t spifsdoinactivest)
{
    ASSERT_ERR(((spifsdoinactivest << 4) & ~0x00000030) == 0);
    sim_reg_spif_wr(SPIF_SETUP_INDEX, (sim_reg_spif_rd(SPIF_SETUP_INDEX) & ~0x00000030) | (spifsdoinactivest << 4));
}

__INLINE uint8_t spif_ss_delay_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_SETUP_INDEX);
    return ((localVal & 0x0000000C) >> 2);
}

__INLINE void spif_ss_delay_setf(uint8_t spifssdelay)
{
    ASSERT_ERR(((spifssdelay << 2) & ~0x0000000C) == 0);
    sim_reg_spif_wr(SPIF_SETUP_INDEX, (sim_reg_spif_rd(SPIF_SETUP_INDEX) & ~0x0000000C) | (spifssdelay << 2));
}

__INLINE uint8_t spif_ss_setup_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_SETUP_INDEX);
    return ((localVal & 0x00000003) >> 0);
}

__INLINE void spif_ss_setup_setf(uint8_t spifsssetup)
{
    ASSERT_ERR(((spifsssetup << 0) & ~0x00000003) == 0);
    sim_reg_spif_wr(SPIF_SETUP_INDEX, (sim_reg_spif_rd(SPIF_SETUP_INDEX) & ~0x00000003) | (spifsssetup << 0));
}

/**
 * @brief SPIF_STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     08      SPIF_FIRST_DATA   0
 *     04        SPIF_OVERFLOW   0
 *     00             SPIF_INT   0
 * </pre>
 */
#define SPIF_STATUS_INDEX  0x00000004

__INLINE uint32_t spif_status_get(void)
{
    return sim_reg_spif_rd(SPIF_STATUS_INDEX);
}

__INLINE void spif_status_set(uint32_t value)
{
    sim_reg_spif_wr(SPIF_STATUS_INDEX, value);
}

// field definitions
#define SPIF_FIRST_DATA_BIT    0x00000100
#define SPIF_FIRST_DATA_POS    8
#define SPIF_OVERFLOW_BIT      0x00000010
#define SPIF_OVERFLOW_POS      4
#define SPIF_INT_BIT           0x00000001
#define SPIF_INT_POS           0

#define SPIF_FIRST_DATA_RST    0x0
#define SPIF_OVERFLOW_RST      0x0
#define SPIF_INT_RST           0x0

__INLINE void spif_status_pack(uint8_t spifoverflow, uint8_t spifint)
{
    ASSERT_ERR(((spifoverflow << 4) & ~0x00000010) == 0);
    ASSERT_ERR(((spifint << 0) & ~0x00000001) == 0);
    sim_reg_spif_wr(SPIF_STATUS_INDEX,  (spifoverflow << 4) | (spifint << 0));
}

__INLINE void spif_status_unpack(uint8_t* spiffirstdata, uint8_t* spifoverflow, uint8_t* spifint)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_STATUS_INDEX);

    *spiffirstdata = (localVal & 0x00000100) >> 8;
    *spifoverflow = (localVal & 0x00000010) >> 4;
    *spifint = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t spif_first_data_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_STATUS_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE uint8_t spif_overflow_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_STATUS_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE void spif_overflow_setf(uint8_t spifoverflow)
{
    ASSERT_ERR(((spifoverflow << 4) & ~0x00000010) == 0);
    sim_reg_spif_wr(SPIF_STATUS_INDEX, (sim_reg_spif_rd(SPIF_STATUS_INDEX) & ~0x00000010) | (spifoverflow << 4));
}

__INLINE uint8_t spif_int_getf(void)
{
    uint32_t localVal = sim_reg_spif_rd(SPIF_STATUS_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void spif_int_setf(uint8_t spifint)
{
    ASSERT_ERR(((spifint << 0) & ~0x00000001) == 0);
    sim_reg_spif_wr(SPIF_STATUS_INDEX, (sim_reg_spif_rd(SPIF_STATUS_INDEX) & ~0x00000001) | (spifint << 0));
}


#endif // _REG_SPIF_H_

