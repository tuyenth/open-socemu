#ifndef PL081_H_
#define PL081_H_

/// ARM DMA controller IP

#include "utils.h"
#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_PL081_DMACINTSTAT,
    REG_PL081_DMACINTTCSTAT,
    REG_PL081_DMACINTTCCLR,
    REG_PL081_DMACINTERRSTAT,
    REG_PL081_DMACINTERRCLR,
    REG_PL081_DMACRAWINTC,
    REG_PL081_DMACRAWINTERR,
    REG_PL081_DMACENBLDCHNS,
    REG_PL081_DMACSOFTBREQ,
    REG_PL081_DMACSOFTSREQ,
    REG_PL081_DMACSOFTLBREQ,
    REG_PL081_DMACSOFTLSREQ,
    REG_PL081_DMACCONFIG,
    REG_PL081_DMACSYNC,
    REG_PL081_DMACC0SRCADDR = 0x100/4,
    REG_PL081_DMACC0DESTADDR,
    REG_PL081_DMACC0LLIREG,
    REG_PL081_DMACC0CONTROL,
    REG_PL081_DMACC0CONFIG,
    REG_PL081_DMACC1SRCADDR = 0x120/4,
    REG_PL081_DMACC1DESTADDR,
    REG_PL081_DMACC1LLIREG,
    REG_PL081_DMACC1CONTROL,
    REG_PL081_DMACC1CONFIG,
    REG_PL081_DMACC2SRCADDR = 0x140/4,
    REG_PL081_DMACC2DESTADDR,
    REG_PL081_DMACC2LLIREG,
    REG_PL081_DMACC2CONTROL,
    REG_PL081_DMACC2CONFIG,
    REG_PL081_DMACC3SRCADDR = 0x160/4,
    REG_PL081_DMACC3DESTADDR,
    REG_PL081_DMACC3LLIREG,
    REG_PL081_DMACC3CONTROL,
    REG_PL081_DMACC3CONFIG,
    REG_PL081_DMACC4SRCADDR = 0x180/4,
    REG_PL081_DMACC4DESTADDR,
    REG_PL081_DMACC4LLIREG,
    REG_PL081_DMACC4CONTROL,
    REG_PL081_DMACC4CONFIG,
    REG_PL081_DMACC5SRCADDR = 0x1a0/4,
    REG_PL081_DMACC5DESTADDR,
    REG_PL081_DMACC5LLIREG,
    REG_PL081_DMACC5CONTROL,
    REG_PL081_DMACC5CONFIG,
    REG_PL081_DMACC6SRCADDR = 0x1c0/4,
    REG_PL081_DMACC6DESTADDR,
    REG_PL081_DMACC6LLIREG,
    REG_PL081_DMACC6CONTROL,
    REG_PL081_DMACC6CONFIG,
    REG_PL081_DMACC7SRCADDR = 0x1e0/4,
    REG_PL081_DMACC7DESTADDR,
    REG_PL081_DMACC7LLIREG,
    REG_PL081_DMACC7CONTROL,
    REG_PL081_DMACC7CONFIG,
    REG_PL081_DMACTCR = 0x500/4,
    REG_PL081_DMACITOP1,
    REG_PL081_DMACITOP2,
    REG_PL081_DMACITOP3,
    REG_PL081_DMACPERIPHID0 = 0xfe0/4,
    REG_PL081_DMACPERIPHID1,
    REG_PL081_DMACPERIPHID2,
    REG_PL081_DMACPERIPHID3,
    REG_PL081_DMACPCELLID0,
    REG_PL081_DMACPCELLID1,
    REG_PL081_DMACPCELLID2,
    REG_PL081_DMACPCELLID3,
    REG_PL081_COUNT
};

struct Pl081 : Peripheral<REG_PL081_COUNT>
{
    /// Constructor
    Pl081(sc_core::sc_module_name name)
    : Peripheral<REG_PL081_COUNT>(name)
    {
        // initialize the registers content
    }

private:
    /** Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset)
    {
        uint32_t result;
        // retrieve the required parameters
        uint32_t index = offset/4;

        // sanity check
        assert(index < REG_PL081_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        default:
            // read the register value
            result = m_reg[index];
            break;
        }

        return result;
    }


    /** Register write function
     * @param[in] offset Offset of the register to read
     * @param[in] offset Value to write in the register
     */
    void
    reg_wr(uint32_t offset, uint32_t value)
    {
        // retrieve the required parameters
        uint32_t index = offset/4;

        // sanity check
        assert(index < REG_PL081_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        default:
            m_reg[index] = value;
            break;
        }
    }

};

#endif /*PL081_H_*/
