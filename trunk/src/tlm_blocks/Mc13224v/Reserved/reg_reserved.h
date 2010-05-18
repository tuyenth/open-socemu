#ifndef _REG_RESERVED_H_
#define _REG_RESERVED_H_

#include <stdint.h>
#include "compiler.h"
#include "arch.h"
#include "reg_sim.h"

#define REG_RESERVED_BASE_ADDR 0x8000C000

#define REG_RESERVED_COUNT 9

#define REG_RESERVED_DECODING_MASK 0x0000003F

/**
 * @brief RESERVED0 register definition
 */
#define RESERVED0_INDEX  0x00000000

__INLINE uint32_t reserved0_get(void)
{
    return sim_reg_reserved_rd(RESERVED0_INDEX);
}

__INLINE void reserved0_set(uint32_t value)
{
    sim_reg_reserved_wr(RESERVED0_INDEX, value);
}

/**
 * @brief RESERVED1 register definition
 */
#define RESERVED1_INDEX  0x00000001

__INLINE uint32_t reserved1_get(void)
{
    return sim_reg_reserved_rd(RESERVED1_INDEX);
}

__INLINE void reserved1_set(uint32_t value)
{
    sim_reg_reserved_wr(RESERVED1_INDEX, value);
}

/**
 * @brief RESERVED2 register definition
 */
#define RESERVED2_INDEX  0x00000002

__INLINE uint32_t reserved2_get(void)
{
    return sim_reg_reserved_rd(RESERVED2_INDEX);
}

__INLINE void reserved2_set(uint32_t value)
{
    sim_reg_reserved_wr(RESERVED2_INDEX, value);
}

/**
 * @brief RESERVED3 register definition
 */
#define RESERVED3_INDEX  0x00000003

__INLINE uint32_t reserved3_get(void)
{
    return sim_reg_reserved_rd(RESERVED3_INDEX);
}

__INLINE void reserved3_set(uint32_t value)
{
    sim_reg_reserved_wr(RESERVED3_INDEX, value);
}

/**
 * @brief RESERVED4 register definition
 */
#define RESERVED4_INDEX  0x00000004

__INLINE uint32_t reserved4_get(void)
{
    return sim_reg_reserved_rd(RESERVED4_INDEX);
}

__INLINE void reserved4_set(uint32_t value)
{
    sim_reg_reserved_wr(RESERVED4_INDEX, value);
}

/**
 * @brief RESERVED5 register definition
 */
#define RESERVED5_INDEX  0x00000005

__INLINE uint32_t reserved5_get(void)
{
    return sim_reg_reserved_rd(RESERVED5_INDEX);
}

__INLINE void reserved5_set(uint32_t value)
{
    sim_reg_reserved_wr(RESERVED5_INDEX, value);
}

/**
 * @brief RESERVED6 register definition
 */
#define RESERVED6_INDEX  0x00000006

__INLINE uint32_t reserved6_get(void)
{
    return sim_reg_reserved_rd(RESERVED6_INDEX);
}

__INLINE void reserved6_set(uint32_t value)
{
    sim_reg_reserved_wr(RESERVED6_INDEX, value);
}

/**
 * @brief RESERVED7 register definition
 */
#define RESERVED7_INDEX  0x00000007

__INLINE uint32_t reserved7_get(void)
{
    return sim_reg_reserved_rd(RESERVED7_INDEX);
}

__INLINE void reserved7_set(uint32_t value)
{
    sim_reg_reserved_wr(RESERVED7_INDEX, value);
}

/**
 * @brief RESERVED8 register definition
 */
#define RESERVED8_INDEX  0x00000008

__INLINE uint32_t reserved8_get(void)
{
    return sim_reg_reserved_rd(RESERVED8_INDEX);
}

__INLINE void reserved8_set(uint32_t value)
{
    sim_reg_reserved_wr(RESERVED8_INDEX, value);
}


#endif // _REG_RESERVED_H_

