#ifndef _REG_AIC_H_
#define _REG_AIC_H_

#include <stdint.h>
#include "compiler.h"
#include "arch.h"
#include "reg_sim.h"

#define REG_AIC_BASE_ADDR 0xFFFFF000

#define REG_AIC_COUNT 128

#define REG_AIC_DECODING_MASK 0x000001FF

/**
 * @brief AIC_SMR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  06:05              SRCTYPE   0x0
 *  02:00                PRIOR   0x0
 * </pre>
 */
#define AIC_SMR_INDEX  0x00000000
#define AIC_SMR_COUNT  32

__INLINE uint32_t aic_smr_get(uint32_t idx)
{
    ASSERT_ERR(idx <= 31);
    return sim_reg_aic_rd(AIC_SMR_INDEX + idx);
}

__INLINE void aic_smr_set(uint32_t idx, uint32_t value)
{
    ASSERT_ERR(idx <= 31);
    sim_reg_aic_wr(AIC_SMR_INDEX + idx, value);
}

// field definitions
#define SRCTYPE_MASK   0x00000060
#define SRCTYPE_LSB    5
#define SRCTYPE_WIDTH  0x00000002
#define PRIOR_MASK     0x00000007
#define PRIOR_LSB      0
#define PRIOR_WIDTH    0x00000003

#define SRCTYPE_RST    0x0
#define PRIOR_RST      0x0

__INLINE void aic_smr_pack(uint32_t idx, uint8_t srctype, uint8_t prior)
{
    ASSERT_ERR(idx <= 31);
    ASSERT_ERR(((srctype << 5) & ~0x00000060) == 0);
    ASSERT_ERR(((prior << 0) & ~0x00000007) == 0);
    sim_reg_aic_wr(AIC_SMR_INDEX + idx,  (srctype << 5) | (prior << 0));
}

__INLINE void aic_smr_unpack(uint32_t idx, uint8_t* srctype, uint8_t* prior)
{
    ASSERT_ERR(idx <= 31);
    uint32_t localVal = sim_reg_aic_rd(AIC_SMR_INDEX + idx);

    *srctype = (localVal & 0x00000060) >> 5;
    *prior = (localVal & 0x00000007) >> 0;
}

__INLINE uint8_t aic_smr_srctype_getf(uint32_t idx)
{
    ASSERT_ERR(idx <= 31);
    uint32_t localVal = sim_reg_aic_rd(AIC_SMR_INDEX + idx);
    return ((localVal & 0x00000060) >> 5);
}

__INLINE void aic_smr_srctype_setf(uint32_t idx, uint8_t srctype)
{
    ASSERT_ERR(idx <= 31);
    ASSERT_ERR(((srctype << 5) & ~0x00000060) == 0);
    sim_reg_aic_wr(AIC_SMR_INDEX + idx, (sim_reg_aic_rd(AIC_SMR_INDEX + idx) & ~0x00000060) | (srctype << 5));
}

__INLINE uint8_t aic_smr_prior_getf(uint32_t idx)
{
    ASSERT_ERR(idx <= 31);
    uint32_t localVal = sim_reg_aic_rd(AIC_SMR_INDEX + idx);
    return ((localVal & 0x00000007) >> 0);
}

__INLINE void aic_smr_prior_setf(uint32_t idx, uint8_t prior)
{
    ASSERT_ERR(idx <= 31);
    ASSERT_ERR(((prior << 0) & ~0x00000007) == 0);
    sim_reg_aic_wr(AIC_SMR_INDEX + idx, (sim_reg_aic_rd(AIC_SMR_INDEX + idx) & ~0x00000007) | (prior << 0));
}

/**
 * @brief AIC_SVR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00               VECTOR   0x0
 * </pre>
 */
#define AIC_SVR_INDEX  0x00000020
#define AIC_SVR_COUNT  32

__INLINE uint32_t aic_svr_get(uint32_t idx)
{
    ASSERT_ERR(idx <= 31);
    return sim_reg_aic_rd(AIC_SVR_INDEX + idx);
}

__INLINE void aic_svr_set(uint32_t idx, uint32_t value)
{
    ASSERT_ERR(idx <= 31);
    sim_reg_aic_wr(AIC_SVR_INDEX + idx, value);
}

// field definitions
#define VECTOR_MASK   0xFFFFFFFF
#define VECTOR_LSB    0
#define VECTOR_WIDTH  0x00000020

#define VECTOR_RST    0x0

__INLINE uint32_t aic_svr_vector_getf(uint32_t idx)
{
    ASSERT_ERR(idx <= 31);
    uint32_t localVal = sim_reg_aic_rd(AIC_SVR_INDEX + idx);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

__INLINE void aic_svr_vector_setf(uint32_t idx, uint32_t vector)
{
    ASSERT_ERR(idx <= 31);
    ASSERT_ERR(((vector << 0) & ~0xFFFFFFFF) == 0);
    sim_reg_aic_wr(AIC_SVR_INDEX + idx, vector << 0);
}

/**
 * @brief AIC_IVR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00                 IRQV   0x0
 * </pre>
 */
#define AIC_IVR_INDEX  0x00000040

__INLINE uint32_t aic_ivr_get(void)
{
    return sim_reg_aic_rd(AIC_IVR_INDEX);
}

__INLINE void aic_ivr_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_IVR_INDEX, value);
}

// field definitions
#define IRQV_MASK   0xFFFFFFFF
#define IRQV_LSB    0
#define IRQV_WIDTH  0x00000020

#define IRQV_RST    0x0

__INLINE uint32_t aic_ivr_irqv_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IVR_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

/**
 * @brief AIC_FVR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00                 FIQV   0x0
 * </pre>
 */
#define AIC_FVR_INDEX  0x00000041

__INLINE uint32_t aic_fvr_get(void)
{
    return sim_reg_aic_rd(AIC_FVR_INDEX);
}

__INLINE void aic_fvr_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_FVR_INDEX, value);
}

// field definitions
#define FIQV_MASK   0xFFFFFFFF
#define FIQV_LSB    0
#define FIQV_WIDTH  0x00000020

#define FIQV_RST    0x0

__INLINE uint32_t aic_fvr_fiqv_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FVR_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

/**
 * @brief AIC_ISR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  04:00                IRQID   0x0
 * </pre>
 */
#define AIC_ISR_INDEX  0x00000042

__INLINE uint32_t aic_isr_get(void)
{
    return sim_reg_aic_rd(AIC_ISR_INDEX);
}

__INLINE void aic_isr_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_ISR_INDEX, value);
}

// field definitions
#define IRQID_MASK   0x0000001F
#define IRQID_LSB    0
#define IRQID_WIDTH  0x00000005

#define IRQID_RST    0x0

__INLINE uint8_t aic_isr_irqid_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISR_INDEX);
    ASSERT_ERR((localVal & ~0x0000001F) == 0);
    return (localVal >> 0);
}

/**
 * @brief AIC_IPR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31                PID31   0
 *     30                PID30   0
 *     29                PID29   0
 *     28                PID28   0
 *     27                PID27   0
 *     26                PID26   0
 *     25                PID25   0
 *     24                PID24   0
 *     23                PID23   0
 *     22                PID22   0
 *     21                PID21   0
 *     20                PID20   0
 *     19                PID19   0
 *     18                PID18   0
 *     17                PID17   0
 *     16                PID16   0
 *     15                PID15   0
 *     14                PID14   0
 *     13                PID13   0
 *     12                PID12   0
 *     11                PID11   0
 *     10                PID10   0
 *     09                 PID9   0
 *     08                 PID8   0
 *     07                 PID7   0
 *     06                 PID6   0
 *     05                 PID5   0
 *     04                 PID4   0
 *     03                 PID3   0
 *     02                 PID2   0
 *     01                  SYS   0
 *     00                  FIQ   0
 * </pre>
 */
#define AIC_IPR_INDEX  0x00000043

__INLINE uint32_t aic_ipr_get(void)
{
    return sim_reg_aic_rd(AIC_IPR_INDEX);
}

__INLINE void aic_ipr_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_IPR_INDEX, value);
}

// field definitions
#define PID31_BIT    0x80000000
#define PID31_POS    31
#define PID30_BIT    0x40000000
#define PID30_POS    30
#define PID29_BIT    0x20000000
#define PID29_POS    29
#define PID28_BIT    0x10000000
#define PID28_POS    28
#define PID27_BIT    0x08000000
#define PID27_POS    27
#define PID26_BIT    0x04000000
#define PID26_POS    26
#define PID25_BIT    0x02000000
#define PID25_POS    25
#define PID24_BIT    0x01000000
#define PID24_POS    24
#define PID23_BIT    0x00800000
#define PID23_POS    23
#define PID22_BIT    0x00400000
#define PID22_POS    22
#define PID21_BIT    0x00200000
#define PID21_POS    21
#define PID20_BIT    0x00100000
#define PID20_POS    20
#define PID19_BIT    0x00080000
#define PID19_POS    19
#define PID18_BIT    0x00040000
#define PID18_POS    18
#define PID17_BIT    0x00020000
#define PID17_POS    17
#define PID16_BIT    0x00010000
#define PID16_POS    16
#define PID15_BIT    0x00008000
#define PID15_POS    15
#define PID14_BIT    0x00004000
#define PID14_POS    14
#define PID13_BIT    0x00002000
#define PID13_POS    13
#define PID12_BIT    0x00001000
#define PID12_POS    12
#define PID11_BIT    0x00000800
#define PID11_POS    11
#define PID10_BIT    0x00000400
#define PID10_POS    10
#define PID9_BIT     0x00000200
#define PID9_POS     9
#define PID8_BIT     0x00000100
#define PID8_POS     8
#define PID7_BIT     0x00000080
#define PID7_POS     7
#define PID6_BIT     0x00000040
#define PID6_POS     6
#define PID5_BIT     0x00000020
#define PID5_POS     5
#define PID4_BIT     0x00000010
#define PID4_POS     4
#define PID3_BIT     0x00000008
#define PID3_POS     3
#define PID2_BIT     0x00000004
#define PID2_POS     2
#define SYS_BIT      0x00000002
#define SYS_POS      1
#define FIQ_BIT      0x00000001
#define FIQ_POS      0

#define PID31_RST    0x0
#define PID30_RST    0x0
#define PID29_RST    0x0
#define PID28_RST    0x0
#define PID27_RST    0x0
#define PID26_RST    0x0
#define PID25_RST    0x0
#define PID24_RST    0x0
#define PID23_RST    0x0
#define PID22_RST    0x0
#define PID21_RST    0x0
#define PID20_RST    0x0
#define PID19_RST    0x0
#define PID18_RST    0x0
#define PID17_RST    0x0
#define PID16_RST    0x0
#define PID15_RST    0x0
#define PID14_RST    0x0
#define PID13_RST    0x0
#define PID12_RST    0x0
#define PID11_RST    0x0
#define PID10_RST    0x0
#define PID9_RST     0x0
#define PID8_RST     0x0
#define PID7_RST     0x0
#define PID6_RST     0x0
#define PID5_RST     0x0
#define PID4_RST     0x0
#define PID3_RST     0x0
#define PID2_RST     0x0
#define SYS_RST      0x0
#define FIQ_RST      0x0

__INLINE void aic_ipr_unpack(uint8_t* pid31, uint8_t* pid30, uint8_t* pid29, uint8_t* pid28, uint8_t* pid27, uint8_t* pid26, uint8_t* pid25, uint8_t* pid24, uint8_t* pid23, uint8_t* pid22, uint8_t* pid21, uint8_t* pid20, uint8_t* pid19, uint8_t* pid18, uint8_t* pid17, uint8_t* pid16, uint8_t* pid15, uint8_t* pid14, uint8_t* pid13, uint8_t* pid12, uint8_t* pid11, uint8_t* pid10, uint8_t* pid9, uint8_t* pid8, uint8_t* pid7, uint8_t* pid6, uint8_t* pid5, uint8_t* pid4, uint8_t* pid3, uint8_t* pid2, uint8_t* sys, uint8_t* fiq)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);

    *pid31 = (localVal & 0x80000000) >> 31;
    *pid30 = (localVal & 0x40000000) >> 30;
    *pid29 = (localVal & 0x20000000) >> 29;
    *pid28 = (localVal & 0x10000000) >> 28;
    *pid27 = (localVal & 0x08000000) >> 27;
    *pid26 = (localVal & 0x04000000) >> 26;
    *pid25 = (localVal & 0x02000000) >> 25;
    *pid24 = (localVal & 0x01000000) >> 24;
    *pid23 = (localVal & 0x00800000) >> 23;
    *pid22 = (localVal & 0x00400000) >> 22;
    *pid21 = (localVal & 0x00200000) >> 21;
    *pid20 = (localVal & 0x00100000) >> 20;
    *pid19 = (localVal & 0x00080000) >> 19;
    *pid18 = (localVal & 0x00040000) >> 18;
    *pid17 = (localVal & 0x00020000) >> 17;
    *pid16 = (localVal & 0x00010000) >> 16;
    *pid15 = (localVal & 0x00008000) >> 15;
    *pid14 = (localVal & 0x00004000) >> 14;
    *pid13 = (localVal & 0x00002000) >> 13;
    *pid12 = (localVal & 0x00001000) >> 12;
    *pid11 = (localVal & 0x00000800) >> 11;
    *pid10 = (localVal & 0x00000400) >> 10;
    *pid9 = (localVal & 0x00000200) >> 9;
    *pid8 = (localVal & 0x00000100) >> 8;
    *pid7 = (localVal & 0x00000080) >> 7;
    *pid6 = (localVal & 0x00000040) >> 6;
    *pid5 = (localVal & 0x00000020) >> 5;
    *pid4 = (localVal & 0x00000010) >> 4;
    *pid3 = (localVal & 0x00000008) >> 3;
    *pid2 = (localVal & 0x00000004) >> 2;
    *sys = (localVal & 0x00000002) >> 1;
    *fiq = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t aic_ipr_pid31_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x80000000) >> 31);
}

__INLINE uint8_t aic_ipr_pid30_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x40000000) >> 30);
}

__INLINE uint8_t aic_ipr_pid29_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x20000000) >> 29);
}

__INLINE uint8_t aic_ipr_pid28_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x10000000) >> 28);
}

__INLINE uint8_t aic_ipr_pid27_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x08000000) >> 27);
}

__INLINE uint8_t aic_ipr_pid26_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x04000000) >> 26);
}

__INLINE uint8_t aic_ipr_pid25_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x02000000) >> 25);
}

__INLINE uint8_t aic_ipr_pid24_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x01000000) >> 24);
}

__INLINE uint8_t aic_ipr_pid23_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00800000) >> 23);
}

__INLINE uint8_t aic_ipr_pid22_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00400000) >> 22);
}

__INLINE uint8_t aic_ipr_pid21_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00200000) >> 21);
}

__INLINE uint8_t aic_ipr_pid20_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00100000) >> 20);
}

__INLINE uint8_t aic_ipr_pid19_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00080000) >> 19);
}

__INLINE uint8_t aic_ipr_pid18_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00040000) >> 18);
}

__INLINE uint8_t aic_ipr_pid17_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE uint8_t aic_ipr_pid16_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE uint8_t aic_ipr_pid15_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00008000) >> 15);
}

__INLINE uint8_t aic_ipr_pid14_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00004000) >> 14);
}

__INLINE uint8_t aic_ipr_pid13_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00002000) >> 13);
}

__INLINE uint8_t aic_ipr_pid12_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00001000) >> 12);
}

__INLINE uint8_t aic_ipr_pid11_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000800) >> 11);
}

__INLINE uint8_t aic_ipr_pid10_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE uint8_t aic_ipr_pid9_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE uint8_t aic_ipr_pid8_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE uint8_t aic_ipr_pid7_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE uint8_t aic_ipr_pid6_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE uint8_t aic_ipr_pid5_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE uint8_t aic_ipr_pid4_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE uint8_t aic_ipr_pid3_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE uint8_t aic_ipr_pid2_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE uint8_t aic_ipr_sys_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE uint8_t aic_ipr_fiq_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IPR_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

/**
 * @brief AIC_IMR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31                PID31   0
 *     30                PID30   0
 *     29                PID29   0
 *     28                PID28   0
 *     27                PID27   0
 *     26                PID26   0
 *     25                PID25   0
 *     24                PID24   0
 *     23                PID23   0
 *     22                PID22   0
 *     21                PID21   0
 *     20                PID20   0
 *     19                PID19   0
 *     18                PID18   0
 *     17                PID17   0
 *     16                PID16   0
 *     15                PID15   0
 *     14                PID14   0
 *     13                PID13   0
 *     12                PID12   0
 *     11                PID11   0
 *     10                PID10   0
 *     09                 PID9   0
 *     08                 PID8   0
 *     07                 PID7   0
 *     06                 PID6   0
 *     05                 PID5   0
 *     04                 PID4   0
 *     03                 PID3   0
 *     02                 PID2   0
 *     01                  SYS   0
 *     00                  FIQ   0
 * </pre>
 */
#define AIC_IMR_INDEX  0x00000044

__INLINE uint32_t aic_imr_get(void)
{
    return sim_reg_aic_rd(AIC_IMR_INDEX);
}

__INLINE void aic_imr_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_IMR_INDEX, value);
}

// field definitions
#define PID31_BIT    0x80000000
#define PID31_POS    31
#define PID30_BIT    0x40000000
#define PID30_POS    30
#define PID29_BIT    0x20000000
#define PID29_POS    29
#define PID28_BIT    0x10000000
#define PID28_POS    28
#define PID27_BIT    0x08000000
#define PID27_POS    27
#define PID26_BIT    0x04000000
#define PID26_POS    26
#define PID25_BIT    0x02000000
#define PID25_POS    25
#define PID24_BIT    0x01000000
#define PID24_POS    24
#define PID23_BIT    0x00800000
#define PID23_POS    23
#define PID22_BIT    0x00400000
#define PID22_POS    22
#define PID21_BIT    0x00200000
#define PID21_POS    21
#define PID20_BIT    0x00100000
#define PID20_POS    20
#define PID19_BIT    0x00080000
#define PID19_POS    19
#define PID18_BIT    0x00040000
#define PID18_POS    18
#define PID17_BIT    0x00020000
#define PID17_POS    17
#define PID16_BIT    0x00010000
#define PID16_POS    16
#define PID15_BIT    0x00008000
#define PID15_POS    15
#define PID14_BIT    0x00004000
#define PID14_POS    14
#define PID13_BIT    0x00002000
#define PID13_POS    13
#define PID12_BIT    0x00001000
#define PID12_POS    12
#define PID11_BIT    0x00000800
#define PID11_POS    11
#define PID10_BIT    0x00000400
#define PID10_POS    10
#define PID9_BIT     0x00000200
#define PID9_POS     9
#define PID8_BIT     0x00000100
#define PID8_POS     8
#define PID7_BIT     0x00000080
#define PID7_POS     7
#define PID6_BIT     0x00000040
#define PID6_POS     6
#define PID5_BIT     0x00000020
#define PID5_POS     5
#define PID4_BIT     0x00000010
#define PID4_POS     4
#define PID3_BIT     0x00000008
#define PID3_POS     3
#define PID2_BIT     0x00000004
#define PID2_POS     2
#define SYS_BIT      0x00000002
#define SYS_POS      1
#define FIQ_BIT      0x00000001
#define FIQ_POS      0

#define PID31_RST    0x0
#define PID30_RST    0x0
#define PID29_RST    0x0
#define PID28_RST    0x0
#define PID27_RST    0x0
#define PID26_RST    0x0
#define PID25_RST    0x0
#define PID24_RST    0x0
#define PID23_RST    0x0
#define PID22_RST    0x0
#define PID21_RST    0x0
#define PID20_RST    0x0
#define PID19_RST    0x0
#define PID18_RST    0x0
#define PID17_RST    0x0
#define PID16_RST    0x0
#define PID15_RST    0x0
#define PID14_RST    0x0
#define PID13_RST    0x0
#define PID12_RST    0x0
#define PID11_RST    0x0
#define PID10_RST    0x0
#define PID9_RST     0x0
#define PID8_RST     0x0
#define PID7_RST     0x0
#define PID6_RST     0x0
#define PID5_RST     0x0
#define PID4_RST     0x0
#define PID3_RST     0x0
#define PID2_RST     0x0
#define SYS_RST      0x0
#define FIQ_RST      0x0

__INLINE void aic_imr_unpack(uint8_t* pid31, uint8_t* pid30, uint8_t* pid29, uint8_t* pid28, uint8_t* pid27, uint8_t* pid26, uint8_t* pid25, uint8_t* pid24, uint8_t* pid23, uint8_t* pid22, uint8_t* pid21, uint8_t* pid20, uint8_t* pid19, uint8_t* pid18, uint8_t* pid17, uint8_t* pid16, uint8_t* pid15, uint8_t* pid14, uint8_t* pid13, uint8_t* pid12, uint8_t* pid11, uint8_t* pid10, uint8_t* pid9, uint8_t* pid8, uint8_t* pid7, uint8_t* pid6, uint8_t* pid5, uint8_t* pid4, uint8_t* pid3, uint8_t* pid2, uint8_t* sys, uint8_t* fiq)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);

    *pid31 = (localVal & 0x80000000) >> 31;
    *pid30 = (localVal & 0x40000000) >> 30;
    *pid29 = (localVal & 0x20000000) >> 29;
    *pid28 = (localVal & 0x10000000) >> 28;
    *pid27 = (localVal & 0x08000000) >> 27;
    *pid26 = (localVal & 0x04000000) >> 26;
    *pid25 = (localVal & 0x02000000) >> 25;
    *pid24 = (localVal & 0x01000000) >> 24;
    *pid23 = (localVal & 0x00800000) >> 23;
    *pid22 = (localVal & 0x00400000) >> 22;
    *pid21 = (localVal & 0x00200000) >> 21;
    *pid20 = (localVal & 0x00100000) >> 20;
    *pid19 = (localVal & 0x00080000) >> 19;
    *pid18 = (localVal & 0x00040000) >> 18;
    *pid17 = (localVal & 0x00020000) >> 17;
    *pid16 = (localVal & 0x00010000) >> 16;
    *pid15 = (localVal & 0x00008000) >> 15;
    *pid14 = (localVal & 0x00004000) >> 14;
    *pid13 = (localVal & 0x00002000) >> 13;
    *pid12 = (localVal & 0x00001000) >> 12;
    *pid11 = (localVal & 0x00000800) >> 11;
    *pid10 = (localVal & 0x00000400) >> 10;
    *pid9 = (localVal & 0x00000200) >> 9;
    *pid8 = (localVal & 0x00000100) >> 8;
    *pid7 = (localVal & 0x00000080) >> 7;
    *pid6 = (localVal & 0x00000040) >> 6;
    *pid5 = (localVal & 0x00000020) >> 5;
    *pid4 = (localVal & 0x00000010) >> 4;
    *pid3 = (localVal & 0x00000008) >> 3;
    *pid2 = (localVal & 0x00000004) >> 2;
    *sys = (localVal & 0x00000002) >> 1;
    *fiq = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t aic_imr_pid31_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x80000000) >> 31);
}

__INLINE uint8_t aic_imr_pid30_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x40000000) >> 30);
}

__INLINE uint8_t aic_imr_pid29_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x20000000) >> 29);
}

__INLINE uint8_t aic_imr_pid28_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x10000000) >> 28);
}

__INLINE uint8_t aic_imr_pid27_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x08000000) >> 27);
}

__INLINE uint8_t aic_imr_pid26_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x04000000) >> 26);
}

__INLINE uint8_t aic_imr_pid25_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x02000000) >> 25);
}

__INLINE uint8_t aic_imr_pid24_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x01000000) >> 24);
}

__INLINE uint8_t aic_imr_pid23_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00800000) >> 23);
}

__INLINE uint8_t aic_imr_pid22_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00400000) >> 22);
}

__INLINE uint8_t aic_imr_pid21_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00200000) >> 21);
}

__INLINE uint8_t aic_imr_pid20_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00100000) >> 20);
}

__INLINE uint8_t aic_imr_pid19_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00080000) >> 19);
}

__INLINE uint8_t aic_imr_pid18_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00040000) >> 18);
}

__INLINE uint8_t aic_imr_pid17_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE uint8_t aic_imr_pid16_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE uint8_t aic_imr_pid15_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00008000) >> 15);
}

__INLINE uint8_t aic_imr_pid14_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00004000) >> 14);
}

__INLINE uint8_t aic_imr_pid13_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00002000) >> 13);
}

__INLINE uint8_t aic_imr_pid12_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00001000) >> 12);
}

__INLINE uint8_t aic_imr_pid11_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000800) >> 11);
}

__INLINE uint8_t aic_imr_pid10_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE uint8_t aic_imr_pid9_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE uint8_t aic_imr_pid8_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE uint8_t aic_imr_pid7_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE uint8_t aic_imr_pid6_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE uint8_t aic_imr_pid5_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE uint8_t aic_imr_pid4_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE uint8_t aic_imr_pid3_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE uint8_t aic_imr_pid2_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE uint8_t aic_imr_sys_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE uint8_t aic_imr_fiq_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IMR_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

/**
 * @brief AIC_CISR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     01                 NIRQ   0
 *     00                 NFIQ   0
 * </pre>
 */
#define AIC_CISR_INDEX  0x00000045

__INLINE uint32_t aic_cisr_get(void)
{
    return sim_reg_aic_rd(AIC_CISR_INDEX);
}

__INLINE void aic_cisr_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_CISR_INDEX, value);
}

// field definitions
#define NIRQ_BIT    0x00000002
#define NIRQ_POS    1
#define NFIQ_BIT    0x00000001
#define NFIQ_POS    0

#define NIRQ_RST    0x0
#define NFIQ_RST    0x0

__INLINE void aic_cisr_unpack(uint8_t* nirq, uint8_t* nfiq)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_CISR_INDEX);

    *nirq = (localVal & 0x00000002) >> 1;
    *nfiq = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t aic_cisr_nirq_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_CISR_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE uint8_t aic_cisr_nfiq_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_CISR_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

/**
 * @brief AIC_IECR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31                PID31   0
 *     30                PID30   0
 *     29                PID29   0
 *     28                PID28   0
 *     27                PID27   0
 *     26                PID26   0
 *     25                PID25   0
 *     24                PID24   0
 *     23                PID23   0
 *     22                PID22   0
 *     21                PID21   0
 *     20                PID20   0
 *     19                PID19   0
 *     18                PID18   0
 *     17                PID17   0
 *     16                PID16   0
 *     15                PID15   0
 *     14                PID14   0
 *     13                PID13   0
 *     12                PID12   0
 *     11                PID11   0
 *     10                PID10   0
 *     09                 PID9   0
 *     08                 PID8   0
 *     07                 PID7   0
 *     06                 PID6   0
 *     05                 PID5   0
 *     04                 PID4   0
 *     03                 PID3   0
 *     02                 PID2   0
 *     01                  SYS   0
 *     00                  FIQ   0
 * </pre>
 */
#define AIC_IECR_INDEX  0x00000048

__INLINE uint32_t aic_iecr_get(void)
{
    return sim_reg_aic_rd(AIC_IECR_INDEX);
}

__INLINE void aic_iecr_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_IECR_INDEX, value);
}

// field definitions
#define PID31_BIT    0x80000000
#define PID31_POS    31
#define PID30_BIT    0x40000000
#define PID30_POS    30
#define PID29_BIT    0x20000000
#define PID29_POS    29
#define PID28_BIT    0x10000000
#define PID28_POS    28
#define PID27_BIT    0x08000000
#define PID27_POS    27
#define PID26_BIT    0x04000000
#define PID26_POS    26
#define PID25_BIT    0x02000000
#define PID25_POS    25
#define PID24_BIT    0x01000000
#define PID24_POS    24
#define PID23_BIT    0x00800000
#define PID23_POS    23
#define PID22_BIT    0x00400000
#define PID22_POS    22
#define PID21_BIT    0x00200000
#define PID21_POS    21
#define PID20_BIT    0x00100000
#define PID20_POS    20
#define PID19_BIT    0x00080000
#define PID19_POS    19
#define PID18_BIT    0x00040000
#define PID18_POS    18
#define PID17_BIT    0x00020000
#define PID17_POS    17
#define PID16_BIT    0x00010000
#define PID16_POS    16
#define PID15_BIT    0x00008000
#define PID15_POS    15
#define PID14_BIT    0x00004000
#define PID14_POS    14
#define PID13_BIT    0x00002000
#define PID13_POS    13
#define PID12_BIT    0x00001000
#define PID12_POS    12
#define PID11_BIT    0x00000800
#define PID11_POS    11
#define PID10_BIT    0x00000400
#define PID10_POS    10
#define PID9_BIT     0x00000200
#define PID9_POS     9
#define PID8_BIT     0x00000100
#define PID8_POS     8
#define PID7_BIT     0x00000080
#define PID7_POS     7
#define PID6_BIT     0x00000040
#define PID6_POS     6
#define PID5_BIT     0x00000020
#define PID5_POS     5
#define PID4_BIT     0x00000010
#define PID4_POS     4
#define PID3_BIT     0x00000008
#define PID3_POS     3
#define PID2_BIT     0x00000004
#define PID2_POS     2
#define SYS_BIT      0x00000002
#define SYS_POS      1
#define FIQ_BIT      0x00000001
#define FIQ_POS      0

#define PID31_RST    0x0
#define PID30_RST    0x0
#define PID29_RST    0x0
#define PID28_RST    0x0
#define PID27_RST    0x0
#define PID26_RST    0x0
#define PID25_RST    0x0
#define PID24_RST    0x0
#define PID23_RST    0x0
#define PID22_RST    0x0
#define PID21_RST    0x0
#define PID20_RST    0x0
#define PID19_RST    0x0
#define PID18_RST    0x0
#define PID17_RST    0x0
#define PID16_RST    0x0
#define PID15_RST    0x0
#define PID14_RST    0x0
#define PID13_RST    0x0
#define PID12_RST    0x0
#define PID11_RST    0x0
#define PID10_RST    0x0
#define PID9_RST     0x0
#define PID8_RST     0x0
#define PID7_RST     0x0
#define PID6_RST     0x0
#define PID5_RST     0x0
#define PID4_RST     0x0
#define PID3_RST     0x0
#define PID2_RST     0x0
#define SYS_RST      0x0
#define FIQ_RST      0x0

__INLINE void aic_iecr_pack(uint8_t pid31, uint8_t pid30, uint8_t pid29, uint8_t pid28, uint8_t pid27, uint8_t pid26, uint8_t pid25, uint8_t pid24, uint8_t pid23, uint8_t pid22, uint8_t pid21, uint8_t pid20, uint8_t pid19, uint8_t pid18, uint8_t pid17, uint8_t pid16, uint8_t pid15, uint8_t pid14, uint8_t pid13, uint8_t pid12, uint8_t pid11, uint8_t pid10, uint8_t pid9, uint8_t pid8, uint8_t pid7, uint8_t pid6, uint8_t pid5, uint8_t pid4, uint8_t pid3, uint8_t pid2, uint8_t sys, uint8_t fiq)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((fiq << 0) & ~0x00000001) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX,  (pid31 << 31) | (pid30 << 30) | (pid29 << 29) | (pid28 << 28) | (pid27 << 27) | (pid26 << 26) | (pid25 << 25) | (pid24 << 24) | (pid23 << 23) | (pid22 << 22) | (pid21 << 21) | (pid20 << 20) | (pid19 << 19) | (pid18 << 18) | (pid17 << 17) | (pid16 << 16) | (pid15 << 15) | (pid14 << 14) | (pid13 << 13) | (pid12 << 12) | (pid11 << 11) | (pid10 << 10) | (pid9 << 9) | (pid8 << 8) | (pid7 << 7) | (pid6 << 6) | (pid5 << 5) | (pid4 << 4) | (pid3 << 3) | (pid2 << 2) | (sys << 1) | (fiq << 0));
}

__INLINE void aic_iecr_unpack(uint8_t* pid31, uint8_t* pid30, uint8_t* pid29, uint8_t* pid28, uint8_t* pid27, uint8_t* pid26, uint8_t* pid25, uint8_t* pid24, uint8_t* pid23, uint8_t* pid22, uint8_t* pid21, uint8_t* pid20, uint8_t* pid19, uint8_t* pid18, uint8_t* pid17, uint8_t* pid16, uint8_t* pid15, uint8_t* pid14, uint8_t* pid13, uint8_t* pid12, uint8_t* pid11, uint8_t* pid10, uint8_t* pid9, uint8_t* pid8, uint8_t* pid7, uint8_t* pid6, uint8_t* pid5, uint8_t* pid4, uint8_t* pid3, uint8_t* pid2, uint8_t* sys, uint8_t* fiq)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);

    *pid31 = (localVal & 0x80000000) >> 31;
    *pid30 = (localVal & 0x40000000) >> 30;
    *pid29 = (localVal & 0x20000000) >> 29;
    *pid28 = (localVal & 0x10000000) >> 28;
    *pid27 = (localVal & 0x08000000) >> 27;
    *pid26 = (localVal & 0x04000000) >> 26;
    *pid25 = (localVal & 0x02000000) >> 25;
    *pid24 = (localVal & 0x01000000) >> 24;
    *pid23 = (localVal & 0x00800000) >> 23;
    *pid22 = (localVal & 0x00400000) >> 22;
    *pid21 = (localVal & 0x00200000) >> 21;
    *pid20 = (localVal & 0x00100000) >> 20;
    *pid19 = (localVal & 0x00080000) >> 19;
    *pid18 = (localVal & 0x00040000) >> 18;
    *pid17 = (localVal & 0x00020000) >> 17;
    *pid16 = (localVal & 0x00010000) >> 16;
    *pid15 = (localVal & 0x00008000) >> 15;
    *pid14 = (localVal & 0x00004000) >> 14;
    *pid13 = (localVal & 0x00002000) >> 13;
    *pid12 = (localVal & 0x00001000) >> 12;
    *pid11 = (localVal & 0x00000800) >> 11;
    *pid10 = (localVal & 0x00000400) >> 10;
    *pid9 = (localVal & 0x00000200) >> 9;
    *pid8 = (localVal & 0x00000100) >> 8;
    *pid7 = (localVal & 0x00000080) >> 7;
    *pid6 = (localVal & 0x00000040) >> 6;
    *pid5 = (localVal & 0x00000020) >> 5;
    *pid4 = (localVal & 0x00000010) >> 4;
    *pid3 = (localVal & 0x00000008) >> 3;
    *pid2 = (localVal & 0x00000004) >> 2;
    *sys = (localVal & 0x00000002) >> 1;
    *fiq = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t aic_iecr_pid31_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x80000000) >> 31);
}

__INLINE void aic_iecr_pid31_setf(uint8_t pid31)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid31 << 31);
}

__INLINE uint8_t aic_iecr_pid30_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x40000000) >> 30);
}

__INLINE void aic_iecr_pid30_setf(uint8_t pid30)
{
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid30 << 30);
}

__INLINE uint8_t aic_iecr_pid29_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x20000000) >> 29);
}

__INLINE void aic_iecr_pid29_setf(uint8_t pid29)
{
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid29 << 29);
}

__INLINE uint8_t aic_iecr_pid28_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x10000000) >> 28);
}

__INLINE void aic_iecr_pid28_setf(uint8_t pid28)
{
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid28 << 28);
}

__INLINE uint8_t aic_iecr_pid27_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x08000000) >> 27);
}

__INLINE void aic_iecr_pid27_setf(uint8_t pid27)
{
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid27 << 27);
}

__INLINE uint8_t aic_iecr_pid26_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x04000000) >> 26);
}

__INLINE void aic_iecr_pid26_setf(uint8_t pid26)
{
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid26 << 26);
}

__INLINE uint8_t aic_iecr_pid25_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x02000000) >> 25);
}

__INLINE void aic_iecr_pid25_setf(uint8_t pid25)
{
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid25 << 25);
}

__INLINE uint8_t aic_iecr_pid24_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x01000000) >> 24);
}

__INLINE void aic_iecr_pid24_setf(uint8_t pid24)
{
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid24 << 24);
}

__INLINE uint8_t aic_iecr_pid23_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00800000) >> 23);
}

__INLINE void aic_iecr_pid23_setf(uint8_t pid23)
{
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid23 << 23);
}

__INLINE uint8_t aic_iecr_pid22_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00400000) >> 22);
}

__INLINE void aic_iecr_pid22_setf(uint8_t pid22)
{
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid22 << 22);
}

__INLINE uint8_t aic_iecr_pid21_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00200000) >> 21);
}

__INLINE void aic_iecr_pid21_setf(uint8_t pid21)
{
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid21 << 21);
}

__INLINE uint8_t aic_iecr_pid20_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00100000) >> 20);
}

__INLINE void aic_iecr_pid20_setf(uint8_t pid20)
{
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid20 << 20);
}

__INLINE uint8_t aic_iecr_pid19_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00080000) >> 19);
}

__INLINE void aic_iecr_pid19_setf(uint8_t pid19)
{
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid19 << 19);
}

__INLINE uint8_t aic_iecr_pid18_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00040000) >> 18);
}

__INLINE void aic_iecr_pid18_setf(uint8_t pid18)
{
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid18 << 18);
}

__INLINE uint8_t aic_iecr_pid17_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE void aic_iecr_pid17_setf(uint8_t pid17)
{
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid17 << 17);
}

__INLINE uint8_t aic_iecr_pid16_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE void aic_iecr_pid16_setf(uint8_t pid16)
{
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid16 << 16);
}

__INLINE uint8_t aic_iecr_pid15_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00008000) >> 15);
}

__INLINE void aic_iecr_pid15_setf(uint8_t pid15)
{
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid15 << 15);
}

__INLINE uint8_t aic_iecr_pid14_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00004000) >> 14);
}

__INLINE void aic_iecr_pid14_setf(uint8_t pid14)
{
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid14 << 14);
}

__INLINE uint8_t aic_iecr_pid13_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00002000) >> 13);
}

__INLINE void aic_iecr_pid13_setf(uint8_t pid13)
{
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid13 << 13);
}

__INLINE uint8_t aic_iecr_pid12_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00001000) >> 12);
}

__INLINE void aic_iecr_pid12_setf(uint8_t pid12)
{
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid12 << 12);
}

__INLINE uint8_t aic_iecr_pid11_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000800) >> 11);
}

__INLINE void aic_iecr_pid11_setf(uint8_t pid11)
{
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid11 << 11);
}

__INLINE uint8_t aic_iecr_pid10_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE void aic_iecr_pid10_setf(uint8_t pid10)
{
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid10 << 10);
}

__INLINE uint8_t aic_iecr_pid9_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE void aic_iecr_pid9_setf(uint8_t pid9)
{
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid9 << 9);
}

__INLINE uint8_t aic_iecr_pid8_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE void aic_iecr_pid8_setf(uint8_t pid8)
{
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid8 << 8);
}

__INLINE uint8_t aic_iecr_pid7_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE void aic_iecr_pid7_setf(uint8_t pid7)
{
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid7 << 7);
}

__INLINE uint8_t aic_iecr_pid6_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE void aic_iecr_pid6_setf(uint8_t pid6)
{
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid6 << 6);
}

__INLINE uint8_t aic_iecr_pid5_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE void aic_iecr_pid5_setf(uint8_t pid5)
{
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid5 << 5);
}

__INLINE uint8_t aic_iecr_pid4_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE void aic_iecr_pid4_setf(uint8_t pid4)
{
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid4 << 4);
}

__INLINE uint8_t aic_iecr_pid3_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE void aic_iecr_pid3_setf(uint8_t pid3)
{
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid3 << 3);
}

__INLINE uint8_t aic_iecr_pid2_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void aic_iecr_pid2_setf(uint8_t pid2)
{
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, pid2 << 2);
}

__INLINE uint8_t aic_iecr_sys_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void aic_iecr_sys_setf(uint8_t sys)
{
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, sys << 1);
}

__INLINE uint8_t aic_iecr_fiq_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IECR_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void aic_iecr_fiq_setf(uint8_t fiq)
{
    ASSERT_ERR(((fiq << 0) & ~0x00000001) == 0);
    sim_reg_aic_wr(AIC_IECR_INDEX, fiq << 0);
}

/**
 * @brief AIC_IDCR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31                PID31   0
 *     30                PID30   0
 *     29                PID29   0
 *     28                PID28   0
 *     27                PID27   0
 *     26                PID26   0
 *     25                PID25   0
 *     24                PID24   0
 *     23                PID23   0
 *     22                PID22   0
 *     21                PID21   0
 *     20                PID20   0
 *     19                PID19   0
 *     18                PID18   0
 *     17                PID17   0
 *     16                PID16   0
 *     15                PID15   0
 *     14                PID14   0
 *     13                PID13   0
 *     12                PID12   0
 *     11                PID11   0
 *     10                PID10   0
 *     09                 PID9   0
 *     08                 PID8   0
 *     07                 PID7   0
 *     06                 PID6   0
 *     05                 PID5   0
 *     04                 PID4   0
 *     03                 PID3   0
 *     02                 PID2   0
 *     01                  SYS   0
 *     00                  FIQ   0
 * </pre>
 */
#define AIC_IDCR_INDEX  0x00000049

__INLINE uint32_t aic_idcr_get(void)
{
    return sim_reg_aic_rd(AIC_IDCR_INDEX);
}

__INLINE void aic_idcr_clear(uint32_t value)
{
    sim_reg_aic_wr(AIC_IDCR_INDEX, value);
}

// field definitions
#define PID31_BIT    0x80000000
#define PID31_POS    31
#define PID30_BIT    0x40000000
#define PID30_POS    30
#define PID29_BIT    0x20000000
#define PID29_POS    29
#define PID28_BIT    0x10000000
#define PID28_POS    28
#define PID27_BIT    0x08000000
#define PID27_POS    27
#define PID26_BIT    0x04000000
#define PID26_POS    26
#define PID25_BIT    0x02000000
#define PID25_POS    25
#define PID24_BIT    0x01000000
#define PID24_POS    24
#define PID23_BIT    0x00800000
#define PID23_POS    23
#define PID22_BIT    0x00400000
#define PID22_POS    22
#define PID21_BIT    0x00200000
#define PID21_POS    21
#define PID20_BIT    0x00100000
#define PID20_POS    20
#define PID19_BIT    0x00080000
#define PID19_POS    19
#define PID18_BIT    0x00040000
#define PID18_POS    18
#define PID17_BIT    0x00020000
#define PID17_POS    17
#define PID16_BIT    0x00010000
#define PID16_POS    16
#define PID15_BIT    0x00008000
#define PID15_POS    15
#define PID14_BIT    0x00004000
#define PID14_POS    14
#define PID13_BIT    0x00002000
#define PID13_POS    13
#define PID12_BIT    0x00001000
#define PID12_POS    12
#define PID11_BIT    0x00000800
#define PID11_POS    11
#define PID10_BIT    0x00000400
#define PID10_POS    10
#define PID9_BIT     0x00000200
#define PID9_POS     9
#define PID8_BIT     0x00000100
#define PID8_POS     8
#define PID7_BIT     0x00000080
#define PID7_POS     7
#define PID6_BIT     0x00000040
#define PID6_POS     6
#define PID5_BIT     0x00000020
#define PID5_POS     5
#define PID4_BIT     0x00000010
#define PID4_POS     4
#define PID3_BIT     0x00000008
#define PID3_POS     3
#define PID2_BIT     0x00000004
#define PID2_POS     2
#define SYS_BIT      0x00000002
#define SYS_POS      1
#define FIQ_BIT      0x00000001
#define FIQ_POS      0

#define PID31_RST    0x0
#define PID30_RST    0x0
#define PID29_RST    0x0
#define PID28_RST    0x0
#define PID27_RST    0x0
#define PID26_RST    0x0
#define PID25_RST    0x0
#define PID24_RST    0x0
#define PID23_RST    0x0
#define PID22_RST    0x0
#define PID21_RST    0x0
#define PID20_RST    0x0
#define PID19_RST    0x0
#define PID18_RST    0x0
#define PID17_RST    0x0
#define PID16_RST    0x0
#define PID15_RST    0x0
#define PID14_RST    0x0
#define PID13_RST    0x0
#define PID12_RST    0x0
#define PID11_RST    0x0
#define PID10_RST    0x0
#define PID9_RST     0x0
#define PID8_RST     0x0
#define PID7_RST     0x0
#define PID6_RST     0x0
#define PID5_RST     0x0
#define PID4_RST     0x0
#define PID3_RST     0x0
#define PID2_RST     0x0
#define SYS_RST      0x0
#define FIQ_RST      0x0

__INLINE void aic_idcr_pack(uint8_t pid31, uint8_t pid30, uint8_t pid29, uint8_t pid28, uint8_t pid27, uint8_t pid26, uint8_t pid25, uint8_t pid24, uint8_t pid23, uint8_t pid22, uint8_t pid21, uint8_t pid20, uint8_t pid19, uint8_t pid18, uint8_t pid17, uint8_t pid16, uint8_t pid15, uint8_t pid14, uint8_t pid13, uint8_t pid12, uint8_t pid11, uint8_t pid10, uint8_t pid9, uint8_t pid8, uint8_t pid7, uint8_t pid6, uint8_t pid5, uint8_t pid4, uint8_t pid3, uint8_t pid2, uint8_t sys, uint8_t fiq)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((fiq << 0) & ~0x00000001) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX,  (pid31 << 31) | (pid30 << 30) | (pid29 << 29) | (pid28 << 28) | (pid27 << 27) | (pid26 << 26) | (pid25 << 25) | (pid24 << 24) | (pid23 << 23) | (pid22 << 22) | (pid21 << 21) | (pid20 << 20) | (pid19 << 19) | (pid18 << 18) | (pid17 << 17) | (pid16 << 16) | (pid15 << 15) | (pid14 << 14) | (pid13 << 13) | (pid12 << 12) | (pid11 << 11) | (pid10 << 10) | (pid9 << 9) | (pid8 << 8) | (pid7 << 7) | (pid6 << 6) | (pid5 << 5) | (pid4 << 4) | (pid3 << 3) | (pid2 << 2) | (sys << 1) | (fiq << 0));
}

__INLINE void aic_idcr_unpack(uint8_t* pid31, uint8_t* pid30, uint8_t* pid29, uint8_t* pid28, uint8_t* pid27, uint8_t* pid26, uint8_t* pid25, uint8_t* pid24, uint8_t* pid23, uint8_t* pid22, uint8_t* pid21, uint8_t* pid20, uint8_t* pid19, uint8_t* pid18, uint8_t* pid17, uint8_t* pid16, uint8_t* pid15, uint8_t* pid14, uint8_t* pid13, uint8_t* pid12, uint8_t* pid11, uint8_t* pid10, uint8_t* pid9, uint8_t* pid8, uint8_t* pid7, uint8_t* pid6, uint8_t* pid5, uint8_t* pid4, uint8_t* pid3, uint8_t* pid2, uint8_t* sys, uint8_t* fiq)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);

    *pid31 = (localVal & 0x80000000) >> 31;
    *pid30 = (localVal & 0x40000000) >> 30;
    *pid29 = (localVal & 0x20000000) >> 29;
    *pid28 = (localVal & 0x10000000) >> 28;
    *pid27 = (localVal & 0x08000000) >> 27;
    *pid26 = (localVal & 0x04000000) >> 26;
    *pid25 = (localVal & 0x02000000) >> 25;
    *pid24 = (localVal & 0x01000000) >> 24;
    *pid23 = (localVal & 0x00800000) >> 23;
    *pid22 = (localVal & 0x00400000) >> 22;
    *pid21 = (localVal & 0x00200000) >> 21;
    *pid20 = (localVal & 0x00100000) >> 20;
    *pid19 = (localVal & 0x00080000) >> 19;
    *pid18 = (localVal & 0x00040000) >> 18;
    *pid17 = (localVal & 0x00020000) >> 17;
    *pid16 = (localVal & 0x00010000) >> 16;
    *pid15 = (localVal & 0x00008000) >> 15;
    *pid14 = (localVal & 0x00004000) >> 14;
    *pid13 = (localVal & 0x00002000) >> 13;
    *pid12 = (localVal & 0x00001000) >> 12;
    *pid11 = (localVal & 0x00000800) >> 11;
    *pid10 = (localVal & 0x00000400) >> 10;
    *pid9 = (localVal & 0x00000200) >> 9;
    *pid8 = (localVal & 0x00000100) >> 8;
    *pid7 = (localVal & 0x00000080) >> 7;
    *pid6 = (localVal & 0x00000040) >> 6;
    *pid5 = (localVal & 0x00000020) >> 5;
    *pid4 = (localVal & 0x00000010) >> 4;
    *pid3 = (localVal & 0x00000008) >> 3;
    *pid2 = (localVal & 0x00000004) >> 2;
    *sys = (localVal & 0x00000002) >> 1;
    *fiq = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t aic_idcr_pid31_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x80000000) >> 31);
}

__INLINE void aic_idcr_pid31_clearf(uint8_t pid31)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid31 << 31);
}

__INLINE uint8_t aic_idcr_pid30_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x40000000) >> 30);
}

__INLINE void aic_idcr_pid30_clearf(uint8_t pid30)
{
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid30 << 30);
}

__INLINE uint8_t aic_idcr_pid29_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x20000000) >> 29);
}

__INLINE void aic_idcr_pid29_clearf(uint8_t pid29)
{
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid29 << 29);
}

__INLINE uint8_t aic_idcr_pid28_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x10000000) >> 28);
}

__INLINE void aic_idcr_pid28_clearf(uint8_t pid28)
{
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid28 << 28);
}

__INLINE uint8_t aic_idcr_pid27_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x08000000) >> 27);
}

__INLINE void aic_idcr_pid27_clearf(uint8_t pid27)
{
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid27 << 27);
}

__INLINE uint8_t aic_idcr_pid26_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x04000000) >> 26);
}

__INLINE void aic_idcr_pid26_clearf(uint8_t pid26)
{
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid26 << 26);
}

__INLINE uint8_t aic_idcr_pid25_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x02000000) >> 25);
}

__INLINE void aic_idcr_pid25_clearf(uint8_t pid25)
{
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid25 << 25);
}

__INLINE uint8_t aic_idcr_pid24_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x01000000) >> 24);
}

__INLINE void aic_idcr_pid24_clearf(uint8_t pid24)
{
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid24 << 24);
}

__INLINE uint8_t aic_idcr_pid23_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00800000) >> 23);
}

__INLINE void aic_idcr_pid23_clearf(uint8_t pid23)
{
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid23 << 23);
}

__INLINE uint8_t aic_idcr_pid22_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00400000) >> 22);
}

__INLINE void aic_idcr_pid22_clearf(uint8_t pid22)
{
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid22 << 22);
}

__INLINE uint8_t aic_idcr_pid21_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00200000) >> 21);
}

__INLINE void aic_idcr_pid21_clearf(uint8_t pid21)
{
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid21 << 21);
}

__INLINE uint8_t aic_idcr_pid20_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00100000) >> 20);
}

__INLINE void aic_idcr_pid20_clearf(uint8_t pid20)
{
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid20 << 20);
}

__INLINE uint8_t aic_idcr_pid19_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00080000) >> 19);
}

__INLINE void aic_idcr_pid19_clearf(uint8_t pid19)
{
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid19 << 19);
}

__INLINE uint8_t aic_idcr_pid18_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00040000) >> 18);
}

__INLINE void aic_idcr_pid18_clearf(uint8_t pid18)
{
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid18 << 18);
}

__INLINE uint8_t aic_idcr_pid17_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE void aic_idcr_pid17_clearf(uint8_t pid17)
{
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid17 << 17);
}

__INLINE uint8_t aic_idcr_pid16_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE void aic_idcr_pid16_clearf(uint8_t pid16)
{
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid16 << 16);
}

__INLINE uint8_t aic_idcr_pid15_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00008000) >> 15);
}

__INLINE void aic_idcr_pid15_clearf(uint8_t pid15)
{
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid15 << 15);
}

__INLINE uint8_t aic_idcr_pid14_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00004000) >> 14);
}

__INLINE void aic_idcr_pid14_clearf(uint8_t pid14)
{
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid14 << 14);
}

__INLINE uint8_t aic_idcr_pid13_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00002000) >> 13);
}

__INLINE void aic_idcr_pid13_clearf(uint8_t pid13)
{
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid13 << 13);
}

__INLINE uint8_t aic_idcr_pid12_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00001000) >> 12);
}

__INLINE void aic_idcr_pid12_clearf(uint8_t pid12)
{
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid12 << 12);
}

__INLINE uint8_t aic_idcr_pid11_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000800) >> 11);
}

__INLINE void aic_idcr_pid11_clearf(uint8_t pid11)
{
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid11 << 11);
}

__INLINE uint8_t aic_idcr_pid10_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE void aic_idcr_pid10_clearf(uint8_t pid10)
{
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid10 << 10);
}

__INLINE uint8_t aic_idcr_pid9_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE void aic_idcr_pid9_clearf(uint8_t pid9)
{
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid9 << 9);
}

__INLINE uint8_t aic_idcr_pid8_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE void aic_idcr_pid8_clearf(uint8_t pid8)
{
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid8 << 8);
}

__INLINE uint8_t aic_idcr_pid7_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE void aic_idcr_pid7_clearf(uint8_t pid7)
{
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid7 << 7);
}

__INLINE uint8_t aic_idcr_pid6_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE void aic_idcr_pid6_clearf(uint8_t pid6)
{
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid6 << 6);
}

__INLINE uint8_t aic_idcr_pid5_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE void aic_idcr_pid5_clearf(uint8_t pid5)
{
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid5 << 5);
}

__INLINE uint8_t aic_idcr_pid4_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE void aic_idcr_pid4_clearf(uint8_t pid4)
{
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid4 << 4);
}

__INLINE uint8_t aic_idcr_pid3_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE void aic_idcr_pid3_clearf(uint8_t pid3)
{
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid3 << 3);
}

__INLINE uint8_t aic_idcr_pid2_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void aic_idcr_pid2_clearf(uint8_t pid2)
{
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, pid2 << 2);
}

__INLINE uint8_t aic_idcr_sys_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void aic_idcr_sys_clearf(uint8_t sys)
{
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, sys << 1);
}

__INLINE uint8_t aic_idcr_fiq_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_IDCR_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void aic_idcr_fiq_clearf(uint8_t fiq)
{
    ASSERT_ERR(((fiq << 0) & ~0x00000001) == 0);
    sim_reg_aic_wr(AIC_IDCR_INDEX, fiq << 0);
}

/**
 * @brief AIC_ICCR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31                PID31   0
 *     30                PID30   0
 *     29                PID29   0
 *     28                PID28   0
 *     27                PID27   0
 *     26                PID26   0
 *     25                PID25   0
 *     24                PID24   0
 *     23                PID23   0
 *     22                PID22   0
 *     21                PID21   0
 *     20                PID20   0
 *     19                PID19   0
 *     18                PID18   0
 *     17                PID17   0
 *     16                PID16   0
 *     15                PID15   0
 *     14                PID14   0
 *     13                PID13   0
 *     12                PID12   0
 *     11                PID11   0
 *     10                PID10   0
 *     09                 PID9   0
 *     08                 PID8   0
 *     07                 PID7   0
 *     06                 PID6   0
 *     05                 PID5   0
 *     04                 PID4   0
 *     03                 PID3   0
 *     02                 PID2   0
 *     01                  SYS   0
 *     00                  FIQ   0
 * </pre>
 */
#define AIC_ICCR_INDEX  0x0000004A

__INLINE uint32_t aic_iccr_get(void)
{
    return sim_reg_aic_rd(AIC_ICCR_INDEX);
}

__INLINE void aic_iccr_clear(uint32_t value)
{
    sim_reg_aic_wr(AIC_ICCR_INDEX, value);
}

// field definitions
#define PID31_BIT    0x80000000
#define PID31_POS    31
#define PID30_BIT    0x40000000
#define PID30_POS    30
#define PID29_BIT    0x20000000
#define PID29_POS    29
#define PID28_BIT    0x10000000
#define PID28_POS    28
#define PID27_BIT    0x08000000
#define PID27_POS    27
#define PID26_BIT    0x04000000
#define PID26_POS    26
#define PID25_BIT    0x02000000
#define PID25_POS    25
#define PID24_BIT    0x01000000
#define PID24_POS    24
#define PID23_BIT    0x00800000
#define PID23_POS    23
#define PID22_BIT    0x00400000
#define PID22_POS    22
#define PID21_BIT    0x00200000
#define PID21_POS    21
#define PID20_BIT    0x00100000
#define PID20_POS    20
#define PID19_BIT    0x00080000
#define PID19_POS    19
#define PID18_BIT    0x00040000
#define PID18_POS    18
#define PID17_BIT    0x00020000
#define PID17_POS    17
#define PID16_BIT    0x00010000
#define PID16_POS    16
#define PID15_BIT    0x00008000
#define PID15_POS    15
#define PID14_BIT    0x00004000
#define PID14_POS    14
#define PID13_BIT    0x00002000
#define PID13_POS    13
#define PID12_BIT    0x00001000
#define PID12_POS    12
#define PID11_BIT    0x00000800
#define PID11_POS    11
#define PID10_BIT    0x00000400
#define PID10_POS    10
#define PID9_BIT     0x00000200
#define PID9_POS     9
#define PID8_BIT     0x00000100
#define PID8_POS     8
#define PID7_BIT     0x00000080
#define PID7_POS     7
#define PID6_BIT     0x00000040
#define PID6_POS     6
#define PID5_BIT     0x00000020
#define PID5_POS     5
#define PID4_BIT     0x00000010
#define PID4_POS     4
#define PID3_BIT     0x00000008
#define PID3_POS     3
#define PID2_BIT     0x00000004
#define PID2_POS     2
#define SYS_BIT      0x00000002
#define SYS_POS      1
#define FIQ_BIT      0x00000001
#define FIQ_POS      0

#define PID31_RST    0x0
#define PID30_RST    0x0
#define PID29_RST    0x0
#define PID28_RST    0x0
#define PID27_RST    0x0
#define PID26_RST    0x0
#define PID25_RST    0x0
#define PID24_RST    0x0
#define PID23_RST    0x0
#define PID22_RST    0x0
#define PID21_RST    0x0
#define PID20_RST    0x0
#define PID19_RST    0x0
#define PID18_RST    0x0
#define PID17_RST    0x0
#define PID16_RST    0x0
#define PID15_RST    0x0
#define PID14_RST    0x0
#define PID13_RST    0x0
#define PID12_RST    0x0
#define PID11_RST    0x0
#define PID10_RST    0x0
#define PID9_RST     0x0
#define PID8_RST     0x0
#define PID7_RST     0x0
#define PID6_RST     0x0
#define PID5_RST     0x0
#define PID4_RST     0x0
#define PID3_RST     0x0
#define PID2_RST     0x0
#define SYS_RST      0x0
#define FIQ_RST      0x0

__INLINE void aic_iccr_pack(uint8_t pid31, uint8_t pid30, uint8_t pid29, uint8_t pid28, uint8_t pid27, uint8_t pid26, uint8_t pid25, uint8_t pid24, uint8_t pid23, uint8_t pid22, uint8_t pid21, uint8_t pid20, uint8_t pid19, uint8_t pid18, uint8_t pid17, uint8_t pid16, uint8_t pid15, uint8_t pid14, uint8_t pid13, uint8_t pid12, uint8_t pid11, uint8_t pid10, uint8_t pid9, uint8_t pid8, uint8_t pid7, uint8_t pid6, uint8_t pid5, uint8_t pid4, uint8_t pid3, uint8_t pid2, uint8_t sys, uint8_t fiq)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((fiq << 0) & ~0x00000001) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX,  (pid31 << 31) | (pid30 << 30) | (pid29 << 29) | (pid28 << 28) | (pid27 << 27) | (pid26 << 26) | (pid25 << 25) | (pid24 << 24) | (pid23 << 23) | (pid22 << 22) | (pid21 << 21) | (pid20 << 20) | (pid19 << 19) | (pid18 << 18) | (pid17 << 17) | (pid16 << 16) | (pid15 << 15) | (pid14 << 14) | (pid13 << 13) | (pid12 << 12) | (pid11 << 11) | (pid10 << 10) | (pid9 << 9) | (pid8 << 8) | (pid7 << 7) | (pid6 << 6) | (pid5 << 5) | (pid4 << 4) | (pid3 << 3) | (pid2 << 2) | (sys << 1) | (fiq << 0));
}

__INLINE void aic_iccr_unpack(uint8_t* pid31, uint8_t* pid30, uint8_t* pid29, uint8_t* pid28, uint8_t* pid27, uint8_t* pid26, uint8_t* pid25, uint8_t* pid24, uint8_t* pid23, uint8_t* pid22, uint8_t* pid21, uint8_t* pid20, uint8_t* pid19, uint8_t* pid18, uint8_t* pid17, uint8_t* pid16, uint8_t* pid15, uint8_t* pid14, uint8_t* pid13, uint8_t* pid12, uint8_t* pid11, uint8_t* pid10, uint8_t* pid9, uint8_t* pid8, uint8_t* pid7, uint8_t* pid6, uint8_t* pid5, uint8_t* pid4, uint8_t* pid3, uint8_t* pid2, uint8_t* sys, uint8_t* fiq)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);

    *pid31 = (localVal & 0x80000000) >> 31;
    *pid30 = (localVal & 0x40000000) >> 30;
    *pid29 = (localVal & 0x20000000) >> 29;
    *pid28 = (localVal & 0x10000000) >> 28;
    *pid27 = (localVal & 0x08000000) >> 27;
    *pid26 = (localVal & 0x04000000) >> 26;
    *pid25 = (localVal & 0x02000000) >> 25;
    *pid24 = (localVal & 0x01000000) >> 24;
    *pid23 = (localVal & 0x00800000) >> 23;
    *pid22 = (localVal & 0x00400000) >> 22;
    *pid21 = (localVal & 0x00200000) >> 21;
    *pid20 = (localVal & 0x00100000) >> 20;
    *pid19 = (localVal & 0x00080000) >> 19;
    *pid18 = (localVal & 0x00040000) >> 18;
    *pid17 = (localVal & 0x00020000) >> 17;
    *pid16 = (localVal & 0x00010000) >> 16;
    *pid15 = (localVal & 0x00008000) >> 15;
    *pid14 = (localVal & 0x00004000) >> 14;
    *pid13 = (localVal & 0x00002000) >> 13;
    *pid12 = (localVal & 0x00001000) >> 12;
    *pid11 = (localVal & 0x00000800) >> 11;
    *pid10 = (localVal & 0x00000400) >> 10;
    *pid9 = (localVal & 0x00000200) >> 9;
    *pid8 = (localVal & 0x00000100) >> 8;
    *pid7 = (localVal & 0x00000080) >> 7;
    *pid6 = (localVal & 0x00000040) >> 6;
    *pid5 = (localVal & 0x00000020) >> 5;
    *pid4 = (localVal & 0x00000010) >> 4;
    *pid3 = (localVal & 0x00000008) >> 3;
    *pid2 = (localVal & 0x00000004) >> 2;
    *sys = (localVal & 0x00000002) >> 1;
    *fiq = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t aic_iccr_pid31_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x80000000) >> 31);
}

__INLINE void aic_iccr_pid31_clearf(uint8_t pid31)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid31 << 31);
}

__INLINE uint8_t aic_iccr_pid30_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x40000000) >> 30);
}

__INLINE void aic_iccr_pid30_clearf(uint8_t pid30)
{
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid30 << 30);
}

__INLINE uint8_t aic_iccr_pid29_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x20000000) >> 29);
}

__INLINE void aic_iccr_pid29_clearf(uint8_t pid29)
{
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid29 << 29);
}

__INLINE uint8_t aic_iccr_pid28_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x10000000) >> 28);
}

__INLINE void aic_iccr_pid28_clearf(uint8_t pid28)
{
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid28 << 28);
}

__INLINE uint8_t aic_iccr_pid27_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x08000000) >> 27);
}

__INLINE void aic_iccr_pid27_clearf(uint8_t pid27)
{
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid27 << 27);
}

__INLINE uint8_t aic_iccr_pid26_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x04000000) >> 26);
}

__INLINE void aic_iccr_pid26_clearf(uint8_t pid26)
{
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid26 << 26);
}

__INLINE uint8_t aic_iccr_pid25_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x02000000) >> 25);
}

__INLINE void aic_iccr_pid25_clearf(uint8_t pid25)
{
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid25 << 25);
}

__INLINE uint8_t aic_iccr_pid24_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x01000000) >> 24);
}

__INLINE void aic_iccr_pid24_clearf(uint8_t pid24)
{
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid24 << 24);
}

__INLINE uint8_t aic_iccr_pid23_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00800000) >> 23);
}

__INLINE void aic_iccr_pid23_clearf(uint8_t pid23)
{
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid23 << 23);
}

__INLINE uint8_t aic_iccr_pid22_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00400000) >> 22);
}

__INLINE void aic_iccr_pid22_clearf(uint8_t pid22)
{
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid22 << 22);
}

__INLINE uint8_t aic_iccr_pid21_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00200000) >> 21);
}

__INLINE void aic_iccr_pid21_clearf(uint8_t pid21)
{
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid21 << 21);
}

__INLINE uint8_t aic_iccr_pid20_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00100000) >> 20);
}

__INLINE void aic_iccr_pid20_clearf(uint8_t pid20)
{
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid20 << 20);
}

__INLINE uint8_t aic_iccr_pid19_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00080000) >> 19);
}

__INLINE void aic_iccr_pid19_clearf(uint8_t pid19)
{
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid19 << 19);
}

__INLINE uint8_t aic_iccr_pid18_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00040000) >> 18);
}

__INLINE void aic_iccr_pid18_clearf(uint8_t pid18)
{
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid18 << 18);
}

__INLINE uint8_t aic_iccr_pid17_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE void aic_iccr_pid17_clearf(uint8_t pid17)
{
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid17 << 17);
}

__INLINE uint8_t aic_iccr_pid16_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE void aic_iccr_pid16_clearf(uint8_t pid16)
{
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid16 << 16);
}

__INLINE uint8_t aic_iccr_pid15_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00008000) >> 15);
}

__INLINE void aic_iccr_pid15_clearf(uint8_t pid15)
{
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid15 << 15);
}

__INLINE uint8_t aic_iccr_pid14_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00004000) >> 14);
}

__INLINE void aic_iccr_pid14_clearf(uint8_t pid14)
{
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid14 << 14);
}

__INLINE uint8_t aic_iccr_pid13_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00002000) >> 13);
}

__INLINE void aic_iccr_pid13_clearf(uint8_t pid13)
{
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid13 << 13);
}

__INLINE uint8_t aic_iccr_pid12_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00001000) >> 12);
}

__INLINE void aic_iccr_pid12_clearf(uint8_t pid12)
{
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid12 << 12);
}

__INLINE uint8_t aic_iccr_pid11_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000800) >> 11);
}

__INLINE void aic_iccr_pid11_clearf(uint8_t pid11)
{
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid11 << 11);
}

__INLINE uint8_t aic_iccr_pid10_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE void aic_iccr_pid10_clearf(uint8_t pid10)
{
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid10 << 10);
}

__INLINE uint8_t aic_iccr_pid9_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE void aic_iccr_pid9_clearf(uint8_t pid9)
{
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid9 << 9);
}

__INLINE uint8_t aic_iccr_pid8_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE void aic_iccr_pid8_clearf(uint8_t pid8)
{
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid8 << 8);
}

__INLINE uint8_t aic_iccr_pid7_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE void aic_iccr_pid7_clearf(uint8_t pid7)
{
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid7 << 7);
}

__INLINE uint8_t aic_iccr_pid6_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE void aic_iccr_pid6_clearf(uint8_t pid6)
{
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid6 << 6);
}

__INLINE uint8_t aic_iccr_pid5_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE void aic_iccr_pid5_clearf(uint8_t pid5)
{
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid5 << 5);
}

__INLINE uint8_t aic_iccr_pid4_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE void aic_iccr_pid4_clearf(uint8_t pid4)
{
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid4 << 4);
}

__INLINE uint8_t aic_iccr_pid3_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE void aic_iccr_pid3_clearf(uint8_t pid3)
{
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid3 << 3);
}

__INLINE uint8_t aic_iccr_pid2_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void aic_iccr_pid2_clearf(uint8_t pid2)
{
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, pid2 << 2);
}

__INLINE uint8_t aic_iccr_sys_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void aic_iccr_sys_clearf(uint8_t sys)
{
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, sys << 1);
}

__INLINE uint8_t aic_iccr_fiq_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ICCR_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void aic_iccr_fiq_clearf(uint8_t fiq)
{
    ASSERT_ERR(((fiq << 0) & ~0x00000001) == 0);
    sim_reg_aic_wr(AIC_ICCR_INDEX, fiq << 0);
}

/**
 * @brief AIC_ISCR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31                PID31   0
 *     30                PID30   0
 *     29                PID29   0
 *     28                PID28   0
 *     27                PID27   0
 *     26                PID26   0
 *     25                PID25   0
 *     24                PID24   0
 *     23                PID23   0
 *     22                PID22   0
 *     21                PID21   0
 *     20                PID20   0
 *     19                PID19   0
 *     18                PID18   0
 *     17                PID17   0
 *     16                PID16   0
 *     15                PID15   0
 *     14                PID14   0
 *     13                PID13   0
 *     12                PID12   0
 *     11                PID11   0
 *     10                PID10   0
 *     09                 PID9   0
 *     08                 PID8   0
 *     07                 PID7   0
 *     06                 PID6   0
 *     05                 PID5   0
 *     04                 PID4   0
 *     03                 PID3   0
 *     02                 PID2   0
 *     01                  SYS   0
 *     00                  FIQ   0
 * </pre>
 */
#define AIC_ISCR_INDEX  0x0000004B

__INLINE uint32_t aic_iscr_get(void)
{
    return sim_reg_aic_rd(AIC_ISCR_INDEX);
}

__INLINE void aic_iscr_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_ISCR_INDEX, value);
}

// field definitions
#define PID31_BIT    0x80000000
#define PID31_POS    31
#define PID30_BIT    0x40000000
#define PID30_POS    30
#define PID29_BIT    0x20000000
#define PID29_POS    29
#define PID28_BIT    0x10000000
#define PID28_POS    28
#define PID27_BIT    0x08000000
#define PID27_POS    27
#define PID26_BIT    0x04000000
#define PID26_POS    26
#define PID25_BIT    0x02000000
#define PID25_POS    25
#define PID24_BIT    0x01000000
#define PID24_POS    24
#define PID23_BIT    0x00800000
#define PID23_POS    23
#define PID22_BIT    0x00400000
#define PID22_POS    22
#define PID21_BIT    0x00200000
#define PID21_POS    21
#define PID20_BIT    0x00100000
#define PID20_POS    20
#define PID19_BIT    0x00080000
#define PID19_POS    19
#define PID18_BIT    0x00040000
#define PID18_POS    18
#define PID17_BIT    0x00020000
#define PID17_POS    17
#define PID16_BIT    0x00010000
#define PID16_POS    16
#define PID15_BIT    0x00008000
#define PID15_POS    15
#define PID14_BIT    0x00004000
#define PID14_POS    14
#define PID13_BIT    0x00002000
#define PID13_POS    13
#define PID12_BIT    0x00001000
#define PID12_POS    12
#define PID11_BIT    0x00000800
#define PID11_POS    11
#define PID10_BIT    0x00000400
#define PID10_POS    10
#define PID9_BIT     0x00000200
#define PID9_POS     9
#define PID8_BIT     0x00000100
#define PID8_POS     8
#define PID7_BIT     0x00000080
#define PID7_POS     7
#define PID6_BIT     0x00000040
#define PID6_POS     6
#define PID5_BIT     0x00000020
#define PID5_POS     5
#define PID4_BIT     0x00000010
#define PID4_POS     4
#define PID3_BIT     0x00000008
#define PID3_POS     3
#define PID2_BIT     0x00000004
#define PID2_POS     2
#define SYS_BIT      0x00000002
#define SYS_POS      1
#define FIQ_BIT      0x00000001
#define FIQ_POS      0

#define PID31_RST    0x0
#define PID30_RST    0x0
#define PID29_RST    0x0
#define PID28_RST    0x0
#define PID27_RST    0x0
#define PID26_RST    0x0
#define PID25_RST    0x0
#define PID24_RST    0x0
#define PID23_RST    0x0
#define PID22_RST    0x0
#define PID21_RST    0x0
#define PID20_RST    0x0
#define PID19_RST    0x0
#define PID18_RST    0x0
#define PID17_RST    0x0
#define PID16_RST    0x0
#define PID15_RST    0x0
#define PID14_RST    0x0
#define PID13_RST    0x0
#define PID12_RST    0x0
#define PID11_RST    0x0
#define PID10_RST    0x0
#define PID9_RST     0x0
#define PID8_RST     0x0
#define PID7_RST     0x0
#define PID6_RST     0x0
#define PID5_RST     0x0
#define PID4_RST     0x0
#define PID3_RST     0x0
#define PID2_RST     0x0
#define SYS_RST      0x0
#define FIQ_RST      0x0

__INLINE void aic_iscr_pack(uint8_t pid31, uint8_t pid30, uint8_t pid29, uint8_t pid28, uint8_t pid27, uint8_t pid26, uint8_t pid25, uint8_t pid24, uint8_t pid23, uint8_t pid22, uint8_t pid21, uint8_t pid20, uint8_t pid19, uint8_t pid18, uint8_t pid17, uint8_t pid16, uint8_t pid15, uint8_t pid14, uint8_t pid13, uint8_t pid12, uint8_t pid11, uint8_t pid10, uint8_t pid9, uint8_t pid8, uint8_t pid7, uint8_t pid6, uint8_t pid5, uint8_t pid4, uint8_t pid3, uint8_t pid2, uint8_t sys, uint8_t fiq)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((fiq << 0) & ~0x00000001) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX,  (pid31 << 31) | (pid30 << 30) | (pid29 << 29) | (pid28 << 28) | (pid27 << 27) | (pid26 << 26) | (pid25 << 25) | (pid24 << 24) | (pid23 << 23) | (pid22 << 22) | (pid21 << 21) | (pid20 << 20) | (pid19 << 19) | (pid18 << 18) | (pid17 << 17) | (pid16 << 16) | (pid15 << 15) | (pid14 << 14) | (pid13 << 13) | (pid12 << 12) | (pid11 << 11) | (pid10 << 10) | (pid9 << 9) | (pid8 << 8) | (pid7 << 7) | (pid6 << 6) | (pid5 << 5) | (pid4 << 4) | (pid3 << 3) | (pid2 << 2) | (sys << 1) | (fiq << 0));
}

__INLINE void aic_iscr_unpack(uint8_t* pid31, uint8_t* pid30, uint8_t* pid29, uint8_t* pid28, uint8_t* pid27, uint8_t* pid26, uint8_t* pid25, uint8_t* pid24, uint8_t* pid23, uint8_t* pid22, uint8_t* pid21, uint8_t* pid20, uint8_t* pid19, uint8_t* pid18, uint8_t* pid17, uint8_t* pid16, uint8_t* pid15, uint8_t* pid14, uint8_t* pid13, uint8_t* pid12, uint8_t* pid11, uint8_t* pid10, uint8_t* pid9, uint8_t* pid8, uint8_t* pid7, uint8_t* pid6, uint8_t* pid5, uint8_t* pid4, uint8_t* pid3, uint8_t* pid2, uint8_t* sys, uint8_t* fiq)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);

    *pid31 = (localVal & 0x80000000) >> 31;
    *pid30 = (localVal & 0x40000000) >> 30;
    *pid29 = (localVal & 0x20000000) >> 29;
    *pid28 = (localVal & 0x10000000) >> 28;
    *pid27 = (localVal & 0x08000000) >> 27;
    *pid26 = (localVal & 0x04000000) >> 26;
    *pid25 = (localVal & 0x02000000) >> 25;
    *pid24 = (localVal & 0x01000000) >> 24;
    *pid23 = (localVal & 0x00800000) >> 23;
    *pid22 = (localVal & 0x00400000) >> 22;
    *pid21 = (localVal & 0x00200000) >> 21;
    *pid20 = (localVal & 0x00100000) >> 20;
    *pid19 = (localVal & 0x00080000) >> 19;
    *pid18 = (localVal & 0x00040000) >> 18;
    *pid17 = (localVal & 0x00020000) >> 17;
    *pid16 = (localVal & 0x00010000) >> 16;
    *pid15 = (localVal & 0x00008000) >> 15;
    *pid14 = (localVal & 0x00004000) >> 14;
    *pid13 = (localVal & 0x00002000) >> 13;
    *pid12 = (localVal & 0x00001000) >> 12;
    *pid11 = (localVal & 0x00000800) >> 11;
    *pid10 = (localVal & 0x00000400) >> 10;
    *pid9 = (localVal & 0x00000200) >> 9;
    *pid8 = (localVal & 0x00000100) >> 8;
    *pid7 = (localVal & 0x00000080) >> 7;
    *pid6 = (localVal & 0x00000040) >> 6;
    *pid5 = (localVal & 0x00000020) >> 5;
    *pid4 = (localVal & 0x00000010) >> 4;
    *pid3 = (localVal & 0x00000008) >> 3;
    *pid2 = (localVal & 0x00000004) >> 2;
    *sys = (localVal & 0x00000002) >> 1;
    *fiq = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t aic_iscr_pid31_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x80000000) >> 31);
}

__INLINE void aic_iscr_pid31_setf(uint8_t pid31)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid31 << 31);
}

__INLINE uint8_t aic_iscr_pid30_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x40000000) >> 30);
}

__INLINE void aic_iscr_pid30_setf(uint8_t pid30)
{
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid30 << 30);
}

__INLINE uint8_t aic_iscr_pid29_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x20000000) >> 29);
}

__INLINE void aic_iscr_pid29_setf(uint8_t pid29)
{
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid29 << 29);
}

__INLINE uint8_t aic_iscr_pid28_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x10000000) >> 28);
}

__INLINE void aic_iscr_pid28_setf(uint8_t pid28)
{
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid28 << 28);
}

__INLINE uint8_t aic_iscr_pid27_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x08000000) >> 27);
}

__INLINE void aic_iscr_pid27_setf(uint8_t pid27)
{
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid27 << 27);
}

__INLINE uint8_t aic_iscr_pid26_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x04000000) >> 26);
}

__INLINE void aic_iscr_pid26_setf(uint8_t pid26)
{
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid26 << 26);
}

__INLINE uint8_t aic_iscr_pid25_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x02000000) >> 25);
}

__INLINE void aic_iscr_pid25_setf(uint8_t pid25)
{
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid25 << 25);
}

__INLINE uint8_t aic_iscr_pid24_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x01000000) >> 24);
}

__INLINE void aic_iscr_pid24_setf(uint8_t pid24)
{
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid24 << 24);
}

__INLINE uint8_t aic_iscr_pid23_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00800000) >> 23);
}

__INLINE void aic_iscr_pid23_setf(uint8_t pid23)
{
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid23 << 23);
}

__INLINE uint8_t aic_iscr_pid22_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00400000) >> 22);
}

__INLINE void aic_iscr_pid22_setf(uint8_t pid22)
{
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid22 << 22);
}

__INLINE uint8_t aic_iscr_pid21_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00200000) >> 21);
}

__INLINE void aic_iscr_pid21_setf(uint8_t pid21)
{
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid21 << 21);
}

__INLINE uint8_t aic_iscr_pid20_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00100000) >> 20);
}

__INLINE void aic_iscr_pid20_setf(uint8_t pid20)
{
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid20 << 20);
}

__INLINE uint8_t aic_iscr_pid19_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00080000) >> 19);
}

__INLINE void aic_iscr_pid19_setf(uint8_t pid19)
{
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid19 << 19);
}

__INLINE uint8_t aic_iscr_pid18_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00040000) >> 18);
}

__INLINE void aic_iscr_pid18_setf(uint8_t pid18)
{
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid18 << 18);
}

__INLINE uint8_t aic_iscr_pid17_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE void aic_iscr_pid17_setf(uint8_t pid17)
{
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid17 << 17);
}

__INLINE uint8_t aic_iscr_pid16_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE void aic_iscr_pid16_setf(uint8_t pid16)
{
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid16 << 16);
}

__INLINE uint8_t aic_iscr_pid15_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00008000) >> 15);
}

__INLINE void aic_iscr_pid15_setf(uint8_t pid15)
{
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid15 << 15);
}

__INLINE uint8_t aic_iscr_pid14_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00004000) >> 14);
}

__INLINE void aic_iscr_pid14_setf(uint8_t pid14)
{
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid14 << 14);
}

__INLINE uint8_t aic_iscr_pid13_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00002000) >> 13);
}

__INLINE void aic_iscr_pid13_setf(uint8_t pid13)
{
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid13 << 13);
}

__INLINE uint8_t aic_iscr_pid12_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00001000) >> 12);
}

__INLINE void aic_iscr_pid12_setf(uint8_t pid12)
{
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid12 << 12);
}

__INLINE uint8_t aic_iscr_pid11_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000800) >> 11);
}

__INLINE void aic_iscr_pid11_setf(uint8_t pid11)
{
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid11 << 11);
}

__INLINE uint8_t aic_iscr_pid10_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE void aic_iscr_pid10_setf(uint8_t pid10)
{
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid10 << 10);
}

__INLINE uint8_t aic_iscr_pid9_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE void aic_iscr_pid9_setf(uint8_t pid9)
{
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid9 << 9);
}

__INLINE uint8_t aic_iscr_pid8_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE void aic_iscr_pid8_setf(uint8_t pid8)
{
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid8 << 8);
}

__INLINE uint8_t aic_iscr_pid7_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE void aic_iscr_pid7_setf(uint8_t pid7)
{
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid7 << 7);
}

__INLINE uint8_t aic_iscr_pid6_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE void aic_iscr_pid6_setf(uint8_t pid6)
{
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid6 << 6);
}

__INLINE uint8_t aic_iscr_pid5_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE void aic_iscr_pid5_setf(uint8_t pid5)
{
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid5 << 5);
}

__INLINE uint8_t aic_iscr_pid4_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE void aic_iscr_pid4_setf(uint8_t pid4)
{
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid4 << 4);
}

__INLINE uint8_t aic_iscr_pid3_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE void aic_iscr_pid3_setf(uint8_t pid3)
{
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid3 << 3);
}

__INLINE uint8_t aic_iscr_pid2_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void aic_iscr_pid2_setf(uint8_t pid2)
{
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, pid2 << 2);
}

__INLINE uint8_t aic_iscr_sys_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void aic_iscr_sys_setf(uint8_t sys)
{
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, sys << 1);
}

__INLINE uint8_t aic_iscr_fiq_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_ISCR_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void aic_iscr_fiq_setf(uint8_t fiq)
{
    ASSERT_ERR(((fiq << 0) & ~0x00000001) == 0);
    sim_reg_aic_wr(AIC_ISCR_INDEX, fiq << 0);
}

/**
 * @brief AIC_EOICR register definition
 */
#define AIC_EOICR_INDEX  0x0000004C

__INLINE uint32_t aic_eoicr_get(void)
{
    return sim_reg_aic_rd(AIC_EOICR_INDEX);
}

__INLINE void aic_eoicr_clear(uint32_t value)
{
    sim_reg_aic_wr(AIC_EOICR_INDEX, value);
}

/**
 * @brief AIC_SPU register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00                 SIVR   0x0
 * </pre>
 */
#define AIC_SPU_INDEX  0x0000004D

__INLINE uint32_t aic_spu_get(void)
{
    return sim_reg_aic_rd(AIC_SPU_INDEX);
}

__INLINE void aic_spu_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_SPU_INDEX, value);
}

// field definitions
#define SIVR_MASK   0xFFFFFFFF
#define SIVR_LSB    0
#define SIVR_WIDTH  0x00000020

#define SIVR_RST    0x0

__INLINE uint32_t aic_spu_sivr_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_SPU_INDEX);
    ASSERT_ERR((localVal & ~0xFFFFFFFF) == 0);
    return (localVal >> 0);
}

__INLINE void aic_spu_sivr_setf(uint32_t sivr)
{
    ASSERT_ERR(((sivr << 0) & ~0xFFFFFFFF) == 0);
    sim_reg_aic_wr(AIC_SPU_INDEX, sivr << 0);
}

/**
 * @brief AIC_DCR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     01                 GMSK   0
 *     00                 PROT   0
 * </pre>
 */
#define AIC_DCR_INDEX  0x0000004E

__INLINE uint32_t aic_dcr_get(void)
{
    return sim_reg_aic_rd(AIC_DCR_INDEX);
}

__INLINE void aic_dcr_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_DCR_INDEX, value);
}

// field definitions
#define GMSK_BIT    0x00000002
#define GMSK_POS    1
#define PROT_BIT    0x00000001
#define PROT_POS    0

#define GMSK_RST    0x0
#define PROT_RST    0x0

__INLINE void aic_dcr_pack(uint8_t gmsk, uint8_t prot)
{
    ASSERT_ERR(((gmsk << 1) & ~0x00000002) == 0);
    ASSERT_ERR(((prot << 0) & ~0x00000001) == 0);
    sim_reg_aic_wr(AIC_DCR_INDEX,  (gmsk << 1) | (prot << 0));
}

__INLINE void aic_dcr_unpack(uint8_t* gmsk, uint8_t* prot)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_DCR_INDEX);

    *gmsk = (localVal & 0x00000002) >> 1;
    *prot = (localVal & 0x00000001) >> 0;
}

__INLINE uint8_t aic_dcr_gmsk_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_DCR_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void aic_dcr_gmsk_setf(uint8_t gmsk)
{
    ASSERT_ERR(((gmsk << 1) & ~0x00000002) == 0);
    sim_reg_aic_wr(AIC_DCR_INDEX, (sim_reg_aic_rd(AIC_DCR_INDEX) & ~0x00000002) | (gmsk << 1));
}

__INLINE uint8_t aic_dcr_prot_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_DCR_INDEX);
    return ((localVal & 0x00000001) >> 0);
}

__INLINE void aic_dcr_prot_setf(uint8_t prot)
{
    ASSERT_ERR(((prot << 0) & ~0x00000001) == 0);
    sim_reg_aic_wr(AIC_DCR_INDEX, (sim_reg_aic_rd(AIC_DCR_INDEX) & ~0x00000001) | (prot << 0));
}

/**
 * @brief AIC_FFER register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31                PID31   0
 *     30                PID30   0
 *     29                PID29   0
 *     28                PID28   0
 *     27                PID27   0
 *     26                PID26   0
 *     25                PID25   0
 *     24                PID24   0
 *     23                PID23   0
 *     22                PID22   0
 *     21                PID21   0
 *     20                PID20   0
 *     19                PID19   0
 *     18                PID18   0
 *     17                PID17   0
 *     16                PID16   0
 *     15                PID15   0
 *     14                PID14   0
 *     13                PID13   0
 *     12                PID12   0
 *     11                PID11   0
 *     10                PID10   0
 *     09                 PID9   0
 *     08                 PID8   0
 *     07                 PID7   0
 *     06                 PID6   0
 *     05                 PID5   0
 *     04                 PID4   0
 *     03                 PID3   0
 *     02                 PID2   0
 *     01                  SYS   0
 * </pre>
 */
#define AIC_FFER_INDEX  0x00000050

__INLINE uint32_t aic_ffer_get(void)
{
    return sim_reg_aic_rd(AIC_FFER_INDEX);
}

__INLINE void aic_ffer_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_FFER_INDEX, value);
}

// field definitions
#define PID31_BIT    0x80000000
#define PID31_POS    31
#define PID30_BIT    0x40000000
#define PID30_POS    30
#define PID29_BIT    0x20000000
#define PID29_POS    29
#define PID28_BIT    0x10000000
#define PID28_POS    28
#define PID27_BIT    0x08000000
#define PID27_POS    27
#define PID26_BIT    0x04000000
#define PID26_POS    26
#define PID25_BIT    0x02000000
#define PID25_POS    25
#define PID24_BIT    0x01000000
#define PID24_POS    24
#define PID23_BIT    0x00800000
#define PID23_POS    23
#define PID22_BIT    0x00400000
#define PID22_POS    22
#define PID21_BIT    0x00200000
#define PID21_POS    21
#define PID20_BIT    0x00100000
#define PID20_POS    20
#define PID19_BIT    0x00080000
#define PID19_POS    19
#define PID18_BIT    0x00040000
#define PID18_POS    18
#define PID17_BIT    0x00020000
#define PID17_POS    17
#define PID16_BIT    0x00010000
#define PID16_POS    16
#define PID15_BIT    0x00008000
#define PID15_POS    15
#define PID14_BIT    0x00004000
#define PID14_POS    14
#define PID13_BIT    0x00002000
#define PID13_POS    13
#define PID12_BIT    0x00001000
#define PID12_POS    12
#define PID11_BIT    0x00000800
#define PID11_POS    11
#define PID10_BIT    0x00000400
#define PID10_POS    10
#define PID9_BIT     0x00000200
#define PID9_POS     9
#define PID8_BIT     0x00000100
#define PID8_POS     8
#define PID7_BIT     0x00000080
#define PID7_POS     7
#define PID6_BIT     0x00000040
#define PID6_POS     6
#define PID5_BIT     0x00000020
#define PID5_POS     5
#define PID4_BIT     0x00000010
#define PID4_POS     4
#define PID3_BIT     0x00000008
#define PID3_POS     3
#define PID2_BIT     0x00000004
#define PID2_POS     2
#define SYS_BIT      0x00000002
#define SYS_POS      1

#define PID31_RST    0x0
#define PID30_RST    0x0
#define PID29_RST    0x0
#define PID28_RST    0x0
#define PID27_RST    0x0
#define PID26_RST    0x0
#define PID25_RST    0x0
#define PID24_RST    0x0
#define PID23_RST    0x0
#define PID22_RST    0x0
#define PID21_RST    0x0
#define PID20_RST    0x0
#define PID19_RST    0x0
#define PID18_RST    0x0
#define PID17_RST    0x0
#define PID16_RST    0x0
#define PID15_RST    0x0
#define PID14_RST    0x0
#define PID13_RST    0x0
#define PID12_RST    0x0
#define PID11_RST    0x0
#define PID10_RST    0x0
#define PID9_RST     0x0
#define PID8_RST     0x0
#define PID7_RST     0x0
#define PID6_RST     0x0
#define PID5_RST     0x0
#define PID4_RST     0x0
#define PID3_RST     0x0
#define PID2_RST     0x0
#define SYS_RST      0x0

__INLINE void aic_ffer_pack(uint8_t pid31, uint8_t pid30, uint8_t pid29, uint8_t pid28, uint8_t pid27, uint8_t pid26, uint8_t pid25, uint8_t pid24, uint8_t pid23, uint8_t pid22, uint8_t pid21, uint8_t pid20, uint8_t pid19, uint8_t pid18, uint8_t pid17, uint8_t pid16, uint8_t pid15, uint8_t pid14, uint8_t pid13, uint8_t pid12, uint8_t pid11, uint8_t pid10, uint8_t pid9, uint8_t pid8, uint8_t pid7, uint8_t pid6, uint8_t pid5, uint8_t pid4, uint8_t pid3, uint8_t pid2, uint8_t sys)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX,  (pid31 << 31) | (pid30 << 30) | (pid29 << 29) | (pid28 << 28) | (pid27 << 27) | (pid26 << 26) | (pid25 << 25) | (pid24 << 24) | (pid23 << 23) | (pid22 << 22) | (pid21 << 21) | (pid20 << 20) | (pid19 << 19) | (pid18 << 18) | (pid17 << 17) | (pid16 << 16) | (pid15 << 15) | (pid14 << 14) | (pid13 << 13) | (pid12 << 12) | (pid11 << 11) | (pid10 << 10) | (pid9 << 9) | (pid8 << 8) | (pid7 << 7) | (pid6 << 6) | (pid5 << 5) | (pid4 << 4) | (pid3 << 3) | (pid2 << 2) | (sys << 1));
}

__INLINE void aic_ffer_unpack(uint8_t* pid31, uint8_t* pid30, uint8_t* pid29, uint8_t* pid28, uint8_t* pid27, uint8_t* pid26, uint8_t* pid25, uint8_t* pid24, uint8_t* pid23, uint8_t* pid22, uint8_t* pid21, uint8_t* pid20, uint8_t* pid19, uint8_t* pid18, uint8_t* pid17, uint8_t* pid16, uint8_t* pid15, uint8_t* pid14, uint8_t* pid13, uint8_t* pid12, uint8_t* pid11, uint8_t* pid10, uint8_t* pid9, uint8_t* pid8, uint8_t* pid7, uint8_t* pid6, uint8_t* pid5, uint8_t* pid4, uint8_t* pid3, uint8_t* pid2, uint8_t* sys)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);

    *pid31 = (localVal & 0x80000000) >> 31;
    *pid30 = (localVal & 0x40000000) >> 30;
    *pid29 = (localVal & 0x20000000) >> 29;
    *pid28 = (localVal & 0x10000000) >> 28;
    *pid27 = (localVal & 0x08000000) >> 27;
    *pid26 = (localVal & 0x04000000) >> 26;
    *pid25 = (localVal & 0x02000000) >> 25;
    *pid24 = (localVal & 0x01000000) >> 24;
    *pid23 = (localVal & 0x00800000) >> 23;
    *pid22 = (localVal & 0x00400000) >> 22;
    *pid21 = (localVal & 0x00200000) >> 21;
    *pid20 = (localVal & 0x00100000) >> 20;
    *pid19 = (localVal & 0x00080000) >> 19;
    *pid18 = (localVal & 0x00040000) >> 18;
    *pid17 = (localVal & 0x00020000) >> 17;
    *pid16 = (localVal & 0x00010000) >> 16;
    *pid15 = (localVal & 0x00008000) >> 15;
    *pid14 = (localVal & 0x00004000) >> 14;
    *pid13 = (localVal & 0x00002000) >> 13;
    *pid12 = (localVal & 0x00001000) >> 12;
    *pid11 = (localVal & 0x00000800) >> 11;
    *pid10 = (localVal & 0x00000400) >> 10;
    *pid9 = (localVal & 0x00000200) >> 9;
    *pid8 = (localVal & 0x00000100) >> 8;
    *pid7 = (localVal & 0x00000080) >> 7;
    *pid6 = (localVal & 0x00000040) >> 6;
    *pid5 = (localVal & 0x00000020) >> 5;
    *pid4 = (localVal & 0x00000010) >> 4;
    *pid3 = (localVal & 0x00000008) >> 3;
    *pid2 = (localVal & 0x00000004) >> 2;
    *sys = (localVal & 0x00000002) >> 1;
}

__INLINE uint8_t aic_ffer_pid31_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x80000000) >> 31);
}

__INLINE void aic_ffer_pid31_setf(uint8_t pid31)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid31 << 31);
}

__INLINE uint8_t aic_ffer_pid30_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x40000000) >> 30);
}

__INLINE void aic_ffer_pid30_setf(uint8_t pid30)
{
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid30 << 30);
}

__INLINE uint8_t aic_ffer_pid29_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x20000000) >> 29);
}

__INLINE void aic_ffer_pid29_setf(uint8_t pid29)
{
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid29 << 29);
}

__INLINE uint8_t aic_ffer_pid28_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x10000000) >> 28);
}

__INLINE void aic_ffer_pid28_setf(uint8_t pid28)
{
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid28 << 28);
}

__INLINE uint8_t aic_ffer_pid27_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x08000000) >> 27);
}

__INLINE void aic_ffer_pid27_setf(uint8_t pid27)
{
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid27 << 27);
}

__INLINE uint8_t aic_ffer_pid26_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x04000000) >> 26);
}

__INLINE void aic_ffer_pid26_setf(uint8_t pid26)
{
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid26 << 26);
}

__INLINE uint8_t aic_ffer_pid25_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x02000000) >> 25);
}

__INLINE void aic_ffer_pid25_setf(uint8_t pid25)
{
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid25 << 25);
}

__INLINE uint8_t aic_ffer_pid24_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x01000000) >> 24);
}

__INLINE void aic_ffer_pid24_setf(uint8_t pid24)
{
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid24 << 24);
}

__INLINE uint8_t aic_ffer_pid23_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00800000) >> 23);
}

__INLINE void aic_ffer_pid23_setf(uint8_t pid23)
{
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid23 << 23);
}

__INLINE uint8_t aic_ffer_pid22_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00400000) >> 22);
}

__INLINE void aic_ffer_pid22_setf(uint8_t pid22)
{
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid22 << 22);
}

__INLINE uint8_t aic_ffer_pid21_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00200000) >> 21);
}

__INLINE void aic_ffer_pid21_setf(uint8_t pid21)
{
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid21 << 21);
}

__INLINE uint8_t aic_ffer_pid20_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00100000) >> 20);
}

__INLINE void aic_ffer_pid20_setf(uint8_t pid20)
{
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid20 << 20);
}

__INLINE uint8_t aic_ffer_pid19_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00080000) >> 19);
}

__INLINE void aic_ffer_pid19_setf(uint8_t pid19)
{
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid19 << 19);
}

__INLINE uint8_t aic_ffer_pid18_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00040000) >> 18);
}

__INLINE void aic_ffer_pid18_setf(uint8_t pid18)
{
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid18 << 18);
}

__INLINE uint8_t aic_ffer_pid17_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE void aic_ffer_pid17_setf(uint8_t pid17)
{
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid17 << 17);
}

__INLINE uint8_t aic_ffer_pid16_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE void aic_ffer_pid16_setf(uint8_t pid16)
{
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid16 << 16);
}

__INLINE uint8_t aic_ffer_pid15_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00008000) >> 15);
}

__INLINE void aic_ffer_pid15_setf(uint8_t pid15)
{
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid15 << 15);
}

__INLINE uint8_t aic_ffer_pid14_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00004000) >> 14);
}

__INLINE void aic_ffer_pid14_setf(uint8_t pid14)
{
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid14 << 14);
}

__INLINE uint8_t aic_ffer_pid13_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00002000) >> 13);
}

__INLINE void aic_ffer_pid13_setf(uint8_t pid13)
{
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid13 << 13);
}

__INLINE uint8_t aic_ffer_pid12_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00001000) >> 12);
}

__INLINE void aic_ffer_pid12_setf(uint8_t pid12)
{
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid12 << 12);
}

__INLINE uint8_t aic_ffer_pid11_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00000800) >> 11);
}

__INLINE void aic_ffer_pid11_setf(uint8_t pid11)
{
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid11 << 11);
}

__INLINE uint8_t aic_ffer_pid10_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE void aic_ffer_pid10_setf(uint8_t pid10)
{
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid10 << 10);
}

__INLINE uint8_t aic_ffer_pid9_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE void aic_ffer_pid9_setf(uint8_t pid9)
{
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid9 << 9);
}

__INLINE uint8_t aic_ffer_pid8_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE void aic_ffer_pid8_setf(uint8_t pid8)
{
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid8 << 8);
}

__INLINE uint8_t aic_ffer_pid7_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE void aic_ffer_pid7_setf(uint8_t pid7)
{
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid7 << 7);
}

__INLINE uint8_t aic_ffer_pid6_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE void aic_ffer_pid6_setf(uint8_t pid6)
{
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid6 << 6);
}

__INLINE uint8_t aic_ffer_pid5_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE void aic_ffer_pid5_setf(uint8_t pid5)
{
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid5 << 5);
}

__INLINE uint8_t aic_ffer_pid4_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE void aic_ffer_pid4_setf(uint8_t pid4)
{
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid4 << 4);
}

__INLINE uint8_t aic_ffer_pid3_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE void aic_ffer_pid3_setf(uint8_t pid3)
{
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid3 << 3);
}

__INLINE uint8_t aic_ffer_pid2_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void aic_ffer_pid2_setf(uint8_t pid2)
{
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, pid2 << 2);
}

__INLINE uint8_t aic_ffer_sys_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFER_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void aic_ffer_sys_setf(uint8_t sys)
{
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    sim_reg_aic_wr(AIC_FFER_INDEX, sys << 1);
}

/**
 * @brief AIC_FFDR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31                PID31   0
 *     30                PID30   0
 *     29                PID29   0
 *     28                PID28   0
 *     27                PID27   0
 *     26                PID26   0
 *     25                PID25   0
 *     24                PID24   0
 *     23                PID23   0
 *     22                PID22   0
 *     21                PID21   0
 *     20                PID20   0
 *     19                PID19   0
 *     18                PID18   0
 *     17                PID17   0
 *     16                PID16   0
 *     15                PID15   0
 *     14                PID14   0
 *     13                PID13   0
 *     12                PID12   0
 *     11                PID11   0
 *     10                PID10   0
 *     09                 PID9   0
 *     08                 PID8   0
 *     07                 PID7   0
 *     06                 PID6   0
 *     05                 PID5   0
 *     04                 PID4   0
 *     03                 PID3   0
 *     02                 PID2   0
 *     01                  SYS   0
 * </pre>
 */
#define AIC_FFDR_INDEX  0x00000051

__INLINE uint32_t aic_ffdr_get(void)
{
    return sim_reg_aic_rd(AIC_FFDR_INDEX);
}

__INLINE void aic_ffdr_clear(uint32_t value)
{
    sim_reg_aic_wr(AIC_FFDR_INDEX, value);
}

// field definitions
#define PID31_BIT    0x80000000
#define PID31_POS    31
#define PID30_BIT    0x40000000
#define PID30_POS    30
#define PID29_BIT    0x20000000
#define PID29_POS    29
#define PID28_BIT    0x10000000
#define PID28_POS    28
#define PID27_BIT    0x08000000
#define PID27_POS    27
#define PID26_BIT    0x04000000
#define PID26_POS    26
#define PID25_BIT    0x02000000
#define PID25_POS    25
#define PID24_BIT    0x01000000
#define PID24_POS    24
#define PID23_BIT    0x00800000
#define PID23_POS    23
#define PID22_BIT    0x00400000
#define PID22_POS    22
#define PID21_BIT    0x00200000
#define PID21_POS    21
#define PID20_BIT    0x00100000
#define PID20_POS    20
#define PID19_BIT    0x00080000
#define PID19_POS    19
#define PID18_BIT    0x00040000
#define PID18_POS    18
#define PID17_BIT    0x00020000
#define PID17_POS    17
#define PID16_BIT    0x00010000
#define PID16_POS    16
#define PID15_BIT    0x00008000
#define PID15_POS    15
#define PID14_BIT    0x00004000
#define PID14_POS    14
#define PID13_BIT    0x00002000
#define PID13_POS    13
#define PID12_BIT    0x00001000
#define PID12_POS    12
#define PID11_BIT    0x00000800
#define PID11_POS    11
#define PID10_BIT    0x00000400
#define PID10_POS    10
#define PID9_BIT     0x00000200
#define PID9_POS     9
#define PID8_BIT     0x00000100
#define PID8_POS     8
#define PID7_BIT     0x00000080
#define PID7_POS     7
#define PID6_BIT     0x00000040
#define PID6_POS     6
#define PID5_BIT     0x00000020
#define PID5_POS     5
#define PID4_BIT     0x00000010
#define PID4_POS     4
#define PID3_BIT     0x00000008
#define PID3_POS     3
#define PID2_BIT     0x00000004
#define PID2_POS     2
#define SYS_BIT      0x00000002
#define SYS_POS      1

#define PID31_RST    0x0
#define PID30_RST    0x0
#define PID29_RST    0x0
#define PID28_RST    0x0
#define PID27_RST    0x0
#define PID26_RST    0x0
#define PID25_RST    0x0
#define PID24_RST    0x0
#define PID23_RST    0x0
#define PID22_RST    0x0
#define PID21_RST    0x0
#define PID20_RST    0x0
#define PID19_RST    0x0
#define PID18_RST    0x0
#define PID17_RST    0x0
#define PID16_RST    0x0
#define PID15_RST    0x0
#define PID14_RST    0x0
#define PID13_RST    0x0
#define PID12_RST    0x0
#define PID11_RST    0x0
#define PID10_RST    0x0
#define PID9_RST     0x0
#define PID8_RST     0x0
#define PID7_RST     0x0
#define PID6_RST     0x0
#define PID5_RST     0x0
#define PID4_RST     0x0
#define PID3_RST     0x0
#define PID2_RST     0x0
#define SYS_RST      0x0

__INLINE void aic_ffdr_pack(uint8_t pid31, uint8_t pid30, uint8_t pid29, uint8_t pid28, uint8_t pid27, uint8_t pid26, uint8_t pid25, uint8_t pid24, uint8_t pid23, uint8_t pid22, uint8_t pid21, uint8_t pid20, uint8_t pid19, uint8_t pid18, uint8_t pid17, uint8_t pid16, uint8_t pid15, uint8_t pid14, uint8_t pid13, uint8_t pid12, uint8_t pid11, uint8_t pid10, uint8_t pid9, uint8_t pid8, uint8_t pid7, uint8_t pid6, uint8_t pid5, uint8_t pid4, uint8_t pid3, uint8_t pid2, uint8_t sys)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX,  (pid31 << 31) | (pid30 << 30) | (pid29 << 29) | (pid28 << 28) | (pid27 << 27) | (pid26 << 26) | (pid25 << 25) | (pid24 << 24) | (pid23 << 23) | (pid22 << 22) | (pid21 << 21) | (pid20 << 20) | (pid19 << 19) | (pid18 << 18) | (pid17 << 17) | (pid16 << 16) | (pid15 << 15) | (pid14 << 14) | (pid13 << 13) | (pid12 << 12) | (pid11 << 11) | (pid10 << 10) | (pid9 << 9) | (pid8 << 8) | (pid7 << 7) | (pid6 << 6) | (pid5 << 5) | (pid4 << 4) | (pid3 << 3) | (pid2 << 2) | (sys << 1));
}

__INLINE void aic_ffdr_unpack(uint8_t* pid31, uint8_t* pid30, uint8_t* pid29, uint8_t* pid28, uint8_t* pid27, uint8_t* pid26, uint8_t* pid25, uint8_t* pid24, uint8_t* pid23, uint8_t* pid22, uint8_t* pid21, uint8_t* pid20, uint8_t* pid19, uint8_t* pid18, uint8_t* pid17, uint8_t* pid16, uint8_t* pid15, uint8_t* pid14, uint8_t* pid13, uint8_t* pid12, uint8_t* pid11, uint8_t* pid10, uint8_t* pid9, uint8_t* pid8, uint8_t* pid7, uint8_t* pid6, uint8_t* pid5, uint8_t* pid4, uint8_t* pid3, uint8_t* pid2, uint8_t* sys)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);

    *pid31 = (localVal & 0x80000000) >> 31;
    *pid30 = (localVal & 0x40000000) >> 30;
    *pid29 = (localVal & 0x20000000) >> 29;
    *pid28 = (localVal & 0x10000000) >> 28;
    *pid27 = (localVal & 0x08000000) >> 27;
    *pid26 = (localVal & 0x04000000) >> 26;
    *pid25 = (localVal & 0x02000000) >> 25;
    *pid24 = (localVal & 0x01000000) >> 24;
    *pid23 = (localVal & 0x00800000) >> 23;
    *pid22 = (localVal & 0x00400000) >> 22;
    *pid21 = (localVal & 0x00200000) >> 21;
    *pid20 = (localVal & 0x00100000) >> 20;
    *pid19 = (localVal & 0x00080000) >> 19;
    *pid18 = (localVal & 0x00040000) >> 18;
    *pid17 = (localVal & 0x00020000) >> 17;
    *pid16 = (localVal & 0x00010000) >> 16;
    *pid15 = (localVal & 0x00008000) >> 15;
    *pid14 = (localVal & 0x00004000) >> 14;
    *pid13 = (localVal & 0x00002000) >> 13;
    *pid12 = (localVal & 0x00001000) >> 12;
    *pid11 = (localVal & 0x00000800) >> 11;
    *pid10 = (localVal & 0x00000400) >> 10;
    *pid9 = (localVal & 0x00000200) >> 9;
    *pid8 = (localVal & 0x00000100) >> 8;
    *pid7 = (localVal & 0x00000080) >> 7;
    *pid6 = (localVal & 0x00000040) >> 6;
    *pid5 = (localVal & 0x00000020) >> 5;
    *pid4 = (localVal & 0x00000010) >> 4;
    *pid3 = (localVal & 0x00000008) >> 3;
    *pid2 = (localVal & 0x00000004) >> 2;
    *sys = (localVal & 0x00000002) >> 1;
}

__INLINE uint8_t aic_ffdr_pid31_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x80000000) >> 31);
}

__INLINE void aic_ffdr_pid31_clearf(uint8_t pid31)
{
    ASSERT_ERR(((pid31 << 31) & ~0x80000000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid31 << 31);
}

__INLINE uint8_t aic_ffdr_pid30_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x40000000) >> 30);
}

__INLINE void aic_ffdr_pid30_clearf(uint8_t pid30)
{
    ASSERT_ERR(((pid30 << 30) & ~0x40000000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid30 << 30);
}

__INLINE uint8_t aic_ffdr_pid29_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x20000000) >> 29);
}

__INLINE void aic_ffdr_pid29_clearf(uint8_t pid29)
{
    ASSERT_ERR(((pid29 << 29) & ~0x20000000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid29 << 29);
}

__INLINE uint8_t aic_ffdr_pid28_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x10000000) >> 28);
}

__INLINE void aic_ffdr_pid28_clearf(uint8_t pid28)
{
    ASSERT_ERR(((pid28 << 28) & ~0x10000000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid28 << 28);
}

__INLINE uint8_t aic_ffdr_pid27_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x08000000) >> 27);
}

__INLINE void aic_ffdr_pid27_clearf(uint8_t pid27)
{
    ASSERT_ERR(((pid27 << 27) & ~0x08000000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid27 << 27);
}

__INLINE uint8_t aic_ffdr_pid26_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x04000000) >> 26);
}

__INLINE void aic_ffdr_pid26_clearf(uint8_t pid26)
{
    ASSERT_ERR(((pid26 << 26) & ~0x04000000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid26 << 26);
}

__INLINE uint8_t aic_ffdr_pid25_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x02000000) >> 25);
}

__INLINE void aic_ffdr_pid25_clearf(uint8_t pid25)
{
    ASSERT_ERR(((pid25 << 25) & ~0x02000000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid25 << 25);
}

__INLINE uint8_t aic_ffdr_pid24_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x01000000) >> 24);
}

__INLINE void aic_ffdr_pid24_clearf(uint8_t pid24)
{
    ASSERT_ERR(((pid24 << 24) & ~0x01000000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid24 << 24);
}

__INLINE uint8_t aic_ffdr_pid23_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00800000) >> 23);
}

__INLINE void aic_ffdr_pid23_clearf(uint8_t pid23)
{
    ASSERT_ERR(((pid23 << 23) & ~0x00800000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid23 << 23);
}

__INLINE uint8_t aic_ffdr_pid22_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00400000) >> 22);
}

__INLINE void aic_ffdr_pid22_clearf(uint8_t pid22)
{
    ASSERT_ERR(((pid22 << 22) & ~0x00400000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid22 << 22);
}

__INLINE uint8_t aic_ffdr_pid21_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00200000) >> 21);
}

__INLINE void aic_ffdr_pid21_clearf(uint8_t pid21)
{
    ASSERT_ERR(((pid21 << 21) & ~0x00200000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid21 << 21);
}

__INLINE uint8_t aic_ffdr_pid20_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00100000) >> 20);
}

__INLINE void aic_ffdr_pid20_clearf(uint8_t pid20)
{
    ASSERT_ERR(((pid20 << 20) & ~0x00100000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid20 << 20);
}

__INLINE uint8_t aic_ffdr_pid19_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00080000) >> 19);
}

__INLINE void aic_ffdr_pid19_clearf(uint8_t pid19)
{
    ASSERT_ERR(((pid19 << 19) & ~0x00080000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid19 << 19);
}

__INLINE uint8_t aic_ffdr_pid18_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00040000) >> 18);
}

__INLINE void aic_ffdr_pid18_clearf(uint8_t pid18)
{
    ASSERT_ERR(((pid18 << 18) & ~0x00040000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid18 << 18);
}

__INLINE uint8_t aic_ffdr_pid17_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE void aic_ffdr_pid17_clearf(uint8_t pid17)
{
    ASSERT_ERR(((pid17 << 17) & ~0x00020000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid17 << 17);
}

__INLINE uint8_t aic_ffdr_pid16_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE void aic_ffdr_pid16_clearf(uint8_t pid16)
{
    ASSERT_ERR(((pid16 << 16) & ~0x00010000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid16 << 16);
}

__INLINE uint8_t aic_ffdr_pid15_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00008000) >> 15);
}

__INLINE void aic_ffdr_pid15_clearf(uint8_t pid15)
{
    ASSERT_ERR(((pid15 << 15) & ~0x00008000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid15 << 15);
}

__INLINE uint8_t aic_ffdr_pid14_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00004000) >> 14);
}

__INLINE void aic_ffdr_pid14_clearf(uint8_t pid14)
{
    ASSERT_ERR(((pid14 << 14) & ~0x00004000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid14 << 14);
}

__INLINE uint8_t aic_ffdr_pid13_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00002000) >> 13);
}

__INLINE void aic_ffdr_pid13_clearf(uint8_t pid13)
{
    ASSERT_ERR(((pid13 << 13) & ~0x00002000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid13 << 13);
}

__INLINE uint8_t aic_ffdr_pid12_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00001000) >> 12);
}

__INLINE void aic_ffdr_pid12_clearf(uint8_t pid12)
{
    ASSERT_ERR(((pid12 << 12) & ~0x00001000) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid12 << 12);
}

__INLINE uint8_t aic_ffdr_pid11_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00000800) >> 11);
}

__INLINE void aic_ffdr_pid11_clearf(uint8_t pid11)
{
    ASSERT_ERR(((pid11 << 11) & ~0x00000800) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid11 << 11);
}

__INLINE uint8_t aic_ffdr_pid10_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE void aic_ffdr_pid10_clearf(uint8_t pid10)
{
    ASSERT_ERR(((pid10 << 10) & ~0x00000400) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid10 << 10);
}

__INLINE uint8_t aic_ffdr_pid9_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE void aic_ffdr_pid9_clearf(uint8_t pid9)
{
    ASSERT_ERR(((pid9 << 9) & ~0x00000200) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid9 << 9);
}

__INLINE uint8_t aic_ffdr_pid8_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE void aic_ffdr_pid8_clearf(uint8_t pid8)
{
    ASSERT_ERR(((pid8 << 8) & ~0x00000100) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid8 << 8);
}

__INLINE uint8_t aic_ffdr_pid7_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE void aic_ffdr_pid7_clearf(uint8_t pid7)
{
    ASSERT_ERR(((pid7 << 7) & ~0x00000080) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid7 << 7);
}

__INLINE uint8_t aic_ffdr_pid6_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE void aic_ffdr_pid6_clearf(uint8_t pid6)
{
    ASSERT_ERR(((pid6 << 6) & ~0x00000040) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid6 << 6);
}

__INLINE uint8_t aic_ffdr_pid5_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE void aic_ffdr_pid5_clearf(uint8_t pid5)
{
    ASSERT_ERR(((pid5 << 5) & ~0x00000020) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid5 << 5);
}

__INLINE uint8_t aic_ffdr_pid4_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE void aic_ffdr_pid4_clearf(uint8_t pid4)
{
    ASSERT_ERR(((pid4 << 4) & ~0x00000010) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid4 << 4);
}

__INLINE uint8_t aic_ffdr_pid3_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE void aic_ffdr_pid3_clearf(uint8_t pid3)
{
    ASSERT_ERR(((pid3 << 3) & ~0x00000008) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid3 << 3);
}

__INLINE uint8_t aic_ffdr_pid2_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE void aic_ffdr_pid2_clearf(uint8_t pid2)
{
    ASSERT_ERR(((pid2 << 2) & ~0x00000004) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, pid2 << 2);
}

__INLINE uint8_t aic_ffdr_sys_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFDR_INDEX);
    return ((localVal & 0x00000002) >> 1);
}

__INLINE void aic_ffdr_sys_clearf(uint8_t sys)
{
    ASSERT_ERR(((sys << 1) & ~0x00000002) == 0);
    sim_reg_aic_wr(AIC_FFDR_INDEX, sys << 1);
}

/**
 * @brief AIC_FFSR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31                PID31   0
 *     30                PID30   0
 *     29                PID29   0
 *     28                PID28   0
 *     27                PID27   0
 *     26                PID26   0
 *     25                PID25   0
 *     24                PID24   0
 *     23                PID23   0
 *     22                PID22   0
 *     21                PID21   0
 *     20                PID20   0
 *     19                PID19   0
 *     18                PID18   0
 *     17                PID17   0
 *     16                PID16   0
 *     15                PID15   0
 *     14                PID14   0
 *     13                PID13   0
 *     12                PID12   0
 *     11                PID11   0
 *     10                PID10   0
 *     09                 PID9   0
 *     08                 PID8   0
 *     07                 PID7   0
 *     06                 PID6   0
 *     05                 PID5   0
 *     04                 PID4   0
 *     03                 PID3   0
 *     02                 PID2   0
 *     01                  SYS   0
 * </pre>
 */
#define AIC_FFSR_INDEX  0x00000052

__INLINE uint32_t aic_ffsr_get(void)
{
    return sim_reg_aic_rd(AIC_FFSR_INDEX);
}

__INLINE void aic_ffsr_set(uint32_t value)
{
    sim_reg_aic_wr(AIC_FFSR_INDEX, value);
}

// field definitions
#define PID31_BIT    0x80000000
#define PID31_POS    31
#define PID30_BIT    0x40000000
#define PID30_POS    30
#define PID29_BIT    0x20000000
#define PID29_POS    29
#define PID28_BIT    0x10000000
#define PID28_POS    28
#define PID27_BIT    0x08000000
#define PID27_POS    27
#define PID26_BIT    0x04000000
#define PID26_POS    26
#define PID25_BIT    0x02000000
#define PID25_POS    25
#define PID24_BIT    0x01000000
#define PID24_POS    24
#define PID23_BIT    0x00800000
#define PID23_POS    23
#define PID22_BIT    0x00400000
#define PID22_POS    22
#define PID21_BIT    0x00200000
#define PID21_POS    21
#define PID20_BIT    0x00100000
#define PID20_POS    20
#define PID19_BIT    0x00080000
#define PID19_POS    19
#define PID18_BIT    0x00040000
#define PID18_POS    18
#define PID17_BIT    0x00020000
#define PID17_POS    17
#define PID16_BIT    0x00010000
#define PID16_POS    16
#define PID15_BIT    0x00008000
#define PID15_POS    15
#define PID14_BIT    0x00004000
#define PID14_POS    14
#define PID13_BIT    0x00002000
#define PID13_POS    13
#define PID12_BIT    0x00001000
#define PID12_POS    12
#define PID11_BIT    0x00000800
#define PID11_POS    11
#define PID10_BIT    0x00000400
#define PID10_POS    10
#define PID9_BIT     0x00000200
#define PID9_POS     9
#define PID8_BIT     0x00000100
#define PID8_POS     8
#define PID7_BIT     0x00000080
#define PID7_POS     7
#define PID6_BIT     0x00000040
#define PID6_POS     6
#define PID5_BIT     0x00000020
#define PID5_POS     5
#define PID4_BIT     0x00000010
#define PID4_POS     4
#define PID3_BIT     0x00000008
#define PID3_POS     3
#define PID2_BIT     0x00000004
#define PID2_POS     2
#define SYS_BIT      0x00000002
#define SYS_POS      1

#define PID31_RST    0x0
#define PID30_RST    0x0
#define PID29_RST    0x0
#define PID28_RST    0x0
#define PID27_RST    0x0
#define PID26_RST    0x0
#define PID25_RST    0x0
#define PID24_RST    0x0
#define PID23_RST    0x0
#define PID22_RST    0x0
#define PID21_RST    0x0
#define PID20_RST    0x0
#define PID19_RST    0x0
#define PID18_RST    0x0
#define PID17_RST    0x0
#define PID16_RST    0x0
#define PID15_RST    0x0
#define PID14_RST    0x0
#define PID13_RST    0x0
#define PID12_RST    0x0
#define PID11_RST    0x0
#define PID10_RST    0x0
#define PID9_RST     0x0
#define PID8_RST     0x0
#define PID7_RST     0x0
#define PID6_RST     0x0
#define PID5_RST     0x0
#define PID4_RST     0x0
#define PID3_RST     0x0
#define PID2_RST     0x0
#define SYS_RST      0x0

__INLINE void aic_ffsr_unpack(uint8_t* pid31, uint8_t* pid30, uint8_t* pid29, uint8_t* pid28, uint8_t* pid27, uint8_t* pid26, uint8_t* pid25, uint8_t* pid24, uint8_t* pid23, uint8_t* pid22, uint8_t* pid21, uint8_t* pid20, uint8_t* pid19, uint8_t* pid18, uint8_t* pid17, uint8_t* pid16, uint8_t* pid15, uint8_t* pid14, uint8_t* pid13, uint8_t* pid12, uint8_t* pid11, uint8_t* pid10, uint8_t* pid9, uint8_t* pid8, uint8_t* pid7, uint8_t* pid6, uint8_t* pid5, uint8_t* pid4, uint8_t* pid3, uint8_t* pid2, uint8_t* sys)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);

    *pid31 = (localVal & 0x80000000) >> 31;
    *pid30 = (localVal & 0x40000000) >> 30;
    *pid29 = (localVal & 0x20000000) >> 29;
    *pid28 = (localVal & 0x10000000) >> 28;
    *pid27 = (localVal & 0x08000000) >> 27;
    *pid26 = (localVal & 0x04000000) >> 26;
    *pid25 = (localVal & 0x02000000) >> 25;
    *pid24 = (localVal & 0x01000000) >> 24;
    *pid23 = (localVal & 0x00800000) >> 23;
    *pid22 = (localVal & 0x00400000) >> 22;
    *pid21 = (localVal & 0x00200000) >> 21;
    *pid20 = (localVal & 0x00100000) >> 20;
    *pid19 = (localVal & 0x00080000) >> 19;
    *pid18 = (localVal & 0x00040000) >> 18;
    *pid17 = (localVal & 0x00020000) >> 17;
    *pid16 = (localVal & 0x00010000) >> 16;
    *pid15 = (localVal & 0x00008000) >> 15;
    *pid14 = (localVal & 0x00004000) >> 14;
    *pid13 = (localVal & 0x00002000) >> 13;
    *pid12 = (localVal & 0x00001000) >> 12;
    *pid11 = (localVal & 0x00000800) >> 11;
    *pid10 = (localVal & 0x00000400) >> 10;
    *pid9 = (localVal & 0x00000200) >> 9;
    *pid8 = (localVal & 0x00000100) >> 8;
    *pid7 = (localVal & 0x00000080) >> 7;
    *pid6 = (localVal & 0x00000040) >> 6;
    *pid5 = (localVal & 0x00000020) >> 5;
    *pid4 = (localVal & 0x00000010) >> 4;
    *pid3 = (localVal & 0x00000008) >> 3;
    *pid2 = (localVal & 0x00000004) >> 2;
    *sys = (localVal & 0x00000002) >> 1;
}

__INLINE uint8_t aic_ffsr_pid31_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x80000000) >> 31);
}

__INLINE uint8_t aic_ffsr_pid30_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x40000000) >> 30);
}

__INLINE uint8_t aic_ffsr_pid29_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x20000000) >> 29);
}

__INLINE uint8_t aic_ffsr_pid28_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x10000000) >> 28);
}

__INLINE uint8_t aic_ffsr_pid27_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x08000000) >> 27);
}

__INLINE uint8_t aic_ffsr_pid26_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x04000000) >> 26);
}

__INLINE uint8_t aic_ffsr_pid25_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x02000000) >> 25);
}

__INLINE uint8_t aic_ffsr_pid24_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x01000000) >> 24);
}

__INLINE uint8_t aic_ffsr_pid23_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00800000) >> 23);
}

__INLINE uint8_t aic_ffsr_pid22_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00400000) >> 22);
}

__INLINE uint8_t aic_ffsr_pid21_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00200000) >> 21);
}

__INLINE uint8_t aic_ffsr_pid20_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00100000) >> 20);
}

__INLINE uint8_t aic_ffsr_pid19_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00080000) >> 19);
}

__INLINE uint8_t aic_ffsr_pid18_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00040000) >> 18);
}

__INLINE uint8_t aic_ffsr_pid17_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00020000) >> 17);
}

__INLINE uint8_t aic_ffsr_pid16_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00010000) >> 16);
}

__INLINE uint8_t aic_ffsr_pid15_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00008000) >> 15);
}

__INLINE uint8_t aic_ffsr_pid14_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00004000) >> 14);
}

__INLINE uint8_t aic_ffsr_pid13_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00002000) >> 13);
}

__INLINE uint8_t aic_ffsr_pid12_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00001000) >> 12);
}

__INLINE uint8_t aic_ffsr_pid11_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00000800) >> 11);
}

__INLINE uint8_t aic_ffsr_pid10_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00000400) >> 10);
}

__INLINE uint8_t aic_ffsr_pid9_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00000200) >> 9);
}

__INLINE uint8_t aic_ffsr_pid8_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00000100) >> 8);
}

__INLINE uint8_t aic_ffsr_pid7_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00000080) >> 7);
}

__INLINE uint8_t aic_ffsr_pid6_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00000040) >> 6);
}

__INLINE uint8_t aic_ffsr_pid5_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00000020) >> 5);
}

__INLINE uint8_t aic_ffsr_pid4_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00000010) >> 4);
}

__INLINE uint8_t aic_ffsr_pid3_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00000008) >> 3);
}

__INLINE uint8_t aic_ffsr_pid2_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00000004) >> 2);
}

__INLINE uint8_t aic_ffsr_sys_getf(void)
{
    uint32_t localVal = sim_reg_aic_rd(AIC_FFSR_INDEX);
    return ((localVal & 0x00000002) >> 1);
}


#endif // _REG_AIC_H_

