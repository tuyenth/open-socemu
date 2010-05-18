#ifndef _REG_ITC_H_
#define _REG_ITC_H_

#include <stdint.h>
#include "compiler.h"
#include "arch.h"
#include "reg_sim.h"

#define REG_ITC_BASE_ADDR 0x80020000

#define REG_ITC_COUNT 16

#define REG_ITC_DECODING_MASK 0x0000003F

/**
 * @brief INTCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     20                 NIAD   0
 *     19                 FIAD   0
 * </pre>
 */
#define INTCNTL_INDEX  0x00000000

__INLINE uint32_t intcntl_get(void)
{
    return sim_reg_itc_rd(INTCNTL_INDEX);
}

__INLINE void intcntl_set(uint32_t value)
{
    sim_reg_itc_wr(INTCNTL_INDEX, value);
}

// field definitions
#define NIAD_BIT    0x00100000
#define NIAD_POS    20
#define FIAD_BIT    0x00080000
#define FIAD_POS    19

#define NIAD_RST    0x0
#define FIAD_RST    0x0

__INLINE void intcntl_pack(uint8_t niad, uint8_t fiad)
{
    ASSERT_ERR(((niad << 20) & ~0x00100000) == 0);
    ASSERT_ERR(((fiad << 19) & ~0x00080000) == 0);
    sim_reg_itc_wr(INTCNTL_INDEX,  (niad << 20) | (fiad << 19));
}

__INLINE void intcntl_unpack(uint8_t* niad, uint8_t* fiad)
{
    uint32_t localVal = sim_reg_itc_rd(INTCNTL_INDEX);

    *niad = (localVal & 0x00100000) >> 20;
    *fiad = (localVal & 0x00080000) >> 19;
}

__INLINE uint8_t niad_getf(void)
{
    uint32_t localVal = sim_reg_itc_rd(INTCNTL_INDEX);
    return ((localVal & 0x00100000) >> 20);
}

__INLINE void niad_setf(uint8_t niad)
{
    ASSERT_ERR(((niad << 20) & ~0x00100000) == 0);
    sim_reg_itc_wr(INTCNTL_INDEX, (sim_reg_itc_rd(INTCNTL_INDEX) & ~0x00100000) | (niad << 20));
}

__INLINE uint8_t fiad_getf(void)
{
    uint32_t localVal = sim_reg_itc_rd(INTCNTL_INDEX);
    return ((localVal & 0x00080000) >> 19);
}

__INLINE void fiad_setf(uint8_t fiad)
{
    ASSERT_ERR(((fiad << 19) & ~0x00080000) == 0);
    sim_reg_itc_wr(INTCNTL_INDEX, (sim_reg_itc_rd(INTCNTL_INDEX) & ~0x00080000) | (fiad << 19));
}

/**
 * @brief NIMASK register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  04:00               NIMASK   0x0
 * </pre>
 */
#define NIMASK_INDEX  0x00000001

__INLINE uint32_t nimask_get(void)
{
    return sim_reg_itc_rd(NIMASK_INDEX);
}

__INLINE void nimask_set(uint32_t value)
{
    sim_reg_itc_wr(NIMASK_INDEX, value);
}

// field definitions
#define NIMASK_MASK   0x0000001F
#define NIMASK_LSB    0
#define NIMASK_WIDTH  0x00000005

#define NIMASK_RST    0x0

__INLINE uint8_t nimask_getf(void)
{
    uint32_t localVal = sim_reg_itc_rd(NIMASK_INDEX);
    ASSERT_ERR((localVal & ~0x0000001F) == 0);
    return (localVal >> 0);
}

__INLINE void nimask_setf(uint8_t nimask)
{
    ASSERT_ERR(((nimask << 0) & ~0x0000001F) == 0);
    sim_reg_itc_wr(NIMASK_INDEX, nimask << 0);
}

/**
 * @brief INTENNUM register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  03:00                ENNUM   0x0
 * </pre>
 */
#define INTENNUM_INDEX  0x00000002

__INLINE void intennum_set(uint32_t value)
{
    sim_reg_itc_wr(INTENNUM_INDEX, value);
}

// field definitions
#define ENNUM_MASK   0x0000000F
#define ENNUM_LSB    0
#define ENNUM_WIDTH  0x00000004

#define ENNUM_RST    0x0

__INLINE void ennum_setf(uint8_t ennum)
{
    ASSERT_ERR(((ennum << 0) & ~0x0000000F) == 0);
    sim_reg_itc_wr(INTENNUM_INDEX, ennum << 0);
}

/**
 * @brief INTDISNUM register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  03:00               DISNUM   0x0
 * </pre>
 */
#define INTDISNUM_INDEX  0x00000003

__INLINE void intdisnum_set(uint32_t value)
{
    sim_reg_itc_wr(INTDISNUM_INDEX, value);
}

// field definitions
#define DISNUM_MASK   0x0000000F
#define DISNUM_LSB    0
#define DISNUM_WIDTH  0x00000004

#define DISNUM_RST    0x0

__INLINE void disnum_setf(uint8_t disnum)
{
    ASSERT_ERR(((disnum << 0) & ~0x0000000F) == 0);
    sim_reg_itc_wr(INTDISNUM_INDEX, disnum << 0);
}

/**
 * @brief INTENABLE register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00            INTENABLE   0x0
 * </pre>
 */
#define INTENABLE_INDEX  0x00000004

__INLINE uint32_t intenable_get(void)
{
    return sim_reg_itc_rd(INTENABLE_INDEX);
}

__INLINE void intenable_set(uint32_t value)
{
    sim_reg_itc_wr(INTENABLE_INDEX, value);
}

// field definitions
#define INTENABLE_MASK   0x0000FFFF
#define INTENABLE_LSB    0
#define INTENABLE_WIDTH  0x00000010

#define INTENABLE_RST    0x0

__INLINE uint16_t intenable_getf(void)
{
    uint32_t localVal = sim_reg_itc_rd(INTENABLE_INDEX);
    ASSERT_ERR((localVal & ~0x0000FFFF) == 0);
    return (localVal >> 0);
}

__INLINE void intenable_setf(uint16_t intenable)
{
    ASSERT_ERR(((intenable << 0) & ~0x0000FFFF) == 0);
    sim_reg_itc_wr(INTENABLE_INDEX, intenable << 0);
}

/**
 * @brief INTTYPE register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00              INTTYPE   0x0
 * </pre>
 */
#define INTTYPE_INDEX  0x00000005

__INLINE uint32_t inttype_get(void)
{
    return sim_reg_itc_rd(INTTYPE_INDEX);
}

__INLINE void inttype_set(uint32_t value)
{
    sim_reg_itc_wr(INTTYPE_INDEX, value);
}

// field definitions
#define INTTYPE_MASK   0x0000FFFF
#define INTTYPE_LSB    0
#define INTTYPE_WIDTH  0x00000010

#define INTTYPE_RST    0x0

__INLINE uint16_t inttype_getf(void)
{
    uint32_t localVal = sim_reg_itc_rd(INTTYPE_INDEX);
    ASSERT_ERR((localVal & ~0x0000FFFF) == 0);
    return (localVal >> 0);
}

__INLINE void inttype_setf(uint16_t inttype)
{
    ASSERT_ERR(((inttype << 0) & ~0x0000FFFF) == 0);
    sim_reg_itc_wr(INTTYPE_INDEX, inttype << 0);
}

/**
 * @brief NIVECTOR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  03:00             NIVECTOR   0x0
 * </pre>
 */
#define NIVECTOR_INDEX  0x0000000A

__INLINE uint32_t nivector_get(void)
{
    return sim_reg_itc_rd(NIVECTOR_INDEX);
}

__INLINE void nivector_set(uint32_t value)
{
    sim_reg_itc_wr(NIVECTOR_INDEX, value);
}

// field definitions
#define NIVECTOR_MASK   0x0000000F
#define NIVECTOR_LSB    0
#define NIVECTOR_WIDTH  0x00000004

#define NIVECTOR_RST    0x0

__INLINE uint8_t nivector_getf(void)
{
    uint32_t localVal = sim_reg_itc_rd(NIVECTOR_INDEX);
    ASSERT_ERR((localVal & ~0x0000000F) == 0);
    return (localVal >> 0);
}

/**
 * @brief FIVECTOR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  03:00             FIVECTOR   0x0
 * </pre>
 */
#define FIVECTOR_INDEX  0x0000000B

__INLINE uint32_t fivector_get(void)
{
    return sim_reg_itc_rd(FIVECTOR_INDEX);
}

__INLINE void fivector_set(uint32_t value)
{
    sim_reg_itc_wr(FIVECTOR_INDEX, value);
}

// field definitions
#define FIVECTOR_MASK   0x0000000F
#define FIVECTOR_LSB    0
#define FIVECTOR_WIDTH  0x00000004

#define FIVECTOR_RST    0x0

__INLINE uint8_t fivector_getf(void)
{
    uint32_t localVal = sim_reg_itc_rd(FIVECTOR_INDEX);
    ASSERT_ERR((localVal & ~0x0000000F) == 0);
    return (localVal >> 0);
}

/**
 * @brief INTSRC register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00               INTSRC   0x0
 * </pre>
 */
#define INTSRC_INDEX  0x0000000C

__INLINE uint32_t intsrc_get(void)
{
    return sim_reg_itc_rd(INTSRC_INDEX);
}

__INLINE void intsrc_set(uint32_t value)
{
    sim_reg_itc_wr(INTSRC_INDEX, value);
}

// field definitions
#define INTSRC_MASK   0x0000FFFF
#define INTSRC_LSB    0
#define INTSRC_WIDTH  0x00000010

#define INTSRC_RST    0x0

__INLINE uint16_t intsrc_getf(void)
{
    uint32_t localVal = sim_reg_itc_rd(INTSRC_INDEX);
    ASSERT_ERR((localVal & ~0x0000FFFF) == 0);
    return (localVal >> 0);
}

/**
 * @brief INTFRC register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00               INTFRC   0x0
 * </pre>
 */
#define INTFRC_INDEX  0x0000000D

__INLINE uint32_t intfrc_get(void)
{
    return sim_reg_itc_rd(INTFRC_INDEX);
}

__INLINE void intfrc_set(uint32_t value)
{
    sim_reg_itc_wr(INTFRC_INDEX, value);
}

// field definitions
#define INTFRC_MASK   0x0000FFFF
#define INTFRC_LSB    0
#define INTFRC_WIDTH  0x00000010

#define INTFRC_RST    0x0

__INLINE uint16_t intfrc_getf(void)
{
    uint32_t localVal = sim_reg_itc_rd(INTFRC_INDEX);
    ASSERT_ERR((localVal & ~0x0000FFFF) == 0);
    return (localVal >> 0);
}

__INLINE void intfrc_setf(uint16_t intfrc)
{
    ASSERT_ERR(((intfrc << 0) & ~0x0000FFFF) == 0);
    sim_reg_itc_wr(INTFRC_INDEX, intfrc << 0);
}

/**
 * @brief NIPEND register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00               NIPEND   0x0
 * </pre>
 */
#define NIPEND_INDEX  0x0000000E

__INLINE uint32_t nipend_get(void)
{
    return sim_reg_itc_rd(NIPEND_INDEX);
}

__INLINE void nipend_set(uint32_t value)
{
    sim_reg_itc_wr(NIPEND_INDEX, value);
}

// field definitions
#define NIPEND_MASK   0x0000FFFF
#define NIPEND_LSB    0
#define NIPEND_WIDTH  0x00000010

#define NIPEND_RST    0x0

__INLINE uint16_t nipend_getf(void)
{
    uint32_t localVal = sim_reg_itc_rd(NIPEND_INDEX);
    ASSERT_ERR((localVal & ~0x0000FFFF) == 0);
    return (localVal >> 0);
}

/**
 * @brief FIPEND register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00               FIPEND   0x0
 * </pre>
 */
#define FIPEND_INDEX  0x0000000F

__INLINE uint32_t fipend_get(void)
{
    return sim_reg_itc_rd(FIPEND_INDEX);
}

__INLINE void fipend_set(uint32_t value)
{
    sim_reg_itc_wr(FIPEND_INDEX, value);
}

// field definitions
#define FIPEND_MASK   0x0000FFFF
#define FIPEND_LSB    0
#define FIPEND_WIDTH  0x00000010

#define FIPEND_RST    0x0

__INLINE uint16_t fipend_getf(void)
{
    uint32_t localVal = sim_reg_itc_rd(FIPEND_INDEX);
    ASSERT_ERR((localVal & ~0x0000FFFF) == 0);
    return (localVal >> 0);
}


#endif // _REG_ITC_H_

