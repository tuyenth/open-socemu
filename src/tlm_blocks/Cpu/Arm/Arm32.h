#include "CpuBase.h"

/// debug level
#define ARM32_DEBUG_LEVEL 0

/// Macro to print debug messages
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define ARM32_TLM_DBG(__l, __f, ...)                                                    \
    do {                                                                                \
        if (ARM32_DEBUG_LEVEL >= __l) {                                                 \
            TLM_DBG(__f, __VA_ARGS__);                                                  \
        }                                                                               \
    } while (false)


#define ARCH(__a)                                                                       \
    do {                                                                                \
        if (!this->has_##__a())                                                         \
        {                                                                               \
            this->exception |= EXCEPT_ILLEGAL_OP;                                       \
            return;                                                                     \
        }                                                                               \
    } while (false)


template<typename GDB>
struct Arm32: CpuBase<GDB>
{
    /** Arm32 constructor
     * @param[in] name Name of the module
     * @param[in] parameters Command line parameters
     * @param[in, out] config Parameters of the current block (and sub-blocks)
     */
    Arm32(sc_core::sc_module_name name, Parameters& parameters, MSP& config)
    : CpuBase<GDB>(name, parameters, config)
    {
        this->reset();
    }

    /** Check if there is a pending exception
     * @return True if there is a pending exception, false otherwise
     */
    virtual bool
    handle_exception()
    {
        return false;
    }

    /// Reset the CPU
    virtual void
    reset(void)
    {
        this->regs[REG_PC] = 0;
        this->TF = 0;
    }

    /// Return the current program counter
    virtual uint64_t
    get_pc()
    {
        return this->regs[REG_PC];
    }

    /// Change the current program counter
    virtual void
    set_pc(uint64_t pc)
    {
        this->regs[REG_PC] = (uint32_t)pc;
    }

    /** Change the program counter location
     * @param[in] pc New program counter value to set
     */
    virtual void
    gdb_set_pc(uint64_t pc)
    {
        this->set_pc(pc);
    }

    /// Fetch the next instruction
    virtual void
    fetch_insn()
    {
        // fetch the next instruction
        this->insn = rd_l_aligned(this->get_pc());
    }

    /// Execute a single instruction
    virtual void
    execute_insn()
    {
        // check the current execution mode
        if (this->TF)
        {
            // try to execute a thumb operation
            this->exec_thumb();
        }
        else
        {
            // try to execute a thumb operation
            this->exec_arm();
        }
    }

protected:
    /// Supported exceptions
    enum exceptions
    {
        /// Illegal operation
        EXCEPT_ILLEGAL_OP = 1<<0,
        /// Regular interrupt
        EXCEPT_INTERRUPT = 1<<1
    };

    enum cpsr_flags
    {
        CPSR_M = 0x1F,
        CPSR_T = 1<<5,
        CPSR_F = 1<<6,
        CPSR_I = 1<<7,
        CPSR_A = 1<<8,
        CPSR_E = 1<<9,
        CPSR_IT_7_2 = 0x3F<<10,
        CPSR_GE = 0xF<<16,
        CPSR_RESERVED = 0xF<<20,
        CPSR_J = 1<<24,
        CPSR_IT_1_0 = 3<<25,
        CPSR_IT = CPSR_IT_7_2 | CPSR_IT_1_0,
        CPSR_EXEC = CPSR_IT | CPSR_J | CPSR_T,
        CPSR_Q = 1<<27,
        CPSR_V = 1<<28,
        CPSR_C = 1<<29,
        CPSR_Z = 1<<30,
        CPSR_N = 1<<31,
        CPSR_NZCV = CPSR_N | CPSR_Z | CPSR_C | CPSR_V,
        CPSR_USER = CPSR_NZCV | CPSR_Q | CPSR_GE,
        CPSR_CACHED = CPSR_USER | CPSR_IT | CPSR_T | CPSR_M
    };

    enum arm_modes
    {
        MODE_USR = 0x10,
        MODE_FIQ = 0x11,
        MODE_IRQ = 0x12,
        MODE_SVC = 0x13,
        MODE_MONITOR = 0x16,
        MODE_ABORT = 0x17,
        MODE_UNDEFINED = 0x1B,
        MODE_SYSTEM = 0x1F
    };

    enum registers
    {
        REG_R0 = 0,
        REG_R1 = 1,
        REG_R2 = 2,
        REG_R3 = 3,
        REG_R4 = 4,
        REG_R5 = 5,
        REG_R6 = 6,
        REG_R7 = 7,
        REG_R8 = 8,
        REG_R9 = 9,
        REG_R10 = 10,
        REG_R11 = 11,
        REG_R12 = 12,
        REG_SP = 13,
        REG_LR = 14,
        REG_PC = 15,
        REG_SPSR = 16,
        REG_NUM = 17
    };


    /** Indicates if the architecture 6 is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_6()
    {
        return true;
    }

    /** Indicates if the architecture T2 is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_T2()
    {
        return true;
    }

    /** Indicates if the architecture M is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_M()
    {
        return false;
    }

    /** Hack for Qemu import (6T2 or Thumb2 are used indifferently)
     * @return True if the feature is supported
     */
    bool
    has_6T2()
    {
        return this->has_T2();
    }

    /** Indicate if the current execution state is USER mode
     * @return True if the processor is running with user privileges
     */
    bool
    IS_USER()
    {
        if (has_M())
        {
            // not implemented yet
            assert(0);
        }
        else
        {
            return (this->MF == MODE_USR);
        }
    }

    /** Change the current mode
     * @param[in] mode New mode
     */
    void
    switch_mode(uint32_t mode)
    {
        // to be implemented
        assert(0);
    }

    /** Test the condition code
     * @param[in] cond The condition code from the operand
     * @return True if the condition passed, False otherwise
     */
    bool
    test_cc(uint32_t cond)
    {
        switch (cond)
        {
        case 0: // EQ
            if (this->ZF)
                return true;
            break;
        case 1: // NE
            if (!this->ZF)
                return true;
            break;
        case 2: // CS
            if (this->CF)
                return true;
            break;
        case 3: // CC
            if (!this->CF)
                return true;
            break;
        case 4: // MI
            if (this->NF)
                return true;
            break;
        case 5: // PL
            if (!this->NF)
                return true;
            break;
        case 6: // VS
            if (this->VF)
                return true;
            break;
        case 7: // VC
            if (!this->VF)
                return true;
            break;
        case 8: // HI
            if ((this->CF) && (!this->ZF))
                return true;
            break;
        case 9: // LS
            if ((!this->CF) || this->ZF)
                return true;
            break;
        case 10: // GE
            if (this->NF == this->VF)
                return true;
            break;
        case 11: // LT
            if (this->NF != this->VF)
                return true;
            break;
        case 12: // GT
            if ((!this->ZF) && (this->NF == this->VF))
                return true;
            break;
        case 13: // LE
            if (this->ZF || (this->NF != this->VF))
                return true;
            break;
        default:
            // we should never get here
            assert(false);
            break;
        }
        return false;
    }

    /** Get the mask of the PSR to apply for a PSR update instruction
     * @return The mask to apply to the PSR
     */
    uint32_t
    msr_mask(int flags, int spsr)
    {
        uint32_t mask;
        mask = 0;
        if (flags & (1 << 0))
            mask |= 0xff;
        if (flags & (1 << 1))
            mask |= 0xff00;
        if (flags & (1 << 2))
            mask |= 0xff0000;
        if (flags & (1 << 3))
            mask |= 0xff000000;

        // mask out undefined bits
        mask &= ~CPSR_RESERVED;
        if (!this->has_6())
            mask &= ~(CPSR_E | CPSR_GE);
        if (!this->has_T2())
            mask &= ~CPSR_IT;
        // mask out execution state bits
        if (!spsr)
            mask &= ~CPSR_EXEC;
        // mask out privileged bits
        if (IS_USER())
            mask &= CPSR_USER;
        return mask;
    }

    /** Read the current CPSR value
     * @return The current CPSR value
     */
    uint32_t
    get_cpsr()
    {
        return this->raw_cpsr | (this->NF << 31) | (this->ZF << 30) |
            (this->CF << 29) | (this->VF << 28) | (this->QF << 27)
            | (this->TF << 5) | ((this->ITF & 3) << 25)
            | (this->GEF << 16)
            | ((this->ITF & 0xfc) << 8)
            | this->MF;
    }

    /** Write a new value and mask in the CPSR
     * @param[in] val Value to write
     * @param[in] mask Mask to apply to the value
     */
    void
    set_cpsr(uint32_t val, uint32_t mask)
    {
        if (mask & CPSR_NZCV) {
            this->NF = (val >> 31) & 1;
            this->ZF = (val >> 30) & 1;
            this->CF = (val >> 29) & 1;
            this->VF = (val >> 28) & 1;
        }
        if (mask & CPSR_Q)
            this->QF = ((val & CPSR_Q) != 0);
        if (mask & CPSR_T)
            this->TF = ((val & CPSR_T) != 0);
        if (mask & CPSR_IT_1_0) {
            this->ITF &= ~3;
            this->ITF |= (val >> 25) & 3;
        }
        if (mask & CPSR_IT_7_2) {
            this->ITF &= 3;
            this->ITF |= (val >> 8) & 0xfc;
        }
        if (mask & CPSR_GE) {
            this->GEF = (val >> 16) & 0xf;
        }

        // check if mode bits have changed
        if ((this->raw_cpsr ^ val) & mask & CPSR_M) {
            this->switch_mode(val & CPSR_M);
        }

        // save the other bits
        mask &= ~CPSR_CACHED;
        this->raw_cpsr = (this->raw_cpsr & ~mask) | (val & mask);
    }

    /** Write a new value and mask in the SPSR
     * @param[in] val Value to write
     * @param[in] mask Mask to apply to the value
     */
    void
    set_spsr(uint32_t val, uint32_t mask)
    {
        this->regs[REG_SPSR] = (this->regs[REG_SPSR] & ~mask) | (val & mask);
    }

    /** Set CPSR or SPSR with value and masking flags
     * @param[in] mask Mask to apply to the write access
     * @param[in] spsr 0 if CPSR access, otherwise SPSR access
     * @param[in] t0 Value to write
     */
    virtual bool
    set_psr(uint32_t mask, int spsr, uint32_t t0)
    {
        if (spsr) {
            // SPSR access not supported in user mode
            if (IS_USER())
                return true;

            set_spsr(t0, mask);
        } else {
            set_cpsr(t0, mask);
        }
        return false;
    }

    /// Called when there is a WFI instruction
    virtual void
    exec_wfi()
    {

    }

    virtual void
    exec_uncond()
    {
        // to be implemented
        this->exception |= EXCEPT_ILLEGAL_OP;

    }

    virtual void
    exec_nop_hint(uint32_t val)
    {
        // currently, this is an error
        this->exception |= EXCEPT_ILLEGAL_OP;

        switch (val)
        {
        case 3: // wfi
            this->exec_wfi();
            break;
        case 1: // yield
        case 2: // wfe (wait for event)
        case 4: // sev (send event)
            break;
        default:
            if (val & 0xF0)
            {
                // debug hint
            }
            // nop
            break;
        }
    }

    virtual void
    exec_arm_nop_hint()
    {
        // execute the hint instruction
        exec_nop_hint(this->insn & 0xFF);

    }

    virtual void
    exec_thumb_nop_hint()
    {
        // execute the hint instruction
        exec_nop_hint((this->insn >> 4) & 0xF);

        // move to the next instruction
        this->set_pc(this->get_pc() + 2);
    }

    virtual void
    exec_arm()
    {
        uint32_t cond, rd, val, shift, tmp, i;

        // by default, move to the next instruction
        this->set_pc(this->get_pc() + 4);

        // retrieve the condition flags
        cond = insn >> 28;

        // check if it is an unconditional instruction
        if (unlikely(cond == 0xf))
        {
            // unconditional instructions
            this->exec_uncond();
            return;
        }

        // if it is not AL condition code, check the executability of the operation
        if (cond != 0xe)
        {
            if (!test_cc(cond))
            {
                return;
            }
        }

        if ((insn & 0x0f900000) == 0x03000000)
        {
            if ((insn & (1 << 21)) == 0)
            {
                ARCH(6T2);
                rd = (insn >> 12) & 0xf;
                val = ((insn >> 4) & 0xf000) | (insn & 0xfff);
                if ((insn & (1 << 22)) == 0) {
                    /* MOVW */
                    tmp = val;
                } else {
                    /* MOVT */
                    tmp = this->regs[rd];
                    tmp &= 0xFFFF;
                    tmp |= val << 16;
                }
                this->regs[rd] = tmp;
                return;
            }
            else
            {
                if (((insn >> 12) & 0xf) != 0xf) {
                    this->exception |= EXCEPT_ILLEGAL_OP;
                    return;
                }
                if (((insn >> 16) & 0xf) == 0) {
                    // NOP
                    this->exec_arm_nop_hint();
                    return;
                } else {
                    // MSR(immediate)
                    val = insn & 0xff;
                    shift = ((insn >> 8) & 0xf) * 2;
                    if (shift)
                        val = (val >> shift) | (val << (32 - shift));
                    i = ((insn & (1 << 22)) != 0);
                    if (set_psr(msr_mask((insn >> 16) & 0xf, i), i, val))
                    {
                        this->exception |= EXCEPT_ILLEGAL_OP;
                    }
                    return;
                }
            }
        }
    }

    virtual void
    exec_thumb()
    {
        this->set_pc(this->get_pc()+2);
    }

    /// Last fetched instruction content
    uint32_t insn;
    /// Current Negative flag
    uint32_t NF;
    /// Current Zero flag
    uint32_t ZF;
    /// Current Carry flag
    uint32_t CF;
    /// Current oVerflow flag
    uint32_t VF;
    /// Current saturation flag
    uint32_t QF;
    /// Current Greater than or Equal flags
    uint32_t GEF;
    /// Current Thumb flag
    uint32_t TF;
    /// Current Mode flags
    uint32_t MF;
    /// Current IfThen flags
    uint32_t ITF;
    /// Other CPSR flags
    uint32_t raw_cpsr;

    /// Exception
    uint32_t exception;
    /// Current registers (includes the CPSR and the SPSR)
    uint32_t regs[REG_NUM];
    /// Banked registers
    uint32_t banked_regs[4][REG_NUM];
};
