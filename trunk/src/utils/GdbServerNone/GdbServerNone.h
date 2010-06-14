#ifndef GDBSERVERNONE_H_
#define GDBSERVERNONE_H_
#include <stdint.h>

#include "Parameters.h"

struct GdbServerNone
{
    /// Define the interface from the remote GDB commands to the ISS
    struct GdbCb
    {
        /// GDB command to set a new PC
        void (*gdb_set_pc_cb)(void *obj, uint64_t pc);
        /// GDB command to read the registers of the ISS
        int (*gdb_read_registers_cb)(void *obj, uint8_t *mem_buf);
        /// GDB command to write the registers of the ISS
        void (*gdb_write_registers_cb)(void *obj, uint8_t *mem_buf, int size);
        /// GDB command to read the CPU memory
        int (*gdb_rd_cb)(void *obj, uint64_t addr, uint8_t* dataptr, uint32_t len);
        /// GDB command to write into the CPU memory
        int (*gdb_wr_cb)(void *obj, uint64_t addr, uint8_t* dataptr, uint32_t len);
    };

    /** Constructor
     * @param[in] parameters Command line parameters
     * @param[in] config Parameters of the current block (and sub-blocks)
     */
    GdbServerNone(Parameters& parameters, MSP& config)
    {
        // get the gdb wait indication from the command line parameters
        this->gdb_wait = parameters.gdb_wait.get_bool();

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
    virtual void start()
    {
    }

    /** Tell the remote GDB that the process has exited
     * @param code Exit code
     */
    virtual void end(int code)
    {
    }

    /** Debugger call right after instruction fetched and before decoded
     * This function enables the debugger to check for a breakpoint reached, or for an
     * incoming connection request.
     * @return True if the programs should resume normally, false if the just fetched instruction
     * should not be executed
     */
    virtual bool
    after_ins_fetch(void)
    {
        return true;
    }

    /** Handle a signal from the ISS
     * This function sends the signaling packet to the remote GDB.
     * @param sig Signal id to send to the remote GDB (0 means stop until continue from gdb)
     * @return 0 if OK, different than 0 if error
     */
    virtual int handlesig(int sig)
    {
        return 0;
    }

protected:
    // structure containing the callbacks for the gdb server interaction with ISS
    struct GdbCb callbacks;
    /// Indicates that when starting, wait for the GDB connection
    bool gdb_wait;
};

#endif /*GDBSERVERNONE_H_*/

