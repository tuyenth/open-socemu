#ifndef PRC_H_
#define PRC_H_

/// Patch RAM Controller (PRC)

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_PRC_BRK_OUT0,
    REG_PRC_PATCH_IN0,
    REG_PRC_BRK_OUT1,
    REG_PRC_PATCH_IN1,
    REG_PRC_BRK_OUT2,
    REG_PRC_PATCH_IN2,
    REG_PRC_BRK_OUT3,
    REG_PRC_PATCH_IN3,
    REG_PRC_BRK_OUT4,
    REG_PRC_PATCH_IN4,
    REG_PRC_BRK_OUT5,
    REG_PRC_PATCH_IN5,
    REG_PRC_BRK_OUT6,
    REG_PRC_PATCH_IN6,
    REG_PRC_BRK_OUT7,
    REG_PRC_PATCH_IN7,
    REG_PRC_BRK_OUT8,
    REG_PRC_PATCH_IN8,
    REG_PRC_BRK_OUT9,
    REG_PRC_PATCH_IN9,
    REG_PRC_BRK_OUTA,
    REG_PRC_PATCH_INA,
    REG_PRC_BRK_OUTB,
    REG_PRC_PATCH_INB,
    REG_PRC_BRK_OUTC,
    REG_PRC_PATCH_INC,
    REG_PRC_BRK_OUTD,
    REG_PRC_PATCH_IND,
    REG_PRC_BRK_OUTE,
    REG_PRC_PATCH_INE,
    REG_PRC_BRK_OUTF,
    REG_PRC_PATCH_INF,
    REG_PRC_BRK_OUT10,
    REG_PRC_PATCH_IN10,
    REG_PRC_BRK_OUT11,
    REG_PRC_PATCH_IN11,
    REG_PRC_BRK_OUT12,
    REG_PRC_PATCH_IN12,
    REG_PRC_BRK_OUT13,
    REG_PRC_PATCH_IN13,
    REG_PRC_BRK_OUT14,
    REG_PRC_PATCH_IN14,
    REG_PRC_BRK_OUT15,
    REG_PRC_PATCH_IN15,
    REG_PRC_BRK_OUT16,
    REG_PRC_PATCH_IN16,
    REG_PRC_BRK_OUT17,
    REG_PRC_PATCH_IN17,
    REG_PRC_BRK_OUT18,
    REG_PRC_PATCH_IN18,
    REG_PRC_BRK_OUT19,
    REG_PRC_PATCH_IN19,
    REG_PRC_BRK_OUT1A,
    REG_PRC_PATCH_IN1A,
    REG_PRC_BRK_OUT1B,
    REG_PRC_PATCH_IN1B,
    REG_PRC_BRK_OUT1C,
    REG_PRC_PATCH_IN1C,
    REG_PRC_BRK_OUT1D,
    REG_PRC_PATCH_IN1D,
    REG_PRC_BRK_OUT1E,
    REG_PRC_PATCH_IN1E,
    REG_PRC_BRK_OUT1F,
    REG_PRC_PATCH_IN1F,
    REG_PRC_BRK_OUT20,
    REG_PRC_PATCH_IN20,
    REG_PRC_BRK_OUT21,
    REG_PRC_PATCH_IN21,
    REG_PRC_BRK_OUT22,
    REG_PRC_PATCH_IN22,
    REG_PRC_BRK_OUT23,
    REG_PRC_PATCH_IN23,
    REG_PRC_BRK_OUT24,
    REG_PRC_PATCH_IN24,
    REG_PRC_BRK_OUT25,
    REG_PRC_PATCH_IN25,
    REG_PRC_BRK_OUT26,
    REG_PRC_PATCH_IN26,
    REG_PRC_BRK_OUT27,
    REG_PRC_PATCH_IN27,
    REG_PRC_BRK_OUT28,
    REG_PRC_PATCH_IN28,
    REG_PRC_BRK_OUT29,
    REG_PRC_PATCH_IN29,
    REG_PRC_BRK_OUT2A,
    REG_PRC_PATCH_IN2A,
    REG_PRC_BRK_OUT2B,
    REG_PRC_PATCH_IN2B,
    REG_PRC_BRK_OUT2C,
    REG_PRC_PATCH_IN2C,
    REG_PRC_BRK_OUT2D,
    REG_PRC_PATCH_IN2D,
    REG_PRC_BRK_OUT2E,
    REG_PRC_PATCH_IN2E,
    REG_PRC_BRK_OUT2F,
    REG_PRC_PATCH_IN2F,
    REG_PRC_BRK_OUT30,
    REG_PRC_PATCH_IN30,
    REG_PRC_BRK_OUT31,
    REG_PRC_PATCH_IN31,
    REG_PRC_BRK_OUT32,
    REG_PRC_PATCH_IN32,
    REG_PRC_BRK_OUT33,
    REG_PRC_PATCH_IN33,
    REG_PRC_BRK_OUT34,
    REG_PRC_PATCH_IN34,
    REG_PRC_BRK_OUT35,
    REG_PRC_PATCH_IN35,
    REG_PRC_BRK_OUT36,
    REG_PRC_PATCH_IN36,
    REG_PRC_BRK_OUT37,
    REG_PRC_PATCH_IN37,
    REG_PRC_BRK_OUT38,
    REG_PRC_PATCH_IN38,
    REG_PRC_BRK_OUT39,
    REG_PRC_PATCH_IN39,
    REG_PRC_BRK_OUT3A,
    REG_PRC_PATCH_IN3A,
    REG_PRC_BRK_OUT3B,
    REG_PRC_PATCH_IN3B,
    REG_PRC_BRK_OUT3C,
    REG_PRC_PATCH_IN3C,
    REG_PRC_BRK_OUT3D,
    REG_PRC_PATCH_IN3D,
    REG_PRC_BRK_OUT3E,
    REG_PRC_PATCH_IN3E,
    REG_PRC_BRK_OUT3F,
    REG_PRC_PATCH_IN3F,
    REG_PRC_PROTECTION,
    REG_PRC_PERIPHID0 = 0xFE0/4,
    REG_PRC_PERIPHID1,
    REG_PRC_PERIPHID2,
    REG_PRC_PERIPHID3,
    REG_PRC_PCELLID0,
    REG_PRC_PCELLID1,
    REG_PRC_PCELLID2,
    REG_PRC_PCELLID3,
    REG_PRC_COUNT
};

struct Prc : Peripheral<REG_PRC_COUNT>
{
    /// Constructor
    Prc(sc_core::sc_module_name name)
    : Peripheral<REG_PRC_COUNT>(name)
    {
        // initialize the registers content
    }

private:
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

#endif /*PRC_H_*/
