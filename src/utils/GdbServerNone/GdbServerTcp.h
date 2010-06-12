#ifndef GDBSERVERTCP_H_
#define GDBSERVERTCP_H_
#include <stdint.h>

#include "GdbServerNone.h"

struct GdbServerTcp:GdbServerNone
{
    /// Constructor
    GdbServerTcp():GdbServerNone()
    {
    }

    /** Start the GDB server
     * This function waits for the GDB connection to the server before returning.
     * @param port Indicates the TCP port on which the server must wait
     * @param enabled Indicates if the GDBSERVER must be enabled
     * @param blocking Indicates if the socket must be configured in blocking mode
     */
    virtual void start(int port, bool enabled, bool blocking)
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

private:
    struct GdbCb callbacks;
};

#endif /*GDBSERVERTCP_H_*/

