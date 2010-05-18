#ifndef _REG_UART_H_
#define _REG_UART_H_

#include <stdint.h>
#include "compiler.h"
#include "arch.h"
#include "reg_sim.h"

#define REG_UART_BASE_ADDR 0x80005000

#define REG_UART_COUNT 7

#define REG_UART_DECODING_MASK 0x0000001F

/**
 * @brief UCON register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     15                  TST   0
 *     14                 MRXR   0
 *     13                 MTXR   0
 *     12                  FCE   0
 *     11                  FCP   0
 *     10                 XTIM   0
 *     09                  RES   0
 *     07             TX_OEN_B   0
 *     06                CONTX   0
 *     05                   SB   0
 *     04                  ST2   0
 *     03                   EP   0
 *     02                  PEN   0
 *     01                  RXE   0
 *     00                  TXE   0
 * </pre>
 */
#define UCON_INDEX  0x00000000

__INLINE uint32_t ucon_get(void)
{
    return sim_reg_uart_rd(UCON_INDEX);
}

__INLINE void ucon_set(uint32_t value)
{
    sim_reg_uart_wr(UCON_INDEX, value);
}

// field definitions
#define TST_BIT         0x00008000
#define TST_POS         15
#define MRXR_BIT        0x00004000
#define MRXR_POS        14
#define MTXR_BIT        0x00002000
#define MTXR_POS        13
#define FCE_BIT         0x00001000
#define FCE_POS         12
#define FCP_BIT         0x00000800
#define FCP_POS         11
#define XTIM_BIT        0x00000400
#define XTIM_POS        10
#define RES_BIT         0x00000200
#define RES_POS         9
#define TX_OEN_B_BIT    0x00000080
#define TX_OEN_B_POS    7
#define CONTX_BIT       0x00000040
#define CONTX_POS       6
#define SB_BIT          0x00000020
#define SB_POS          5
#define ST2_BIT         0x00000010
#define ST2_POS         4
#define EP_BIT          0x00000008
#define EP_POS          3
#define PEN_BIT         0x00000004
#define PEN_POS         2
#define RXE_BIT         0x00000002
#define RXE_POS         1
#define TXE_BIT         0x00000001
#define TXE_POS         0

#define TST_RST         0x0
#define MRXR_RST        0x0
#define MTXR_RST        0x0
#define FCE_RST         0x0
#define FCP_RST         0x0
#define XTIM_RST        0x0
#define RES_RST         0x0
#define TX_OEN_B_RST    0x0
#define CONTX_RST       0x0
#define SB_RST          0x0
#define ST2_RST         0x0
#define EP_RST          0x0
#define PEN_RST         0x0
#define RXE_RST         0x0
#define TXE_RST         0x0

__INLINE void ucon_pack(uint8_t tst, uint8_t mrxr, uint8_t mtxr, uint8_t fce, uint8_t fcp, uint8_t xtim, uint8_t res, uint8_t txoenb, uint8_t contx, uint8_t sb, uint8_t st2, uint8_t ep, uint8_t pen, uint8_t rxe, uint8_t txe)
{
    ASSERT_ERR(((tst << 15) & ~0x00008000) == 0);
    ASSERT_ERR(((mrxr << 14) & ~0x00004000) == 0);
    ASSERT_ERR(((mtxr << 13) & ~0x00002000) == 0);
    ASSERT_ERR(((fce << 12) & ~0x00001000) == 0);
    ASSERT_ERR(((fcp << 11) & ~0x00000800) == 0);
    ASSERT_ERR(((xtim << 10) & ~0x00000400) == 0);
    ASSERT_ERR(((res << 9) & ~0x00000200) == 0);
    ASSERT_ERR(((txoenb << 7) & ~0x00000080) == 0);
    ASSERT_ERR(((contx << 6) & ~0x00000040) == 0);
    ASSERT_ERR(((sb << 5) & ~0x00000020) == 0);
    ASSERT_ERR(((st2 << 4) & ~0x00000010) == 0);
    ASSERT_ERR(((ep << 3) & ~0x00000008) == 0);
    ASSERT_ERR(((pen << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((rxe << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((txe << 0) & ~0x00000001) == 0);
    sim_reg_uart_wr(UCON_INDEX,  (tst << 15) | (mrxr << 14) | (mtxr << 13) | (fce << 12) | (fcp << 11) | (xtim << 10) | (res << 9) | (txoenb << 7) | (contx << 6) | (sb << 5) | (st2 << 4) | (ep << 3) | (pen << 2) | (rxe << 1) | (txe << 0));
}

__INLINE void ucon_unpack(uint8_t* tst, uint8_t* mrxr, uint8_t* mtxr, uint8_t* fce, uint8_t* fcp, uint8_t* xtim, uint8_t* res, uint8_t* txoenb, uint8_t* contx, uint8_t* sb, uint8_t* st2, uint8_t* ep, uint8_t* pen, uint8_t* rxe, uint8_t* txe)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);

    *tst = (localVal & 0x00008000) >> 15;
    *mrxr = (localVal & 0x00004000) >> 14;
    *mtxr = (localVal & 0x00002000) >> 13;
    *fce = (localVal & 0x00001000) >> 12;
    *fcp = (localVal & 0x00000800) >> 11;
    *xtim = (localVal & 0x00000400) >> 10;
    *res = (localVal & 0x00000200) >> 9;
    *txoenb = (localVal & 0x00000080) >> 7;
    *contx = (localVal & 0x00000040) >> 6;
    *sb = (localVal & 0x00000020) >> 5;
    *st2 = (localVal & 0x00000010) >> 4;
    *ep = (localVal & 0x00000008) >> 3;
    *pen = (localVal & 0x00000004) >> 2;
    *rxe = (localVal & 0x00000002) >> 1;
    *txe = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t tst_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00008000) >> 15);
}

__INLINE void tst_setf(uint8_t tst)
{
    ASSERT_ERR(((tst << 15) & ~0x00008000) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00008000) | (tst << 15));
}

__INLINE uint8_t mrxr_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00004000) >> 14);
}

__INLINE void mrxr_setf(uint8_t mrxr)
{
    ASSERT_ERR(((mrxr << 14) & ~0x00004000) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00004000) | (mrxr << 14));
}

__INLINE uint8_t mtxr_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00002000) >> 13);
}

__INLINE void mtxr_setf(uint8_t mtxr)
{
    ASSERT_ERR(((mtxr << 13) & ~0x00002000) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00002000) | (mtxr << 13));
}

__INLINE uint8_t fce_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00001000) >> 12);
}

__INLINE void fce_setf(uint8_t fce)
{
    ASSERT_ERR(((fce << 12) & ~0x00001000) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00001000) | (fce << 12));
}

__INLINE uint8_t fcp_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00000800) >> 11);
}

__INLINE void fcp_setf(uint8_t fcp)
{
    ASSERT_ERR(((fcp << 11) & ~0x00000800) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00000800) | (fcp << 11));
}

__INLINE uint8_t xtim_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE void xtim_setf(uint8_t xtim)
{
    ASSERT_ERR(((xtim << 10) & ~0x00000400) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00000400) | (xtim << 10));
}

__INLINE uint8_t res_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE void res_setf(uint8_t res)
{
    ASSERT_ERR(((res << 9) & ~0x00000200) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00000200) | (res << 9));
}

__INLINE uint8_t tx_oen_b_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE void tx_oen_b_setf(uint8_t txoenb)
{
    ASSERT_ERR(((txoenb << 7) & ~0x00000080) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00000080) | (txoenb << 7));
}

__INLINE uint8_t contx_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE void contx_setf(uint8_t contx)
{
    ASSERT_ERR(((contx << 6) & ~0x00000040) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00000040) | (contx << 6));
}

__INLINE uint8_t sb_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE void sb_setf(uint8_t sb)
{
    ASSERT_ERR(((sb << 5) & ~0x00000020) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00000020) | (sb << 5));
}

__INLINE uint8_t st2_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE void st2_setf(uint8_t st2)
{
    ASSERT_ERR(((st2 << 4) & ~0x00000010) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00000010) | (st2 << 4));
}

__INLINE uint8_t ep_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE void ep_setf(uint8_t ep)
{
    ASSERT_ERR(((ep << 3) & ~0x00000008) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00000008) | (ep << 3));
}

__INLINE uint8_t pen_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void pen_setf(uint8_t pen)
{
    ASSERT_ERR(((pen << 2) & ~0x00000004) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00000004) | (pen << 2));
}

__INLINE uint8_t rxe_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void rxe_setf(uint8_t rxe)
{
    ASSERT_ERR(((rxe << 1) & ~0x00000002) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00000002) | (rxe << 1));
}

__INLINE uint8_t txe_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCON_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void txe_setf(uint8_t txe)
{
    ASSERT_ERR(((txe << 0) & ~0x00000001) == 0);
    sim_reg_uart_wr(UCON_INDEX, (sim_reg_uart_rd(UCON_INDEX) & ~0x00000001) | (txe << 0));
}

/**
 * @brief USTAT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     07                TXRDY   0
 *     06                RXRDY   0
 *     05                  RUE   0
 *     04                  ROE   0
 *     03                  TOE   0
 *     02                   FE   0
 *     01                   PE   0
 *     00                   SE   0
 * </pre>
 */
#define USTAT_INDEX  0x00000001

__INLINE uint32_t ustat_get(void)
{
    return sim_reg_uart_rd(USTAT_INDEX);
}

__INLINE void ustat_set(uint32_t value)
{
    sim_reg_uart_wr(USTAT_INDEX, value);
}

// field definitions
#define TXRDY_BIT    0x00000080
#define TXRDY_POS    7
#define RXRDY_BIT    0x00000040
#define RXRDY_POS    6
#define RUE_BIT      0x00000020
#define RUE_POS      5
#define ROE_BIT      0x00000010
#define ROE_POS      4
#define TOE_BIT      0x00000008
#define TOE_POS      3
#define FE_BIT       0x00000004
#define FE_POS       2
#define PE_BIT       0x00000002
#define PE_POS       1
#define SE_BIT       0x00000001
#define SE_POS       0

#define TXRDY_RST    0x0
#define RXRDY_RST    0x0
#define RUE_RST      0x0
#define ROE_RST      0x0
#define TOE_RST      0x0
#define FE_RST       0x0
#define PE_RST       0x0
#define SE_RST       0x0

__INLINE void ustat_unpack(uint8_t* txrdy, uint8_t* rxrdy, uint8_t* rue, uint8_t* roe, uint8_t* toe, uint8_t* fe, uint8_t* pe, uint8_t* se)
{
    uint32_t localVal = sim_reg_uart_rd(USTAT_INDEX);

    *txrdy = (localVal & 0x00000080) >> 7;
    *rxrdy = (localVal & 0x00000040) >> 6;
    *rue = (localVal & 0x00000020) >> 5;
    *roe = (localVal & 0x00000010) >> 4;
    *toe = (localVal & 0x00000008) >> 3;
    *fe = (localVal & 0x00000004) >> 2;
    *pe = (localVal & 0x00000002) >> 1;
    *se = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t txrdy_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(USTAT_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE uint8_t rxrdy_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(USTAT_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE uint8_t rue_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(USTAT_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE uint8_t roe_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(USTAT_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE uint8_t toe_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(USTAT_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE uint8_t fe_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(USTAT_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE uint8_t pe_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(USTAT_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE uint8_t se_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(USTAT_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

/**
 * @brief UDATA register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  07:00                UDATA   0x0
 * </pre>
 */
#define UDATA_INDEX  0x00000002

__INLINE uint32_t udata_get(void)
{
    return sim_reg_uart_rd(UDATA_INDEX);
}

__INLINE void udata_set(uint32_t value)
{
    sim_reg_uart_wr(UDATA_INDEX, value);
}

// field definitions
#define UDATA_MASK   0x000000FF
#define UDATA_LSB    0
#define UDATA_WIDTH  0x00000008

#define UDATA_RST    0x0

__INLINE uint8_t udata_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UDATA_INDEX);
    ASSERT_ERR((localVal & ~0x000000FF) == 0);
    return (localVal >> 0);
}

__INLINE void udata_setf(uint8_t udata)
{
    ASSERT_ERR(((udata << 0) & ~0x000000FF) == 0);
    sim_reg_uart_wr(UDATA_INDEX, udata << 0);
}

/**
 * @brief URXCON register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  05:00              RXLEVEL   0x0
 * </pre>
 */
#define URXCON_INDEX  0x00000003

__INLINE uint32_t urxcon_get(void)
{
    return sim_reg_uart_rd(URXCON_INDEX);
}

__INLINE void urxcon_set(uint32_t value)
{
    sim_reg_uart_wr(URXCON_INDEX, value);
}

// field definitions
#define RXLEVEL_MASK   0x0000003F
#define RXLEVEL_LSB    0
#define RXLEVEL_WIDTH  0x00000006

#define RXLEVEL_RST    0x0

__INLINE uint8_t rxlevel_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(URXCON_INDEX);
    ASSERT_ERR((localVal & ~0x0000003F) == 0);
    return (localVal >> 0);
}

__INLINE void rxlevel_setf(uint8_t rxlevel)
{
    ASSERT_ERR(((rxlevel << 0) & ~0x0000003F) == 0);
    sim_reg_uart_wr(URXCON_INDEX, rxlevel << 0);
}

/**
 * @brief UTXCON register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  05:00              TXLEVEL   0x0
 * </pre>
 */
#define UTXCON_INDEX  0x00000004

__INLINE uint32_t utxcon_get(void)
{
    return sim_reg_uart_rd(UTXCON_INDEX);
}

__INLINE void utxcon_set(uint32_t value)
{
    sim_reg_uart_wr(UTXCON_INDEX, value);
}

// field definitions
#define TXLEVEL_MASK   0x0000003F
#define TXLEVEL_LSB    0
#define TXLEVEL_WIDTH  0x00000006

#define TXLEVEL_RST    0x0

__INLINE uint8_t txlevel_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UTXCON_INDEX);
    ASSERT_ERR((localVal & ~0x0000003F) == 0);
    return (localVal >> 0);
}

__INLINE void txlevel_setf(uint8_t txlevel)
{
    ASSERT_ERR(((txlevel << 0) & ~0x0000003F) == 0);
    sim_reg_uart_wr(UTXCON_INDEX, txlevel << 0);
}

/**
 * @brief UCTS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  04:00            CTS_LEVEL   0x0
 * </pre>
 */
#define UCTS_INDEX  0x00000005

__INLINE uint32_t ucts_get(void)
{
    return sim_reg_uart_rd(UCTS_INDEX);
}

__INLINE void ucts_set(uint32_t value)
{
    sim_reg_uart_wr(UCTS_INDEX, value);
}

// field definitions
#define CTS_LEVEL_MASK   0x0000001F
#define CTS_LEVEL_LSB    0
#define CTS_LEVEL_WIDTH  0x00000005

#define CTS_LEVEL_RST    0x0

__INLINE uint8_t cts_level_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UCTS_INDEX);
    ASSERT_ERR((localVal & ~0x0000001F) == 0);
    return (localVal >> 0);
}

__INLINE void cts_level_setf(uint8_t ctslevel)
{
    ASSERT_ERR(((ctslevel << 0) & ~0x0000001F) == 0);
    sim_reg_uart_wr(UCTS_INDEX, ctslevel << 0);
}

/**
 * @brief UBR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16               UBRINC   0x0
 *  15:00               UBRMOD   0x0
 * </pre>
 */
#define UBR_INDEX  0x00000006

__INLINE uint32_t ubr_get(void)
{
    return sim_reg_uart_rd(UBR_INDEX);
}

__INLINE void ubr_set(uint32_t value)
{
    sim_reg_uart_wr(UBR_INDEX, value);
}

// field definitions
#define UBRINC_MASK   0xFFFF0000
#define UBRINC_LSB    16
#define UBRINC_WIDTH  0x00000010
#define UBRMOD_MASK   0x0000FFFF
#define UBRMOD_LSB    0
#define UBRMOD_WIDTH  0x00000010

#define UBRINC_RST    0x0
#define UBRMOD_RST    0x0

__INLINE void ubr_pack(uint16_t ubrinc, uint16_t ubrmod)
{
    ASSERT_ERR(((ubrinc << 16) & ~0xFFFF0000) == 0);
    ASSERT_ERR(((ubrmod << 0) & ~0x0000FFFF) == 0);
    sim_reg_uart_wr(UBR_INDEX,  (ubrinc << 16) | (ubrmod << 0));
}

__INLINE void ubr_unpack(uint16_t* ubrinc, uint16_t* ubrmod)
{
    uint32_t localVal = sim_reg_uart_rd(UBR_INDEX);

    *ubrinc = (localVal & 0xFFFF0000) >> 16;
    *ubrmod = (localVal & 0x0000FFFF) >> 0;
}

__INLINE uint16_t ubrinc_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UBR_INDEX);
    return ((localVal & 0xFFFF0000) >> 16);
}

__INLINE void ubrinc_setf(uint16_t ubrinc)
{
    ASSERT_ERR(((ubrinc << 16) & ~0xFFFF0000) == 0);
    sim_reg_uart_wr(UBR_INDEX, (sim_reg_uart_rd(UBR_INDEX) & ~0xFFFF0000) | (ubrinc << 16));
}

__INLINE uint16_t ubrmod_getf(void)
{
    uint32_t localVal = sim_reg_uart_rd(UBR_INDEX);
    return ((localVal & 0x0000FFFF) >> 0);
}

__INLINE void ubrmod_setf(uint16_t ubrmod)
{
    ASSERT_ERR(((ubrmod << 0) & ~0x0000FFFF) == 0);
    sim_reg_uart_wr(UBR_INDEX, (sim_reg_uart_rd(UBR_INDEX) & ~0x0000FFFF) | (ubrmod << 0));
}


#endif // _REG_UART_H_

