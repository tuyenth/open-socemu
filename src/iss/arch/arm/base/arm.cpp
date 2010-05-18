/** @file arm.h
 * @brief arm class implementation.
 *
 * @sa http://infocenter.arm.com/
 */

#include "arm.h"

#include "armemu.h"

// Number of I cycles required for MULT, depending on high bit set in RHS
unsigned ARMul_MultTable[32] =
    { 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9,
    10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 16
};

/// Immediate values conversion from instruction [11..0] bits.
uint32_t ARMul_ImmedTable[4096];

/// Number of bits in a byte table.
char ARMul_BitList[256];

arm::arm(bool gdbserver, bool gdbstart, bool bigendian)
{
    // launch the gdbserver (in blocking mode if it must wait for debugger)
    gdbserver::start(12345, gdbserver, gdbstart);

    // save local variables
    m_gdbconnected = false;

    // initialize the helpers array
    this->init_helpers();

    // default initialize
    this->init_state();

    // configure the endianness
    m_IsBigEndian = bigendian;

}

void
arm::run()
{
    // treat as a first call to the emulator
    FLUSHPIPE;

    // set in run mode
    m_Emulate = RUN;

    // forever loop
    while (1)
    {
        this->emulate();
    }
}

void
arm::init_helpers()
{
    uint32_t i, j;

    // initialize the immediate value converter
    for (i = 0; i < 4096; i++)
    {
        ARMul_ImmedTable[i] = ROTATER (i & 0xffL, (i >> 7L) & 0x1eL);
    }

    // how many bits in a byte converter
    memset(ARMul_BitList, 0, sizeof(ARMul_BitList));

    for (j = 1; j < 256; j <<= 1)
    {
        for (i = 0; i < 256; i++)
        {
            if ((i & j) > 0)
            {
                ARMul_BitList[i]++;
            }
        }
    }

    // multiply by 4 the number of bits (because it is always used like this)
    for (i = 0; i < 256; i++)
    {
        ARMul_BitList[i] *= 4;
    }
}

void
arm::init_state(void)
{
    // clear all the ARM state
    //  - clear the registers
    memset(m_Reg, 0, sizeof(m_Reg));
    memset(m_RegBank, 0, sizeof(m_RegBank));
    //  - initialize the specific registers
    m_Cpsr = 0;
    memset(m_Spsr, 0, sizeof(m_Spsr));
    m_NFlag=m_ZFlag=m_CFlag=m_VFlag=m_IFFlags=m_SFlag=m_TFlag=m_Bank=m_Mode=0;
    //  - initialize the counters
    m_Instr=m_PC=m_Loaded=m_Decoded=m_NumScycles=m_NumNcycles=
        m_PreviousIcycles=m_NumIcycles=m_NumCcycles=0;
    m_NumInstrs=0;
    m_NextInstr=0;
    //  - initialize the signals
    m_NresetSig = true;
    m_NfiqSig = true;
    m_NirqSig = true;
    m_AbortSig = false;
    m_Aborted=m_Base=0;
    //  - default configuration of supported instruction sets
    m_IsBigEndian=m_IsV4=m_IsV5=m_IsV5e=m_IsBaseUpdatedAbort=false;
    //  - device
    m_DeviceId = 0;
    //  - debugger
    m_PCChanged = false;
    memset(m_BreakPts, 0, sizeof(m_BreakPts));
    m_NumBreakPts=0;

    //  - configure the default values
    m_Emulate = RUN;
    m_Cpsr = INTBITS | SVC32MODE;
    m_Mode = SVC32MODE;

    ARMul_CPSRAltered();
    m_Bank = SVCBANK;
    FLUSHPIPE;

}

/// Implementation of virtual function.
void
arm::gdb_single_step(int yesno)
{
    if (yesno)
    {
        m_Emulate = ONCE;
    }
    else
    {
        m_Emulate = RUN;
    }
}

/// Implementation of virtual function.
void
arm::gdb_set_pc(uint64_t pc)
{
    // check if the PC needs to be changed
    if (pc != m_PC)
    {
        // save the program counter in the register and in the current counter
        m_PC = m_Reg[15] = pc;

        // set the fact that the PC was modified
        m_PCChanged = true;
    }
}

/// Implementation of virtual function.
int
arm::gdb_read_registers(uint8_t *mem_buf)
{
    int i;
    uint8_t *ptr;

    ptr = mem_buf;

    // 15 core integer registers (4 bytes each)
    for (i = 0; i < 15; i++)
    {
        *(uint32_t *)ptr = ARMul_GetReg(m_Mode, i);
        ptr += 4;
    }

    // R15 = Program Counter register
    *(uint32_t *)ptr = m_PC;
    ptr += 4;

    /* 8 FPA registers (12 bytes each), FPS (4 bytes).
       Not yet implemented.  */
    memset (ptr, 0, (8 * 12) + 4);
    ptr += 8 * 12 + 4;
    /* CPSR (4 bytes).  */
    *(uint32_t *)ptr = ARMul_GetCPSR();
    ptr += 4;

    // return the size of the buffer filled
    return ptr - mem_buf;
}

/// Implementation of virtual function.
void
arm::gdb_write_registers(uint8_t *mem_buf, int size)
{
    int i;
    uint8_t *ptr;

    ptr = mem_buf;

    // 15 core integer registers (4 bytes each)
    for (i = 0; i < 15; i++)
    {
        ARMul_SetReg(m_Mode, i, *(uint32_t *)ptr);
        ptr += 4;
    }

    // R15 = Program Counter register
    gdb_set_pc(*(uint32_t *)ptr);
    ptr += 4;

    /* 8 FPA registers (12 bytes each), FPS (4 bytes).
       Not yet implemented.  */
    ptr += 8 * 12 + 4;
    /* CPSR (4 bytes).  */
    ARMul_SetCPSR(*(uint32_t *)ptr);
    ptr += 4;

}

/// Implementation of virtual function.
int
arm::gdb_breakpoint_insert(uint64_t addr)
{
    int i;

    // check if the breakpoint is already registered
    for (i = 0; i < m_NumBreakPts; i++)
    {
        if (m_BreakPts[i] == addr)
        {
            return i;
        }
    }
    // check if there are elements free
    if (m_NumBreakPts < ARM_MAX_BREAKPOINTS)
    {
        // save the breakpoint
        m_BreakPts[m_NumBreakPts] = addr;
        m_NumBreakPts++;

        return (m_NumBreakPts - 1);
    }

    return -1;
}

/// Implementation of virtual function.
void
arm::gdb_breakpoint_remove(uint64_t addr)
{
    int i,j;

    // check for spurious remove operations
    if (m_NumBreakPts == 0)
    {
        return;
    }

    // check if the breakpoint is already registered
    for (i = 0; i < m_NumBreakPts; i++)
    {
        if (m_BreakPts[i] == addr)
        {
            break;
        }
    }
    // shift the remaining breakpoints
    for (j = i+1; j < m_NumBreakPts; j++)
    {
        m_BreakPts[i] = m_BreakPts[j];
    }
    m_NumBreakPts--;
}

