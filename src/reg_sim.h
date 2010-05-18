#ifndef _REG_SIM_H_
#define _REG_SIM_H_

#include <stdint.h>

// provide access to the register image
extern uint32_t* reg_aic;
#define sim_reg_aic_wr(index, data) reg_aic[index] = data
#define sim_reg_aic_rd(index) reg_aic[index]

// provide access to the register image
extern uint32_t* reg_smc;
#define sim_reg_smc_wr(index, data) reg_smc[index] = data
#define sim_reg_smc_rd(index) reg_smc[index]

// provide access to the register image
extern uint32_t* reg_crm;
#define sim_reg_crm_wr(index, data) reg_crm[index] = data
#define sim_reg_crm_rd(index) reg_crm[index]

// provide access to the register image
extern uint32_t* reg_gpio;
#define sim_reg_gpio_wr(index, data) reg_gpio[index] = data
#define sim_reg_gpio_rd(index) reg_gpio[index]

// provide access to the register image
extern uint32_t* reg_itc;
#define sim_reg_itc_wr(index, data) reg_itc[index] = data
#define sim_reg_itc_rd(index) reg_itc[index]

// provide access to the register image
extern uint32_t* reg_spif;
#define sim_reg_spif_wr(index, data) reg_spif[index] = data
#define sim_reg_spif_rd(index) reg_spif[index]

// provide access to the register image
extern uint32_t* reg_spi;
#define sim_reg_spi_wr(index, data) reg_spi[index] = data
#define sim_reg_spi_rd(index) reg_spi[index]

// provide access to the register image (not implemented because multiple instance)
#define sim_reg_uart_wr(index, data)
#define sim_reg_uart_rd(index) 0

// provide access to the register image
extern uint32_t* reg_reserved;
#define sim_reg_reserved_wr(index, data) reg_reserved[index] = data
#define sim_reg_reserved_rd(index) reg_reserved[index]

#endif // _REG_SIM_H_
