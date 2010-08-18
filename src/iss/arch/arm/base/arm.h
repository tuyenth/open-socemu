/** @file arm.h
 * @brief arm class definition.
 *
 * @sa http://infocenter.arm.com/
 */

#ifndef _ARM_H_
#define _ARM_H_

#include <iostream>
#include <stdint.h>

// derived classes
#include "gdbserver.h"

struct arm: public gdbserver
{
    /// ARM constructor
    /** Function that performs the complete initialization of the ARM.
     * @param[in] gdbserver Specifies if the GDB remote connection must be supported.
     * @param[in] bigendian Big or little endian mode.
     */
    arm(bool gdbserver = false, bool gdbstart = false, bool bigendian = false);

    /// Run the ARM
    void
    run(void);

    bool irq_get(void)
    {
        // signal is active low
        return (m_NirqSig == false);
    }

    void irq_set(void)
    {
        // signal is active low
        m_NirqSig = false;
    }

    void irq_clear(void)
    {
        // signal is active low
        m_NirqSig = true;
    }

    bool fiq_get(void)
    {
        // signal is active low
        return (m_NfiqSig == false);
    }

    void fiq_set(void)
    {
        // signal is active low
        m_NfiqSig = false;
    }

    void fiq_clear(void)
    {
        // signal is active low
        m_NfiqSig = true;
    }

    /// Count the number of instructions executed
    uint64_t m_NumInstrs;

protected:
    /// Maximum number of breakpoints supported by the ISS
    enum
    {
        ARM_MAX_BREAKPOINTS = 16
    };

    enum fault_t
    {
        NO_FAULT = 0x0,
        ALIGNMENT_FAULT = 0x1,
        SECTION_TRANSLATION_FAULT = 0x5,
        PAGE_TRANSLATION_FAULT = 0x7,
        SECTION_DOMAIN_FAULT = 0x9,
        PAGE_DOMAIN_FAULT = 0xB,
        SECTION_PERMISSION_FAULT = 0xD,
        SUBPAGE_PERMISSION_FAULT = 0xF
    };

    /// Values for Emulate
    enum arm_emulate_type
    {
        /// Stop
        STOP,
        /// Single step
        ONCE,
        /// Continuous run
        RUN
    };

    /// Next instruction fetch type
    enum arm_fetch_type
    {
        /// Next instruction is sequential to previous bus access
        SEQ             = 0,
        /// Next instruction is not sequential to previous bus access
        NONSEQ          = 1,
        /// Next instruction REG[15] was already incremented but sequential
        PCINCEDSEQ      = 2,
        /// Next instruction REG[15] was already incremented but not sequential
        PCINCEDNONSEQ   = 3,
        /// Used to flush the entire pipeline
        PRIMEPIPE       = 4
    };

    /// Emulation cycle type
    enum arm_emulate_type m_Emulate;
    /// Current register bank
    uint32_t m_Reg[16];
    /// All register banks
    uint32_t m_RegBank[7][16];
    /// Current PSR
    uint32_t m_Cpsr;
    /// Saved PSR per execution mode
    uint32_t m_Spsr[7];

    /// Flags's copy of the CPSR for speed
    uint32_t m_NFlag, m_ZFlag, m_CFlag, m_VFlag, m_IFFlags;
    uint32_t m_SFlag;
    /// Thumb state
    uint32_t m_TFlag;
    /// Current register bank
    uint32_t m_Bank;
    /// Current execution mode
    uint32_t m_Mode;

    /// Current pipeline state
    uint32_t m_Instr, m_PC;
    /// Saved pipeline state
    uint32_t m_Loaded, m_Decoded;
    /// Internal CPU counters
    uint32_t m_NumScycles, m_NumNcycles, m_PreviousIcycles, m_NumIcycles, m_NumCcycles;
    /// Next instruction step type
    uint8_t m_NextInstr;

    /// Reset signal
    bool m_NresetSig;
    /// FIQ signal
    bool m_NfiqSig;
    /// IRQ signal
    bool m_NirqSig;
    /// Abort signal
    bool m_AbortSig;
    /// Sticky flag for aborts
    uint32_t m_Aborted;
    /// Extra information for base write-back in abort model
    uint32_t m_Base;

    /// Indicate if CPU is big or little endian
    bool m_IsBigEndian;
    /// Indicate if CPU supports v4 instruction set
    bool m_IsV4;
    /// Indicate if CPU supports v5 instruction set
    bool m_IsV5;
    /// Indicate if CPU supports v5exp (elsegundo) instruction set
    bool m_IsV5e;
    /** Indicates if CPU supports base updated abort model (old protocol) or
     * base restored abort model (new protocol).
     */
    bool m_IsBaseUpdatedAbort;

    /// Device ID returned from internal register
    uint32_t m_DeviceId;

    /** Debugger related variables.
     * @{
     */
    bool m_PCChanged;
    uint32_t m_BreakPts[ARM_MAX_BREAKPOINTS];
    uint8_t m_NumBreakPts;
    /// @}

    /** Scheduler related virtual function.  Indicates to the scheduler the number of core
     * cycles that were spent for the execution stage of the current instruction.
     *
     * param[in] cycles Number of cycles needed to execute the last instruction.
     */
    virtual void
    arm_exec_cycles(int cycles)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return;
    }

    /** MMU related virtual functions that must be implemented by the object that
     * derives the core.  If it is an MMU, it should implement the appropriate
     * mechanism.  If it is not an MMU, these functions should still be implemented as a
     * simple pass-through to the bus.
     * @{
     */
    /** Request a byte read.
     *
     * @param[in] va Virtual address of the byte.
     * @param[in, out] data Data variable to fill.
     * @return Eventually the fault.
     */
    virtual enum fault_t
    mmu_read_byte(uint32_t va, uint32_t* data)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return SECTION_PERMISSION_FAULT;
    }

    /** Request a half word read.
     *
     * @param[in] va Virtual address of the half word.
     * @param[in, out] data Data variable to fill.
     * @return Eventually the fault.
     */
    virtual enum fault_t
    mmu_read_halfword(uint32_t va, uint32_t* data)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return SECTION_PERMISSION_FAULT;
    }

    /** Request a word read.
     *
     * @param[in] va Virtual address of the word.
     * @param[in, out] data Data variable to fill.
     * @return Eventually the fault.
     */
    virtual enum fault_t
    mmu_read_word(uint32_t va, uint32_t* data)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return SECTION_PERMISSION_FAULT;
    }

    /** Request a byte write.
     *
     * @param[in] va Virtual address of the byte.
     * @param[in] data Data to write.
     * @return Eventually the fault.
     */
    virtual enum fault_t
    mmu_write_byte(uint32_t va, uint32_t data)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return SECTION_PERMISSION_FAULT;
    }

    /** Request a half word write.
     *
     * @param[in] va Virtual address of the half word.
     * @param[in] data Data to write.
     * @return Eventually the fault.
     */
    virtual enum fault_t
    mmu_write_halfword(uint32_t va, uint32_t data)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return SECTION_PERMISSION_FAULT;
    }

    /** Request a word write.
     *
     * @param[in] va Virtual address of the word.
     * @param[in] data Data to write.
     * @return Eventually the fault.
     */
    virtual enum fault_t
    mmu_write_word(uint32_t va, uint32_t data)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return SECTION_PERMISSION_FAULT;
    }

    /** Request an instruction load.
     *
     * @param[in] va Virtual address of the instruction.
     * @param[in, out] instr Instruction variable to fill.
     * @return Eventually the fault.
     */
    virtual enum fault_t
    mmu_load_instr(uint32_t va, uint32_t* instr)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return SECTION_PERMISSION_FAULT;
    }

    /** Request a coprocessor read.
     *
     * @param[in] instr Instruction content.
     * @param[in, out] value Data variable to fill.
     * @return 0 when successful, otherwise error.
     */
    virtual int
    mmu_mrc(uint32_t instr, uint32_t* data)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return 0;
    }

    /** Request an coprocessor write.
     *
     * @param[in] instr Instruction content.
     * @param[in] data Data to write.
     * @return 0 when successful, otherwise error.
     */
    virtual int
    mmu_mcr(uint32_t instr, uint32_t data)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return SECTION_PERMISSION_FAULT;
    }

    /** Request the vector base address.
     *
     * @return The vector base address.
     */
    virtual uint32_t
    mmu_vector_base(void)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return 0;
    }

    /** Request to set the fault.
     *
     * @param fault Fault to set.
     * @param address Address at which the fault occured.
     */
    virtual void
    mmu_set_fault(enum fault_t fault, uint32_t address)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return;
    }
    /// @}

    /// Implementation of gdbserver virtual function
    void gdb_single_step(int yesno);

    /// Implementation of gdbserver virtual function
    void gdb_set_pc(uint64_t pc);

    /// Implementation of gdbserver virtual function
    int gdb_read_registers(uint8_t *mem_buf);

    /// Implementation of gdbserver virtual function
    void gdb_write_registers(uint8_t *mem_buf, int size);

    /// Implementation of gdbserver virtual function
    int gdb_breakpoint_insert(uint64_t addr);

    /// Implementation of gdbserver virtual function
    void gdb_breakpoint_remove(uint64_t addr);


    /** Retrieve a register value.
     * @param[in] mode ARM mode to use to retrieve the register value.
     * @param[in] reg Index of the register.
     * @return The register value.
     */
    uint32_t
    ARMul_GetReg(uint8_t mode, uint8_t reg);

    /** Set a register value.
     * @param[in] mode ARM mode to use to set the register value.
     * @param[in] reg Index of the register.
     * @param[in] value Value to set in the register.
     */
    void
    ARMul_SetReg(uint8_t mode, uint8_t reg, uint32_t value);
    /** This routine returns the value of the CPSR.  */
    uint32_t
    ARMul_GetCPSR();
    /** This routine sets the value of the CPSR.  */
    void
    ARMul_SetCPSR(uint32_t value);
    /** Get an SPSR from the specified mode.  */
    uint32_t
    ARMul_GetSPSR(uint32_t mode);
    /** This routine does a write to an SPSR.  */
    void
    ARMul_SetSPSR(uint32_t mode, uint32_t value);

private:
    enum tdstate
    {
        t_undefined,    /* Undefined Thumb instruction.  */
        t_decoded,      /* Instruction decoded to ARM equivalent.  */
        t_branch        /* Thumb branch (already processed).  */
    };
    enum totype
    {
        t_norm,
        t_shift,
        t_neg,
        t_mul
    };

    /// Initializes helper arrays
    void
    init_helpers();
    /// Initializes ARM state
    void
    init_state();

    /// Emulate one instruction cycle
    void
    emulate();

    void
    ARMul_AddOverflow(uint32_t a, uint32_t b, uint32_t result);
    void
    ARMul_SubOverflow(uint32_t a, uint32_t b, uint32_t result);
    /* Assigns the C flag after an addition of a and b to give result.  */
    void
    ARMul_AddCarry(uint32_t a, uint32_t b, uint32_t result);
    /* Assigns the C flag after an subtraction of a and b to give result.  */
    void
    ARMul_SubCarry(uint32_t a, uint32_t b, uint32_t result);

    /* Align a word access to a non word boundary.  */
    uint32_t
    ARMul_Align(uint32_t address, uint32_t data)
    {
        // assumes the address is really unaligned, as a shift by 32 is undefined in C.
        // Get the word address
        address = (address & 3) << 3;
        // rotate right
        return ((data >> address) | (data << (32 - address)));
    }

    /** Check if there is an interrupt pending and not masked by processor. */
    bool IntPending();
    /** This function does the work of generating the addresses used in an
     * LDC instruction.  The code here is always post-indexed, it's up to the
     * caller to get the input address correct and to handle base register
     * modification. It also handles the Busy-Waiting.  */
    void
    ARMul_LDC(uint32_t instr, uint32_t address);
    /** This function does the work of generating the addresses used in an
     * STC instruction.  The code here is always post-indexed, it's up to the
     * caller to get the input address correct and to handle base register
     * modification. It also handles the Busy-Waiting.  */
    void
    ARMul_STC(uint32_t instr, uint32_t address);
    /** This function does the Busy-Waiting for an MCR instruction.  */
    void
    ARMul_MCR(uint32_t instr, uint32_t source);
    /* This function does the Busy-Waiting for an MRC instruction.  */
    uint32_t
    ARMul_MRC(uint32_t instr);
    /** This function does the Busy-Waiting for an CDP instruction.  */
    void
    ARMul_CDP(uint32_t instr);

    /** This routine evaluates most Data Processing register RHS's with the S
     * bit clear.  It is intended to be called from the macro DPRegRHS, which
     * filters the common case of an unshifted register with in line code.
     * @param[in] instr Instruction.
     * @return The register content.
     */
    uint32_t
    GetDPRegRHS(uint32_t instr);

    /** This routine evaluates most Logical Data Processing register RHS's
     *  with the S bit set.  It is intended to be called from the macro
     *  DPSRegRHS, which filters the common case of an unshifted register
     *  with in line code.
     * @param[in] instr Instruction.
     * @return The register content.
     */
    uint32_t
    GetDPSRegRHS(uint32_t instr);

    /** This routine evaluates most Load and Store register RHS's.  It is
     *  intended to be called from the macro LSRegRHS, which filters the
     *  common case of an unshifted register with in line code.
     */
    uint32_t
    GetLSRegRHS(uint32_t instr);

    /** This routine evaluates the ARM7T halfword and signed transfer RHS's.  */
    uint32_t
    GetLS7RHS(uint32_t instr);

    /** This routine handles writes to register 15 when the S bit is not set.  */
    void
    WriteR15(uint32_t src);

    /** This routine handles writes to register 15 when the S bit is set.  */
    void
    WriteSR15(uint32_t src);

    /** In machines capable of running in Thumb mode, BX, BLX, LDR and LDM
       will switch to Thumb mode if the least significant bit is set.  */
    void
    WriteR15Branch(uint32_t src);

    /** This routine updates the state of the emulator after the Cpsr has been
     * changed.  Both the processor flags and register bank are updated.
     */
    void
    ARMul_CPSRAltered();

    /** This routine does all the nasty bits involved in a write to the CPSR,
     * including updating the register bank, given a MSR instruction.  */
    void
    ARMul_FixCPSR(uint32_t instr, uint32_t rhs);
    /** This routine does a write to the current SPSR, given an MSR instruction.  */
    void
    ARMul_FixSPSR(uint32_t instr, uint32_t rhs);
    /** This function handles Undefined instructions, as CP isntruction.  */
    void
    ARMul_UndefInstr();
    /** This routine causes an Abort to occur, including selecting the correct
     * mode, register bank, and the saving of registers.  Call with the
     * appropriate vector's memory address (0,4,8 ....)
     */
    void
    ARMul_Abort(uint32_t vector);

    /** Decode a 16bit Thumb instruction.  The instruction is in the low
     * 16-bits of the tinstr field, with the following Thumb instruction
     * held in the high 16-bits.  Passing in two Thumb instructions allows
     * easier simulation of the special dual BL instruction.
     */
    enum tdstate
    ARMul_ThumbDecode(uint32_t pc,uint32_t tinstr,uint32_t* ainstr);

    /** This function does the work of loading the registers listed in an LDM
     * instruction, when the S bit is clear.  The code here is always increment
     * after, it's up to the caller to get the input address correct and to
     * handle base register modification.
     */
    void
    LoadMult(uint32_t instr, uint32_t address, uint32_t WBBase);

    /** This function does the work of loading the registers listed in an LDM
     * instruction, when the S bit is set. The code here is always increment
     * after, it's up to the caller to get the input address correct and to
     * handle base register modification.
     */
    void
    LoadSMult(uint32_t instr, uint32_t address, uint32_t WBBase);

    /** This function does the work of loading a word.  */
    bool
    LoadWord (uint32_t instr, uint32_t address);

    /** This function does the work of loading a halfword.  */
    bool
    LoadHalfWord(uint32_t instr, uint32_t address, bool signextend);

    /** This function does the work of loading a byte for a LDRB instruction.  */
    bool
    LoadByte(uint32_t instr, uint32_t address, bool signextend);

    /** This function does the work of storing the registers listed in an STM
     * instruction, when the S bit is clear.  The code here is always increment
     * after, it's up to the caller to get the input address correct and to
     * handle base register modification.
     */
    void
    StoreMult(uint32_t instr, uint32_t address, uint32_t WBBase);
    /** This function does the work of storing the registers listed in an STM
     * instruction when the S bit is set.  The code here is always increment
     * after, it's up to the caller to get the input address correct and to
     * handle base register modification.
     */
    void
    StoreSMult(uint32_t instr, uint32_t address, uint32_t WBBase);
    /** This function does the work of storing a word from a STR instruction.  */
    bool
    StoreWord(uint32_t instr, uint32_t address);
    /** This function does the work of storing a byte for a STRH instruction.  */
    bool
    StoreHalfWord(uint32_t instr, uint32_t address);
    /** This function does the work of storing a byte for a STRB instruction.  */
    bool
    StoreByte(uint32_t instr, uint32_t address);

    /** This function does the work of loading two words for a LDRD instruction.  */
    void
    Handle_Load_Double(uint32_t instr);
    /** This function does the work of storing two words for a STRD instruction.  */
    void
    Handle_Store_Double(uint32_t instr);

    /** This function does the work of multiplying two 32bit values to give a
     * 64bit result.
     */
    uint8_t
    Multiply64(uint32_t instr, bool msigned, bool scc);

    /** This function does the work of multiplying two 32bit values and adding a
     * 64bit value to give a 64bit result.
     */
    uint8_t
    MultiplyAdd64(uint32_t instr, bool msigned, bool scc);

    /** This function does the work of adding two 32bit values together, and
     * calculating if a carry has occurred.
     */
    uint32_t
    Add32(uint32_t a1, uint32_t a2, int *carry)
    {
        uint32_t result = (a1 + a2);
        unsigned int uresult = (unsigned int) result;
        unsigned int ua1 = (unsigned int) a1;

        /* If (result == RdLo) and (state->Reg[nRdLo] == 0),
           or (result > RdLo) then we have no carry.  */
        if ((uresult == ua1) ? (a2 != 0) : (uresult < ua1))
            *carry = 1;
        else
            *carry = 0;

        return result;
    }
    /** This routine controls the saving and restoring of registers across mode
     * changes.  The regbank matrix is largely unused, only rows 13 and 14 are
     * used across all modes, 8 to 14 are used for FIQ, all others use the USER
     * column.  It's easier this way.  old and new parameter are modes numbers.
     * Notice the side effect of changing the Bank variable.
     */
    uint32_t
    ARMul_SwitchMode(uint32_t oldmode, uint32_t newmode);
    /** Assigns the N and Z flags depending on the value of result.  */
    void
    ARMul_NegZero(uint32_t result);
    /** Compute whether an addition of A and B, giving RESULT, overflowed.  */
    bool
    AddOverflow(uint32_t a, uint32_t b, uint32_t result);
    /** Compute whether a subtraction of A and B, giving RESULT, overflowed.  */
    bool
    SubOverflow(uint32_t a, uint32_t b, uint32_t result);

    uint32_t
    ARMul_ReLoadInstr(uint32_t address, uint32_t isize);
    uint32_t
    ARMul_LoadInstrS(uint32_t address, uint32_t isize);
    uint32_t
    ARMul_LoadInstrN(uint32_t address, uint32_t isize);
    uint32_t
    ARMul_ReadWord(uint32_t address);
    uint32_t
    ARMul_LoadWordS(uint32_t address);
    uint32_t
    ARMul_LoadWordN(uint32_t address);
    uint32_t
    ARMul_LoadHalfWord(uint32_t address);
    uint32_t
    ARMul_ReadByte(uint32_t address);
    uint32_t
    ARMul_LoadByte(uint32_t address);
    void
    ARMul_WriteWord(uint32_t address, uint32_t data);
    void
    ARMul_StoreWordS(uint32_t address, uint32_t data);
    void
    ARMul_StoreWordN(uint32_t address, uint32_t data);
    void
    ARMul_StoreHalfWord(uint32_t address, uint32_t data);
    void
    ARMul_WriteByte(uint32_t address, uint32_t data);
    void
    ARMul_StoreByte(uint32_t address, uint32_t data);
    uint32_t
    ARMul_SwapWord(uint32_t address, uint32_t data);
    uint32_t
    ARMul_SwapByte(uint32_t address, uint32_t data);

    /** Indicate the number of I cycles (internal execution) that have elapsed.
     * param[in] number Number of I cycles to increment by.
     */
    void
    ARMul_Icycles(unsigned number)
    {
        m_NumIcycles += number;
        m_AbortSig = false;
    }


    /** Indicate the number of C cycles (co-processor access) that have elapsed.
     * param[in] number Number of C cycles to increment by.
     */
    void
    ARMul_Ccycles(unsigned number)
    {
        m_NumCcycles += number;
        m_AbortSig = false;
    }

    /// Specifies if the a remote gdb is connected
    bool m_gdbconnected;

};

/// ARM min comparison macro
#define arm_min(x,y) (((x) < (y)) ? (x) : (y))
/// ARM max comparison macro
#define arm_max(x,y) (((x) > (y)) ? (x) : (y))

/***************************************************************************\
*                   Macros to extract instruction fields                    *
\***************************************************************************/

/// Bit n of instruction
#define BIT(n) ((uint32_t)(instr>>(n))&1)
/// Bits m to n of instruction
#define BITS(m,n) ((uint32_t)(instr<<(31-(n))) >> ((31-(n))+(m)))
/// Bits 31 to n of instruction
#define TOPBITS(n) (instr >> (n))

/***************************************************************************\
*                      The hardware vector addresses                        *
\***************************************************************************/

#define ARMResetV 0L
#define ARMUndefinedInstrV 4L
#define ARMSWIV 8L
#define ARMPrefetchAbortV 12L
#define ARMDataAbortV 16L
#define ARMAddrExceptnV 20L
#define ARMIRQV 24L
#define ARMFIQV 28L
#define ARMErrorV 32L       /* This is an offset, not an address ! */

#define ARMul_ResetV ARMResetV
#define ARMul_UndefinedInstrV ARMUndefinedInstrV
#define ARMul_SWIV ARMSWIV
#define ARMul_PrefetchAbortV ARMPrefetchAbortV
#define ARMul_DataAbortV ARMDataAbortV
#define ARMul_AddrExceptnV ARMAddrExceptnV
#define ARMul_IRQV ARMIRQV
#define ARMul_FIQV ARMFIQV

/***************************************************************************\
*                          Mode and Bank Constants                          *
\***************************************************************************/

#define USER26MODE 0L
#define FIQ26MODE 1L
#define IRQ26MODE 2L
#define SVC26MODE 3L
#define USER32MODE 16L
#define FIQ32MODE 17L
#define IRQ32MODE 18L
#define SVC32MODE 19L
#define ABORT32MODE 23L
#define UNDEF32MODE 27L

#define USERBANK 0
#define FIQBANK 1
#define IRQBANK 2
#define SVCBANK 3
#define ABORTBANK 4
#define UNDEFBANK 5
#define DUMMYBANK 6
#define SYSTEMBANK USERBANK
#define BANK_CAN_ACCESS_SPSR(bank)  \
  ((bank) != USERBANK && (bank) != SYSTEMBANK && (bank) != DUMMYBANK)

/***************************************************************************\
*                  Definitions of things to handle aborts                   *
\***************************************************************************/

/* SWI -1 */
#define ARMul_ABORTWORD (m_TFlag ? 0xefffdfff : 0xefffffff)
#define ARMul_DATAABORT()  m_AbortSig = true; \
                           m_Aborted = ARMul_DataAbortV


/***************************************************************************\
*                            Host-dependent stuff                           *
\***************************************************************************/

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

#ifndef NFLAG
#define NFLAG   m_NFlag
#endif //NFLAG

#ifndef ZFLAG
#define ZFLAG   m_ZFlag
#endif //ZFLAG

#ifndef CFLAG
#define CFLAG   m_CFlag
#endif //CFLAG

#ifndef VFLAG
#define VFLAG   m_VFlag
#endif //VFLAG

#ifndef IFLAG
#define IFLAG   (m_IFFlags >> 1)
#endif //IFLAG

#ifndef FFLAG
#define FFLAG   (m_IFFlags & 1)
#endif //FFLAG

#ifndef IFFLAGS
#define IFFLAGS m_IFFlags
#endif //VFLAG

#define FLAG_MASK   0xf0000000
#define NBIT_SHIFT  31
#define ZBIT_SHIFT  30
#define CBIT_SHIFT  29
#define VBIT_SHIFT  28


// standard message output
#define ARM_WARN(format, ...)                                               \
    fprintf(stderr, "%s(%d): " format "\n",  __FILE__, __LINE__, __VA_ARGS__)

#endif // _ARM_H_
