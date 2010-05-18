#ifndef _REG_GPIO_H_
#define _REG_GPIO_H_

#include <stdint.h>
#include "compiler.h"
#include "arch.h"
#include "reg_sim.h"

#define REG_GPIO_BASE_ADDR 0x80000000

#define REG_GPIO_COUNT 26

#define REG_GPIO_DECODING_MASK 0x0000007F

/**
 * @brief GPIO_PAD_DIR0 register definition
 */
#define GPIO_PAD_DIR0_INDEX  0x00000000

__INLINE uint32_t gpio_pad_dir0_get(void)
{
    return sim_reg_gpio_rd(GPIO_PAD_DIR0_INDEX);
}

__INLINE void gpio_pad_dir0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_DIR0_INDEX, value);
}

/**
 * @brief GPIO_PAD_DIR1 register definition
 */
#define GPIO_PAD_DIR1_INDEX  0x00000001

__INLINE uint32_t gpio_pad_dir1_get(void)
{
    return sim_reg_gpio_rd(GPIO_PAD_DIR1_INDEX);
}

__INLINE void gpio_pad_dir1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_DIR1_INDEX, value);
}

/**
 * @brief GPIO_DATA0 register definition
 */
#define GPIO_DATA0_INDEX  0x00000002

__INLINE uint32_t gpio_data0_get(void)
{
    return sim_reg_gpio_rd(GPIO_DATA0_INDEX);
}

__INLINE void gpio_data0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_DATA0_INDEX, value);
}

/**
 * @brief GPIO_DATA1 register definition
 */
#define GPIO_DATA1_INDEX  0x00000003

__INLINE uint32_t gpio_data1_get(void)
{
    return sim_reg_gpio_rd(GPIO_DATA1_INDEX);
}

__INLINE void gpio_data1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_DATA1_INDEX, value);
}

/**
 * @brief GPIO_PAD_PU_EN0 register definition
 */
#define GPIO_PAD_PU_EN0_INDEX  0x00000004

__INLINE uint32_t gpio_pad_pu_en0_get(void)
{
    return sim_reg_gpio_rd(GPIO_PAD_PU_EN0_INDEX);
}

__INLINE void gpio_pad_pu_en0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_PU_EN0_INDEX, value);
}

/**
 * @brief GPIO_PAD_PU_EN1 register definition
 */
#define GPIO_PAD_PU_EN1_INDEX  0x00000005

__INLINE uint32_t gpio_pad_pu_en1_get(void)
{
    return sim_reg_gpio_rd(GPIO_PAD_PU_EN1_INDEX);
}

__INLINE void gpio_pad_pu_en1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_PU_EN1_INDEX, value);
}

/**
 * @brief GPIO_FUNC_SEL0 register definition
 */
#define GPIO_FUNC_SEL0_INDEX  0x00000006

__INLINE uint32_t gpio_func_sel0_get(void)
{
    return sim_reg_gpio_rd(GPIO_FUNC_SEL0_INDEX);
}

__INLINE void gpio_func_sel0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_FUNC_SEL0_INDEX, value);
}

/**
 * @brief GPIO_FUNC_SEL1 register definition
 */
#define GPIO_FUNC_SEL1_INDEX  0x00000007

__INLINE uint32_t gpio_func_sel1_get(void)
{
    return sim_reg_gpio_rd(GPIO_FUNC_SEL1_INDEX);
}

__INLINE void gpio_func_sel1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_FUNC_SEL1_INDEX, value);
}

/**
 * @brief GPIO_FUNC_SEL2 register definition
 */
#define GPIO_FUNC_SEL2_INDEX  0x00000008

__INLINE uint32_t gpio_func_sel2_get(void)
{
    return sim_reg_gpio_rd(GPIO_FUNC_SEL2_INDEX);
}

__INLINE void gpio_func_sel2_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_FUNC_SEL2_INDEX, value);
}

/**
 * @brief GPIO_FUNC_SEL3 register definition
 */
#define GPIO_FUNC_SEL3_INDEX  0x00000009

__INLINE uint32_t gpio_func_sel3_get(void)
{
    return sim_reg_gpio_rd(GPIO_FUNC_SEL3_INDEX);
}

__INLINE void gpio_func_sel3_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_FUNC_SEL3_INDEX, value);
}

/**
 * @brief GPIO_DATA_SEL0 register definition
 */
#define GPIO_DATA_SEL0_INDEX  0x0000000A

__INLINE uint32_t gpio_data_sel0_get(void)
{
    return sim_reg_gpio_rd(GPIO_DATA_SEL0_INDEX);
}

__INLINE void gpio_data_sel0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_DATA_SEL0_INDEX, value);
}

/**
 * @brief GPIO_DATA_SEL1 register definition
 */
#define GPIO_DATA_SEL1_INDEX  0x0000000B

__INLINE uint32_t gpio_data_sel1_get(void)
{
    return sim_reg_gpio_rd(GPIO_DATA_SEL1_INDEX);
}

__INLINE void gpio_data_sel1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_DATA_SEL1_INDEX, value);
}

/**
 * @brief GPIO_PAD_PU_SEL0 register definition
 */
#define GPIO_PAD_PU_SEL0_INDEX  0x0000000C

__INLINE uint32_t gpio_pad_pu_sel0_get(void)
{
    return sim_reg_gpio_rd(GPIO_PAD_PU_SEL0_INDEX);
}

__INLINE void gpio_pad_pu_sel0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_PU_SEL0_INDEX, value);
}

/**
 * @brief GPIO_PAD_PU_SEL1 register definition
 */
#define GPIO_PAD_PU_SEL1_INDEX  0x0000000D

__INLINE uint32_t gpio_pad_pu_sel1_get(void)
{
    return sim_reg_gpio_rd(GPIO_PAD_PU_SEL1_INDEX);
}

__INLINE void gpio_pad_pu_sel1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_PU_SEL1_INDEX, value);
}

/**
 * @brief GPIO_PAD_HYST_EN0 register definition
 */
#define GPIO_PAD_HYST_EN0_INDEX  0x0000000E

__INLINE uint32_t gpio_pad_hyst_en0_get(void)
{
    return sim_reg_gpio_rd(GPIO_PAD_HYST_EN0_INDEX);
}

__INLINE void gpio_pad_hyst_en0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_HYST_EN0_INDEX, value);
}

/**
 * @brief GPIO_PAD_HYST_EN1 register definition
 */
#define GPIO_PAD_HYST_EN1_INDEX  0x0000000F

__INLINE uint32_t gpio_pad_hyst_en1_get(void)
{
    return sim_reg_gpio_rd(GPIO_PAD_HYST_EN1_INDEX);
}

__INLINE void gpio_pad_hyst_en1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_HYST_EN1_INDEX, value);
}

/**
 * @brief GPIO_PAD_KEEP0 register definition
 */
#define GPIO_PAD_KEEP0_INDEX  0x00000010

__INLINE uint32_t gpio_pad_keep0_get(void)
{
    return sim_reg_gpio_rd(GPIO_PAD_KEEP0_INDEX);
}

__INLINE void gpio_pad_keep0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_KEEP0_INDEX, value);
}

/**
 * @brief GPIO_PAD_KEEP1 register definition
 */
#define GPIO_PAD_KEEP1_INDEX  0x00000011

__INLINE uint32_t gpio_pad_keep1_get(void)
{
    return sim_reg_gpio_rd(GPIO_PAD_KEEP1_INDEX);
}

__INLINE void gpio_pad_keep1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_KEEP1_INDEX, value);
}

/**
 * @brief GPIO_DATA_SET0 register definition
 */
#define GPIO_DATA_SET0_INDEX  0x00000012

__INLINE void gpio_data_set0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_DATA_SET0_INDEX, value);
}

/**
 * @brief GPIO_DATA_SET1 register definition
 */
#define GPIO_DATA_SET1_INDEX  0x00000013

__INLINE void gpio_data_set1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_DATA_SET1_INDEX, value);
}

/**
 * @brief GPIO_DATA_RESET0 register definition
 */
#define GPIO_DATA_RESET0_INDEX  0x00000014

__INLINE void gpio_data_reset0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_DATA_RESET0_INDEX, value);
}

/**
 * @brief GPIO_DATA_RESET1 register definition
 */
#define GPIO_DATA_RESET1_INDEX  0x00000015

__INLINE void gpio_data_reset1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_DATA_RESET1_INDEX, value);
}

/**
 * @brief GPIO_PAD_DIR_SET0 register definition
 */
#define GPIO_PAD_DIR_SET0_INDEX  0x00000016

__INLINE void gpio_pad_dir_set0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_DIR_SET0_INDEX, value);
}

/**
 * @brief GPIO_PAD_DIR_SET1 register definition
 */
#define GPIO_PAD_DIR_SET1_INDEX  0x00000017

__INLINE void gpio_pad_dir_set1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_DIR_SET1_INDEX, value);
}

/**
 * @brief GPIO_PAD_DIR_RESET0 register definition
 */
#define GPIO_PAD_DIR_RESET0_INDEX  0x00000018

__INLINE void gpio_pad_dir_reset0_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_DIR_RESET0_INDEX, value);
}

/**
 * @brief GPIO_PAD_DIR_RESET1 register definition
 */
#define GPIO_PAD_DIR_RESET1_INDEX  0x00000019

__INLINE void gpio_pad_dir_reset1_set(uint32_t value)
{
    sim_reg_gpio_wr(GPIO_PAD_DIR_RESET1_INDEX, value);
}


#endif // _REG_GPIO_H_

