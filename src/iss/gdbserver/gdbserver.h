/** @file gdbserver.h
 * @brief GDB server class definition
 *
 * This file contains the GDB server class definition.
 */

#ifndef GDBSERVER_H_
#define GDBSERVER_H_

#include <iostream>
#include <stdint.h>
#include "signal.h"

struct gdbserver
{
    /// Define the syscall callback type
    typedef void (*syscall_complete_cb)(int, int);

    /// Constructor
    gdbserver(): m_serverfd(-1)
    {

    }

    /** Configure GDB server to support syscalls
     * If gdb is connected when the first semihosting syscall occurs then use
     * remote gdb syscalls.  Otherwise use native file IO.
     * @return true if syscalls can be supported, false otherwise
     */
    bool use_gdb_syscalls();

    /** Start the GDB server
     * This function waits for the GDB connection to the server before returning.
     * @param port Indicates the TCP port on which the server must wait
     * @param enabled Indicates if the GDBSERVER must be enabled
     * @param blocking Indicates if the socket must be configured in blocking mode
     */
    void start(int port, bool enabled, bool blocking);

    /** Tell the remote GDB that the process has exited
     * @param code Exit code
     */
    void end(int code);

    /** Check if there was a remote GDB connection request
     * This function checks if a remote GDB has sent a connection request.
     * @param[in] forced Forces the client check even if gdbserver was not enabled
     * @return true if connection request was received and accepted, false otherwise
     */
    bool checkremote(bool forced);

    /** Handle a signal from the ISS
     * This function sends the signaling packet to the remote GDB.
     * @param sig Signal id to send to the remote GDB (0 means stop until continue from gdb)
     * @return 0 if OK, different than 0 if error
     */
    int handlesig(int sig);

    /** Check if there was a stop request
     * This function checks if the remote GDB has sent a stop request.
     * @return true if stop request was received, false otherwise
     */
    bool checkctrlc();

    /** Send a gdb syscall request
     * This function accepts limited printf-style format specifiers, specifically:
     *  - %x  - target_ulong argument printed in hex.
     *  - %lx - 64-bit argument printed in hex.
     *  - %s  - string pointer (target_ulong) and length (int) pair.
     * @param cb Pointer to the callback
     * @param fmt Format of the string
     * @param ... Arguments to the format
     */
    void do_syscall(syscall_complete_cb cb, char *fmt, ...);

    /** Indicate to the ISS that the next run cycle has to last only one instruction
     * @param yesno If 0, step mode disabled, otherwise enabled
     */
    virtual void gdb_single_step(int yesno)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
    }

    /** Force a new program counter in the ISS
     * @param pc The program counter to force in the ISS
     */
    virtual void gdb_set_pc(uint64_t pc)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
    }

    /** Request the ISS to read the CPU registers and fill the buffer
     * @param[out] mem_buf Destination buffer for the registers content
     * @return Number of bytes written in the buffer
     * @warning There is no check for the buffer boundaries.  It is expected to be
     * large enough to hold all the registers.
     */
    virtual int gdb_read_registers(uint8_t *mem_buf)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return 0;
    }

    /** Write the CPU registers
     * @param[in] mem_buf Buffer containing the registers content
     * @param[in] size Size of the buffer
     */
    virtual void gdb_write_registers(uint8_t *mem_buf, int size)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
    }


    /** Add a new breakpoint in the ISS
     * @param addr Address at which the ISS must stop
     * @return The breakpoint index or -1 if it failed adding the breakpoint
     */
    virtual int gdb_breakpoint_insert(uint64_t addr)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return -1;
    }

    /** Remove a breakpoint from the ISS
     * @param addr Breakpoint address which must be removed
     */
    virtual void gdb_breakpoint_remove(uint64_t addr)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
    }

    /** Add a new watchpoint in the ISS
     * @param addr Address at which the ISS must stop
     * @return The watchpoint index or -1 if it failed adding the breakpoint
     */
    virtual int gdb_watchpoint_insert(uint64_t addr)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return -1;
    }

    /** Remove a watchpoint from the ISS
     * @param addr Watchpoint address which must be removed
     */
    virtual void gdb_watchpoint_remove(uint64_t addr)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
    }

    /** Read a word from the ISS into the system without affecting the platform timing
     * @param[in] addr Address to read
     * @param[out] dataptr Read data
     * @param[in] len Length of the data to read
     * @return -1 if error, otherwise number of bytes read
     */
    virtual int gdb_read(uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return -1;
    }

    /** Write a word from the ISS into the system without affecting the platform timing
     * @param[in] addr Address to write
     * @param[in] dataptr Data to write
     * @param[in] len Length of the data to write
     * @return -1 if error, otherwise number of bytes written
     */
    virtual int gdb_write(uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return -1;
    }


private:
    enum RSState {
        RS_IDLE,
        RS_GETLINE,
        RS_CHKSUM1,
        RS_CHKSUM2,
        RS_SYSCALL,
    };

    /** Handle a packet received from the remote GDB
     * @param[in] line_buf Input buffer
     */
    RSState handle_packet(const char *line_buf);

    /// Open a socket locally
    int open();

    /// Get a char from the remote GDB connection
    int get_char();

    /** Send buffer to remote GDB
     * @param buf Buffer to send to remove GDB
     * @param len Length of the buffer
     */
    void put_buffer(const uint8_t *buf, int len);

    /** Send packet to remote GDB
     * @param buf Packet to send to remove GDB
     * @return 0 if OK, different than 0 if error
     */
    int put_packet(char *buf);

    /** Handle the read byte from the remote GDB connection
     * @param[in] ch Character read from the remove GDB connection
     */
    void read_byte(int ch);

    /** Read or write the platform memory
     * @param[in] addr Address to read / write
     * @param[in, out] buf Buffer to read to / write from
     * @param[in] len Length to read / write
     * @return false if OK, true otherwise
     */
    bool cpu_gdb_rw_memory(uint32_t addr, uint8_t* buf, uint32_t len, uint8_t rw);

    /// Remote GDB received char parsing state
    enum RSState m_state;
    /// Packet buffer
    char m_line_buf[4096];
    int m_line_buf_index;
    int m_line_csum;
    uint8_t m_last_packet[4100];
    int m_last_packet_len;
    int m_clientfd;
    int m_running_state;
    int m_serverfd;
    int port;
    bool m_enabled;
    bool m_blocking;

    /// Semihosting callback
    syscall_complete_cb m_current_syscall_cb;

};

#endif /*GDB_H_*/

