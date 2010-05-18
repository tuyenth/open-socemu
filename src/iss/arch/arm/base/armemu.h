#ifndef __ARMEMU_H__
#define __ARMEMU_H__

extern uint32_t isize;

/* Condition code values.  */
#define EQ 0
#define NE 1
#define CS 2
#define CC 3
#define MI 4
#define PL 5
#define VS 6
#define VC 7
#define HI 8
#define LS 9
#define GE 10
#define LT 11
#define GT 12
#define LE 13
#define AL 14
#define NV 15

/* Shift Opcodes.  */
#define LSL 0
#define LSR 1
#define ASR 2
#define ROR 3

/* Macros to twiddle the status flags and mode.  */
#define NBIT ((unsigned)1L << 31)
#define ZBIT (1L << 30)
#define CBIT (1L << 29)
#define VBIT (1L << 28)
#define SBIT (1L << 27)
#define IBIT (1L << 7)
#define FBIT (1L << 6)
#define IFBITS (3L << 6)
#define R15IBIT (1L << 27)
#define R15FBIT (1L << 26)
#define R15IFBITS (3L << 26)

#define POS(i) ( (~(i)) >> 31 )
#define NEG(i) ( (i) >> 31 )

/* ??? This bit is actually in the low order bit of the PC in the hardware.
   It isn't clear if the simulator needs to model that or not.  */
#define TBIT (1L << 5)
#define TFLAG m_TFlag
#define SETT m_TFlag = 1
#define CLEART m_TFlag = 0
#define ASSIGNT(res) m_TFlag = res
#define INSN_SIZE (TFLAG ? 2 : 4)

#define NFLAG m_NFlag
#define SETN m_NFlag = 1
#define CLEARN m_NFlag = 0
#define ASSIGNN(res) m_NFlag = res

#define ZFLAG m_ZFlag
#define SETZ m_ZFlag = 1
#define CLEARZ m_ZFlag = 0
#define ASSIGNZ(res) m_ZFlag = res

#define CFLAG m_CFlag
#define SETC m_CFlag = 1
#define CLEARC m_CFlag = 0
#define ASSIGNC(res) m_CFlag = res

#define VFLAG m_VFlag
#define SETV m_VFlag = 1
#define CLEARV m_VFlag = 0
#define ASSIGNV(res) m_VFlag = res

#define SFLAG m_SFlag
#define SETS m_SFlag = 1
#define CLEARS m_SFlag = 0
#define ASSIGNS(res) m_SFlag = res

#define IFLAG (m_IFFlags >> 1)
#define FFLAG (m_IFFlags & 1)
#define IFFLAGS m_IFFlags
#define ASSIGNINT(res) m_IFFlags = (((res) >> 6) & 3)
#define ASSIGNR15INT(res) m_IFFlags = (((res) >> 26) & 3) ;

#define PSR_FBITS (0xff000000L)
#define PSR_SBITS (0x00ff0000L)
#define PSR_XBITS (0x0000ff00L)
#define PSR_CBITS (0x000000ffL)

#define CCBITS (0xf8000000L)

#define INTBITS (0xc0L)

#define PCBITS (0xffffffffL)

#define MODEBITS (0x1fL)
#define R15INTBITS (3L << 26)

#define R15PCBITS (0x03ffffffL)

#define R15PCMODEBITS (0x03ffffffL)
#define R15MODEBITS (0x3L)

#define PCMASK PCBITS
#define PCWRAP(pc) (pc)

#define PC (m_Reg[15] & PCMASK)
#define R15CCINTMODE (m_Reg[15] & (CCBITS | R15INTBITS | R15MODEBITS))
#define R15INT (m_Reg[15] & R15INTBITS)
#define R15INTPC (m_Reg[15] & (R15INTBITS | R15PCBITS))
#define R15INTPCMODE (m_Reg[15] & (R15INTBITS | R15PCBITS | R15MODEBITS))
#define R15INTMODE (m_Reg[15] & (R15INTBITS | R15MODEBITS))
#define R15PC (m_Reg[15] & R15PCBITS)
#define R15PCMODE (m_Reg[15] & (R15PCBITS | R15MODEBITS))
#define R15MODE (m_Reg[15] & R15MODEBITS)

#define ECC ((NFLAG << 31) | (ZFLAG << 30) | (CFLAG << 29) | (VFLAG << 28) | (SFLAG << 27))
#define EINT (IFFLAGS << 6)
#define ER15INT (IFFLAGS << 26)
#define EMODE (m_Mode)

#define CPSR (ECC | EINT | EMODE | (TFLAG << 5))


#define GETSPSR(bank) (ARMul_GetSPSR(EMODE))
#define SETPSR_F(d,s) d = ((d) & ~PSR_FBITS) | ((s) & PSR_FBITS)
#define SETPSR_S(d,s) d = ((d) & ~PSR_SBITS) | ((s) & PSR_SBITS)
#define SETPSR_X(d,s) d = ((d) & ~PSR_XBITS) | ((s) & PSR_XBITS)
#define SETPSR_C(d,s) d = ((d) & ~PSR_CBITS) | ((s) & PSR_CBITS)

#define SETABORT(i, m, d)                                           \
    do                                                              \
    {                                                               \
        int SETABORT_mode = (m);                                    \
                                                                    \
        ARMul_SetSPSR(SETABORT_mode, ARMul_GetCPSR());              \
        ARMul_SetCPSR(((ARMul_GetCPSR() & ~(EMODE | TBIT)) | (i) | SETABORT_mode)); \
        m_Reg[14] = temp - (d);                                     \
    }                                                               \
    while (0)


#define TAKEABORT ARMul_Abort(ARMul_DataAbortV)

#define CPTAKEABORT                                 \
    do                                              \
    {                                               \
        if (!m_Aborted)                             \
            ARMul_Abort(ARMul_UndefinedInstrV);     \
        else                                        \
            ARMul_Abort(ARMul_DataAbortV);          \
    }                                               \
    while (0)

/// The next instruction fetch will be a normal cycle.
#define NORMALCYCLE m_NextInstr = SEQ
/// The next instruction fetch will be an N cycle because bus was used.
#define BUSUSEDN    m_NextInstr |= NONSEQ
/// The next instruction fetch will be sequential and pc incremented
#define BUSUSEDINCPCS                                           \
    do                                                          \
    {                                                           \
        if (!m_IsV4)                                            \
        {                                                       \
            /* A standard PC inc and an S cycle. */             \
            m_Reg[15] += isize;                                 \
            m_NextInstr |= PCINCEDSEQ;                          \
        }                                                       \
    }                                                           \
    while (0)
/// The next instruction fetch will be non sequential and pc incremented
#define BUSUSEDINCPCN                                           \
    do                                                          \
    {                                                           \
        if (m_IsV4)                                             \
            BUSUSEDN;                                           \
        else                                                    \
        {                                                       \
            /* A standard PC inc and an N cycle. */             \
            m_Reg[15] += isize;                                 \
            m_NextInstr |= PCINCEDNONSEQ;                       \
        }                                                       \
    }                                                           \
    while (0)
/// The next instruction fetch will be with pc already incremented
#define INCPC                                                   \
    do                                                          \
    {                                                           \
        /* A standard PC inc. */                                \
        m_Reg[15] += isize;                                     \
        m_NextInstr |= PCINCEDSEQ;                              \
    }                                                           \
    while (0)
/// The next instruction fetch will re-fill the pipeline
#define FLUSHPIPE m_NextInstr |= PRIMEPIPE

/* Cycle based emulation.  */
#define OUTPUTCP(i,a,b)
#define NCYCLE
#define SCYCLE
#define ICYCLE
#define CCYCLE
#define NEXTCYCLE(c)

/* Macros to extract parts of instructions.  */
#define DESTReg (BITS (12, 15))
#define LHSReg  (BITS (16, 19))
#define RHSReg  (BITS ( 0,  3))

#define DEST (m_Reg[DESTReg])

#define LHS ((LHSReg == 15) ? (m_Reg[15] & 0xFFFFFFFC): (m_Reg[LHSReg]))

#define MULDESTReg (BITS (16, 19))
#define MULLHSReg  (BITS ( 0,  3))
#define MULRHSReg  (BITS ( 8, 11))
#define MULACCReg  (BITS (12, 15))

#define DPImmRHS (ARMul_ImmedTable[BITS(0, 11)])
#define DPSImmRHS temp = BITS(0,11) ; \
                  rhs = ARMul_ImmedTable[temp] ; \
                  if (temp > 255) /* There was a shift.  */ \
                     ASSIGNC (rhs >> 31) ;

#define DPRegRHS  ((BITS (4,11) == 0) ? m_Reg[RHSReg] \
                                      : GetDPRegRHS(instr))
#define DPSRegRHS ((BITS (4,11) == 0) ? m_Reg[RHSReg] \
                                      : GetDPSRegRHS(instr))

#define LSBase m_Reg[LHSReg]
#define LSImmRHS (BITS(0,11))

#define LSRegRHS ((BITS (4, 11) == 0) ? m_Reg[RHSReg] \
                                      : GetLSRegRHS(instr))

#define LSMNumRegs ((uint32_t) ARMul_BitList[BITS (0, 7)] + \
                    (uint32_t) ARMul_BitList[BITS (8, 15)] )
#define LSMBaseFirst ((LHSReg == 0 && BIT (0)) || \
                      (BIT (LHSReg) && BITS (0, LHSReg - 1) == 0))

#define SWAPSRC (m_Reg[RHSReg])

#define LSCOff (BITS (0, 7) << 2)
#define CPNum   BITS (8, 11)

// Only co processor 14 and 15 are allowed
#define CP_ACCESS_ALLOWED(STATE, CP)                \
    ( ((CP) >= 14) )

/* Macro to rotate n right by b bits.  */
#define ROTATER(n, b) (((n) >> (b)) | ((n) << (32 - (b))))

/* Macros to store results of instructions.  */
#define WRITEDEST(d)                        \
    do                                      \
    {                                       \
      if (DESTReg == 15)                    \
          WriteR15(d);                      \
      else                                  \
          DEST = d;                         \
    }                                       \
    while (0)

#define WRITESDEST(d)           \
    do                          \
    {                           \
        if (DESTReg == 15)      \
            WriteSR15(d);       \
        else                    \
        {                       \
            DEST = d;           \
            ARMul_NegZero(d);   \
        }                       \
    }                           \
    while (0)

#define WRITEDESTB(d)           \
    do                          \
    {                           \
        if (DESTReg == 15)      \
            WriteR15Branch(d);  \
        else                    \
            DEST = d;           \
    }                           \
    while (0)

#define LOADMULT(instr,   address, wb)  LoadMult   (instr, address, wb)
#define LOADSMULT(instr,  address, wb)  LoadSMult  (instr, address, wb)
#define STOREMULT(instr,  address, wb)  StoreMult  (instr, address, wb)
#define STORESMULT(instr, address, wb)  StoreSMult (instr, address, wb)

#define POSBRANCH ((instr & 0x7fffff) << 2)
#define NEGBRANCH ((0xff000000 |(instr & 0xffffff)) << 2)

/// Number of I cycles for a mult.
extern unsigned ARMul_MultTable[];
/// Immediate DP LHS values.
extern uint32_t ARMul_ImmedTable[];
/// Number of bits in a byte table.
extern char ARMul_BitList[];

/* Macros to scrutinize instructions.  */
#define UNDEF_Test
#define UNDEF_Shift
#define UNDEF_MSRPC
#define UNDEF_MRSPC
#define UNDEF_MULPCDest
#define UNDEF_MULDestEQOp1
#define UNDEF_LSRBPC
#define UNDEF_LSRBaseEQOffWb
#define UNDEF_LSRBaseEQDestWb
#define UNDEF_LSRPCBaseWb
#define UNDEF_LSRPCOffWb
#define UNDEF_LSMNoRegs
#define UNDEF_LSMPCBase
#define UNDEF_LSMUserBankWb
#define UNDEF_LSMBaseInListWb
#define UNDEF_SWPPC
#define UNDEF_CoProHS
#define UNDEF_MCRPC
#define UNDEF_LSCPCBaseWb
#define UNDEF_UndefNotBounced
#define UNDEF_ShortInt
#define UNDEF_IllegalMode
#define UNDEF_Prog32SigChange
#define UNDEF_Data32SigChange

#endif
