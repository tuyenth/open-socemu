#ifndef GDBSERVERNONE_H_
#define GDBSERVERNONE_H_
#include <stdint.h>

#include "Parameters.h"

struct GdbServerNone
{
    /// Define the interface from the remote GDB commands to the ISS
    struct GdbCb
    {
        /// Instance reference to pass as a parameter to all the callbacks
        void* obj;
        /// GDB command to set a new PC
        void (*gdb_set_pc_cb)(void* obj, uint64_t pc);
        /// GDB command to read the registers of the ISS
        int (*gdb_rd_reg_cb)(void* obj, uint8_t* mem_buf);
        /// GDB command to write the registers of the ISS
        void (*gdb_wr_reg_cb)(void* obj, uint8_t* mem_buf, int size);
        /// GDB command to read the CPU memory
        int (*gdb_rd_mem_cb)(void* obj, uint64_t addr, uint8_t* dataptr, uint32_t len);
        /// GDB command to write into the CPU memory
        int (*gdb_wr_mem_cb)(void* obj, uint64_t addr, uint8_t* dataptr, uint32_t len);
    };

    /** Constructor
     * @param[in] parameters Command line parameters
     * @param[in] config Parameters of the current block (and sub-blocks)
     */
    GdbServerNone(Parameters& parameters, MSP& config)
    : singlestep(false)
    {
        // get the gdb wait indication from the command line parameters
        this->gdb_wait = parameters.gdb_wait.get_bool();
    }

    /// Destructor
    virtual ~GdbServerNone()
    {
    }


    /** Set the CB interface
     * @param[in] cb Reference to the callbacks from ISS
     */
    void
    set_callbacks(const struct GdbCb& cb)
    {
        this->callbacks = cb;
    }

    /// Start the GDB server
    virtual void
    start()
    {
    }

    /** Tell the remote GDB that the process has exited
     * @param code Exit code
     */
    virtual void
    end(int code)
    {
    }

    /** Debugger call right after instruction fetched and before decoded
     * This function enables the debugger to check for a breakpoint reached, or for an
     * incoming connection request.
     * @param[in] pc Current program counter
     * @return True if the programs should resume normally, false if the just fetched instruction
     * should not be executed
     */
    virtual bool
    before_exec_insn(uint64_t pc)
    {
        return true;
    }

    /** Set the single step mode in the running ISS
     * @param[in] single True if want to stop on next instruction, false otherwise
     */
    void
    set_singlestep(bool single)
    {
        this->singlestep = single;
    }

protected:
    /// Structure containing the callbacks for the gdb server interaction with ISS
    struct GdbCb callbacks;
    /// Indicates that when starting, wait for the GDB connection
    bool gdb_wait;
    /// Indicates the current step mode
    bool singlestep;
};

#endif /*GDBSERVERNONE_H_*/

