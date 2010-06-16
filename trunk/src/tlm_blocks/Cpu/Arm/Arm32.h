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
    , thumb(false)
    {
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
        this->pc = 0;
    }

    /// Fetch the next instruction
    virtual void
    fetch_insn()
    {
        this->insn = rd_l_aligned(this->pc);
    }

    /// Execute a single instruction
    virtual void
    execute_insn()
    {
        // check the current execution mode
        if (this->thumb)
        {
            // try to execute a thumb operation
            if (unlikely(this->execute_thumb()))
            {
                this->exception |= 1;
            }
        }
        else
        {
            // try to execute a thumb operation
            if (unlikely(this->execute_arm()))
            {
                this->exception |= 1;
            }
        }
    }

protected:
    virtual bool
    execute_arm()
    {
        this->pc += 4;
        return false;
    }

    virtual bool
    execute_thumb()
    {
        return false;
    }
    /// Last fetched instruction content
    uint32_t insn;
    /// Current execution mode
    bool thumb;
    /// Exception
    uint32_t exception;

};
