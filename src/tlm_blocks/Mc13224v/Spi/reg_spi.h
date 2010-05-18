#ifndef _REG_SPI_H_
#define _REG_SPI_H_

#include <stdint.h>
#include "compiler.h"
#include "arch.h"
#include "reg_sim.h"

#define REG_SPI_BASE_ADDR 0x80002000

#define REG_SPI_COUNT 5

#define REG_SPI_DECODING_MASK 0x0000001F

/**
 * @brief SPI_TX_DATA register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          SPI_TX_DATA   0x0
 * </pre>
 */
#define SPI_TX_DATA_INDEX  0x00000000

__INLINE uint32_t spi_tx_data_get(void)
{
    return sim_reg_spi_rd(SPI_TX_DATA_INDEX);
}

__INLINE void spi_tx_data_set(uint32_t value)
{
    sim_reg_spi_wr(SPI_TX_DATA_INDEX, value);
}

// field definitions
#define SPI_TX_DATA_MASK   0xFFFFFFFF
#define SPI_TX_DATA_LSB    0
#define SPI_TX_DATA_WIDTH  0x00000020

#define SPI_TX_DATA_RST    0x0

__INLINE uint32_t spi_tx_data_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_TX_DATA_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

__INLINE void spi_tx_data_setf(uint32_t spitxdata)
{
    ASSERT_ERR(((spitxdata << 0) & ~0xFFFFFFFF) == 0);
    sim_reg_spi_wr(SPI_TX_DATA_INDEX, spitxdata << 0);
}

/**
 * @brief SPI_RX_DATA register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          SPI_RX_DATA   0x0
 * </pre>
 */
#define SPI_RX_DATA_INDEX  0x00000001

__INLINE uint32_t spi_rx_data_get(void)
{
    return sim_reg_spi_rd(SPI_RX_DATA_INDEX);
}

__INLINE void spi_rx_data_set(uint32_t value)
{
    sim_reg_spi_wr(SPI_RX_DATA_INDEX, value);
}

// field definitions
#define SPI_RX_DATA_MASK   0xFFFFFFFF
#define SPI_RX_DATA_LSB    0
#define SPI_RX_DATA_WIDTH  0x00000020

#define SPI_RX_DATA_RST    0x0

__INLINE uint32_t spi_rx_data_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_RX_DATA_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

/**
 * @brief SPI_CLK_CTRL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08        SPI_SCK_COUNT   0x0
 *     07            SPI_START   0
 *  06:00      SPI_DATA_LENGTH   0x0
 * </pre>
 */
#define SPI_CLK_CTRL_INDEX  0x00000002

__INLINE uint32_t spi_clk_ctrl_get(void)
{
    return sim_reg_spi_rd(SPI_CLK_CTRL_INDEX);
}

__INLINE void spi_clk_ctrl_set(uint32_t value)
{
    sim_reg_spi_wr(SPI_CLK_CTRL_INDEX, value);
}

// field definitions
#define SPI_SCK_COUNT_MASK     0x0000FF00
#define SPI_SCK_COUNT_LSB      8
#define SPI_SCK_COUNT_WIDTH    0x00000008
#define SPI_START_BIT          0x00000080
#define SPI_START_POS          7
#define SPI_DATA_LENGTH_MASK   0x0000007F
#define SPI_DATA_LENGTH_LSB    0
#define SPI_DATA_LENGTH_WIDTH  0x00000007

#define SPI_SCK_COUNT_RST      0x0
#define SPI_START_RST          0x0
#define SPI_DATA_LENGTH_RST    0x0

__INLINE void spi_clk_ctrl_pack(uint8_t spisckcount, uint8_t spistart, uint8_t spidatalength)
{
    ASSERT_ERR(((spisckcount << 8) & ~0x0000FF00) == 0);
    ASSERT_ERR(((spistart << 7) & ~0x00000080) == 0);
    ASSERT_ERR(((spidatalength << 0) & ~0x0000007F) == 0);
    sim_reg_spi_wr(SPI_CLK_CTRL_INDEX,  (spisckcount << 8) | (spistart << 7) | (spidatalength << 0));
}

__INLINE void spi_clk_ctrl_unpack(uint8_t* spisckcount, uint8_t* spistart, uint8_t* spidatalength)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_CLK_CTRL_INDEX);

    *spisckcount = (localVal & 0x0000FF00) >> 8;
    *spistart = (localVal & 0x00000080) >> 7;
    *spidatalength = (localVal & 0x0000007F) >> 0;
}

__INLINE uint8_t spi_sck_count_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_CLK_CTRL_INDEX);
    return ((localVal & 0x0000FF00) >> 8);
}

__INLINE void spi_sck_count_setf(uint8_t spisckcount)
{
    ASSERT_ERR(((spisckcount << 8) & ~0x0000FF00) == 0);
    sim_reg_spi_wr(SPI_CLK_CTRL_INDEX, (sim_reg_spi_rd(SPI_CLK_CTRL_INDEX) & ~0x0000FF00) | (spisckcount << 8));
}

__INLINE uint8_t spi_start_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_CLK_CTRL_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE void spi_start_setf(uint8_t spistart)
{
    ASSERT_ERR(((spistart << 7) & ~0x00000080) == 0);
    sim_reg_spi_wr(SPI_CLK_CTRL_INDEX, (sim_reg_spi_rd(SPI_CLK_CTRL_INDEX) & ~0x00000080) | (spistart << 7));
}

__INLINE uint8_t spi_data_length_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_CLK_CTRL_INDEX);
    return ((localVal & 0x0000007F) >> 0);
}

__INLINE void spi_data_length_setf(uint8_t spidatalength)
{
    ASSERT_ERR(((spidatalength << 0) & ~0x0000007F) == 0);
    sim_reg_spi_wr(SPI_CLK_CTRL_INDEX, (sim_reg_spi_rd(SPI_CLK_CTRL_INDEX) & ~0x0000007F) | (spidatalength << 0));
}

/**
 * @brief SPI_SETUP register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     17            SPI_3WIRE   0
 *     16             SPI_MODE   0
 *  14:12         SPI_SCK_FREQ   0x0
 *     10       SPI_MISO_PHASE   0
 *     09        SPI_SCK_PHASE   0
 *     08          SPI_SCK_POL   0
 *  05:04   SPI_SDO_INACTIVE_ST   0x0
 *  03:02         SPI_SS_DELAY   0x0
 *  01:00         SPI_SS_SETUP   0x0
 * </pre>
 */
#define SPI_SETUP_INDEX  0x00000003

__INLINE uint32_t spi_setup_get(void)
{
    return sim_reg_spi_rd(SPI_SETUP_INDEX);
}

__INLINE void spi_setup_set(uint32_t value)
{
    sim_reg_spi_wr(SPI_SETUP_INDEX, value);
}

// field definitions
#define SPI_3WIRE_BIT              0x00020000
#define SPI_3WIRE_POS              17
#define SPI_MODE_BIT               0x00010000
#define SPI_MODE_POS               16
#define SPI_SCK_FREQ_MASK          0x00007000
#define SPI_SCK_FREQ_LSB           12
#define SPI_SCK_FREQ_WIDTH         0x00000003
#define SPI_MISO_PHASE_BIT         0x00000400
#define SPI_MISO_PHASE_POS         10
#define SPI_SCK_PHASE_BIT          0x00000200
#define SPI_SCK_PHASE_POS          9
#define SPI_SCK_POL_BIT            0x00000100
#define SPI_SCK_POL_POS            8
#define SPI_SDO_INACTIVE_ST_MASK   0x00000030
#define SPI_SDO_INACTIVE_ST_LSB    4
#define SPI_SDO_INACTIVE_ST_WIDTH  0x00000002
#define SPI_SS_DELAY_MASK          0x0000000C
#define SPI_SS_DELAY_LSB           2
#define SPI_SS_DELAY_WIDTH         0x00000002
#define SPI_SS_SETUP_MASK          0x00000003
#define SPI_SS_SETUP_LSB           0
#define SPI_SS_SETUP_WIDTH         0x00000002

#define SPI_3WIRE_RST              0x0
#define SPI_MODE_RST               0x0
#define SPI_SCK_FREQ_RST           0x0
#define SPI_MISO_PHASE_RST         0x0
#define SPI_SCK_PHASE_RST          0x0
#define SPI_SCK_POL_RST            0x0
#define SPI_SDO_INACTIVE_ST_RST    0x0
#define SPI_SS_DELAY_RST           0x0
#define SPI_SS_SETUP_RST           0x0

__INLINE void spi_setup_pack(uint8_t spi3wire, uint8_t spimode, uint8_t spisckfreq, uint8_t spimisophase, uint8_t spisckphase, uint8_t spisckpol, uint8_t spisdoinactivest, uint8_t spissdelay, uint8_t spisssetup)
{
    ASSERT_ERR(((spi3wire << 17) & ~0x00020000) == 0);
    ASSERT_ERR(((spimode << 16) & ~0x00010000) == 0);
    ASSERT_ERR(((spisckfreq << 12) & ~0x00007000) == 0);
    ASSERT_ERR(((spimisophase << 10) & ~0x00000400) == 0);
    ASSERT_ERR(((spisckphase << 9) & ~0x00000200) == 0);
    ASSERT_ERR(((spisckpol << 8) & ~0x00000100) == 0);
    ASSERT_ERR(((spisdoinactivest << 4) & ~0x00000030) == 0);
    ASSERT_ERR(((spissdelay << 2) & ~0x0000000C) == 0);
    ASSERT_ERR(((spisssetup << 0) & ~0x00000003) == 0);
    sim_reg_spi_wr(SPI_SETUP_INDEX,  (spi3wire << 17) | (spimode << 16) | (spisckfreq << 12) | (spimisophase << 10) | (spisckphase << 9) | (spisckpol << 8) | (spisdoinactivest << 4) | (spissdelay << 2) | (spisssetup << 0));
}

__INLINE void spi_setup_unpack(uint8_t* spi3wire, uint8_t* spimode, uint8_t* spisckfreq, uint8_t* spimisophase, uint8_t* spisckphase, uint8_t* spisckpol, uint8_t* spisdoinactivest, uint8_t* spissdelay, uint8_t* spisssetup)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_SETUP_INDEX);

    *spi3wire = (localVal & 0x00020000) >> 17;
    *spimode = (localVal & 0x00010000) >> 16;
    *spisckfreq = (localVal & 0x00007000) >> 12;
    *spimisophase = (localVal & 0x00000400) >> 10;
    *spisckphase = (localVal & 0x00000200) >> 9;
    *spisckpol = (localVal & 0x00000100) >> 8;
    *spisdoinactivest = (localVal & 0x00000030) >> 4;
    *spissdelay = (localVal & 0x0000000C) >> 2;
    *spisssetup = (localVal & 0x00000003) >> 0;
}

__INLINE uint8_t spi_3wire_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_SETUP_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE void spi_3wire_setf(uint8_t spi3wire)
{
    ASSERT_ERR(((spi3wire << 17) & ~0x00020000) == 0);
    sim_reg_spi_wr(SPI_SETUP_INDEX, (sim_reg_spi_rd(SPI_SETUP_INDEX) & ~0x00020000) | (spi3wire << 17));
}

__INLINE uint8_t spi_mode_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_SETUP_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE void spi_mode_setf(uint8_t spimode)
{
    ASSERT_ERR(((spimode << 16) & ~0x00010000) == 0);
    sim_reg_spi_wr(SPI_SETUP_INDEX, (sim_reg_spi_rd(SPI_SETUP_INDEX) & ~0x00010000) | (spimode << 16));
}

__INLINE uint8_t spi_sck_freq_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_SETUP_INDEX);
    return ((localVal & 0x00007000) >> 12);
}

__INLINE void spi_sck_freq_setf(uint8_t spisckfreq)
{
    ASSERT_ERR(((spisckfreq << 12) & ~0x00007000) == 0);
    sim_reg_spi_wr(SPI_SETUP_INDEX, (sim_reg_spi_rd(SPI_SETUP_INDEX) & ~0x00007000) | (spisckfreq << 12));
}

__INLINE uint8_t spi_miso_phase_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_SETUP_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE void spi_miso_phase_setf(uint8_t spimisophase)
{
    ASSERT_ERR(((spimisophase << 10) & ~0x00000400) == 0);
    sim_reg_spi_wr(SPI_SETUP_INDEX, (sim_reg_spi_rd(SPI_SETUP_INDEX) & ~0x00000400) | (spimisophase << 10));
}

__INLINE uint8_t spi_sck_phase_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_SETUP_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE void spi_sck_phase_setf(uint8_t spisckphase)
{
    ASSERT_ERR(((spisckphase << 9) & ~0x00000200) == 0);
    sim_reg_spi_wr(SPI_SETUP_INDEX, (sim_reg_spi_rd(SPI_SETUP_INDEX) & ~0x00000200) | (spisckphase << 9));
}

__INLINE uint8_t spi_sck_pol_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_SETUP_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE void spi_sck_pol_setf(uint8_t spisckpol)
{
    ASSERT_ERR(((spisckpol << 8) & ~0x00000100) == 0);
    sim_reg_spi_wr(SPI_SETUP_INDEX, (sim_reg_spi_rd(SPI_SETUP_INDEX) & ~0x00000100) | (spisckpol << 8));
}

__INLINE uint8_t spi_sdo_inactive_st_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_SETUP_INDEX);
    return ((localVal & 0x00000030) >> 4);
}

__INLINE void spi_sdo_inactive_st_setf(uint8_t spisdoinactivest)
{
    ASSERT_ERR(((spisdoinactivest << 4) & ~0x00000030) == 0);
    sim_reg_spi_wr(SPI_SETUP_INDEX, (sim_reg_spi_rd(SPI_SETUP_INDEX) & ~0x00000030) | (spisdoinactivest << 4));
}

__INLINE uint8_t spi_ss_delay_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_SETUP_INDEX);
    return ((localVal & 0x0000000C) >> 2);
}

__INLINE void spi_ss_delay_setf(uint8_t spissdelay)
{
    ASSERT_ERR(((spissdelay << 2) & ~0x0000000C) == 0);
    sim_reg_spi_wr(SPI_SETUP_INDEX, (sim_reg_spi_rd(SPI_SETUP_INDEX) & ~0x0000000C) | (spissdelay << 2));
}

__INLINE uint8_t spi_ss_setup_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_SETUP_INDEX);
    return ((localVal & 0x00000003) >> 0);
}

__INLINE void spi_ss_setup_setf(uint8_t spisssetup)
{
    ASSERT_ERR(((spisssetup << 0) & ~0x00000003) == 0);
    sim_reg_spi_wr(SPI_SETUP_INDEX, (sim_reg_spi_rd(SPI_SETUP_INDEX) & ~0x00000003) | (spisssetup << 0));
}

/**
 * @brief SPI_STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     08       SPI_FIRST_DATA   0
 *     04         SPI_OVERFLOW   0
 *     00              SPI_INT   0
 * </pre>
 */
#define SPI_STATUS_INDEX  0x00000004

__INLINE uint32_t spi_status_get(void)
{
    return sim_reg_spi_rd(SPI_STATUS_INDEX);
}

__INLINE void spi_status_set(uint32_t value)
{
    sim_reg_spi_wr(SPI_STATUS_INDEX, value);
}

// field definitions
#define SPI_FIRST_DATA_BIT    0x00000100
#define SPI_FIRST_DATA_POS    8
#define SPI_OVERFLOW_BIT      0x00000010
#define SPI_OVERFLOW_POS      4
#define SPI_INT_BIT           0x00000001
#define SPI_INT_POS           0

#define SPI_FIRST_DATA_RST    0x0
#define SPI_OVERFLOW_RST      0x0
#define SPI_INT_RST           0x0

__INLINE void spi_status_pack(uint8_t spioverflow, uint8_t spiint)
{
    ASSERT_ERR(((spioverflow << 4) & ~0x00000010) == 0);
    ASSERT_ERR(((spiint << 0) & ~0x00000001) == 0);
    sim_reg_spi_wr(SPI_STATUS_INDEX,  (spioverflow << 4) | (spiint << 0));
}

__INLINE void spi_status_unpack(uint8_t* spifirstdata, uint8_t* spioverflow, uint8_t* spiint)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_STATUS_INDEX);

    *spifirstdata = (localVal & 0x00000100) >> 8;
    *spioverflow = (localVal & 0x00000010) >> 4;
    *spiint = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t spi_first_data_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_STATUS_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE uint8_t spi_overflow_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_STATUS_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE void spi_overflow_setf(uint8_t spioverflow)
{
    ASSERT_ERR(((spioverflow << 4) & ~0x00000010) == 0);
    sim_reg_spi_wr(SPI_STATUS_INDEX, (sim_reg_spi_rd(SPI_STATUS_INDEX) & ~0x00000010) | (spioverflow << 4));
}

__INLINE uint8_t spi_int_getf(void)
{
    uint32_t localVal = sim_reg_spi_rd(SPI_STATUS_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void spi_int_setf(uint8_t spiint)
{
    ASSERT_ERR(((spiint << 0) & ~0x00000001) == 0);
    sim_reg_spi_wr(SPI_STATUS_INDEX, (sim_reg_spi_rd(SPI_STATUS_INDEX) & ~0x00000001) | (spiint << 0));
}


#endif // _REG_SPI_H_

