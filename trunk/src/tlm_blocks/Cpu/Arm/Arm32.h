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

#define UNSUPPORTED()                                                                   \
    do {                                                                                \
        hdlr->fn = HDLR_FN(Arm32::illegalop);                                           \
        return;                                                                         \
    } while (false)

#define ARCH(__a)                                                                       \
    do {                                                                                \
        if (!this->has_##__a())                                                         \
        {                                                                               \
            UNSUPPORTED();                                                              \
        }                                                                               \
    } while (false)

#define INSNHDLR_ARM(__f)                                                               \
    void __f ## _arm(void* params[]) {                                                  \
        this->set_pc(this->get_pc() + 4);                                               \
        this->__f(params);                                                              \
    }

#define INSNHDLR_THUMB(__f)                                                             \
    void __f ## _thumb(void* params[]) {                                                \
        this->set_pc(this->get_pc() + 2);                                               \
        this->__f(params);                                                              \
    }


template<typename GDB>
struct Arm32: CpuBase<GDB>
{
    /// Define the INSNHDLR type
    typedef void (CpuBase<GDB>::*INSNHDLR)(void* params[]);

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
        this->insn = this->rd_l(this->get_pc());
    }

    /** Decode an instruction
     * @param hdlr Instruction handler to fill for execution
     */
    virtual void
    decode_insn(struct InsnHandler* hdlr)
    {
        // check the current execution mode
        if (this->TF)
        {
            // try to execute a thumb operation
            this->decode_thumb(hdlr);
        }
        else
        {
            // try to execute a thumb operation
            this->decode_arm(hdlr);
        }
    }

    /** Read the CPU registers and fill the buffer
     * @param[out] mem_buf Destination buffer for the registers content
     * @return Number of bytes written in the buffer
     * @warning There is no check for the buffer boundaries.  It is expected to be
     * large enough to hold all the registers.
     */
    virtual int
    gdb_rd_reg(uint8_t *mem_buf)
    {
        uint8_t *ptr;

        ptr = mem_buf;

        // 15 core integer registers (4 bytes each)
        memcpy(ptr, this->regs, 4*15);
        ptr += 4*15;

        // R15 = Program Counter register
        *(uint32_t *)ptr = (uint32_t)this->get_pc();
        ptr += 4;

        // 8 FPA registers (12 bytes each), FPS (4 bytes), not implemented
        memset (ptr, 0, (8 * 12) + 4);
        ptr += 8 * 12 + 4;
        // CPSR (4 bytes)
        *(uint32_t *)ptr = this->get_cpsr();
        ptr += 4;

        // return the size of the buffer filled
        return ptr - mem_buf;
    }

    /** Write the CPU registers
     * @param[in] mem_buf Buffer containing the registers content
     * @param[in] size Size of the buffer
     */
    virtual void
    gdb_wr_reg(uint8_t *mem_buf, int size)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
    }

protected:
    /// Generic values
    enum
    {
        SIGNBIT = 0x80000000
    };

    /// Supported exceptions
    enum exceptions
    {
        /// Illegal operation
        EXCEPT_ILLEGAL_OP = 1<<0,
        /// Regular interrupt
        EXCEPT_INTERRUPT = 1<<1
    };

    /// Flags in the Program Status Registers
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

    /// Supported modes
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

    /// Stored registers
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

    /** Indicates if the architecture IWMMXT is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_IWMMXT()
    {
        return false;
    }

    /** Indicates if the architecture 5T is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_5T()
    {
        return false;
    }

    /** Indicates if the architecture 5TE is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_5TE()
    {
        return false;
    }

    /** Indicates if the architecture 5TEJ is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_5TEJ()
    {
        return false;
    }

    /** Indicates if the architecture 6 is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_6()
    {
        return false;
    }

    /** Indicates if the architecture 6K is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_6K()
    {
        return false;
    }

    /** Indicates if the architecture 6T2 is supported (6T2 or Thumb2 are used indifferently)
     * @return True if the feature is supported
     */
    bool
    has_6T2()
    {
        return false;
    }

    /** Indicates if the architecture 7 is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_7()
    {
        return false;
    }

    /** Indicates if the M profile is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_M()
    {
        return false;
    }

    /** Indicates if the architecture NEON is supported
     * @return True if the feature is supported
     */
    virtual bool
    has_NEON()
    {
        return false;
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

    /** Perform an addition that also updates the Q flag
     * @param[in] val1 Value to sum
     * @param[in] val2 Value to sum
     * @return The result of the addition
     */
    uint32_t
    add_setq(uint32_t val1, uint32_t val2)
    {
        uint32_t res = val1 + val2;
        if (((res ^ val1) & SIGNBIT) && !((val1 ^ val2) & SIGNBIT))
            this->QF = 1;
        return res;
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
        if (!this->has_6T2())
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
            if (unlikely(IS_USER()))
                return true;
            this->set_spsr(t0, mask);
        } else {
            this->set_cpsr(t0, mask);
        }
        return false;
    }

    /** Execute a Count Leading Zeros
     * @param[in] val Value to count the leading zeros in
     */
    virtual uint32_t
    clz(uint32_t val)
    {
        uint32_t result = 0;
        if (val < (1 << 16))
        {
            result += 16;
            val <<= 16;
        }
        if (val < (1 << 24))
        {
            result += 8;
            val <<= 8;
        }
        if (val < (1 << 28))
        {
            result += 4;
            val <<= 4;
        }
        if (val < (1 << 30))
        {
            result += 2;
            val <<= 2;
        }
        if (val < (uint32_t)(1 << 31))
        {
            result += 1;
            val <<= 1;
        }
        if (val == 0)
            result = 32;
        return result;
    }

    /// Called when there is a WFI instruction
    virtual void
    wfi()
    {

    }

    /** Execute a NOP/HINT instruction
     * @param[in] val NOP/HINT flags
     */
    virtual void
    nop_hint(uint32_t val)
    {
        // currently, this is an error
        this->exception |= EXCEPT_ILLEGAL_OP;

        switch (val)
        {
        case 3: // wfi
            this->wfi();
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
    arm_nop_hint()
    {
        // execute the hint instruction
        nop_hint(this->insn & 0xFF);

    }

    virtual void
    thumb_nop_hint()
    {
        // execute the hint instruction
        nop_hint((this->insn >> 4) & 0xF);

    }

    /** Illegal operand handler instruction handler
     * @param[in] params Parsed parameters
     */
    void
    illegalop(void* params[])
    {
        this->exception |= EXCEPT_ILLEGAL_OP;
    }

    /** NOP instruction handler
     * @param[in] params Parsed parameters
     */
    void
    nop(void* params[])
    {
        // nothing to do
    }
    INSNHDLR_ARM(nop);
    INSNHDLR_THUMB(nop);

    /** PLD instruction handler
     * @param[in] params Parsed parameters
     */
    void
    pld(void* params[])
    {
        // nothing to do
    }
    INSNHDLR_ARM(pld);
    INSNHDLR_THUMB(pld);

    /** SETEND instruction handler
     * @param[in] params Parsed parameters
     */
    void
    setend(void* params[])
    {
        // params[0] = endianstate
        // check if the big endian mode is requested
        if (params[0]) {
            // BE8 mode not implemented
            assert(0);
        }
    }
    INSNHDLR_ARM(setend);
    INSNHDLR_THUMB(setend);

    /** SRS instruction handler
     * @param[in] params Parsed parameters
     */
    void
    srs(void* params[])
    {
        // params[0] = mode
        // params[1] = offset
        // params[2] = writeback
        // params[3] = wb_offset (only if writeback is set)
        uint32_t mode = (uint32_t)params[0];
        int32_t offset = (int32_t)params[1];
        uint32_t addr;

        // not supported in USR mode
        if (IS_USER()) {
            this->exception |= EXCEPT_ILLEGAL_OP;
            return;
        }
        // check if the current mode was indicated
        if (mode == (this->MF)) {
            addr = this->regs[REG_SPSR];
        } else {
            addr = this->banked_regs[mode-MODE_USR][REG_SPSR];
        }
        if (offset)
            addr += offset;
        // write the LR register on the stack
        wr_l(addr, this->regs[REG_LR]);
        addr += 4;
        // write the SPSR register on the stack
        wr_l(addr, this->regs[REG_SPSR]);
        if (params[2] != NULL) {
            // Base writeback
            offset = (int32_t) params[3];
            if (offset)
                addr += offset;
            if (mode == this->MF) {
                this->regs[REG_SPSR] = addr;
            } else {
                this->banked_regs[mode-MODE_USR][REG_SPSR] = addr;
            }
        }
    }
    INSNHDLR_ARM(srs);
    INSNHDLR_THUMB(srs);

    /** RFE instruction handler
     * @param[in] params Parsed parameters
     */
    void
    rfe(void* params[])
    {
        // params[0] = Rn
        // params[1] = offset
        // params[2] = writeback
        // params[3] = wb_offset (only if writeback is set)
        uint32_t rn = (uint32_t)params[0];
        int32_t offset = (int32_t)params[1];
        uint32_t addr = this->regs[rn];
        uint32_t tmp, tmp2;
        // not supported in USR mode
        if (IS_USER()) {
            this->exception |= EXCEPT_ILLEGAL_OP;
            return;
        }
        if (offset)
            addr += offset;
        // load PC into tmp and CPSR into tmp2
        tmp = this->rd_l(addr);
        addr += 4;
        tmp2 = this->rd_l(addr);
        if (params[2] != NULL) {
            offset = (int32_t)params[3];
            // Base writeback
            this->regs[rn] = addr;
            if (offset)
                addr += offset;
            this->regs[rn] = addr;
        }
        // first, we set the cpsr because the PC reg may get overwritten by mode switch
        this->set_cpsr(tmp2, 0xffffffff);
        this->set_pc(tmp);
    }
    INSNHDLR_ARM(rfe);
    INSNHDLR_THUMB(rfe);

    /** BX instruction handler
     * @param[in] params Parsed parameters
     */
    void
    bx(void* params[])
    {
        // params[0] = absolute address (if bit0 set, change to Thumb)
        uint32_t addr = (uint32_t)params[0];
        // update the thumb state
        this->TF = addr & 1;
        // load the new address
        this->set_pc(addr & (~1));
    }
    INSNHDLR_ARM(bx);
    INSNHDLR_THUMB(bx);

    /** BLX instruction handler
     * @param[in] params Parsed parameters
     */
    void
    blx(void* params[])
    {
        // params[0] = sign extended offset (4 aligned)
        // params[1] = bit1 (2 aligned, hbit)
        // params[2] = bit0 (1 aligned, thumbbit)
        int32_t offset = (int32_t)params[0];
        uint32_t hbit = (uint32_t)params[1];
        uint32_t thumbbit = (uint32_t)params[2];
        uint32_t addr = this->get_pc();
        // save the current pointer in the register
        this->regs[REG_LR] = addr;
        // offset + (h bit) + (thumb bit)
        addr += offset | hbit | thumbbit;
        this->bx((void**)&addr);
    }
    INSNHDLR_ARM(blx);
    INSNHDLR_THUMB(blx);

    /** CPS instruction handler
     * @param[in] params Parsed parameters
     */
    void
    cps(void* params[])
    {
        // params[0] = CPSR mask
        // params[1] = CPSR value
        uint32_t mask = (uint32_t)params[0];
        uint32_t val = (uint32_t)params[1];
        // cps (privileged)
        if (IS_USER())
            return;
        this->set_psr(mask, 0, val);
    }
    INSNHDLR_ARM(cps);
    INSNHDLR_THUMB(cps);

    /** Decode an ARM instruction
     * @param hdlr Instruction handler to fill for execution
     */
    virtual void
    decode_arm(struct InsnHandler* hdlr)
    {
        uint32_t cond, rd, rn, rm, rs, sh, val, shift, tmp, tmp2, i, op1;
        uint64_t tmp64;

        ARM32_TLM_DBG(2, "%s 0x%08X", __func__, this->insn);

        // by default, move to the next instruction
        this->set_pc(this->get_pc() + 4);

        // retrieve the condition flags
        cond = insn >> 28;

        // check if it is an unconditional instruction
        if (unlikely(cond == 0xf))
        {
            // Unconditional instructions
            if (((insn >> 25) & 7) == 1) {
                // NEON Data processing
                ARCH(NEON);
                // not implemented yet
                assert(0);
                //if (disas_neon_data_insn(env, s, insn))
                //    goto illegal_op;
                return;
            }
            if ((insn & 0x0f100000) == 0x04000000) {
                // NEON load/store
                ARCH(NEON);
                // not implemented yet
                assert(0);
                //if (disas_neon_ls_insn(env, s, insn))
                //    goto illegal_op;
                return;
            }
            if ((insn & 0x0d70f000) == 0x0550f000) {
                // insn PLD
                hdlr->fn = HDLR_FN(Arm32::pld_arm);
                return;
            }
            else if ((insn & 0x0ffffdff) == 0x01010000) {
                // insn SETEND
                // params[0] = endianstate
                ARCH(6);
                hdlr->fn = HDLR_FN(Arm32::setend_arm);
                hdlr->params[0] = HDLR_PARAM(insn & (1 << 9));
                return;
            } else if ((insn & 0x0fffff00) == 0x057ff000) {
                switch ((insn >> 4) & 0xf) {
                case 1:
                    // insn CLREX
                    ARCH(6K);
                    // not implemented yet
                    assert(0);
                    return;
                case 4: // dsb
                case 5: // dmb
                case 6: // isb
                    ARCH(7);
                    // not implemented yet
                    assert(0);
                    return;
                default:
                    UNSUPPORTED();
                }
            } else if ((insn & 0x0e5fffe0) == 0x084d0500) {
                // insn SRS
                // params[0] = mode
                // params[1] = offset
                // params[2] = writeback
                // params[3] = wb_offset (only if writeback is set)
                int32_t offset;
                ARCH(6);
                hdlr->fn = HDLR_FN(Arm32::srs_arm);
                hdlr->params[0] = HDLR_PARAM(insn & 0x1f);
                i = (insn >> 23) & 3;
                switch (i) {
                case 0: offset = -4; break; // DA
                case 1: offset = 0; break; // IA
                case 2: offset = -8; break; // DB
                case 3: offset = 4; break; // IB
                default: assert(0);
                }
                hdlr->params[1] = HDLR_PARAM(offset);
                hdlr->params[2] = HDLR_PARAM(insn & (1 << 21));
                if (insn & (1 << 21)) {
                    // Base writeback
                    switch (i) {
                    case 0: offset = -8; break;
                    case 1: offset = 4; break;
                    case 2: offset = -4; break;
                    case 3: offset = 0; break;
                    default: assert(0);
                    }
                    hdlr->params[3] = HDLR_PARAM(offset);
                }
                return;
            } else if ((insn & 0x0e50ffe0) == 0x08100a00) {
                // insn RFE
                // params[0] = Rn
                // params[1] = offset
                // params[2] = writeback
                // params[3] = wb_offset (only if writeback is set)
                int32_t offset;
                ARCH(6);
                hdlr->fn = HDLR_FN(Arm32::rfe_arm);
                hdlr->params[0] = HDLR_PARAM((insn >> 16) & 0xf);
                i = (insn >> 23) & 3;
                switch (i) {
                case 0: offset = -4; break; // DA
                case 1: offset = 0; break; // IA
                case 2: offset = -8; break; // DB
                case 3: offset = 4; break; // IB
                default: assert(0);
                }
                hdlr->params[1] = HDLR_PARAM(offset);
                hdlr->params[2] = HDLR_PARAM(insn & (1 << 21));
                if (insn & (1 << 21)) {
                    // Base writeback
                    switch (i) {
                    case 0: offset = -8; break;
                    case 1: offset = 4; break;
                    case 2: offset = -4; break;
                    case 3: offset = 0; break;
                    default: abort();
                    }
                    hdlr->params[3] = HDLR_PARAM(offset);
                }
                return;
            } else if ((insn & 0x0e000000) == 0x0a000000) {
                // insn BLX <offset>
                // params[0] = sign extended offset (4 aligned)
                // params[1] = bit1 (2 aligned, hbit)
                // params[2] = bit0 (1 aligned, thumbbit)
                int32_t offset;
                hdlr->fn = HDLR_FN(Arm32::blx_arm);
                offset = (((int32_t)insn) << 8) >> 6;
                // pipeline offset
                offset += 4;
                hdlr->params[0] = HDLR_PARAM(offset);
                hdlr->params[1] = HDLR_PARAM((insn >> 23) & 2);
                // from ARM switch to thumb)
                hdlr->params[2] = HDLR_PARAM(1);
                return;
            } else if ((insn & 0x0e000f00) == 0x0c000100) {
                ARCH(IWMMXT);
                // not implemented yet
                assert(0);
                // iWMMXt register transfer
                //if (env->cp15.c15_cpar & (1 << 1))
                //    if (!disas_iwmmxt_insn(env, s, insn))
                //        return;
            } else if ((insn & 0x0fe00000) == 0x0c400000) {
                // insn MCRR/MCRR2
                // Coprocessor double register transfer
                // not implemented yet
                assert(0);
            } else if ((insn & 0x0f000010) == 0x0e000010) {
                //
                // Additional coprocessor register transfer
                // not implemented yet
                assert(0);
            } else if ((insn & 0x0ff10020) == 0x01000000) {
                // insn CPS
                // params[0] = CPSR mask
                // params[1] = CPSR value
                uint32_t mask;
                uint32_t val;
                mask = val = 0;
                if (insn & (1 << 19)) {
                    if (insn & (1 << 8))
                        mask |= CPSR_A;
                    if (insn & (1 << 7))
                        mask |= CPSR_I;
                    if (insn & (1 << 6))
                        mask |= CPSR_F;
                    if (insn & (1 << 18))
                        val = mask;
                }
                if (insn & (1 << 17)) {
                    mask |= CPSR_M;
                    val |= (insn & 0x1f);
                }
                // if there is no mask, just NOP
                if (likely(mask != 0)) {
                    hdlr->fn = HDLR_FN(Arm32::cps_arm);
                    hdlr->params[0] = HDLR_PARAM(mask);
                    hdlr->params[1] = HDLR_PARAM(val);
                }
                else {
                    hdlr->fn = HDLR_FN(Arm32::nop_arm);
                }
                return;
            }
            UNSUPPORTED();
        }

        // if it is not AL condition code, check the executability of the operation
        if (cond != 0xe)
        {
            if (!test_cc(cond))
            {
                ARM32_TLM_DBG(2, "conditional execution failed 0x%02X", cond);
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
                    // insn MOVW
                    tmp = val;
                } else {
                    // insn MOVT
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
                    this->arm_nop_hint();
                    return;
                } else {
                    // insn MSR <immediate>
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
        else if ((insn & 0x0f900000) == 0x01000000
                       && (insn & 0x00000090) != 0x00000090) {
            // miscellaneous instructions
            op1 = (insn >> 21) & 3;
            sh = (insn >> 4) & 0xf;
            rm = insn & 0xf;
            switch (sh) {
            case 0x0:
                if (op1 & 1) {
                    // insn MSR <register>
                    tmp = this->regs[rm];
                    // check if SPSR access is set
                    i = ((op1 & 2) != 0);
                    if (this->set_psr(this->msr_mask((insn >> 16) & 0xf, i), i, tmp))
                    {
                        this->exception |= EXCEPT_ILLEGAL_OP;
                    }
                    return;
                } else {
                    // insn MRS <register>
                    rd = (insn >> 12) & 0xf;
                    // check if SPSR access is set
                    if (op1 & 2) {
                        if (unlikely(IS_USER()))
                        {
                            this->exception |= EXCEPT_ILLEGAL_OP;
                            return;
                        }
                        tmp = this->regs[REG_SPSR];
                    } else {
                        tmp = this->get_cpsr();
                    }
                    this->regs[rd] = tmp;
                }
                break;
            case 0x1:
                if (op1 == 1) {
                    // insn BX <register>
                    tmp = this->regs[rm];
                    this->bx((void**)&tmp);
                } else if (op1 == 3) {
                    // insn CLZ
                    rd = (insn >> 12) & 0xf;
                    tmp = this->regs[rm];
                    tmp = this->clz(tmp);
                    this->regs[rd] = tmp;
                } else {
                    this->exception |= EXCEPT_ILLEGAL_OP;
                    return;
                }
                break;
            case 0x2:
                if (op1 == 1) {
                    // insn BXJ <register>
                    ARCH(5TEJ);
                    // trivial implementation equivalent to bx
                    tmp = this->regs[rm];
                    this->bx((void**)&tmp);
                } else {
                    this->exception |= EXCEPT_ILLEGAL_OP;
                    return;
                }
                break;
            case 0x3:
                if (unlikely(op1 != 1))
                {
                    this->exception |= EXCEPT_ILLEGAL_OP;
                    return;
                }
                // insn BLX <register>
                tmp = this->regs[rm];
                tmp2 = this->get_pc();
                this->regs[REG_LR] = tmp2;
                this->bx((void**)&tmp);
                break;
            case 0x5:
                // insn QADD, QDADD, QSUB and QDSUB
                rd = (insn >> 12) & 0xf;
                rn = (insn >> 16) & 0xf;
                tmp = this->regs[rm];
                tmp2 = this->regs[rn];
                // check if the register value must be doubled
                if (op1 & 2)
                {
                    // check if the sign bit is set
                    if ((int32_t)tmp2 > 0x40000000)
                    {
                        // TODO: this was copied from QEMU but not sure about reason
                        tmp2 = ~0x80000000;
                        this->QF = 1;
                    }
                    else if ((int32_t)tmp2 < (int32_t)0xC0000000)
                    {
                        // TODO: this was copied from QEMU but not sure about reason
                        tmp2 = 0x80000000;
                        this->QF = 1;
                    }
                    else
                        tmp2 = tmp2 * 2;
                }
                if (op1 & 1)
                {
                    uint32_t res = tmp - tmp2;
                    if (((res ^ tmp) & SIGNBIT) && ((tmp ^ tmp2) & SIGNBIT)) {
                        this->QF = 1;
                        res = ~(((int32_t)tmp >> 31) ^ SIGNBIT);
                    }
                    tmp = res;
                }
                else
                {
                    uint32_t res = tmp + tmp2;
                    if (((res ^ tmp) & SIGNBIT) && !((tmp ^ tmp2) & SIGNBIT)) {
                        this->QF = 1;
                        res = ~(((int32_t)tmp >> 31) ^ SIGNBIT);
                    }
                    tmp = res;
                }
                this->regs[rd] = tmp;
                break;
            case 7:
                // insn BKPT
                // not implemented for the time being
                assert(0);
                break;
            case 0x8: // SMULBB(op1=3), SMLABB(op1=0), SMLALBB(op1=2), SMLAWB(op1=1)
            case 0xa: // SMULTB(op1=3), SMLATB(op1=0), SMLALTB(op1=2), SMULWB(op1=1)
            case 0xc: // SMULBT(op1=3), SMLABT(op1=0), SMLALBT(op1=2), SMLAWT(op1=1)
            case 0xe: // SMULTT(op1=3), SMLATT(op1=0), SMLALTT(op1=2), SMULWT(op1=1)
                rs = (insn >> 8) & 0xf;
                rn = (insn >> 12) & 0xf;
                rd = (insn >> 16) & 0xf;
                if (op1 == 1) {
                    // insn SM(LA|UL)W(B|T)
                    // (32 * 16) >> 16
                    tmp = this->regs[rm];
                    tmp2 = this->regs[rs];
                    // (B|T)
                    if (sh & 4)
                        // arithmetic shift right (extend sign bit)
                        tmp2 = this->sra(tmp2, 16);
                    else
                        // extend sign bit to the upper 16 bits
                        tmp2 = this->sexth2l(tmp2);

                    // perform multiplication on 64 bit and shift
                    tmp = (uint32_t)(((int64_t)((int32_t)tmp*(int32_t)tmp2)) >> 16);
                    // (LA|UL)
                    if ((sh & 2) == 0) {
                        tmp2 = this->regs[rn];
                        tmp = this->add_setq(tmp, tmp2);
                    }
                    this->regs[rd] = tmp;
                } else {
                    // 16 * 16
                    tmp = this->regs[rm];
                    tmp2 = this->regs[rs];

                    // (B|T) for N (here rm)
                    if (sh & 2)
                        // arithmetic shift right (extend sign bit)
                        tmp = this->sra(tmp, 16);
                    else
                        // extend sign bit to the upper 16 bits
                        tmp = this->sexth2l(tmp);

                    // (B|T) for M (here rs)
                    if (sh & 4)
                        // arithmetic shift right (extend sign bit)
                        tmp2 = this->sra(tmp2, 16);
                    else
                        // extend sign bit to the upper 16 bits
                        tmp2 = this->sexth2l(tmp2);

                    // perform multiplication
                    tmp = (uint32_t)((int32_t)tmp * (int32_t)tmp2);
                    if (op1 == 2) {
                        // SMLAL**
                        tmp64 = this->sextl2q(tmp);
                        tmp64 += ((uint64_t)this->regs[rd] << 32) | this->regs[rn];
                        this->regs[rn] = (uint32_t)tmp64;
                        this->regs[rd] = (uint32_t)(tmp64>>32);
                    } else {
                        if (op1 == 0) {
                            // SMLA**
                            tmp2 = this->regs[rn];
                            tmp = this->add_setq(tmp, tmp2);
                        }
                        this->regs[rd] = tmp;
                    }
                }
                break;
            default:
                this->exception |= EXCEPT_ILLEGAL_OP;
                return;
            }
        }
    }

    /** Decode a THUMB instruction
     * @param hdlr Instruction handler to fill for execution
     */
    virtual void
    decode_thumb(struct InsnHandler* hdlr)
    {
        ARM32_TLM_DBG(2, "%s 0x%08X", __func__, this->insn);

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
    uint32_t banked_regs[MODE_SYSTEM-MODE_USR+1][REG_NUM];
};
