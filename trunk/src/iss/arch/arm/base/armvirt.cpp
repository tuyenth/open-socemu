#include "arm.h"

/***************************************************************************\
*                   ReLoad Instruction                                     *
\***************************************************************************/

uint32_t
arm::ARMul_ReLoadInstr(uint32_t address, uint32_t isize)
{
    uint32_t data;
    enum arm::fault_t fault;

    // check if it is a thumb instruction load
    if ((isize == 2) && (address & 0x2))
    {
        uint32_t lo, hi;
        fault = mmu_load_instr(address & (~3), &lo);

        if (!fault)
        {
            fault = mmu_load_instr((address + 4) & (~3), &hi);
        }
        if (fault)
        {
            return ARMul_ABORTWORD;
        }
        else
        {
            m_AbortSig = false;
        }

        if (m_IsBigEndian == true)
            return (lo << 16) | (hi >> 16);
        else
            return ((hi & 0xFFFF) << 16) | (lo >> 16);
    }

    fault = mmu_load_instr(address, &data);

    if (fault)
    {
        return ARMul_ABORTWORD;
    }
    else {
        m_AbortSig = false;
    }

    return data;
}

/***************************************************************************\
*                   Load Instruction, Sequential Cycle                      *
\***************************************************************************/

uint32_t
arm::ARMul_LoadInstrS(uint32_t address, uint32_t isize)
{
    m_NumScycles++;
    return ARMul_ReLoadInstr(address, isize);
}

/***************************************************************************\
*                 Load Instruction, Non Sequential Cycle                    *
\***************************************************************************/

uint32_t
arm::ARMul_LoadInstrN(uint32_t address, uint32_t isize)
{
    m_NumNcycles++;
    return ARMul_ReLoadInstr(address, isize);
}

/***************************************************************************\
*                      Read Word (but don't tell anyone!)                   *
\***************************************************************************/

uint32_t
arm::ARMul_ReadWord(uint32_t address)
{
    uint32_t data;
    fault_t fault;

    fault = mmu_read_word(address, &data);
    if (fault)
    {
        mmu_set_fault(fault, address);
        ARMul_DATAABORT();
        return ARMul_ABORTWORD;
    }
    else
    {
        m_AbortSig = false;
    }
    return data;
}

/***************************************************************************\
*                        Load Word, Sequential Cycle                        *
\***************************************************************************/

uint32_t
arm::ARMul_LoadWordS(uint32_t address)
{
    m_NumScycles++;
    return ARMul_ReadWord(address);
}

/***************************************************************************\
*                      Load Word, Non Sequential Cycle                      *
\***************************************************************************/

uint32_t
arm::ARMul_LoadWordN(uint32_t address)
{
    m_NumNcycles++;
    return ARMul_ReadWord(address);
}

/***************************************************************************\
*                     Load Halfword, (Non Sequential Cycle)                 *
\***************************************************************************/

uint32_t
arm::ARMul_LoadHalfWord(uint32_t address)
{
    uint32_t data;
    fault_t fault;

    m_NumNcycles++;
    fault = mmu_read_halfword(address, &data);
    if (fault)
    {
        mmu_set_fault(fault, address);
        ARMul_DATAABORT();
        return ARMul_ABORTWORD;
    }
    else
    {
        m_AbortSig = false;
    }
    return data;
}

uint32_t
arm::ARMul_ReadByte(uint32_t address)
{
    uint32_t data;
    fault_t fault;

    fault = mmu_read_byte(address, &data);
    if (fault)
    {
        mmu_set_fault(fault, address);
        ARMul_DATAABORT();
        return ARMul_ABORTWORD;
    }
    else
    {
        m_AbortSig = false;
    }

    return data;

}

/***************************************************************************\
*                     Load Byte, (Non Sequential Cycle)                     *
\***************************************************************************/

uint32_t
arm::ARMul_LoadByte(uint32_t address)
{
    m_NumNcycles++;
    return ARMul_ReadByte(address);
}

/***************************************************************************\
*                     Write Word (but don't tell anyone!)                   *
\***************************************************************************/

void
arm::ARMul_WriteWord(uint32_t address, uint32_t data)
{
    fault_t fault;

    fault = mmu_write_word(address, data);
    if (fault)
    {
        mmu_set_fault(fault, address);
        ARMul_DATAABORT();
        return;
    }
    else
    {
        m_AbortSig = false;
    }
}

/***************************************************************************\
*                       Store Word, Sequential Cycle                        *
\***************************************************************************/

void
arm::ARMul_StoreWordS(uint32_t address, uint32_t data)
{
    m_NumScycles++;
    ARMul_WriteWord(address, data);
}

/***************************************************************************\
*                       Store Word, Non Sequential Cycle                        *
\***************************************************************************/

void
arm::ARMul_StoreWordN(uint32_t address, uint32_t data)
{
    m_NumNcycles++;
    ARMul_WriteWord(address, data);
}

/***************************************************************************\
*                    Store HalfWord, (Non Sequential Cycle)                 *
\***************************************************************************/

void
arm::ARMul_StoreHalfWord(uint32_t address, uint32_t data)
{
    fault_t fault;

    m_NumNcycles++;
    fault = mmu_write_halfword(address, data);
    if (fault)
    {
        mmu_set_fault(fault, address);
        ARMul_DATAABORT();
        return;
    }
    else
    {
        m_AbortSig = false;
    }
}

/***************************************************************************\
*                     Write Byte (but don't tell anyone!)                   *
\***************************************************************************/
void
arm::ARMul_WriteByte(uint32_t address, uint32_t data)
{
    fault_t fault;

    fault = mmu_write_byte(address, data);
    if (fault)
    {
        mmu_set_fault(fault, address);
        ARMul_DATAABORT();
        return;
    }
    else
    {
        m_AbortSig = false;
    }
}

/***************************************************************************\
*                    Store Byte, (Non Sequential Cycle)                     *
\***************************************************************************/

void
arm::ARMul_StoreByte(uint32_t address, uint32_t data)
{
    m_NumNcycles++;
    ARMul_WriteByte(address, data);
}

/***************************************************************************\
*                   Swap Word, (Two Non Sequential Cycles)                  *
\***************************************************************************/

uint32_t
arm::ARMul_SwapWord(uint32_t address, uint32_t data)
{
    uint32_t temp;

    m_NumNcycles++;
    temp = ARMul_ReadWord(address);

    m_NumNcycles++;
    mmu_write_word(address, data);

    return temp;
}

/***************************************************************************\
*                   Swap Byte, (Two Non Sequential Cycles)                  *
\***************************************************************************/

uint32_t
arm::ARMul_SwapByte(uint32_t address, uint32_t data)
{
    uint32_t temp;

    temp = ARMul_LoadByte(address);
    ARMul_StoreByte(address, data);

    return temp;
}
