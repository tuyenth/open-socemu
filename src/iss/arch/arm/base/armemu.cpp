#include <assert.h>

#include "arm.h"

#include "armemu.h"

#define LUNSIGNED (0)		/* unsigned operation */
#define LSIGNED   (1)		/* signed operation */
#define LDEFAULT  (0)		/* default : do nothing */
#define LSCC      (1)		/* set condition codes on result */


/* Short-hand macros for LDR/STR.  */

/* Store post decrement writeback.  */
#define SHDOWNWB()                                      \
  lhs = LHS ;                                           \
  if (StoreHalfWord(instr, lhs))                        \
     LSBase = lhs - GetLS7RHS(instr);

/* Store post increment writeback.  */
#define SHUPWB()                                        \
  lhs = LHS ;                                           \
  if (StoreHalfWord(instr, lhs))                        \
     LSBase = lhs + GetLS7RHS(instr);

/* Store pre decrement.  */
#define SHPREDOWN()                                     \
  (void)StoreHalfWord(instr, LHS - GetLS7RHS(instr));

/* Store pre decrement writeback.  */
#define SHPREDOWNWB()                                   \
  temp = LHS - GetLS7RHS(instr);                        \
  if (StoreHalfWord(instr, temp))                       \
     LSBase = temp;

/* Store pre increment.  */
#define SHPREUP()                                       \
  (void)StoreHalfWord(instr, LHS + GetLS7RHS(instr));

/* Store pre increment writeback.  */
#define SHPREUPWB()                                     \
  temp = LHS + GetLS7RHS(instr);                        \
  if (StoreHalfWord(instr, temp))                       \
     LSBase = temp;

/* Load post decrement writeback.  */
#define LHPOSTDOWN()                                    \
{                                                       \
  int done = 1;                                         \
  lhs = LHS;                                            \
  temp = lhs - GetLS7RHS(instr);                        \
                                                        \
  switch (BITS (5, 6))                                  \
    {                                                   \
    case 1: /* H */                                     \
      if (LoadHalfWord(instr, lhs, LUNSIGNED))          \
         LSBase = temp;                                 \
      break;                                            \
    case 2: /* SB */                                    \
      if (LoadByte(instr, lhs, LSIGNED))                \
         LSBase = temp;                                 \
      break;                                            \
    case 3: /* SH */                                    \
      if (LoadHalfWord(instr, lhs, LSIGNED))            \
         LSBase = temp;                                 \
      break;                                            \
    case 0: /* SWP handled elsewhere.  */               \
    default:                                            \
      done = 0;                                         \
      break;                                            \
    }                                                   \
  if (done)                                             \
     break;                                             \
}

/* Load post increment writeback.  */
#define LHPOSTUP()                                      \
{                                                       \
  int done = 1;                                         \
  lhs = LHS;                                            \
  temp = lhs + GetLS7RHS(instr);                        \
                                                        \
  switch (BITS (5, 6))                                  \
    {                                                   \
    case 1: /* H */                                     \
      if (LoadHalfWord(instr, lhs, LUNSIGNED))          \
         LSBase = temp;                                 \
      break;                                            \
    case 2: /* SB */                                    \
      if (LoadByte(instr, lhs, LSIGNED))                \
         LSBase = temp;                                 \
      break;                                            \
    case 3: /* SH */                                    \
      if (LoadHalfWord(instr, lhs, LSIGNED))            \
         LSBase = temp;                                 \
      break;                                            \
    case 0: /* SWP handled elsewhere.  */               \
    default:                                            \
      done = 0;                                         \
      break;                                            \
    }                                                   \
  if (done)                                             \
     break;                                             \
}

/* Load pre decrement.  */
#define LHPREDOWN()                                     \
{                                                       \
  int done = 1;                                         \
                                                        \
  temp = LHS - GetLS7RHS(instr);                        \
  switch (BITS (5, 6))                                  \
    {                                                   \
    case 1: /* H */                                     \
      (void) LoadHalfWord(instr, temp, LUNSIGNED);      \
      break;                                            \
    case 2: /* SB */                                    \
      (void) LoadByte(instr, temp, LSIGNED);            \
      break;                                            \
    case 3: /* SH */                                    \
      (void) LoadHalfWord(instr, temp, LSIGNED);        \
      break;                                            \
    case 0:                                             \
      /* SWP handled elsewhere.  */                     \
    default:                                            \
      done = 0;                                         \
      break;                                            \
    }                                                   \
  if (done)                                             \
     break;                                             \
}

/* Load pre decrement writeback.  */
#define LHPREDOWNWB()                                   \
{                                                       \
  int done = 1;                                         \
                                                        \
  temp = LHS - GetLS7RHS(instr);                        \
  switch (BITS (5, 6))                                  \
    {                                                   \
    case 1: /* H */                                     \
      if (LoadHalfWord(instr, temp, LUNSIGNED))         \
         LSBase = temp;                                 \
      break;                                            \
    case 2: /* SB */                                    \
      if (LoadByte(instr, temp, LSIGNED))               \
         LSBase = temp;                                 \
      break;                                            \
    case 3: /* SH */                                    \
      if (LoadHalfWord(instr, temp, LSIGNED))           \
         LSBase = temp;                                 \
      break;                                            \
    case 0:                                             \
      /* SWP handled elsewhere.  */                     \
    default:                                            \
      done = 0;                                         \
      break;                                            \
    }                                                   \
  if (done)                                             \
     break;                                             \
}

/* Load pre increment.  */
#define LHPREUP()                                       \
{                                                       \
  int done = 1;                                         \
                                                        \
  temp = LHS + GetLS7RHS(instr);                        \
  switch (BITS (5, 6))                                  \
    {                                                   \
    case 1: /* H */                                     \
      (void) LoadHalfWord(instr, temp, LUNSIGNED);      \
      break;                                            \
    case 2: /* SB */                                    \
      (void) LoadByte(instr, temp, LSIGNED);            \
      break;                                            \
    case 3: /* SH */                                    \
      (void) LoadHalfWord(instr, temp, LSIGNED);        \
      break;                                            \
    case 0:                                             \
      /* SWP handled elsewhere.  */                     \
    default:                                            \
      done = 0;                                         \
      break;                                            \
    }                                                   \
  if (done)                                             \
     break;                                             \
}

/* Load pre increment writeback.  */
#define LHPREUPWB()                                     \
{                                                       \
  int done = 1;                                         \
                                                        \
  temp = LHS + GetLS7RHS(instr);                        \
  switch (BITS (5, 6))                                  \
    {                                                   \
    case 1: /* H */                                     \
      if (LoadHalfWord(instr, temp, LUNSIGNED))         \
	LSBase = temp;                                      \
      break;                                            \
    case 2: /* SB */                                    \
      if (LoadByte(instr, temp, LSIGNED))               \
	LSBase = temp;                                      \
      break;                                            \
    case 3: /* SH */                                    \
      if (LoadHalfWord(instr, temp, LSIGNED))           \
	LSBase = temp;                                      \
      break;                                            \
    case 0:                                             \
      /* SWP handled elsewhere.  */                     \
    default:                                            \
      done = 0;                                         \
      break;                                            \
    }                                                   \
  if (done)                                             \
     break;                                             \
}

/* The PC pipeline value depends on whether ARM
   or Thumb instructions are being executed.  */
uint32_t isize;


void
arm::emulate()
{
	uint32_t instr;		/* The current instruction.  */
	uint32_t dest = 0;	/* Almost the DestBus.  */
	uint32_t temp;		/* Ubiquitous third hand.  */
	uint32_t lhs;		/* Almost the ABus and BBus.  */
	uint32_t rhs;

	/* Execute the next instruction.  */
	do {
		/* Just keep going.  */
		isize = INSN_SIZE;

		switch (m_NextInstr)
		{
		case SEQ:
			/* Advance the pipeline, and an S cycle.  */
			m_Reg[15] += isize;
			m_PC += isize;
			instr = m_Decoded;
			m_Decoded = m_Loaded;
			m_Loaded = ARMul_LoadInstrS(m_PC + (isize * 2), isize);
			break;

		case NONSEQ:
			/* Advance the pipeline, and an N cycle.  */
			m_Reg[15] += isize;
			m_PC += isize;
			instr = m_Decoded;
			m_Decoded = m_Loaded;
			m_Loaded = ARMul_LoadInstrN(m_PC + (isize * 2), isize);
			NORMALCYCLE;
			break;

		case PCINCEDSEQ:
			/* Program counter advanced, and an S cycle.  */
		    m_PC += isize;
			instr = m_Decoded;
			m_Decoded = m_Loaded;
			m_Loaded = ARMul_LoadInstrS(m_PC + (isize * 2), isize);
			NORMALCYCLE;
			break;

		case PCINCEDNONSEQ:
			/* Program counter advanced, and an N cycle.  */
		    m_PC += isize;
			instr = m_Decoded;
			m_Decoded = m_Loaded;
			m_Loaded = ARMul_LoadInstrN(m_PC + (isize * 2), isize);
			NORMALCYCLE;
			break;

		default:
			/* The program counter has been changed.  */
		    m_PC = m_Reg[15];
			m_Reg[15] = m_PC + (isize * 2);
			m_Aborted = 0;

			instr = ARMul_LoadInstrN(m_PC, isize);
			m_Decoded = ARMul_LoadInstrS(m_PC + (isize), isize);
			m_Loaded = ARMul_LoadInstrS(m_PC + (isize * 2), isize);
			NORMALCYCLE;
			break;
		}

        /* Any exceptions ?  */
        if (m_NresetSig == false)
        {
            ARMul_Abort(ARMul_ResetV);
            break;
        }
        else if (!m_NfiqSig && !FFLAG)
        {
            ARMul_Abort(ARMul_FIQV);
            break;
        }
        else if (!m_NirqSig && !IFLAG)
        {
            ARMul_Abort(ARMul_IRQV);
            break;
        }

        // check if there was a new remote connection
        m_gdbconnected = gdbserver::checkremote(false);

        // if debugger is connected
        if (m_gdbconnected)
        {
            // if in run mode, check breakpoints and stop request
            if (m_Emulate == RUN)
            {
                int breakpt;

                // check if a breakpoint was reached
                for (breakpt = 0; breakpt < m_NumBreakPts; breakpt++)
                {
                    if (m_BreakPts[breakpt] == m_PC)
                    {
                        goto armulate_trapped;
                    }
                }

                // check if ctrl-c (stop request) was received
                if (gdbserver::checkctrlc())
                {
                    goto armulate_trapped;
                }

                // not trapped
                goto armulate_nottrapped;
            }
armulate_trapped:
            // send signal to debugger
            gdbserver::handlesig(SIGTRAP);

            // check if the pc was modified
            if (m_PCChanged)
            {
                // flush the pipeline
                FLUSHPIPE;

                // reset the bit
                m_PCChanged = false;

                // do not execute the current instruction
                break;
            }
        }
armulate_nottrapped:
        // increment the number of instructions executed
		m_NumInstrs++;

		/* Provide Thumb instruction decoding. If the processor is in Thumb
		   mode, then we can simply decode the Thumb instruction, and map it
		   to the corresponding ARM instruction (by directly loading the
		   instr variable, and letting the normal ARM simulator
		   execute). There are some caveats to ensure that the correct
		   pipelined PC value is used when executing Thumb code, and also for
		   dealing with the BL instruction.  */
		if (TFLAG)
		{
			uint32_t new_instruction;

			/* Check if in Thumb mode.  */
			switch (ARMul_ThumbDecode(m_PC, instr, &new_instruction))
			{
			case t_undefined:
				/* This is a Thumb instruction.  */
				ARMul_UndefInstr();
				goto donext;

			case t_branch:
				/* Already processed.  */
				goto donext;

			case t_decoded:
				/* ARM instruction available.  */
				//printf("t decode %04lx -> %08lx\n", instr & 0xffff, new_instruction);
				instr = new_instruction;
				/* So continue instruction decoding.  */
				break;
			default:
				break;
			}
		}

		/* Check the condition codes.  */
		if ((temp = TOPBITS (28)) == AL)
			/* Vile deed in the need for speed.  */
			goto mainswitch;

		/* Check the condition code.  */
		switch ((int) TOPBITS (28)) {
		case AL:
			temp = true;
			break;
		case NV:
			if (m_IsV5)
			{
				if (BITS (25, 27) == 5) {	/* BLX(1) */
					uint32_t dest;

					m_Reg[14] = m_PC + 4;

					/* Force entry into Thumb mode.  */
					dest = m_PC + 8 + 1;
					if (BIT (23))
						dest += (NEGBRANCH +
							 (BIT (24) << 1));
					else
						dest += POSBRANCH +
							(BIT (24) << 1);

					WriteR15Branch(dest);
					goto donext;
				}
				else if ((instr & 0xFC70F000) == 0xF450F000)
					/* The PLD instruction.  Ignored.  */
					goto donext;
				else if (((instr & 0xfe500f00) == 0xfc100100)
					 || ((instr & 0xfe500f00) ==
					     0xfc000100))
					/* wldrw and wstrw are unconditional.  */
					goto mainswitch;
				else
					/* UNDEFINED in v5, UNPREDICTABLE in v3, v4, non executed in v1, v2.  */
					ARMul_UndefInstr();
			}
			temp = false;
			break;
		case EQ:
			temp = ZFLAG;
			break;
		case NE:
			temp = !ZFLAG;
			break;
		case VS:
			temp = VFLAG;
			break;
		case VC:
			temp = !VFLAG;
			break;
		case MI:
			temp = NFLAG;
			break;
		case PL:
			temp = !NFLAG;
			break;
		case CS:
			temp = CFLAG;
			break;
		case CC:
			temp = !CFLAG;
			break;
		case HI:
			temp = (CFLAG && !ZFLAG);
			break;
		case LS:
			temp = (!CFLAG || ZFLAG);
			break;
		case GE:
			temp = ((!NFLAG && !VFLAG) || (NFLAG && VFLAG));
			break;
		case LT:
			temp = ((NFLAG && !VFLAG) || (!NFLAG && VFLAG));
			break;
		case GT:
			temp = ((!NFLAG && !VFLAG && !ZFLAG)
				|| (NFLAG && VFLAG && !ZFLAG));
			break;
		case LE:
			temp = ((NFLAG && !VFLAG) || (!NFLAG && VFLAG))
				|| ZFLAG;
			break;
		}		/* cc check */

		/* Actual execution of instructions begins here.  */
		/* If the condition codes don't match, stop here.  */
		if (temp) {
		      mainswitch:

			switch ((int) BITS (20, 27)) {
				/* Data Processing Register RHS Instructions.  */

			case 0x00:	/* AND reg and MUL */
				if (BITS (4, 11) == 0xB) {
					/* STRH register offset, no write-back, down, post indexed.  */
					SHDOWNWB ();
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				if (BITS (4, 7) == 9) {
					/* MUL */
					rhs = m_Reg[MULRHSReg];
					if (MULLHSReg == MULDESTReg) {
						UNDEF_MULDestEQOp1;
						m_Reg[MULDESTReg] = 0;
					}
					else if (MULDESTReg != 15)
						m_Reg[MULDESTReg] =
							m_Reg[MULLHSReg] * rhs;
					else
						UNDEF_MULPCDest;

					for (dest = 0, temp = 0; dest < 32;
					     dest++)
						if (rhs & (1L << dest))
							temp = dest;

					/* Mult takes this many/2 I cycles.  */
					ARMul_Icycles(ARMul_MultTable[temp]);
				}
				else {
					/* AND reg.  */
					rhs = DPRegRHS;
					dest = LHS & rhs;
					WRITEDEST (dest);
				}
				break;

			case 0x01:	/* ANDS reg and MULS */
				if ((BITS (4, 11) & 0xF9) == 0x9)
					/* LDR register offset, no write-back, down, post indexed.  */
					LHPOSTDOWN ();
				/* Fall through to rest of decoding.  */
				if (BITS (4, 7) == 9) {
					/* MULS */
					rhs = m_Reg[MULRHSReg];

					if (MULLHSReg == MULDESTReg) {
						UNDEF_MULDestEQOp1;
						m_Reg[MULDESTReg] = 0;
						CLEARN;
						SETZ;
					}
					else if (MULDESTReg != 15) {
						dest = m_Reg[MULLHSReg] *
							rhs;
						ARMul_NegZero(dest);
						m_Reg[MULDESTReg] = dest;
					}
					else
						UNDEF_MULPCDest;

					for (dest = 0, temp = 0; dest < 32;
					     dest++)
						if (rhs & (1L << dest))
							temp = dest;

					/* Mult takes this many/2 I cycles.  */
					ARMul_Icycles(ARMul_MultTable[temp]);
				}
				else {
					/* ANDS reg.  */
					rhs = DPSRegRHS;
					dest = LHS & rhs;
					WRITESDEST (dest);
				}
				break;

			case 0x02:	/* EOR reg and MLA */
				if (BITS (4, 11) == 0xB) {
					/* STRH register offset, write-back, down, post indexed.  */
					SHDOWNWB ();
					break;
				}
				if (BITS (4, 7) == 9) {	/* MLA */
					rhs = m_Reg[MULRHSReg];
					if (MULLHSReg == MULDESTReg) {
						UNDEF_MULDestEQOp1;
						m_Reg[MULDESTReg] =
							m_Reg[MULACCReg];
					}
					else if (MULDESTReg != 15)
						m_Reg[MULDESTReg] =
							m_Reg[MULLHSReg] * rhs +
							m_Reg[MULACCReg];
					else
						UNDEF_MULPCDest;

					for (dest = 0, temp = 0; dest < 32;
					     dest++)
						if (rhs & (1L << dest))
							temp = dest;

					/* Mult takes this many/2 I cycles.  */
					ARMul_Icycles(ARMul_MultTable[temp]);
				}
				else {
					rhs = DPRegRHS;
					dest = LHS ^ rhs;
					WRITEDEST (dest);
				}
				break;

			case 0x03:	/* EORS reg and MLAS */
				if ((BITS (4, 11) & 0xF9) == 0x9)
					/* LDR register offset, write-back, down, post-indexed.  */
					LHPOSTDOWN ();
				/* Fall through to rest of the decoding.  */
				if (BITS (4, 7) == 9) {
					/* MLAS */
					rhs = m_Reg[MULRHSReg];

					if (MULLHSReg == MULDESTReg) {
						UNDEF_MULDestEQOp1;
						dest = m_Reg[MULACCReg];
						ARMul_NegZero(dest);
						m_Reg[MULDESTReg] = dest;
					}
					else if (MULDESTReg != 15) {
						dest = m_Reg[MULLHSReg] *
							rhs +
							m_Reg[MULACCReg];
						ARMul_NegZero(dest);
						m_Reg[MULDESTReg] = dest;
					}
					else
						UNDEF_MULPCDest;

					for (dest = 0, temp = 0; dest < 32;
					     dest++)
						if (rhs & (1L << dest))
							temp = dest;

					/* Mult takes this many/2 I cycles.  */
					ARMul_Icycles(ARMul_MultTable[temp]);
				}
				else {
					/* EORS Reg.  */
					rhs = DPSRegRHS;
					dest = LHS ^ rhs;
					WRITESDEST (dest);
				}
				break;

			case 0x04:	/* SUB reg */
				if (BITS (4, 7) == 0xB) {
					/* STRH immediate offset, no write-back, down, post indexed.  */
					SHDOWNWB ();
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				rhs = DPRegRHS;
				dest = LHS - rhs;
				WRITEDEST (dest);
				break;

			case 0x05:	/* SUBS reg */
				if ((BITS (4, 7) & 0x9) == 0x9)
					/* LDR immediate offset, no write-back, down, post indexed.  */
					LHPOSTDOWN ();
				/* Fall through to the rest of the instruction decoding.  */
				lhs = LHS;
				rhs = DPRegRHS;
				dest = lhs - rhs;

				if ((lhs >= rhs) || ((rhs | lhs) >> 31)) {
					ARMul_SubCarry(lhs, rhs, dest);
					ARMul_SubOverflow(lhs, rhs, dest);
				}
				else {
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x06:	/* RSB reg */
				if (BITS (4, 7) == 0xB) {
					/* STRH immediate offset, write-back, down, post indexed.  */
					SHDOWNWB ();
					break;
				}
				rhs = DPRegRHS;
				dest = rhs - LHS;
				WRITEDEST (dest);
				break;

			case 0x07:	/* RSBS reg */
				if ((BITS (4, 7) & 0x9) == 0x9)
					/* LDR immediate offset, write-back, down, post indexed.  */
					LHPOSTDOWN ();
				/* Fall through to remainder of instruction decoding.  */
				lhs = LHS;
				rhs = DPRegRHS;
				dest = rhs - lhs;

				if ((rhs >= lhs) || ((rhs | lhs) >> 31)) {
					ARMul_SubCarry(rhs, lhs, dest);
					ARMul_SubOverflow(rhs, lhs, dest);
				}
				else {
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x08:	/* ADD reg */
				if (BITS (4, 11) == 0xB) {
					/* STRH register offset, no write-back, up, post indexed.  */
					SHUPWB ();
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0x9) {
					/* MULL */
					/* 32x32 = 64 */
					ARMul_Icycles(Multiply64(instr, LUNSIGNED, LDEFAULT));
					break;
				}
				rhs = DPRegRHS;
				dest = LHS + rhs;
				WRITEDEST (dest);
				break;

			case 0x09:	/* ADDS reg */
				if ((BITS (4, 11) & 0xF9) == 0x9)
					/* LDR register offset, no write-back, up, post indexed.  */
					LHPOSTUP ();
				/* Fall through to remaining instruction decoding.  */
				if (BITS (4, 7) == 0x9) {
					/* MULL */
					/* 32x32=64 */
					ARMul_Icycles(Multiply64(instr, LUNSIGNED, LSCC));
					break;
				}
				lhs = LHS;
				rhs = DPRegRHS;
				dest = lhs + rhs;
				ASSIGNZ (dest == 0);
				if ((lhs | rhs) >> 30) {
					/* Possible C,V,N to set.  */
					ASSIGNN (NEG (dest));
					ARMul_AddCarry(lhs, rhs, dest);
					ARMul_AddOverflow(lhs, rhs, dest);
				}
				else {
					CLEARN;
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x0a:	/* ADC reg */
				if (BITS (4, 11) == 0xB) {
					/* STRH register offset, write-back, up, post-indexed.  */
					SHUPWB ();
					break;
				}
				if (BITS (4, 7) == 0x9) {
					/* MULL */
					/* 32x32=64 */
					ARMul_Icycles(MultiplyAdd64(instr, LUNSIGNED, LDEFAULT));
					break;
				}
				rhs = DPRegRHS;
				dest = LHS + rhs + CFLAG;
				WRITEDEST (dest);
				break;

			case 0x0b:	/* ADCS reg */
				if ((BITS (4, 11) & 0xF9) == 0x9)
					/* LDR register offset, write-back, up, post indexed.  */
					LHPOSTUP ();
				/* Fall through to remaining instruction decoding.  */
				if (BITS (4, 7) == 0x9) {
					/* MULL */
					/* 32x32=64 */
					ARMul_Icycles(MultiplyAdd64(instr, LUNSIGNED, LSCC));
					break;
				}
				lhs = LHS;
				rhs = DPRegRHS;
				dest = lhs + rhs + CFLAG;
				ASSIGNZ (dest == 0);
				if ((lhs | rhs) >> 30) {
					/* Possible C,V,N to set.  */
					ASSIGNN (NEG (dest));
					ARMul_AddCarry(lhs, rhs, dest);
					ARMul_AddOverflow(lhs, rhs, dest);
				}
				else {
					CLEARN;
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x0c:	/* SBC reg */
				if (BITS (4, 7) == 0xB) {
					/* STRH immediate offset, no write-back, up post indexed.  */
					SHUPWB ();
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0x9) {
					/* MULL */
					/* 32x32=64 */
					ARMul_Icycles(Multiply64(instr, LSIGNED, LDEFAULT));
					break;
				}
				rhs = DPRegRHS;
				dest = LHS - rhs - !CFLAG;
				WRITEDEST (dest);
				break;

			case 0x0d:	/* SBCS reg */
				if ((BITS (4, 7) & 0x9) == 0x9)
					/* LDR immediate offset, no write-back, up, post indexed.  */
					LHPOSTUP ();

				if (BITS (4, 7) == 0x9) {
					/* MULL */
					/* 32x32=64 */
					ARMul_Icycles(Multiply64(instr, LSIGNED, LSCC));
					break;
				}
				lhs = LHS;
				rhs = DPRegRHS;
				dest = lhs - rhs - !CFLAG;
				if ((lhs >= rhs) || ((rhs | lhs) >> 31)) {
					ARMul_SubCarry(lhs, rhs, dest);
					ARMul_SubOverflow(lhs, rhs, dest);
				}
				else {
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x0e:	/* RSC reg */
				if (BITS (4, 7) == 0xB) {
					/* STRH immediate offset, write-back, up, post indexed.  */
					SHUPWB ();
					break;
				}

				if (BITS (4, 7) == 0x9) {
					/* MULL */
					/* 32x32=64 */
					ARMul_Icycles(MultiplyAdd64(instr, LSIGNED, LDEFAULT));
					break;
				}
				rhs = DPRegRHS;
				dest = rhs - LHS - !CFLAG;
				WRITEDEST (dest);
				break;

			case 0x0f:	/* RSCS reg */
				if ((BITS (4, 7) & 0x9) == 0x9)
					/* LDR immediate offset, write-back, up, post indexed.  */
					LHPOSTUP ();
				/* Fall through to remaining instruction decoding.  */

				if (BITS (4, 7) == 0x9) {
					/* MULL */
					/* 32x32=64 */
					ARMul_Icycles(MultiplyAdd64(instr, LSIGNED, LSCC));
					break;
				}
				lhs = LHS;
				rhs = DPRegRHS;
				dest = rhs - lhs - !CFLAG;

				if ((rhs >= lhs) || ((rhs | lhs) >> 31)) {
					ARMul_SubCarry(rhs, lhs, dest);
					ARMul_SubOverflow(rhs, lhs, dest);
				}
				else {
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x10:	/* TST reg and MRS CPSR and SWP word.  */
				if (m_IsV5e) {
					if (BIT (4) == 0 && BIT (7) == 1) {
						/* ElSegundo SMLAxy insn.  */
						uint32_t op1 =
							m_Reg[BITS (0, 3)];
						uint32_t op2 =
							m_Reg[BITS (8, 11)];
						uint32_t Rn =
							m_Reg[BITS (12, 15)];

						if (BIT (5))
							op1 >>= 16;
						if (BIT (6))
							op2 >>= 16;
						op1 &= 0xFFFF;
						op2 &= 0xFFFF;
						if (op1 & 0x8000)
							op1 -= 65536;
						if (op2 & 0x8000)
							op2 -= 65536;
						op1 *= op2;

						if (AddOverflow(op1, Rn, op1 + Rn))
							SETS;
						m_Reg[BITS (16, 19)] =
							op1 + Rn;
						break;
					}

					if (BITS (4, 11) == 5) {
						/* ElSegundo QADD insn.  */
						uint32_t op1 =
							m_Reg[BITS (0, 3)];
						uint32_t op2 =
							m_Reg[BITS (16, 19)];
						uint32_t result = op1 + op2;
						if (AddOverflow(op1, op2, result)) {
							result = POS (result)
								? 0x80000000 :
								0x7fffffff;
							SETS;
						}
						m_Reg[BITS (12, 15)] =
							result;
						break;
					}
				}
				if (BITS (4, 11) == 0xB) {
					/* STRH register offset, no write-back, down, pre indexed.  */
					SHPREDOWN ();
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				if (BITS (4, 11) == 9) {
					/* SWP */
					UNDEF_SWPPC;
					temp = LHS;
					BUSUSEDINCPCS;
					dest = ARMul_SwapWord(temp,
							       m_Reg
							       [RHSReg]);
					if (temp & 3)
						DEST = ARMul_Align(temp, dest);
					else
						DEST = dest;
					if (m_AbortSig || m_Aborted)
						TAKEABORT;
				}
				else if ((BITS (0, 11) == 0) && (LHSReg == 15)) {	/* MRS CPSR */
					UNDEF_MRSPC;
					DEST = ECC | EINT | EMODE;
				}
				else {
					UNDEF_Test;
				}
				break;

			case 0x11:	/* TSTP reg */
				if ((BITS (4, 11) & 0xF9) == 0x9)
					/* LDR register offset, no write-back, down, pre indexed.  */
					LHPREDOWN ();
				/* Continue with remaining instruction decode.  */
				if (DESTReg == 15) {
					/* TSTP reg */
					//chy 2006-02-15 if in user mode, can not set cpsr 0:23
					//from p165 of ARMARM book
					m_Cpsr = GETSPSR (m_Bank);
					ARMul_CPSRAltered();
				}
				else {
					/* TST reg */
					rhs = DPSRegRHS;
					dest = LHS & rhs;
					ARMul_NegZero(dest);
				}
				break;

			case 0x12:	/* TEQ reg and MSR reg to CPSR.  */
				if (m_IsV5) {
					if (BITS (4, 7) == 3) {
						/* BLX(2) */
						uint32_t temp;

						if (TFLAG)
							temp = (m_PC + 2) | 1;
						else
							temp = m_PC + 4;

						WriteR15Branch(m_Reg[RHSReg]);
						m_Reg[14] = temp;
						break;
					}
				}

				if (m_IsV5e) {
					if (BIT (4) == 0 && BIT (7) == 1
					    && (BIT (5) == 0
						|| BITS (12, 15) == 0)) {
						/* ElSegundo SMLAWy/SMULWy insn.  */
						unsigned long long op1 =
							m_Reg[BITS (0, 3)];
						unsigned long long op2 =
							m_Reg[BITS (8, 11)];
						unsigned long long result;

						if (BIT (6))
							op2 >>= 16;
						if (op1 & 0x80000000)
							op1 -= 1ULL << 32;
						op2 &= 0xFFFF;
						if (op2 & 0x8000)
							op2 -= 65536;
						result = (op1 * op2) >> 16;

						if (BIT (5) == 0) {
							uint32_t Rn =
								m_Reg[BITS(12, 15)];

							if (AddOverflow(result, Rn, result + Rn))
								SETS;
							result += Rn;
						}
						m_Reg[BITS (16, 19)] =
							result;
						break;
					}

					if (BITS (4, 11) == 5) {
						/* ElSegundo QSUB insn.  */
						uint32_t op1 =
							m_Reg[BITS (0, 3)];
						uint32_t op2 =
							m_Reg[BITS (16, 19)];
						uint32_t result = op1 - op2;

						if (SubOverflow(op1, op2, result)) {
							result = POS (result)
								? 0x80000000 :
								0x7fffffff;
							SETS;
						}

						m_Reg[BITS (12, 15)] =
							result;
						break;
					}
				}
				if (BITS (4, 11) == 0xB) {
					/* STRH register offset, write-back, down, pre indexed.  */
					SHPREDOWNWB ();
					break;
				}
				if (BITS (4, 27) == 0x12FFF1) {
					/* BX */
					WriteR15Branch(m_Reg[RHSReg]);
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				if (m_IsV5) {
					if (BITS (4, 7) == 0x7)
					{
						/* Hardware is allowed to optionally override this
						   instruction and treat it as a breakpoint.  Since
						   this is a simulator not hardware, we take the position
						   that if a SWI vector was not installed, then an Abort
						   vector was probably not installed either, and so
						   normally this instruction would be ignored, even if an
						   Abort is generated.  This is a bad thing, since GDB
						   uses this instruction for its breakpoints (at least in
						   Thumb mode it does).  So intercept the instruction here
						   and generate a breakpoint SWI instead.  */
					    assert(0);
						/* Force the next instruction to be refetched.  */
						FLUSHPIPE;
						break;
					}
				}
				if (DESTReg == 15) {
					/* MSR reg to CPSR.  */
					UNDEF_MSRPC;
					temp = DPRegRHS;
					/* Don't allow TBIT to be set by MSR.  */
					temp &= ~TBIT;
					ARMul_FixCPSR(instr, temp);
				}
				else
					UNDEF_Test;

				break;

			case 0x13:	/* TEQP reg */
				if ((BITS (4, 11) & 0xF9) == 0x9)
					/* LDR register offset, write-back, down, pre indexed.  */
					LHPREDOWNWB ();
				/* Continue with remaining instruction decode.  */
				if (DESTReg == 15) {
					/* TEQP reg */
					m_Cpsr = GETSPSR (m_Bank);
					ARMul_CPSRAltered();
				}
				else {
					/* TEQ Reg.  */
					rhs = DPSRegRHS;
					dest = LHS ^ rhs;
					ARMul_NegZero(dest);
				}
				break;

			case 0x14:	/* CMP reg and MRS SPSR and SWP byte.  */
				if (m_IsV5e) {
					if (BIT (4) == 0 && BIT (7) == 1) {
						/* ElSegundo SMLALxy insn.  */
						unsigned long long op1 =
							m_Reg[BITS (0, 3)];
						unsigned long long op2 =
							m_Reg[BITS (8, 11)];
						unsigned long long dest;

						if (BIT (5))
							op1 >>= 16;
						if (BIT (6))
							op2 >>= 16;
						op1 &= 0xFFFF;
						if (op1 & 0x8000)
							op1 -= 65536;
						op2 &= 0xFFFF;
						if (op2 & 0x8000)
							op2 -= 65536;

						dest = (unsigned long long)
							m_Reg[BITS (16, 19)] <<
							32;
						dest |= m_Reg[BITS (12, 15)];
						dest += op1 * op2;
						m_Reg[BITS (12, 15)] =
							dest;
						m_Reg[BITS (16, 19)] =
							dest >> 32;
						break;
					}

					if (BITS (4, 11) == 5) {
						/* ElSegundo QDADD insn.  */
						uint32_t op1 =
							m_Reg[BITS (0, 3)];
						uint32_t op2 =
							m_Reg[BITS (16, 19)];
						uint32_t op2d = op2 + op2;
						uint32_t result;

						if (AddOverflow(op2, op2, op2d)) {
							SETS;
							op2d = POS (op2d) ?
								0x80000000 :
								0x7fffffff;
						}

						result = op1 + op2d;
						if (AddOverflow(op1, op2d, result)) {
							SETS;
							result = POS (result)
								? 0x80000000 :
								0x7fffffff;
						}

						m_Reg[BITS (12, 15)] =
							result;
						break;
					}
				}
				if (BITS (4, 7) == 0xB) {
					/* STRH immediate offset, no write-back, down, pre indexed.  */
					SHPREDOWN ();
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				if (BITS (4, 11) == 9) {
					/* SWP */
					UNDEF_SWPPC;
					temp = LHS;
					BUSUSEDINCPCS;
					DEST = ARMul_SwapByte(temp,
							       m_Reg
							       [RHSReg]);
					if (m_AbortSig || m_Aborted)
						TAKEABORT;
				}
				else if ((BITS (0, 11) == 0)
					 && (LHSReg == 15)) {
					/* MRS SPSR */
					UNDEF_MRSPC;
					DEST = GETSPSR (m_Bank);
				}
				else
					UNDEF_Test;

				break;

			case 0x15:	/* CMPP reg.  */
				if ((BITS (4, 7) & 0x9) == 0x9)
					/* LDR immediate offset, no write-back, down, pre indexed.  */
					LHPREDOWN ();
				/* Continue with remaining instruction decode.  */
				if (DESTReg == 15) {
					/* CMPP reg.  */
					m_Cpsr = GETSPSR (m_Bank);
					ARMul_CPSRAltered();
				}
				else {
					/* CMP reg.  */
					lhs = LHS;
					rhs = DPRegRHS;
					dest = lhs - rhs;
					ARMul_NegZero(dest);
					if ((lhs >= rhs)
					    || ((rhs | lhs) >> 31)) {
						ARMul_SubCarry(lhs, rhs, dest);
						ARMul_SubOverflow(lhs, rhs, dest);
					}
					else {
						CLEARC;
						CLEARV;
					}
				}
				break;

			case 0x16:	/* CMN reg and MSR reg to SPSR */
				if (m_IsV5e)
				{
					if (BIT (4) == 0 && BIT (7) == 1 && BITS (12, 15) == 0)
					{
						/* ElSegundo SMULxy insn.  */
						uint32_t op1 = m_Reg[BITS (0, 3)];
						uint32_t op2 = m_Reg[BITS (8, 11)];
						uint32_t Rn = m_Reg[BITS (12, 15)];

						if (BIT (5))
							op1 >>= 16;
						if (BIT (6))
							op2 >>= 16;
						op1 &= 0xFFFF;
						op2 &= 0xFFFF;
						if (op1 & 0x8000)
							op1 -= 65536;
						if (op2 & 0x8000)
							op2 -= 65536;

						m_Reg[BITS (16, 19)] = op1 * op2;
						break;
					}

					if (BITS (4, 11) == 5)
					{
						/* ElSegundo QDSUB insn.  */
						uint32_t op1 = m_Reg[BITS (0, 3)];
						uint32_t op2 = m_Reg[BITS (16, 19)];
						uint32_t op2d = op2 + op2;
						uint32_t result;

						if (AddOverflow(op2, op2, op2d))
						{
							SETS;
							op2d = POS (op2d) ? 0x80000000 : 0x7fffffff;
						}

						result = op1 - op2d;
						if (SubOverflow(op1, op2d, result))
						{
							SETS;
							result = POS (result) ? 0x80000000 : 0x7fffffff;
						}

						m_Reg[BITS (12, 15)] = result;
						break;
					}
				}

				if (m_IsV5)
				{
					if (BITS (4, 11) == 0xF1 && BITS (16, 19) == 0xF)
					{
						/* ARM5 CLZ insn.  */
						uint32_t op1 = m_Reg[BITS (0, 3)];
						int result = 32;

						if (op1)
							for (result = 0; (op1 & 0x80000000) == 0; op1 <<= 1)
								result++;

						m_Reg[BITS (12, 15)] = result;
						break;
					}
				}
				if (BITS (4, 7) == 0xB) {
					/* STRH immediate offset, write-back, down, pre indexed.  */
					SHPREDOWNWB ();
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				if (DESTReg == 15) {
					/* MSR */
					UNDEF_MSRPC;
					ARMul_FixSPSR(instr, DPRegRHS);
				}
				else {
					UNDEF_Test;
				}
				break;

			case 0x17:	/* CMNP reg */
				if ((BITS (4, 7) & 0x9) == 0x9)
					/* LDR immediate offset, write-back, down, pre indexed.  */
					LHPREDOWNWB ();
				/* Continue with remaining instruction decoding.  */
				if (DESTReg == 15) {
					m_Cpsr = GETSPSR (m_Bank);
					ARMul_CPSRAltered();
					break;
				}
				else {
					/* CMN reg.  */
					lhs = LHS;
					rhs = DPRegRHS;
					dest = lhs + rhs;
					ASSIGNZ (dest == 0);
					if ((lhs | rhs) >> 30) {
						/* Possible C,V,N to set.  */
						ASSIGNN (NEG (dest));
						ARMul_AddCarry(lhs, rhs, dest);
						ARMul_AddOverflow(lhs, rhs, dest);
					}
					else {
						CLEARN;
						CLEARC;
						CLEARV;
					}
				}
				break;

			case 0x18:	/* ORR reg */
				if (BITS (4, 11) == 0xB) {
					/* STRH register offset, no write-back, up, pre indexed.  */
					SHPREUP ();
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				rhs = DPRegRHS;
				dest = LHS | rhs;
				WRITEDEST (dest);
				break;

			case 0x19:	/* ORRS reg */
				if ((BITS (4, 11) & 0xF9) == 0x9)
					/* LDR register offset, no write-back, up, pre indexed.  */
					LHPREUP ();
				/* Continue with remaining instruction decoding.  */
				rhs = DPSRegRHS;
				dest = LHS | rhs;
				WRITESDEST (dest);
				break;

			case 0x1a:	/* MOV reg */
				if (BITS (4, 11) == 0xB) {
					/* STRH register offset, write-back, up, pre indexed.  */
					SHPREUPWB ();
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				dest = DPRegRHS;
				WRITEDEST (dest);
				break;

			case 0x1b:	/* MOVS reg */
				if ((BITS (4, 11) & 0xF9) == 0x9)
					/* LDR register offset, write-back, up, pre indexed.  */
					LHPREUPWB ();
				/* Continue with remaining instruction decoding.  */
				dest = DPSRegRHS;
				WRITESDEST (dest);
				break;

			case 0x1c:	/* BIC reg */
				if (BITS (4, 7) == 0xB) {
					/* STRH immediate offset, no write-back, up, pre indexed.  */
					SHPREUP ();
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				else if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				rhs = DPRegRHS;
				dest = LHS & ~rhs;
				WRITEDEST (dest);
				break;

			case 0x1d:	/* BICS reg */
				if ((BITS (4, 7) & 0x9) == 0x9)
					/* LDR immediate offset, no write-back, up, pre indexed.  */
					LHPREUP ();
				/* Continue with instruction decoding.  */
				rhs = DPSRegRHS;
				dest = LHS & ~rhs;
				WRITESDEST (dest);
				break;

			case 0x1e:	/* MVN reg */
				if (BITS (4, 7) == 0xB) {
					/* STRH immediate offset, write-back, up, pre indexed.  */
					SHPREUPWB ();
					break;
				}
				if (BITS (4, 7) == 0xD) {
					Handle_Load_Double(instr);
					break;
				}
				if (BITS (4, 7) == 0xF) {
					Handle_Store_Double(instr);
					break;
				}
				dest = ~DPRegRHS;
				WRITEDEST (dest);
				break;

			case 0x1f:	/* MVNS reg */
				if ((BITS (4, 7) & 0x9) == 0x9)
					/* LDR immediate offset, write-back, up, pre indexed.  */
					LHPREUPWB ();
				/* Continue instruction decoding.  */
				dest = ~DPSRegRHS;
				WRITESDEST (dest);
				break;


				/* Data Processing Immediate RHS Instructions.  */

			case 0x20:	/* AND immed */
				dest = LHS & DPImmRHS;
				WRITEDEST (dest);
				break;

			case 0x21:	/* ANDS immed */
				DPSImmRHS;
				dest = LHS & rhs;
				WRITESDEST (dest);
				break;

			case 0x22:	/* EOR immed */
				dest = LHS ^ DPImmRHS;
				WRITEDEST (dest);
				break;

			case 0x23:	/* EORS immed */
				DPSImmRHS;
				dest = LHS ^ rhs;
				WRITESDEST (dest);
				break;

			case 0x24:	/* SUB immed */
				dest = LHS - DPImmRHS;
				WRITEDEST (dest);
				break;

			case 0x25:	/* SUBS immed */
				lhs = LHS;
				rhs = DPImmRHS;
				dest = lhs - rhs;

				if ((lhs >= rhs) || ((rhs | lhs) >> 31)) {
					ARMul_SubCarry(lhs, rhs, dest);
					ARMul_SubOverflow(lhs, rhs, dest);
				}
				else {
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x26:	/* RSB immed */
				dest = DPImmRHS - LHS;
				WRITEDEST (dest);
				break;

			case 0x27:	/* RSBS immed */
				lhs = LHS;
				rhs = DPImmRHS;
				dest = rhs - lhs;

				if ((rhs >= lhs) || ((rhs | lhs) >> 31)) {
					ARMul_SubCarry(rhs, lhs, dest);
					ARMul_SubOverflow(rhs, lhs, dest);
				}
				else {
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x28:	/* ADD immed */
				dest = LHS + DPImmRHS;
				WRITEDEST (dest);
				break;

			case 0x29:	/* ADDS immed */
				lhs = LHS;
				rhs = DPImmRHS;
				dest = lhs + rhs;
				ASSIGNZ (dest == 0);

				if ((lhs | rhs) >> 30) {
					/* Possible C,V,N to set.  */
					ASSIGNN (NEG (dest));
					ARMul_AddCarry(lhs, rhs, dest);
					ARMul_AddOverflow(lhs, rhs, dest);
				}
				else {
					CLEARN;
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x2a:	/* ADC immed */
				dest = LHS + DPImmRHS + CFLAG;
				WRITEDEST (dest);
				break;

			case 0x2b:	/* ADCS immed */
				lhs = LHS;
				rhs = DPImmRHS;
				dest = lhs + rhs + CFLAG;
				ASSIGNZ (dest == 0);
				if ((lhs | rhs) >> 30) {
					/* Possible C,V,N to set.  */
					ASSIGNN (NEG (dest));
					ARMul_AddCarry(lhs, rhs, dest);
					ARMul_AddOverflow(lhs, rhs, dest);
				}
				else {
					CLEARN;
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x2c:	/* SBC immed */
				dest = LHS - DPImmRHS - !CFLAG;
				WRITEDEST (dest);
				break;

			case 0x2d:	/* SBCS immed */
				lhs = LHS;
				rhs = DPImmRHS;
				dest = lhs - rhs - !CFLAG;
				if ((lhs >= rhs) || ((rhs | lhs) >> 31)) {
					ARMul_SubCarry(lhs, rhs, dest);
					ARMul_SubOverflow(lhs, rhs, dest);
				}
				else {
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x2e:	/* RSC immed */
				dest = DPImmRHS - LHS - !CFLAG;
				WRITEDEST (dest);
				break;

			case 0x2f:	/* RSCS immed */
				lhs = LHS;
				rhs = DPImmRHS;
				dest = rhs - lhs - !CFLAG;
				if ((rhs >= lhs) || ((rhs | lhs) >> 31)) {
					ARMul_SubCarry(rhs, lhs, dest);
					ARMul_SubOverflow(rhs, lhs, dest);
				}
				else {
					CLEARC;
					CLEARV;
				}
				WRITESDEST (dest);
				break;

			case 0x30:	/* TST immed */
				UNDEF_Test;
				break;

			case 0x31:	/* TSTP immed */
				if (DESTReg == 15) {
					/* TSTP immed.  */
					m_Cpsr = GETSPSR (m_Bank);
					ARMul_CPSRAltered();
				}
				else {
					/* TST immed.  */
					DPSImmRHS;
					dest = LHS & rhs;
					ARMul_NegZero(dest);
				}
				break;

			case 0x32:	/* TEQ immed and MSR immed to CPSR */
				if (DESTReg == 15)
					/* MSR immed to CPSR.  */
					ARMul_FixCPSR(instr, DPImmRHS);
				else
					UNDEF_Test;
				break;

			case 0x33:	/* TEQP immed */
				if (DESTReg == 15) {
					/* TEQP immed.  */
					m_Cpsr = GETSPSR (m_Bank);
					ARMul_CPSRAltered();
				}
				else {
					DPSImmRHS;	/* TEQ immed */
					dest = LHS ^ rhs;
					ARMul_NegZero(dest);
				}
				break;

			case 0x34:	/* CMP immed */
				UNDEF_Test;
				break;

			case 0x35:	/* CMPP immed */
				if (DESTReg == 15) {
					/* CMPP immed.  */
					m_Cpsr = GETSPSR (m_Bank);
					ARMul_CPSRAltered();
					break;
				}
				else {
					/* CMP immed.  */
					lhs = LHS;
					rhs = DPImmRHS;
					dest = lhs - rhs;
					ARMul_NegZero(dest);

					if ((lhs >= rhs)
					    || ((rhs | lhs) >> 31)) {
						ARMul_SubCarry(lhs, rhs, dest);
						ARMul_SubOverflow(lhs, rhs, dest);
					}
					else {
						CLEARC;
						CLEARV;
					}
				}
				break;

			case 0x36:	/* CMN immed and MSR immed to SPSR */
				if (DESTReg == 15)
					ARMul_FixSPSR(instr, DPImmRHS);
				else
					UNDEF_Test;
				break;

			case 0x37:	/* CMNP immed.  */
				if (DESTReg == 15) {
					/* CMNP immed.  */
					m_Cpsr = GETSPSR (m_Bank);
					ARMul_CPSRAltered();
					break;
				}
				else {
					/* CMN immed.  */
					lhs = LHS;
					rhs = DPImmRHS;
					dest = lhs + rhs;
					ASSIGNZ (dest == 0);
					if ((lhs | rhs) >> 30) {
						/* Possible C,V,N to set.  */
						ASSIGNN (NEG (dest));
						ARMul_AddCarry(lhs, rhs, dest);
						ARMul_AddOverflow(lhs, rhs, dest);
					}
					else {
						CLEARN;
						CLEARC;
						CLEARV;
					}
				}
				break;

			case 0x38:	/* ORR immed.  */
				dest = LHS | DPImmRHS;
				WRITEDEST (dest);
				break;

			case 0x39:	/* ORRS immed.  */
				DPSImmRHS;
				dest = LHS | rhs;
				WRITESDEST (dest);
				break;

			case 0x3a:	/* MOV immed.  */
				dest = DPImmRHS;
				WRITEDEST (dest);
				break;

			case 0x3b:	/* MOVS immed.  */
				DPSImmRHS;
				WRITESDEST (rhs);
				break;

			case 0x3c:	/* BIC immed.  */
				dest = LHS & ~DPImmRHS;
				WRITEDEST (dest);
				break;

			case 0x3d:	/* BICS immed.  */
				DPSImmRHS;
				dest = LHS & ~rhs;
				WRITESDEST (dest);
				break;

			case 0x3e:	/* MVN immed.  */
				dest = ~DPImmRHS;
				WRITEDEST (dest);
				break;

			case 0x3f:	/* MVNS immed.  */
				DPSImmRHS;
				WRITESDEST (~rhs);
				break;


				/* Single Data Transfer Immediate RHS Instructions.  */

			case 0x40:	/* Store Word, No WriteBack, Post Dec, Immed.  */
				lhs = LHS;
				if (StoreWord(instr, lhs))
					LSBase = lhs - LSImmRHS;
				break;

			case 0x41:	/* Load Word, No WriteBack, Post Dec, Immed.  */
				lhs = LHS;
				if (LoadWord(instr, lhs))
					LSBase = lhs - LSImmRHS;
				break;

			case 0x42:	/* Store Word, WriteBack, Post Dec, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				lhs = LHS;
				temp = lhs - LSImmRHS;
				if (StoreWord(instr, lhs))
					LSBase = temp;
				break;

			case 0x43:	/* Load Word, WriteBack, Post Dec, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				lhs = LHS;
				if (LoadWord(instr, lhs))
					LSBase = lhs - LSImmRHS;
				break;

			case 0x44:	/* Store Byte, No WriteBack, Post Dec, Immed.  */
				lhs = LHS;
				if (StoreByte(instr, lhs))
					LSBase = lhs - LSImmRHS;
				break;

			case 0x45:	/* Load Byte, No WriteBack, Post Dec, Immed.  */
				lhs = LHS;
				if (LoadByte(instr, lhs, LUNSIGNED))
					LSBase = lhs - LSImmRHS;
				break;

			case 0x46:	/* Store Byte, WriteBack, Post Dec, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				lhs = LHS;
				if (StoreByte(instr, lhs))
					LSBase = lhs - LSImmRHS;
				break;

			case 0x47:	/* Load Byte, WriteBack, Post Dec, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				lhs = LHS;
				if (LoadByte(instr, lhs, LUNSIGNED))
					LSBase = lhs - LSImmRHS;
				break;

			case 0x48:	/* Store Word, No WriteBack, Post Inc, Immed.  */
				lhs = LHS;
				if (StoreWord(instr, lhs))
					LSBase = lhs + LSImmRHS;
				break;

			case 0x49:	/* Load Word, No WriteBack, Post Inc, Immed.  */
				lhs = LHS;
				if (LoadWord(instr, lhs))
					LSBase = lhs + LSImmRHS;
				break;

			case 0x4a:	/* Store Word, WriteBack, Post Inc, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				lhs = LHS;
				if (StoreWord(instr, lhs))
					LSBase = lhs + LSImmRHS;
				break;

			case 0x4b:	/* Load Word, WriteBack, Post Inc, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				lhs = LHS;
				if (LoadWord(instr, lhs))
					LSBase = lhs + LSImmRHS;
				break;

			case 0x4c:	/* Store Byte, No WriteBack, Post Inc, Immed.  */
				lhs = LHS;
				if (StoreByte(instr, lhs))
					LSBase = lhs + LSImmRHS;
				break;

			case 0x4d:	/* Load Byte, No WriteBack, Post Inc, Immed.  */
				lhs = LHS;
				if (LoadByte(instr, lhs, LUNSIGNED))
					LSBase = lhs + LSImmRHS;
				break;

			case 0x4e:	/* Store Byte, WriteBack, Post Inc, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				lhs = LHS;
				if (StoreByte(instr, lhs))
					LSBase = lhs + LSImmRHS;
				break;

			case 0x4f:	/* Load Byte, WriteBack, Post Inc, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				lhs = LHS;
				if (LoadByte(instr, lhs, LUNSIGNED))
					LSBase = lhs + LSImmRHS;
				break;


			case 0x50:	/* Store Word, No WriteBack, Pre Dec, Immed.  */
				(void) StoreWord(instr, LHS - LSImmRHS);
				break;

			case 0x51:	/* Load Word, No WriteBack, Pre Dec, Immed.  */
				(void)LoadWord(instr, LHS - LSImmRHS);
				break;

			case 0x52:	/* Store Word, WriteBack, Pre Dec, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				temp = LHS - LSImmRHS;
				if (StoreWord(instr, temp))
					LSBase = temp;
				break;

			case 0x53:	/* Load Word, WriteBack, Pre Dec, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				temp = LHS - LSImmRHS;
				if (LoadWord(instr, temp))
					LSBase = temp;
				break;

			case 0x54:	/* Store Byte, No WriteBack, Pre Dec, Immed.  */
				(void) StoreByte(instr, LHS - LSImmRHS);
				break;

			case 0x55:	/* Load Byte, No WriteBack, Pre Dec, Immed.  */
				(void) LoadByte(instr, LHS - LSImmRHS, LUNSIGNED);
				break;

			case 0x56:	/* Store Byte, WriteBack, Pre Dec, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				temp = LHS - LSImmRHS;
				if (StoreByte(instr, temp))
					LSBase = temp;
				break;

			case 0x57:	/* Load Byte, WriteBack, Pre Dec, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				temp = LHS - LSImmRHS;
				if (LoadByte(instr, temp, LUNSIGNED))
					LSBase = temp;
				break;

			case 0x58:	/* Store Word, No WriteBack, Pre Inc, Immed.  */
				(void) StoreWord(instr, LHS + LSImmRHS);
				break;

			case 0x59:	/* Load Word, No WriteBack, Pre Inc, Immed.  */
				(void) LoadWord(instr, LHS + LSImmRHS);
				break;

			case 0x5a:	/* Store Word, WriteBack, Pre Inc, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				temp = LHS + LSImmRHS;
				if (StoreWord(instr, temp))
					LSBase = temp;
				break;

			case 0x5b:	/* Load Word, WriteBack, Pre Inc, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				temp = LHS + LSImmRHS;
				if (LoadWord(instr, temp))
					LSBase = temp;
				break;

			case 0x5c:	/* Store Byte, No WriteBack, Pre Inc, Immed.  */
				(void) StoreByte(instr, LHS + LSImmRHS);
				break;

			case 0x5d:	/* Load Byte, No WriteBack, Pre Inc, Immed.  */
				(void) LoadByte(instr, LHS + LSImmRHS, LUNSIGNED);
				break;

			case 0x5e:	/* Store Byte, WriteBack, Pre Inc, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				temp = LHS + LSImmRHS;
				if (StoreByte(instr, temp))
					LSBase = temp;
				break;

			case 0x5f:	/* Load Byte, WriteBack, Pre Inc, Immed.  */
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				temp = LHS + LSImmRHS;
				if (LoadByte(instr, temp, LUNSIGNED))
					LSBase = temp;
				break;


				/* Single Data Transfer Register RHS Instructions.  */

			case 0x60:	/* Store Word, No WriteBack, Post Dec, Reg.  */
				if (BIT (4)) {
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				if (StoreWord(instr, lhs))
					LSBase = lhs - LSRegRHS;
				break;

			case 0x61:	/* Load Word, No WriteBack, Post Dec, Reg.  */
				if (BIT (4))
				{
				    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				temp = lhs - LSRegRHS;
				if (LoadWord(instr, lhs))
					LSBase = temp;
				break;

			case 0x62:	/* Store Word, WriteBack, Post Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				if (StoreWord(instr, lhs))
					LSBase = lhs - LSRegRHS;
				break;

			case 0x63:	/* Load Word, WriteBack, Post Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				temp = lhs - LSRegRHS;
				if (LoadWord(instr, lhs))
					LSBase = temp;
				break;

			case 0x64:	/* Store Byte, No WriteBack, Post Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				if (StoreByte(instr, lhs))
					LSBase = lhs - LSRegRHS;
				break;

			case 0x65:	/* Load Byte, No WriteBack, Post Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				temp = lhs - LSRegRHS;
				if (LoadByte(instr, lhs, LUNSIGNED))
					LSBase = temp;
				break;

			case 0x66:	/* Store Byte, WriteBack, Post Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				if (StoreByte(instr, lhs))
					LSBase = lhs - LSRegRHS;
				break;

			case 0x67:	/* Load Byte, WriteBack, Post Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				temp = lhs - LSRegRHS;
				if (LoadByte(instr, lhs, LUNSIGNED))
					LSBase = temp;
				break;

			case 0x68:	/* Store Word, No WriteBack, Post Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				if (StoreWord(instr, lhs))
					LSBase = lhs + LSRegRHS;
				break;

			case 0x69:	/* Load Word, No WriteBack, Post Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				temp = lhs + LSRegRHS;
				if (LoadWord(instr, lhs))
					LSBase = temp;
				break;

			case 0x6a:	/* Store Word, WriteBack, Post Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				if (StoreWord(instr, lhs))
					LSBase = lhs + LSRegRHS;
				break;

			case 0x6b:	/* Load Word, WriteBack, Post Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				temp = lhs + LSRegRHS;
				if (LoadWord(instr, lhs))
					LSBase = temp;
				break;

			case 0x6c:	/* Store Byte, No WriteBack, Post Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				if (StoreByte(instr, lhs))
					LSBase = lhs + LSRegRHS;
				break;

			case 0x6d:	/* Load Byte, No WriteBack, Post Inc, Reg.  */
				if (BIT (4)) {
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				temp = lhs + LSRegRHS;
				if (LoadByte(instr, lhs, LUNSIGNED))
					LSBase = temp;
				break;

			case 0x6e:	/* Store Byte, WriteBack, Post Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				if (StoreByte(instr, lhs))
					LSBase = lhs + LSRegRHS;
				break;

			case 0x6f:	/* Load Byte, WriteBack, Post Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				lhs = LHS;
				temp = lhs + LSRegRHS;
				if (LoadByte(instr, lhs, LUNSIGNED))
					LSBase = temp;
				break;


			case 0x70:	/* Store Word, No WriteBack, Pre Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				(void) StoreWord(instr, LHS - LSRegRHS);
				break;

			case 0x71:	/* Load Word, No WriteBack, Pre Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				(void) LoadWord(instr, LHS - LSRegRHS);
				break;

			case 0x72:	/* Store Word, WriteBack, Pre Dec, Reg.  */
				if (BIT (4)) {
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				temp = LHS - LSRegRHS;
				if (StoreWord(instr, temp))
					LSBase = temp;
				break;

			case 0x73:	/* Load Word, WriteBack, Pre Dec, Reg.  */
				if (BIT (4)) {
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				temp = LHS - LSRegRHS;
				if (LoadWord(instr, temp))
					LSBase = temp;
				break;

			case 0x74:	/* Store Byte, No WriteBack, Pre Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				(void) StoreByte(instr, LHS - LSRegRHS);
				break;

			case 0x75:	/* Load Byte, No WriteBack, Pre Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				(void) LoadByte(instr, LHS - LSRegRHS, LUNSIGNED);
				break;

			case 0x76:	/* Store Byte, WriteBack, Pre Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				temp = LHS - LSRegRHS;
				if (StoreByte(instr, temp))
					LSBase = temp;
				break;

			case 0x77:	/* Load Byte, WriteBack, Pre Dec, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				temp = LHS - LSRegRHS;
				if (LoadByte(instr, temp, LUNSIGNED))
					LSBase = temp;
				break;

			case 0x78:	/* Store Word, No WriteBack, Pre Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				(void) StoreWord(instr, LHS + LSRegRHS);
				break;

			case 0x79:	/* Load Word, No WriteBack, Pre Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				(void) LoadWord(instr, LHS + LSRegRHS);
				break;

			case 0x7a:	/* Store Word, WriteBack, Pre Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				temp = LHS + LSRegRHS;
				if (StoreWord(instr, temp))
					LSBase = temp;
				break;

			case 0x7b:	/* Load Word, WriteBack, Pre Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				temp = LHS + LSRegRHS;
				if (LoadWord(instr, temp))
					LSBase = temp;
				break;

			case 0x7c:	/* Store Byte, No WriteBack, Pre Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				(void) StoreByte(instr, LHS + LSRegRHS);
				break;

			case 0x7d:	/* Load Byte, No WriteBack, Pre Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				(void) LoadByte(instr, LHS + LSRegRHS, LUNSIGNED);
				break;

			case 0x7e:	/* Store Byte, WriteBack, Pre Inc, Reg.  */
				if (BIT (4))
				{
                    // currently not handled ARM v6 instruction
					ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				temp = LHS + LSRegRHS;
				if (StoreByte(instr, temp))
					LSBase = temp;
				break;

			case 0x7f:	/* Load Byte, WriteBack, Pre Inc, Reg.  */
				if (BIT (4))
				{
					/* Check for the special breakpoint opcode.
					   This value should correspond to the value defined
					   as ARM_BE_BREAKPOINT in gdb/arm/tm-arm.h.  */
					if (BITS (0, 19) == 0xfdefe)
					{
						ARMul_Abort(ARMul_SWIV);
					}
					else
						ARMul_UndefInstr();
					break;
				}
				UNDEF_LSRBaseEQOffWb;
				UNDEF_LSRBaseEQDestWb;
				UNDEF_LSRPCBaseWb;
				UNDEF_LSRPCOffWb;
				temp = LHS + LSRegRHS;
				if (LoadByte(instr, temp, LUNSIGNED))
					LSBase = temp;
				break;


				/* Multiple Data Transfer Instructions.  */

			case 0x80:	/* Store, No WriteBack, Post Dec.  */
				STOREMULT (instr, LSBase - LSMNumRegs + 4L,
					   0L);
				break;

			case 0x81:	/* Load, No WriteBack, Post Dec.  */
				LOADMULT (instr, LSBase - LSMNumRegs + 4L,
					  0L);
				break;

			case 0x82:	/* Store, WriteBack, Post Dec.  */
				temp = LSBase - LSMNumRegs;
				STOREMULT (instr, temp + 4L, temp);
				break;

			case 0x83:	/* Load, WriteBack, Post Dec.  */
				temp = LSBase - LSMNumRegs;
				LOADMULT (instr, temp + 4L, temp);
				break;

			case 0x84:	/* Store, Flags, No WriteBack, Post Dec.  */
				STORESMULT (instr, LSBase - LSMNumRegs + 4L,
					    0L);
				break;

			case 0x85:	/* Load, Flags, No WriteBack, Post Dec.  */
				LOADSMULT (instr, LSBase - LSMNumRegs + 4L,
					   0L);
				break;

			case 0x86:	/* Store, Flags, WriteBack, Post Dec.  */
				temp = LSBase - LSMNumRegs;
				STORESMULT (instr, temp + 4L, temp);
				break;

			case 0x87:	/* Load, Flags, WriteBack, Post Dec.  */
				temp = LSBase - LSMNumRegs;
				LOADSMULT (instr, temp + 4L, temp);
				break;

			case 0x88:	/* Store, No WriteBack, Post Inc.  */
				STOREMULT (instr, LSBase, 0L);
				break;

			case 0x89:	/* Load, No WriteBack, Post Inc.  */
				LOADMULT (instr, LSBase, 0L);
				break;

			case 0x8a:	/* Store, WriteBack, Post Inc.  */
				temp = LSBase;
				STOREMULT (instr, temp, temp + LSMNumRegs);
				break;

			case 0x8b:	/* Load, WriteBack, Post Inc.  */
				temp = LSBase;
				LOADMULT (instr, temp, temp + LSMNumRegs);
				break;

			case 0x8c:	/* Store, Flags, No WriteBack, Post Inc.  */
				STORESMULT (instr, LSBase, 0L);
				break;

			case 0x8d:	/* Load, Flags, No WriteBack, Post Inc.  */
				LOADSMULT (instr, LSBase, 0L);
				break;

			case 0x8e:	/* Store, Flags, WriteBack, Post Inc.  */
				temp = LSBase;
				STORESMULT (instr, temp, temp + LSMNumRegs);
				break;

			case 0x8f:	/* Load, Flags, WriteBack, Post Inc.  */
				temp = LSBase;
				LOADSMULT (instr, temp, temp + LSMNumRegs);
				break;

			case 0x90:	/* Store, No WriteBack, Pre Dec.  */
				STOREMULT (instr, LSBase - LSMNumRegs, 0L);
				break;

			case 0x91:	/* Load, No WriteBack, Pre Dec.  */
				LOADMULT (instr, LSBase - LSMNumRegs, 0L);
				break;

			case 0x92:	/* Store, WriteBack, Pre Dec.  */
				temp = LSBase - LSMNumRegs;
				STOREMULT (instr, temp, temp);
				break;

			case 0x93:	/* Load, WriteBack, Pre Dec.  */
				temp = LSBase - LSMNumRegs;
				LOADMULT (instr, temp, temp);
				break;

			case 0x94:	/* Store, Flags, No WriteBack, Pre Dec.  */
				STORESMULT (instr, LSBase - LSMNumRegs, 0L);
				break;

			case 0x95:	/* Load, Flags, No WriteBack, Pre Dec.  */
				LOADSMULT (instr, LSBase - LSMNumRegs, 0L);
				break;

			case 0x96:	/* Store, Flags, WriteBack, Pre Dec.  */
				temp = LSBase - LSMNumRegs;
				STORESMULT (instr, temp, temp);
				break;

			case 0x97:	/* Load, Flags, WriteBack, Pre Dec.  */
				temp = LSBase - LSMNumRegs;
				LOADSMULT (instr, temp, temp);
				break;

			case 0x98:	/* Store, No WriteBack, Pre Inc.  */
				STOREMULT (instr, LSBase + 4L, 0L);
				break;

			case 0x99:	/* Load, No WriteBack, Pre Inc.  */
				LOADMULT (instr, LSBase + 4L, 0L);
				break;

			case 0x9a:	/* Store, WriteBack, Pre Inc.  */
				temp = LSBase;
				STOREMULT (instr, temp + 4L,
					   temp + LSMNumRegs);
				break;

			case 0x9b:	/* Load, WriteBack, Pre Inc.  */
				temp = LSBase;
				LOADMULT (instr, temp + 4L,
					  temp + LSMNumRegs);
				break;

			case 0x9c:	/* Store, Flags, No WriteBack, Pre Inc.  */
				STORESMULT (instr, LSBase + 4L, 0L);
				break;

			case 0x9d:	/* Load, Flags, No WriteBack, Pre Inc.  */
				LOADSMULT (instr, LSBase + 4L, 0L);
				break;

			case 0x9e:	/* Store, Flags, WriteBack, Pre Inc.  */
				temp = LSBase;
				STORESMULT (instr, temp + 4L,
					    temp + LSMNumRegs);
				break;

			case 0x9f:	/* Load, Flags, WriteBack, Pre Inc.  */
				temp = LSBase;
				LOADSMULT (instr, temp + 4L,
					   temp + LSMNumRegs);
				break;


				/* Branch forward.  */
			case 0xa0:
			case 0xa1:
			case 0xa2:
			case 0xa3:
			case 0xa4:
			case 0xa5:
			case 0xa6:
			case 0xa7:
				m_Reg[15] = m_PC + 8 + POSBRANCH;
				FLUSHPIPE;
				break;


				/* Branch backward.  */
			case 0xa8:
			case 0xa9:
			case 0xaa:
			case 0xab:
			case 0xac:
			case 0xad:
			case 0xae:
			case 0xaf:
				m_Reg[15] = m_PC + 8 + NEGBRANCH;
				FLUSHPIPE;
				break;


				/* Branch and Link forward.  */
			case 0xb0:
			case 0xb1:
			case 0xb2:
			case 0xb3:
			case 0xb4:
			case 0xb5:
			case 0xb6:
			case 0xb7:
				/* Put PC into Link.  */
				m_Reg[14] = m_PC + 4;
				m_Reg[15] = m_PC + 8 + POSBRANCH;
				FLUSHPIPE;
				break;


				/* Branch and Link backward.  */
			case 0xb8:
			case 0xb9:
			case 0xba:
			case 0xbb:
			case 0xbc:
			case 0xbd:
			case 0xbe:
			case 0xbf:
				/* Put PC into Link.  */
				m_Reg[14] = m_PC + 4;
				m_Reg[15] = m_PC + 8 + NEGBRANCH;
				FLUSHPIPE;
				break;


				/* Co-Processor Data Transfers.  */
			case 0xc4:
				if (m_IsV5) {
					/* Reading from R15 is UNPREDICTABLE.  */
					if (BITS (12, 15) == 15
					    || BITS (16, 19) == 15)
						ARMul_UndefInstr();
					/* Is access to coprocessor 0 allowed ?  */
					else if (!CP_ACCESS_ALLOWED
						 (state, CPNum))
						ARMul_UndefInstr();
					else
						/* FIXME: Not sure what to do for other v5 processors.  */
						ARMul_UndefInstr();
					break;
				}
				/* Drop through.  */

			case 0xc0:	/* Store , No WriteBack , Post Dec.  */
				ARMul_STC(instr, LHS);
				break;

			case 0xc5:
				if (m_IsV5) {
					/* Writes to R15 are UNPREDICATABLE.  */
					if (DESTReg == 15 || LHSReg == 15)
						ARMul_UndefInstr();
					/* Is access to the coprocessor allowed ?  */
					else if (!CP_ACCESS_ALLOWED(state, CPNum))
						ARMul_UndefInstr();
					else
						/* FIXME: Not sure what to do for other v5 processors.  */
						ARMul_UndefInstr();
					break;
				}
				/* Drop through.  */

			case 0xc1:	/* Load , No WriteBack , Post Dec.  */
				ARMul_LDC(instr, LHS);
				break;

			case 0xc2:
			case 0xc6:	/* Store , WriteBack , Post Dec.  */
				lhs = LHS;
				m_Base = lhs - LSCOff;
				ARMul_STC(instr, lhs);
				break;

			case 0xc3:
			case 0xc7:	/* Load , WriteBack , Post Dec.  */
				lhs = LHS;
				m_Base = lhs - LSCOff;
				ARMul_LDC(instr, lhs);
				break;

			case 0xc8:
			case 0xcc:	/* Store , No WriteBack , Post Inc.  */
				ARMul_STC(instr, LHS);
				break;

			case 0xc9:
			case 0xcd:	/* Load , No WriteBack , Post Inc.  */
				ARMul_LDC(instr, LHS);
				break;

			case 0xca:
			case 0xce:	/* Store , WriteBack , Post Inc.  */
				lhs = LHS;
				m_Base = lhs + LSCOff;
				ARMul_STC(instr, LHS);
				break;

			case 0xcb:
			case 0xcf:	/* Load , WriteBack , Post Inc.  */
				lhs = LHS;
				m_Base = lhs + LSCOff;
				ARMul_LDC(instr, LHS);
				break;

			case 0xd0:
			case 0xd4:	/* Store , No WriteBack , Pre Dec.  */
				ARMul_STC(instr, LHS - LSCOff);
				break;

			case 0xd1:
			case 0xd5:	/* Load , No WriteBack , Pre Dec.  */
				ARMul_LDC(instr, LHS - LSCOff);
				break;

			case 0xd2:
			case 0xd6:	/* Store , WriteBack , Pre Dec.  */
				lhs = LHS - LSCOff;
				m_Base = lhs;
				ARMul_STC(instr, lhs);
				break;

			case 0xd3:
			case 0xd7:	/* Load , WriteBack , Pre Dec.  */
				lhs = LHS - LSCOff;
				m_Base = lhs;
				ARMul_LDC(instr, lhs);
				break;

			case 0xd8:
			case 0xdc:	/* Store , No WriteBack , Pre Inc.  */
				ARMul_STC(instr, LHS + LSCOff);
				break;

			case 0xd9:
			case 0xdd:	/* Load , No WriteBack , Pre Inc.  */
				ARMul_LDC(instr, LHS + LSCOff);
				break;

			case 0xda:
			case 0xde:	/* Store , WriteBack , Pre Inc.  */
				lhs = LHS + LSCOff;
				m_Base = lhs;
				ARMul_STC(instr, lhs);
				break;

			case 0xdb:
			case 0xdf:	/* Load , WriteBack , Pre Inc.  */
				lhs = LHS + LSCOff;
				m_Base = lhs;
				ARMul_LDC(instr, lhs);
				break;


				/* Co-Processor Register Transfers (MCR) and Data Ops.  */

			case 0xe2:
				if (!CP_ACCESS_ALLOWED (state, CPNum)) {
					ARMul_UndefInstr();
					break;
				}

			case 0xe0:
			case 0xe4:
			case 0xe6:
			case 0xe8:
			case 0xea:
			case 0xec:
			case 0xee:
				if (BIT (4)) {
					/* MCR.  */
					if (DESTReg == 15) {
						UNDEF_MCRPC;
						ARMul_MCR(instr, m_Reg[15] + isize);
					}
					else
						ARMul_MCR(instr, DEST);
				}
				else
					/* CDP Part 1.  */
					ARMul_CDP(instr);
				break;


				/* Co-Processor Register Transfers (MRC) and Data Ops.  */
			case 0xe1:
			case 0xe3:
			case 0xe5:
			case 0xe7:
			case 0xe9:
			case 0xeb:
			case 0xed:
			case 0xef:
				if (BIT (4)) {
					/* MRC */
					temp = ARMul_MRC(instr);
					if (DESTReg == 15) {
						ASSIGNN ((temp & NBIT) != 0);
						ASSIGNZ ((temp & ZBIT) != 0);
						ASSIGNC ((temp & CBIT) != 0);
						ASSIGNV ((temp & VBIT) != 0);
					}
					else
						DEST = temp;
				}
				else
					/* CDP Part 2.  */
					ARMul_CDP(instr);
				break;


				/* SWI instruction.  */
			case 0xf0:
			case 0xf1:
			case 0xf2:
			case 0xf3:
			case 0xf4:
			case 0xf5:
			case 0xf6:
			case 0xf7:
			case 0xf8:
			case 0xf9:
			case 0xfa:
			case 0xfb:
			case 0xfc:
			case 0xfd:
			case 0xfe:
			case 0xff:
				if (instr == ARMul_ABORTWORD)
				{
					ARMul_Abort(ARMul_PrefetchAbortV);
					break;
				}
				ARMul_Abort(ARMul_SWIV);
				break;
			}
		}
	}
	while (false);

donext:
    // indicate the number of internal cycles that have elapsed since last time here
    arm_exec_cycles(m_NumIcycles - m_PreviousIcycles);
    m_PreviousIcycles = m_NumIcycles;
    return;

}

uint32_t
arm::GetDPRegRHS(uint32_t instr)
{
	uint32_t shamt, base;

	base = RHSReg;
	if (BIT (4)) {
		/* Shift amount in a register.  */
		UNDEF_Shift;
		INCPC;
		base = m_Reg[base];
		ARMul_Icycles(1);
		shamt = m_Reg[BITS (8, 11)] & 0xff;
		switch ((int) BITS (5, 6)) {
		case LSL:
			if (shamt == 0)
				return (base);
			else if (shamt >= 32)
				return (0);
			else
				return (base << shamt);
		case LSR:
			if (shamt == 0)
				return (base);
			else if (shamt >= 32)
				return (0);
			else
				return (base >> shamt);
		case ASR:
			if (shamt == 0)
				return (base);
			else if (shamt >= 32)
				return ((uint32_t) ((int) base >> 31L));
			else
				return ((uint32_t)
					(( int) base >> (int) shamt));
		case ROR:
			shamt &= 0x1f;
			if (shamt == 0)
				return (base);
			else
				return ((base << (32 - shamt)) |
					(base >> shamt));
		}
	}
	else {
		/* Shift amount is a constant.  */
		base = m_Reg[base];
		shamt = BITS (7, 11);
		switch ((int) BITS (5, 6)) {
		case LSL:
			return (base << shamt);
		case LSR:
			if (shamt == 0)
				return (0);
			else
				return (base >> shamt);
		case ASR:
			if (shamt == 0)
				return ((uint32_t) (( int) base >> 31L));
			else
				return ((uint32_t)
					(( int) base >> (int) shamt));
		case ROR:
			if (shamt == 0)
				/* It's an RRX.  */
				return ((base >> 1) | (CFLAG << 31));
			else
				return ((base << (32 - shamt)) |
					(base >> shamt));
		}
	}

	return 0;
}

uint32_t
arm::GetDPSRegRHS(uint32_t instr)
{
	uint32_t shamt, base;

	base = RHSReg;
	if (BIT (4)) {
		/* Shift amount in a register.  */
		UNDEF_Shift;
		INCPC;
		base = m_Reg[base];
		ARMul_Icycles(1);
		shamt = m_Reg[BITS (8, 11)] & 0xff;
		switch ((int) BITS (5, 6)) {
		case LSL:
			if (shamt == 0)
				return (base);
			else if (shamt == 32) {
				ASSIGNC (base & 1);
				return (0);
			}
			else if (shamt > 32) {
				CLEARC;
				return (0);
			}
			else {
				ASSIGNC ((base >> (32 - shamt)) & 1);
				return (base << shamt);
			}
		case LSR:
			if (shamt == 0)
				return (base);
			else if (shamt == 32) {
				ASSIGNC (base >> 31);
				return (0);
			}
			else if (shamt > 32) {
				CLEARC;
				return (0);
			}
			else {
				ASSIGNC ((base >> (shamt - 1)) & 1);
				return (base >> shamt);
			}
		case ASR:
			if (shamt == 0)
				return (base);
			else if (shamt >= 32) {
				ASSIGNC (base >> 31L);
				return ((uint32_t) (( int) base >> 31L));
			}
			else {
				ASSIGNC ((uint32_t)
					 (( int) base >>
					  (int) (shamt - 1)) & 1);
				return ((uint32_t)
					((int) base >> (int) shamt));
			}
		case ROR:
			if (shamt == 0)
				return (base);
			shamt &= 0x1f;
			if (shamt == 0) {
				ASSIGNC (base >> 31);
				return (base);
			}
			else {
				ASSIGNC ((base >> (shamt - 1)) & 1);
				return ((base << (32 - shamt)) |
					(base >> shamt));
			}
		}
	}
	else {
		/* Shift amount is a constant.  */
		base = m_Reg[base];
		shamt = BITS (7, 11);

		switch ((int) BITS (5, 6)) {
		case LSL:
			ASSIGNC ((base >> (32 - shamt)) & 1);
			return (base << shamt);
		case LSR:
			if (shamt == 0) {
				ASSIGNC (base >> 31);
				return (0);
			}
			else {
				ASSIGNC ((base >> (shamt - 1)) & 1);
				return (base >> shamt);
			}
		case ASR:
			if (shamt == 0) {
				ASSIGNC (base >> 31L);
				return ((uint32_t) ((int) base >> 31L));
			}
			else {
				ASSIGNC ((uint32_t)
					 ((int) base >>
					  (int) (shamt - 1)) & 1);
				return ((uint32_t)
					(( int) base >> (int) shamt));
			}
		case ROR:
			if (shamt == 0) {
				/* It's an RRX.  */
				shamt = CFLAG;
				ASSIGNC (base & 1);
				return ((base >> 1) | (shamt << 31));
			}
			else {
				ASSIGNC ((base >> (shamt - 1)) & 1);
				return ((base << (32 - shamt)) |
					(base >> shamt));
			}
		}
	}

	return 0;
}

uint32_t
arm::GetLSRegRHS(uint32_t instr)
{
    uint32_t shamt, base;

    base = RHSReg;
    base = m_Reg[base];

    shamt = BITS (7, 11);
    switch ((int) BITS (5, 6)) {
    case LSL:
        return (base << shamt);
    case LSR:
        if (shamt == 0)
            return (0);
        else
            return (base >> shamt);
    case ASR:
        if (shamt == 0)
            return ((uint32_t) (( int) base >> 31L));
        else
            return ((uint32_t) (( int) base >> (int) shamt));
    case ROR:
        if (shamt == 0)
            /* It's an RRX.  */
            return ((base >> 1) | (CFLAG << 31));
        else
            return ((base << (32 - shamt)) | (base >> shamt));
    default:
        break;
    }
    return 0;
}


uint32_t
arm::GetLS7RHS(uint32_t instr)
{
    if (BIT (22) == 0) {
        /* Register.  */
        return m_Reg[RHSReg];
    }

    /* Immediate.  */
    return BITS (0, 3) | (BITS (8, 11) << 4);
}


void
arm::WriteR15(uint32_t src)
{
	/* The ARM documentation states that the two least significant bits
	   are discarded when setting PC, except in the cases handled by
	   WriteR15Branch() below.  It's probably an oversight: in THUMB
	   mode, the second least significant bit should probably not be
	   discarded.  */
	if (TFLAG)
		src &= 0xfffffffe;
	else
		src &= 0xfffffffc;

	m_Reg[15] = src & PCBITS;

	FLUSHPIPE;
}

void
arm::WriteSR15(uint32_t src)
{
    if (m_Bank > 0) {
        m_Cpsr = m_Spsr[m_Bank];
        ARMul_CPSRAltered();
    }
    if (TFLAG)
        src &= 0xfffffffe;
    else
        src &= 0xfffffffc;
    m_Reg[15] = src & PCBITS;
    FLUSHPIPE;
}

void
arm::WriteR15Branch(uint32_t src)
{
	if (src & 1) {
		/* Thumb bit.  */
		SETT;
		m_Reg[15] = src & 0xfffffffe;
	}
	else {
		CLEART;
		m_Reg[15] = src & 0xfffffffc;
	}
	m_Cpsr = ARMul_GetCPSR();
	FLUSHPIPE;
}

void
arm::LoadMult(uint32_t instr, uint32_t address, uint32_t WBBase)
{
    uint32_t dest, temp;

    UNDEF_LSMNoRegs;
    UNDEF_LSMPCBase;
    UNDEF_LSMBaseInListWb;
    BUSUSEDINCPCS;
/*chy 2004-05-23 may write twice
  if (BIT (21) && LHSReg != 15)
    LSBase = WBBase;
*/
    /* N cycle first.  */
    for (temp = 0; !BIT (temp); temp++);

    dest = ARMul_LoadWordN(address);

    if (!m_AbortSig && !m_Aborted)
        m_Reg[temp++] = dest;
    else if (!m_Aborted) {
        m_Aborted = ARMul_DataAbortV;
    }
/*chy 2004-05-23 chy goto end*/
    if (m_Aborted)
        goto L_ldm_makeabort;
    /* S cycles from here on.  */
    for (; temp < 16; temp++)
        if (BIT (temp)) {
            /* Load this register.  */
            address += 4;
            dest = ARMul_LoadWordS(address);

            if (!m_AbortSig && !m_Aborted)
                m_Reg[temp] = dest;
            else if (!m_Aborted) {
                m_Aborted = ARMul_DataAbortV;
            }
            /*chy 2004-05-23 chy goto end */
            if (m_Aborted)
                goto L_ldm_makeabort;

        }

    if (BIT (15) && !m_Aborted)
        /* PC is in the reg list.  */
        WriteR15Branch(PC);

    /* To write back the final register.  */
/*  ARMul_Icycles(1);*/
/*chy 2004-05-23, see below
  if (m_Aborted)
    {
      if (BIT (21) && LHSReg != 15)
        LSBase = WBBase;

      TAKEABORT;
    }
*/
/*chy 2004-05-23 should compare the Abort Models*/
      L_ldm_makeabort:
    /* To write back the final register.  */
    ARMul_Icycles(1);

    /* chy 2005-11-24, bug found by benjl@cse.unsw.edu.au, etc */
    /*
       if (m_Aborted)
       {
       if (BIT (21) && LHSReg != 15)
       if(!(m_AbortSig && m_Aborted && m_IsBaseUpdatedAbort==false))
       LSBase = WBBase;
       TAKEABORT;
       }else if (BIT (21) && LHSReg != 15)
       LSBase = WBBase;
     */
    if (m_Aborted) {
        if (BIT (21) && LHSReg != 15) {
            if (!(m_AbortSig))
            {
            }
        }
        TAKEABORT;
    }
    else if (BIT (21) && LHSReg != 15) {
        LSBase = WBBase;
    }
    /* chy 2005-11-24, over */

}

/* This function does the work of loading the registers listed in an LDM
   instruction, when the S bit is set. The code here is always increment
   after, it's up to the caller to get the input address correct and to
   handle base register modification.  */
void
arm::LoadSMult(uint32_t instr, uint32_t address, uint32_t WBBase)
{
    uint32_t dest, temp;

    UNDEF_LSMNoRegs;
    UNDEF_LSMPCBase;
    UNDEF_LSMBaseInListWb;

    BUSUSEDINCPCS;

/* chy 2004-05-23, may write twice
  if (BIT (21) && LHSReg != 15)
    LSBase = WBBase;
*/
    if (!BIT (15) && m_Bank != USERBANK) {
        /* Temporary reg bank switch.  */
        (void) ARMul_SwitchMode(m_Mode, USER26MODE);
        UNDEF_LSMUserBankWb;
    }

    /* N cycle first.  */
    for (temp = 0; !BIT (temp); temp++);

    dest = ARMul_LoadWordN(address);

    if (!m_AbortSig)
        m_Reg[temp++] = dest;
    else if (!m_Aborted) {
        m_Aborted = ARMul_DataAbortV;
    }

/*chy 2004-05-23 chy goto end*/
    if (m_Aborted)
        goto L_ldm_s_makeabort;
    /* S cycles from here on.  */
    for (; temp < 16; temp++)
        if (BIT (temp)) {
            /* Load this register.  */
            address += 4;
            dest = ARMul_LoadWordS(address);

            if (!m_AbortSig && !m_Aborted)
                m_Reg[temp] = dest;
            else if (!m_Aborted) {
                m_Aborted = ARMul_DataAbortV;
            }
            /*chy 2004-05-23 chy goto end */
            if (m_Aborted)
                goto L_ldm_s_makeabort;
        }

/*chy 2004-05-23 label of ldm_s_makeabort*/
      L_ldm_s_makeabort:
/*chy 2004-06-06 LSBase process should be here, not in the end of this function. Because ARMul_CPSRAltered maybe change R13(SP) R14(lr). If not, simulate INSTR  ldmia sp!,[....pc]^ error.*/
/*chy 2004-05-23 should compare the Abort Models*/
    if (m_Aborted) {
        if (BIT (21) && LHSReg != 15)
            if (!
                (m_AbortSig && m_Aborted
                 && m_IsBaseUpdatedAbort == false))
                LSBase = WBBase;
        TAKEABORT;
    }
    else if (BIT (21) && LHSReg != 15)
        LSBase = WBBase;

    if (BIT (15) && !m_Aborted) {
        /* PC is in the reg list.  */
            //chy 2006-02-16 , should not consider system mode, don't conside 26bit mode
            if (m_Mode != USER26MODE && m_Mode != USER32MODE ){
            m_Cpsr = GETSPSR (m_Bank);
            ARMul_CPSRAltered();
        }

        WriteR15(PC);
    }

            //chy 2006-02-16 , should not consider system mode, don't conside 26bit mode
    if (!BIT (15) && m_Mode != USER26MODE
        && m_Mode != USER32MODE )
        /* Restore the correct bank.  */
        (void) ARMul_SwitchMode(USER26MODE, m_Mode);

    /* To write back the final register.  */
    ARMul_Icycles(1);
/* chy 2004-05-23, see below
  if (m_Aborted)
    {
      if (BIT (21) && LHSReg != 15)
        LSBase = WBBase;

      TAKEABORT;
    }
*/
}

bool
arm::LoadWord(uint32_t instr, uint32_t address)
{
	uint32_t dest;

	BUSUSEDINCPCS;

	dest = ARMul_LoadWordN(address);

	if (m_Aborted) {
		TAKEABORT;
		return m_IsBaseUpdatedAbort;
	}
	if (address & 3)
		dest = ARMul_Align(address, dest);
	WRITEDESTB (dest);
	ARMul_Icycles(1);

	return (DESTReg != LHSReg);
}

bool
arm::LoadHalfWord(uint32_t instr, uint32_t address, bool signextend)
{
	uint32_t dest;

	BUSUSEDINCPCS;
	dest = ARMul_LoadHalfWord(address);
	if (m_Aborted) {
		TAKEABORT;
		return m_IsBaseUpdatedAbort;
	}
	UNDEF_LSRBPC;
	if (signextend)
		if (dest & 1 << (16 - 1))
			dest = (dest & ((1 << 16) - 1)) - (1 << 16);

	WRITEDEST (dest);
	ARMul_Icycles(1);

	return (DESTReg != LHSReg);
}

/* This function does the work of loading a byte for a LDRB instruction.  */

bool
arm::LoadByte(uint32_t instr, uint32_t address, bool signextend)
{
	uint32_t dest;

	BUSUSEDINCPCS;
	dest = ARMul_LoadByte(address);
	if (m_Aborted) {
		TAKEABORT;
		return m_IsBaseUpdatedAbort;
	}
	UNDEF_LSRBPC;
	if (signextend)
		if (dest & 1 << (8 - 1))
			dest = (dest & ((1 << 8) - 1)) - (1 << 8);

	WRITEDEST (dest);
	ARMul_Icycles(1);

	return (DESTReg != LHSReg);
}

void
arm::Handle_Load_Double(uint32_t instr)
{
	uint32_t dest_reg;
	uint32_t addr_reg;
	uint32_t write_back = BIT (21);
	uint32_t immediate = BIT (22);
	uint32_t add_to_base = BIT (23);
	uint32_t pre_indexed = BIT (24);
	uint32_t offset;
	uint32_t addr;
	uint32_t sum;
	uint32_t base;
	uint32_t value1;
	uint32_t value2;

	BUSUSEDINCPCS;

	/* If the writeback bit is set, the pre-index bit must be clear.  */
	if (write_back && !pre_indexed) {
		ARMul_UndefInstr();
		return;
	}

	/* Extract the base address register.  */
	addr_reg = LHSReg;

	/* Extract the destination register and check it.  */
	dest_reg = DESTReg;

	/* Destination register must be even.  */
	if ((dest_reg & 1)
	    /* Destination register cannot be LR.  */
	    || (dest_reg == 14)) {
		ARMul_UndefInstr();
		return;
	}

	/* Compute the base address.  */
	base = m_Reg[addr_reg];

	/* Compute the offset.  */
	offset = immediate ? ((BITS (8, 11) << 4) | BITS (0, 3)) : m_Reg[RHSReg];

	/* Compute the sum of the two.  */
	if (add_to_base)
		sum = base + offset;
	else
		sum = base - offset;

	/* If this is a pre-indexed mode use the sum.  */
	if (pre_indexed)
		addr = sum;
	else
		addr = base;

	/* The address must be aligned on a 8 byte boundary.  */
	if (addr & 0x7) {
		ARMul_UndefInstr();
		return;
	}

	/* For pre indexed or post indexed addressing modes,
	   check that the destination registers do not overlap
	   the address registers.  */
	if ((!pre_indexed || write_back)
	    && (addr_reg == dest_reg || addr_reg == dest_reg + 1)) {
		ARMul_UndefInstr();
		return;
	}

	/* Load the words.  */
	value1 = ARMul_LoadWordN(addr);
	value2 = ARMul_LoadWordN(addr + 4);

	/* Check for data aborts.  */
	if (m_Aborted) {
		TAKEABORT;
		return;
	}

	ARMul_Icycles(2);

	/* Store the values.  */
	m_Reg[dest_reg] = value1;
	m_Reg[dest_reg + 1] = value2;

	/* Do the post addressing and writeback.  */
	if (!pre_indexed)
		addr = sum;

	if (!pre_indexed || write_back)
		m_Reg[addr_reg] = addr;
}

void
arm::Handle_Store_Double(uint32_t instr)
{
	uint32_t src_reg;
	uint32_t addr_reg;
	uint32_t write_back = BIT (21);
	uint32_t immediate = BIT (22);
	uint32_t add_to_base = BIT (23);
	uint32_t pre_indexed = BIT (24);
	uint32_t offset;
	uint32_t addr;
	uint32_t sum;
	uint32_t base;

	BUSUSEDINCPCS;

	/* If the writeback bit is set, the pre-index bit must be clear.  */
	if (write_back && !pre_indexed) {
		ARMul_UndefInstr();
		return;
	}

	/* Extract the base address register.  */
	addr_reg = LHSReg;

	/* Base register cannot be PC.  */
	if (addr_reg == 15) {
		ARMul_UndefInstr();
		return;
	}

	/* Extract the source register.  */
	src_reg = DESTReg;

	/* Source register must be even.  */
	if (src_reg & 1) {
		ARMul_UndefInstr();
		return;
	}

	/* Compute the base address.  */
	base = m_Reg[addr_reg];

	/* Compute the offset.  */
	offset = immediate ? ((BITS (8, 11) << 4) | BITS (0, 3)) : m_Reg[RHSReg];

	/* Compute the sum of the two.  */
	if (add_to_base)
		sum = base + offset;
	else
		sum = base - offset;

	/* If this is a pre-indexed mode use the sum.  */
	if (pre_indexed)
		addr = sum;
	else
		addr = base;

	/* The address must be aligned on a 8 byte boundary.  */
	if (addr & 0x7) {
		ARMul_UndefInstr();
		return;
	}

	/* For pre indexed or post indexed addressing modes,
	   check that the destination registers do not overlap
	   the address registers.  */
	if ((!pre_indexed || write_back)
	    && (addr_reg == src_reg || addr_reg == src_reg + 1)) {
		ARMul_UndefInstr();
		return;
	}

	/* Load the words.  */
	ARMul_StoreWordN(addr, m_Reg[src_reg]);
	ARMul_StoreWordN(addr + 4, m_Reg[src_reg + 1]);

	if (m_Aborted) {
		TAKEABORT;
		return;
	}

	/* Do the post addressing and writeback.  */
	if (!pre_indexed)
		addr = sum;

	if (!pre_indexed || write_back)
		m_Reg[addr_reg] = addr;
}

void
arm::StoreMult(uint32_t instr, uint32_t address, uint32_t WBBase)
{
    uint32_t temp;

    UNDEF_LSMNoRegs;
    UNDEF_LSMPCBase;
    UNDEF_LSMBaseInListWb;

    if (!TFLAG)
        /* N-cycle, increment the PC and update the NextInstr state.  */
        BUSUSEDINCPCN;

    /* N cycle first.  */
    for (temp = 0; !BIT (temp); temp++);

    ARMul_StoreWordN(address, m_Reg[temp++]);

    if (m_AbortSig && !m_Aborted) {
        m_Aborted = ARMul_DataAbortV;
    }

//chy 2004-05-23, needn't store other when aborted
    if (m_Aborted)
        goto L_stm_takeabort;

    /* S cycles from here on.  */
    for (; temp < 16; temp++)
        if (BIT (temp)) {
            /* Save this register.  */
            address += 4;

            ARMul_StoreWordS(address, m_Reg[temp]);

            if (m_AbortSig && !m_Aborted) {
                m_Aborted = ARMul_DataAbortV;
            }
            //chy 2004-05-23, needn't store other when aborted
            if (m_Aborted)
                goto L_stm_takeabort;

        }

//chy 2004-05-23,should compare the Abort Models
      L_stm_takeabort:
    if (BIT (21) && LHSReg != 15) {
        if (!
            (m_AbortSig && m_Aborted
             && m_IsBaseUpdatedAbort == false))
            LSBase = WBBase;
    }
    if (m_Aborted)
        TAKEABORT;
}

void
arm::StoreSMult(uint32_t instr, uint32_t address, uint32_t WBBase)
{
    uint32_t temp;

    UNDEF_LSMNoRegs;
    UNDEF_LSMPCBase;
    UNDEF_LSMBaseInListWb;

    BUSUSEDINCPCN;

    if (m_Bank != USERBANK) {
        /* Force User Bank.  */
        (void) ARMul_SwitchMode(m_Mode, USER26MODE);
        UNDEF_LSMUserBankWb;
    }

    for (temp = 0; !BIT (temp); temp++);    /* N cycle first.  */

    ARMul_StoreWordN(address, m_Reg[temp++]);

    if (m_AbortSig && !m_Aborted) {
        m_Aborted = ARMul_DataAbortV;
    }

//chy 2004-05-23, needn't store other when aborted
    if (m_Aborted)
        goto L_stm_s_takeabort;
    /* S cycles from here on.  */
    for (; temp < 16; temp++)
        if (BIT (temp)) {
            /* Save this register.  */
            address += 4;

            ARMul_StoreWordS(address, m_Reg[temp]);

            if (m_AbortSig && !m_Aborted) {
                m_Aborted = ARMul_DataAbortV;
            }
            //chy 2004-05-23, needn't store other when aborted
            if (m_Aborted)
                goto L_stm_s_takeabort;
        }

            //chy 2006-02-16 , should not consider system mode, don't conside 26bit mode
    if (m_Mode != USER26MODE && m_Mode != USER32MODE )
        /* Restore the correct bank.  */
        (void) ARMul_SwitchMode(USER26MODE, m_Mode);


//chy 2004-05-23,should compare the Abort Models
      L_stm_s_takeabort:
    if (BIT (21) && LHSReg != 15) {
        if (!
            (m_AbortSig && m_Aborted
             && m_IsBaseUpdatedAbort == false))
            LSBase = WBBase;
    }

    if (m_Aborted)
        TAKEABORT;
}

bool
arm::StoreWord(uint32_t instr, uint32_t address)
{
    BUSUSEDINCPCN;
    ARMul_StoreWordN(address, DEST);
    if (m_Aborted)
    {
        TAKEABORT;
        return m_IsBaseUpdatedAbort;
    }

    return true;
}

bool
arm::StoreHalfWord(uint32_t instr, uint32_t address)
{
    BUSUSEDINCPCN;

    ARMul_StoreHalfWord(address, DEST);

    if (m_Aborted)
    {
        TAKEABORT;
        return m_IsBaseUpdatedAbort;
    }
    return true;
}


bool
arm::StoreByte(uint32_t instr, uint32_t address)
{
    BUSUSEDINCPCN;
    ARMul_StoreByte(address, DEST);
    if (m_Aborted)
    {
        TAKEABORT;
        return m_IsBaseUpdatedAbort;
    }
    UNDEF_LSRBPC;
    return true;
}

/* This function does the work of multiplying
   two 32bit values to give a 64bit result.  */
uint8_t
arm::Multiply64(uint32_t instr, bool msigned, bool scc)
{
    /* Operand register numbers.  */
    int nRdHi, nRdLo, nRs, nRm;
    uint32_t RdHi = 0, RdLo = 0, Rm;
    /* Cycle count.  */
    int scount;

    nRdHi = BITS (16, 19);
    nRdLo = BITS (12, 15);
    nRs = BITS (8, 11);
    nRm = BITS (0, 3);

    /* Needed to calculate the cycle count.  */
    Rm = m_Reg[nRm];

    /* Check for illegal operand combinations first.  */
    if (nRdHi != 15
            && nRdLo != 15
            && nRs != 15
            && nRm != 15 && nRdHi != nRdLo && nRdHi != nRm && nRdLo != nRm)
    {
        /* Intermediate results.  */
        uint32_t lo, mid1, mid2, hi;
        int carry;
        uint32_t Rs = m_Reg[nRs];
        int sign = 0;

        if (msigned)
        {
            /* Compute sign of result and adjust operands if necessary.  */
            sign = (Rm ^ Rs) & 0x80000000;

            if (((signed int) Rm) < 0)
                Rm = -Rm;

            if (((signed int) Rs) < 0)
                Rs = -Rs;
        }

        /* We can split the 32x32 into four 16x16 operations. This
		   ensures that we do not lose precision on 32bit only hosts.  */
        lo = ((Rs & 0xFFFF) * (Rm & 0xFFFF));
        mid1 = ((Rs & 0xFFFF) * ((Rm >> 16) & 0xFFFF));
        mid2 = (((Rs >> 16) & 0xFFFF) * (Rm & 0xFFFF));
        hi = (((Rs >> 16) & 0xFFFF) * ((Rm >> 16) & 0xFFFF));

        /* We now need to add all of these results together, taking
		   care to propogate the carries from the additions.  */
        RdLo = Add32 (lo, (mid1 << 16), &carry);
        RdHi = carry;
        RdLo = Add32 (RdLo, (mid2 << 16), &carry);
        RdHi += (carry + ((mid1 >> 16) & 0xFFFF) +
                ((mid2 >> 16) & 0xFFFF) + hi);

        if (sign)
        {
            /* Negate result if necessary.  */
            RdLo = ~RdLo;
            RdHi = ~RdHi;
            if (RdLo == 0xFFFFFFFF) {
                RdLo = 0;
                RdHi += 1;
            }
            else
                RdLo += 1;
        }

        m_Reg[nRdLo] = RdLo;
        m_Reg[nRdHi] = RdHi;
    }
    else
    {
        fprintf (stderr, "sim: MULTIPLY64 - INVALID ARGUMENTS\n");
    }

    if (scc)
    {
        /* Ensure that both RdHi and RdLo are used to compute Z,
		   but don't let RdLo's sign bit make it to N.  */
        ARMul_NegZero(RdHi | (RdLo >> 16) | (RdLo & 0xFFFF));
    }

    /* The cycle count depends on whether the instruction is a signed or
	   unsigned multiply, and what bits are clear in the multiplier.  */
    if (msigned && (Rm & ((unsigned) 1 << 31)))
    {
        /* Invert the bits to make the check against zero.  */
        Rm = ~Rm;
    }

    if ((Rm & 0xFFFFFF00) == 0)
        scount = 1;
    else if ((Rm & 0xFFFF0000) == 0)
        scount = 2;
    else if ((Rm & 0xFF000000) == 0)
        scount = 3;
    else
        scount = 4;

    return 2 + scount;
}

/* This function does the work of multiplying two 32bit
   values and adding a 64bit value to give a 64bit result.  */
uint8_t
arm::MultiplyAdd64(uint32_t instr, bool msigned, bool scc)
{
    unsigned scount;
    uint32_t RdLo, RdHi;
    int nRdHi, nRdLo;
    int carry = 0;

    nRdHi = BITS (16, 19);
    nRdLo = BITS (12, 15);

    RdHi = m_Reg[nRdHi];
    RdLo = m_Reg[nRdLo];

    scount = Multiply64(instr, msigned, LDEFAULT);

    RdLo = Add32(RdLo, m_Reg[nRdLo], &carry);
    RdHi = (RdHi + m_Reg[nRdHi]) + carry;

    m_Reg[nRdLo] = RdLo;
    m_Reg[nRdHi] = RdHi;

    if (scc)
    {
        /* Ensure that both RdHi and RdLo are used to compute Z,
		   but don't let RdLo's sign bit make it to N.  */
        ARMul_NegZero(RdHi | (RdLo >> 16) | (RdLo & 0xFFFF));
    }

    /* Extra cycle for addition.  */
    return scount + 1;
}

