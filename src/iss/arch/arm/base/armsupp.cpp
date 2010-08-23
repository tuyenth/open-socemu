#include "arm.h"

#include "armemu.h"

/** Return the register bank ID for a given mode
 * @param[in] mode Mode for which the bank is requested
 * @return The bank ID
 */
static uint8_t
ModeToBank(uint8_t mode)
{
    static uint8_t bankofmode[] = {
        USERBANK, FIQBANK, IRQBANK, SVCBANK,
        DUMMYBANK, DUMMYBANK, DUMMYBANK, DUMMYBANK,
        DUMMYBANK, DUMMYBANK, DUMMYBANK, DUMMYBANK,
        DUMMYBANK, DUMMYBANK, DUMMYBANK, DUMMYBANK,
        USERBANK, FIQBANK, IRQBANK, SVCBANK,
        DUMMYBANK, DUMMYBANK, DUMMYBANK, ABORTBANK,
        DUMMYBANK, DUMMYBANK, DUMMYBANK, UNDEFBANK,
        DUMMYBANK, DUMMYBANK, DUMMYBANK, SYSTEMBANK
    };

    if (mode >= (sizeof (bankofmode) / sizeof (bankofmode[0])))
        return DUMMYBANK;

    return bankofmode[mode];
}

uint32_t
arm::ARMul_SwitchMode(uint32_t oldmode, uint32_t newmode)
{
    unsigned i;
    uint32_t oldbank;
    uint32_t newbank;

    oldbank = ModeToBank(oldmode);
    newbank = m_Bank = ModeToBank (newmode);

    /* Do we really need to do it?  */
    if (oldbank != newbank) {
        /* Save away the old registers.  */
        switch (oldbank) {
        case USERBANK:
        case IRQBANK:
        case SVCBANK:
        case ABORTBANK:
        case UNDEFBANK:
            if (newbank == FIQBANK)
                for (i = 8; i < 13; i++)
                    m_RegBank[USERBANK][i] =
                        m_Reg[i];
            m_RegBank[oldbank][13] = m_Reg[13];
            m_RegBank[oldbank][14] = m_Reg[14];
            break;
        case FIQBANK:
            for (i = 8; i < 15; i++)
                m_RegBank[FIQBANK][i] = m_Reg[i];
            break;
        case DUMMYBANK:
            for (i = 8; i < 15; i++)
                m_RegBank[DUMMYBANK][i] = 0;
            break;
        default:
            abort ();
        }

        /* Restore the new registers.  */
        switch (newbank) {
        case USERBANK:
        case IRQBANK:
        case SVCBANK:
        case ABORTBANK:
        case UNDEFBANK:
            if (oldbank == FIQBANK)
                for (i = 8; i < 13; i++)
                    m_Reg[i] =
                        m_RegBank[USERBANK][i];
            m_Reg[13] = m_RegBank[newbank][13];
            m_Reg[14] = m_RegBank[newbank][14];
            break;
        case FIQBANK:
            for (i = 8; i < 15; i++)
                m_Reg[i] = m_RegBank[FIQBANK][i];
            break;
        case DUMMYBANK:
            for (i = 8; i < 15; i++)
                m_Reg[i] = 0;
            break;
        default:
            abort ();
        }
    }

    return newmode;
}

void
arm::ARMul_NegZero(uint32_t result)
{
    if (NEG (result)) {
        SETN;
        CLEARZ;
    }
    else if (result == 0) {
        CLEARN;
        SETZ;
    }
    else {
        CLEARN;
        CLEARZ;
    }
}

bool
arm::AddOverflow(uint32_t a, uint32_t b, uint32_t result)
{
    return ((NEG (a) && NEG (b) && POS (result))
        || (POS (a) && POS (b) && NEG (result)));
}

bool
arm::SubOverflow(uint32_t a, uint32_t b, uint32_t result)
{
    return ((NEG (a) && POS (b) && POS (result))
        || (POS (a) && NEG (b) && NEG (result)));
}


uint32_t
arm::ARMul_GetReg(uint8_t mode, uint8_t reg)
{
    mode &= MODEBITS;
    if (mode != m_Mode)
    {
        return (m_RegBank[ModeToBank ((uint32_t) mode)][reg]);
    }
    else
    {
        return (m_Reg[reg]);
    }
}

void
arm::ARMul_SetReg(uint8_t mode, uint8_t reg, uint32_t value)
{
    mode &= MODEBITS;
    if (mode != m_Mode)
    {
        m_RegBank[ModeToBank ((uint32_t) mode)][reg] = value;
    }
    else
    {
        m_Reg[reg] = value;
    }
}

void
arm::ARMul_AddOverflow(uint32_t a, uint32_t b, uint32_t result)
{
    ASSIGNV(AddOverflow(a, b, result));
}

void
arm::ARMul_SubOverflow(uint32_t a, uint32_t b, uint32_t result)
{
    ASSIGNV(SubOverflow(a, b, result));
}

/* Assigns the C flag after an addition of a and b to give result.  */

void
arm::ARMul_AddCarry(uint32_t a, uint32_t b, uint32_t result)
{
    ASSIGNC ((NEG (a) && NEG (b)) ||
         (NEG (a) && POS (result)) || (NEG (b) && POS (result)));
}

/* Assigns the C flag after an subtraction of a and b to give result.  */

void
arm::ARMul_SubCarry(uint32_t a, uint32_t b, uint32_t result)
{
    ASSIGNC ((NEG (a) && POS (b)) ||
         (NEG (a) && POS (result)) || (POS (b) && POS (result)));
}

bool
arm::IntPending()
{
    /* Any exceptions.  */
    if (m_NresetSig == false)
    {
        ARMul_Abort(ARMul_ResetV);
        return true;
    }
    else if (!m_NfiqSig && !FFLAG)
    {
        ARMul_Abort(ARMul_FIQV);
        return true;
    }
    else if (!m_NirqSig && !IFLAG)
    {
        ARMul_Abort(ARMul_IRQV);
        return true;
    }

    return false;
}

uint32_t
arm::ARMul_GetCPSR()
{
    return (CPSR);
}

void
arm::ARMul_SetCPSR(uint32_t value)
{
    m_Cpsr = value;
    ARMul_CPSRAltered();
}

uint32_t
arm::ARMul_GetSPSR(uint32_t mode)
{
    uint32_t bank = ModeToBank (mode & MODEBITS);

    if (!BANK_CAN_ACCESS_SPSR (bank))
        return ARMul_GetCPSR();

    return m_Spsr[bank];
}

void
arm::ARMul_SetSPSR(uint32_t mode, uint32_t value)
{
    uint32_t bank = ModeToBank (mode & MODEBITS);

    if (BANK_CAN_ACCESS_SPSR (bank))
        m_Spsr[bank] = value;
}

void
arm::ARMul_CPSRAltered()
{
    uint32_t oldmode;

    oldmode = m_Mode;

    if (m_Mode != (m_Cpsr & MODEBITS))
    {
        m_Mode = ARMul_SwitchMode(m_Mode, m_Cpsr & MODEBITS);
    }
    //m_Cpsr &= ~MODEBITS;

    ASSIGNINT (m_Cpsr & INTBITS);
    //m_Cpsr &= ~INTBITS;
    ASSIGNN ((m_Cpsr & NBIT) != 0);
    //m_Cpsr &= ~NBIT;
    ASSIGNZ ((m_Cpsr & ZBIT) != 0);
    //m_Cpsr &= ~ZBIT;
    ASSIGNC ((m_Cpsr & CBIT) != 0);
    //m_Cpsr &= ~CBIT;
    ASSIGNV ((m_Cpsr & VBIT) != 0);
    //m_Cpsr &= ~VBIT;
    ASSIGNS ((m_Cpsr & SBIT) != 0);
    //m_Cpsr &= ~SBIT;
    ASSIGNT ((m_Cpsr & TBIT) != 0);
    //m_Cpsr &= ~TBIT;

}

void
arm::ARMul_FixCPSR(uint32_t instr, uint32_t rhs)
{
    m_Cpsr = ARMul_GetCPSR();

    if (m_Mode != USER32MODE)
    {
        // in user mode, only write flags
        if (BIT (16))
            SETPSR_C (m_Cpsr, rhs);
        if (BIT (17))
            SETPSR_X (m_Cpsr, rhs);
        if (BIT (18))
            SETPSR_S (m_Cpsr, rhs);
    }
    if (BIT (19))
        SETPSR_F (m_Cpsr, rhs);
    ARMul_CPSRAltered();
}

void
arm::ARMul_FixSPSR(uint32_t instr, uint32_t rhs)
{
    if (BANK_CAN_ACCESS_SPSR (m_Bank)) {
        if (BIT (16))
            SETPSR_C (m_Spsr[m_Bank], rhs);
        if (BIT (17))
            SETPSR_X (m_Spsr[m_Bank], rhs);
        if (BIT (18))
            SETPSR_S (m_Spsr[m_Bank], rhs);
        if (BIT (19))
            SETPSR_F (m_Spsr[m_Bank], rhs);
    }
}

void
arm::ARMul_LDC(uint32_t instr, uint32_t address)
{
    UNDEF_LSCPCBaseWb;

    // check if coprocessor access is allowed
    if (!CP_ACCESS_ALLOWED (state, CPNum))
    {
        ARMul_UndefInstr();
        return;
    }

    ARMul_UndefInstr();
}

void
arm::ARMul_STC(uint32_t instr, uint32_t address)
{
    UNDEF_LSCPCBaseWb;

    // check if coprocessor access is allowed
    if (!CP_ACCESS_ALLOWED (state, CPNum)) {
        ARMul_UndefInstr();
        return;
    }

    ARMul_UndefInstr();
}

void
arm::ARMul_MCR(uint32_t instr, uint32_t source)
{
    // check if coprocessor access is allowed
    if (!CP_ACCESS_ALLOWED (state, CPNum))
    {
        ARMul_UndefInstr();
        return;
    }

    if (mmu_mcr(instr, source))
    {
        ARMul_Abort(ARMul_UndefinedInstrV);
    }
    else
    {
        BUSUSEDINCPCN;
        ARMul_Ccycles(1);
    }
}

uint32_t
arm::ARMul_MRC(uint32_t instr)
{
    uint32_t result = 0;

    // check if coprocessor access is allowed
    if (!CP_ACCESS_ALLOWED (state, CPNum))
    {
        ARMul_UndefInstr();
        return 0;
    }

    if (mmu_mrc(instr, &result))
    {
        ARMul_Abort(ARMul_UndefinedInstrV);
        // parent will destroy the flags otherwise
        result = ECC;
    }
    else
    {
        BUSUSEDINCPCN;
        ARMul_Ccycles(1);
        ARMul_Icycles(1);
    }

    return result;
}

void
arm::ARMul_CDP(uint32_t instr)
{
    // check if coprocessor access is allowed
    if (!CP_ACCESS_ALLOWED (state, CPNum))
    {
        ARMul_UndefInstr();
        return;
    }

    ARMul_UndefInstr();
}

void
arm::ARMul_UndefInstr()
{
    ARMul_Abort(ARMul_UndefinedInstrV);
}

void
arm::ARMul_Abort(uint32_t vector)
{
    uint32_t temp;
    int isize = INSN_SIZE;
    int esize = (TFLAG ? 0 : 4);
    int e2size = (TFLAG ? -4 : 0);

    m_Aborted = false;

    temp = m_Reg[15];

    switch (vector)
    {
    case ARMul_ResetV:  /* RESET */
        SETABORT (INTBITS, SVC32MODE, 0);
        break;
    case ARMul_UndefinedInstrV: /* Undefined Instruction */
        SETABORT (IBIT, UNDEF32MODE, isize);
        break;
    case ARMul_SWIV:    /* Software Interrupt */
        SETABORT (IBIT, SVC32MODE, isize);
        break;
    case ARMul_PrefetchAbortV:  /* Prefetch Abort */
        SETABORT (IBIT, ABORT32MODE, esize);
        break;
    case ARMul_DataAbortV:  /* Data Abort */
        SETABORT (IBIT, ABORT32MODE, e2size);
        break;
    case ARMul_IRQV:    /* IRQ */
        SETABORT (IBIT, IRQ32MODE, esize);
        break;
    case ARMul_FIQV:    /* FIQ */
        SETABORT (INTBITS, FIQ32MODE, esize);
        break;
    }

    // High exception address?
    vector += mmu_vector_base();

    // set the next address and flush the pipeline
    m_Reg[15] = vector & PCBITS;
    FLUSHPIPE;
}

