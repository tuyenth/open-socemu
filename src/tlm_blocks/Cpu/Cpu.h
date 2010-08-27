#ifndef CPU_H_
#define CPU_H_

#include "BusMaster.h"
#include "IntSlave.h"

// command line and configuration parameters
#include "Parameters.h"

// other objects
#include "mmu.h"

/// debug level
#define CPU_DEBUG_LEVEL 0

/// Macro to print debug messages
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define CPU_TLM_DBG(__l, __f, ...)                                                      \
    do {                                                                                \
        if (CPU_DEBUG_LEVEL >= __l) {                                                   \
            TLM_DBG(__f, __VA_ARGS__);                                                  \
        }                                                                               \
    } while (false)

struct Cpu : BusMaster
{
    // Module has a thread
    SC_HAS_PROCESS(Cpu);

    /** Cpu constructor
     * @param[in] name Name of the module
     * @param[in] cpuname Specifies the CPU core to use
     * @param[in] parameters Command line parameters
     * @param[in] config Parameters of the current block (and sub-blocks)
     */
    Cpu(sc_core::sc_module_name name, const std::string& cpuname, Parameters& parameters, MSP& config);

    /// irq
    IntSlave<Cpu> irq;
    /// fiq
    IntSlave<Cpu> fiq;

    /// Main module thread
    void
    thread_process();

    /** Function to read a word from the system, going through timing process
     * @param[in] addr Address to read from
     * @return The value read
     */
    uint32_t
    rd_l(uint32_t addr)
    {
        uint32_t data;

        CPU_TLM_DBG(3, "rd L addr=0x%08X", addr);

        TLM_B_RD_WORD(master_socket, master_b_pl, master_b_delay, addr, data);

        CPU_TLM_DBG(2, "rd L addr=0x%08X data=0x%08X", addr, data);
        return data;
    }

    /** Function to read a short from the system, going through timing process
     * @param[in] addr Address to read from
     * @return The value read
     */
    uint32_t
    rd_s(uint32_t addr)
    {
        uint32_t data;
        TLM_B_RD_HALFWORD(master_socket, master_b_pl, master_b_delay, addr, data);

        CPU_TLM_DBG(2, "rd H addr=0x%08X data=0x%08X", addr, data);
        return data;
    }

    /** Function to read a byte from the system, going through timing process
     * @param[in] addr Address to read from
     * @return The value read
     */
    uint32_t
    rd_b(uint32_t addr)
    {
        uint32_t data;
        TLM_B_RD_BYTE(master_socket, master_b_pl, master_b_delay, addr, data);

        CPU_TLM_DBG(2, "rd B addr=0x%08X data=0x%08X", addr, data);
        return data;
    }

    /** Function to write a long into the system, going through the timing process
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    void
    wr_l(uint32_t addr, uint32_t data)
    {
        CPU_TLM_DBG(2, "wr W addr=0x%08X data=0x%08X", addr, data);

        TLM_B_WR_WORD(master_socket, master_b_pl, master_b_delay, addr, data);
    }

    /** Function to write a short into the system, going through the timing process
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    void
    wr_s(uint32_t addr, uint32_t data)
    {
        CPU_TLM_DBG(2, "wr H addr=0x%08X data=0x%08X", addr, data);

        TLM_B_WR_HALFWORD(master_socket, master_b_pl, master_b_delay, addr, data);
    }

    /** Function to write a byte into the system, going through the timing process
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    void
    wr_b(uint32_t addr, uint32_t data)
    {
        CPU_TLM_DBG(2, "wr B addr=0x%08X data=0x%08X", addr, data);

        TLM_B_WR_BYTE(master_socket, master_b_pl, master_b_delay, addr, data);
    }

    /** Function to make a debug read access into the system
     * @param[in] addr Address to write to
     * @param[in] dataptr Data to write
     */
    int
    gdb_rd(uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        TLM_DBG_RD(master_socket, master_b_pl, addr, dataptr, len);

        CPU_TLM_DBG(2, "rd D addr=0x%08llX n_bytes=%d", addr, n_bytes);

        return n_bytes;
    }

    /** Function to make a debug write access into the system
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    int
    gdb_wr(uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        TLM_DBG_WR(master_socket, master_b_pl, addr, dataptr, len);

        CPU_TLM_DBG(2, "wr D addr=0x%08llX n_bytes=%d", addr, n_bytes);

        return n_bytes;
    }

    /** Function to wait for some CPU cycles
     * @param[in] cycles Number of cycles to wait for
     */
    void
    exec_cycles(int cycles)
    {
        // this function is called after every instruction execution

        // for example, it is possible to wait for the time to execute
        // sc_core::wait(100*cycles, sc_core::SC_NS);
        return;
    }

    /// Wait for an interrupt to happen
    void
    wfi(void)
    {
        CPU_TLM_DBG(1, "WFI: enter %d", 0);

        // check if neither the IRQ nor the FIQ is asserted
        while ((!this->m_arm->irq_get()) && (!this->m_arm->fiq_get()))
        {
            // wait for an interrupt (timeout to poll on debugger activity)
            sc_core::wait(1, sc_core::SC_MS, m_interrupt);

            // check if there is a remote connection (or a request)
            if (this->m_arm->checkremote(true))
            {
                // check if ctrl-c (stop request) was received
                if (this->m_arm->checkctrlc())
                {
                    // send signal to debugger
                    this->m_arm->handlesig(SIGTRAP);
                }
            }
        }
        CPU_TLM_DBG(1, "WFI: exit %d", 0);
    }

    /** Callback to read a word from the system, going through timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to read from
     * @return The value read
     */
    static uint32_t
    rd_l_cb(void* obj, uint32_t addr);

    /** Callback to read a short from the system, going through timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to read from
     * @return The value read
     */
    static uint32_t
    rd_s_cb(void* obj, uint32_t addr);

    /** Callback to read a byte from the system, going through timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to read from
     * @return The value read
     */
    static uint32_t
    rd_b_cb(void* obj, uint32_t addr);

    /** Callback to write a long into the system, going through the timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    static void
    wr_l_cb(void* obj, uint32_t addr, uint32_t data);

    /** Callback to write a short into the system, going through the timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    static void
    wr_s_cb(void* obj, uint32_t addr, uint32_t data);

    /** Callback to write a byte into the system, going through the timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    static void
    wr_b_cb(void* obj, uint32_t addr, uint32_t data);

    /** Callback to make a debug read access into the system
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to read at
     * @param[in, out] dataptr Pointer to the buffer to fill
     * @param[in] len Length of the data to read
     * @return The number of bytes actually read
     */
    static int
    gdb_rd_cb(void* obj, uint64_t addr, uint8_t* dataptr, uint32_t len);

    /** Callback to make a debug write access into the system
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to write at
     * @param[in] dataptr Pointer to the buffer to read from
     * @param[in] len Length of the data to write
     * @return The number of bytes actually written
     */
    static int
    gdb_wr_cb(void* obj, uint64_t addr, uint8_t* dataptr, uint32_t len);

    /** Callback to wait for some CPU cycles
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] cycles Number of cycles to wait for
     */
    static void
    exec_cycles_cb(void* obj, int cycles);

    /** Callback to wait for an interrupt to happen
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     */
    static void
    wfi_cb(void* obj);

private:
    /// ELF file name and path
    std::string* m_elfpath;

    /// MMU class (can be any kind of ARM)
    struct mmu* m_arm;

    /// Event used to wait for an interrupt
    sc_core::sc_event m_interrupt;

    /** Interrupt set handler
     * @param[in] opaque Pointer passed in parameter when registering
     */
    void
    interrupt_set(void* opaque);

    /** Interrupt clear handler
     * @param[in] opaque Pointer passed in parameter when registering
     */
    void
    interrupt_clr(void* opaque);
};

#endif /*CPU_H_*/
