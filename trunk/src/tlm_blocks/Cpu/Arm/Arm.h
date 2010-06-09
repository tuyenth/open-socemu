#ifndef ARM_H_
#define ARM_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

// main derived class
#include "SimpleMaster.h"

/// debug level
#define ARM_DEBUG_LEVEL 0

/// Macro to print debug messages.
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define ARM_TLM_DBG(__l, __f, ...)                                                      \
    do {                                                                                \
        if (ARM_DEBUG_LEVEL >= __l) {                                                   \
            TLM_DBG(__f, __VA_ARGS__);                                                  \
        }                                                                               \
    } while (false)

#if 0
struct Arm : SimpleMaster
{
    /// Socket to receive IRQ set and clear commands
    tlm_utils::simple_target_socket<Arm> irq_s_socket;
    /// Socket to receive FIQ set and clear commands
    tlm_utils::simple_target_socket<Arm> fiq_s_socket;

    // Module has a thread
    SC_HAS_PROCESS(Arm);

    /** Arm constructor
     * @param[in] name Name of the module
     * @param[in] gdbserver Specifies if the GDB remote connection must be supported
     * @param[in] gdbstart Specifies if ISS must wait for GDB remote connection before starting
     */
    Arm(sc_core::sc_module_name name, bool gdbserver, bool gdbstart)
    : SimpleMaster(name)
    , irq_s_socket("irq_s_socket")
    , fiq_s_socket("fiq_s_socket")
    {
        // only blocking calls supported by IRQ and FIQ sockets
        irq_s_socket.register_b_transport(this, &Arm::irq_s_b_transport);
        fiq_s_socket.register_b_transport(this, &Arm::fiq_s_b_transport);

        SC_THREAD(thread_process);
    }

    /// Main module thread
    void
    thread_process();

    /// IRQ blocking socket call handler.
    virtual void
    irq_s_b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);

    /// FIQ blocking socket call handler.
    virtual void
    fiq_s_b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);

    /** Function to read an 32 bit instruction from the system, going through timing process
     * @param[in] addr Address to read the instruction at
     * @return The value read
     */
    virtual uint32_t
    rd_i_l(uint32_t addr)
    {
        uint32_t data;

        ARM_TLM_DBG(3, "rd instruction L addr=0x%08X", addr);

        if (unlikely(addr & 3))
        {
            uint32_t datal, datah, addrm;

            addrm = addr & (~3);
            TLM_B_RD_WORD(master_socket, master_nb_pl, master_nb_delay, addrm, datal);
            TLM_B_RD_WORD(master_socket, master_nb_pl, master_nb_delay, addrm + 4, datah);
            data = (datah << 16) | (datal >> 16);
        }
        else
        {
            TLM_B_RD_WORD(master_socket, master_nb_pl, master_nb_delay, addr, data);
        }

        ARM_TLM_DBG(2, "rd instruction L addr=0x%08X data=0x%08X", addr, data);
        return data;
    }

    /** Function to read an 16 bit instruction from the system, going through timing process
     * @param[in] addr Address to read
     * @return The value read
     */
    virtual uint32_t
    rd_i_s(uint32_t addr)
    {
        uint32_t data;

        ARM_TLM_DBG(3, "rd instruction H addr=0x%08X", addr);

        // read the word at the word aligned address
        TLM_B_RD_WORD(master_socket, master_nb_pl, master_nb_delay, addr & (~3), data);

        // check the alignment
        if (addr & 2)
        {
            data = data >> 16;
        }
        data &= 0x0000FFFF;

        ARM_TLM_DBG(2, "rd instruction H addr=0x%08X data=0x%08X", addr, data);
        return data;
    }

    /** Function to read a word from the system, going through timing process
     * @param[in] addr Address to read from
     * @return The value read
     */
    virtual uint32_t
    rd_l(uint32_t addr)
    {
        uint32_t data;

        ARM_TLM_DBG(3, "rd L addr=0x%08X", addr);

        TLM_B_RD_WORD(master_socket, master_nb_pl, master_nb_delay, addr, data);

        ARM_TLM_DBG(2, "rd L addr=0x%08X data=0x%08X", addr, data);
        return data;
    }

    /** Function to read a short from the system, going through timing process
     * @param[in] addr Address to read from
     * @return The value read
     */
    virtual uint32_t
    rd_s(uint32_t addr)
    {
        uint32_t data;
        TLM_B_RD_HALFWORD(master_socket, master_nb_pl, master_nb_delay, addr, data);

        ARM_TLM_DBG(2, "rd H addr=0x%08X data=0x%08X", addr, data);
        return data;
    }

    /** Function to read a byte from the system, going through timing process
     * @param[in] addr Address to read from
     * @return The value read
     */
    virtual uint32_t
    rd_b(uint32_t addr)
    {
        uint32_t data;
        TLM_B_RD_BYTE(master_socket, master_nb_pl, master_nb_delay, addr, data);

        ARM_TLM_DBG(2, "rd B addr=0x%08X data=0x%08X", addr, data);
        return data;
    }

    /** Function to write a long into the system, going through the timing process
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    virtual void
    wr_l(uint32_t addr, uint32_t data)
    {
        ARM_TLM_DBG(2, "wr W addr=0x%08X data=0x%08X", addr, data);

        TLM_B_WR_WORD(master_socket, master_nb_pl, master_nb_delay, addr, data);
    }

    /** Function to write a short into the system, going through the timing process
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    virtual void
    wr_s(uint32_t addr, uint32_t data)
    {
        ARM_TLM_DBG(2, "wr H addr=0x%08X data=0x%08X", addr, data);

        TLM_B_WR_HALFWORD(master_socket, master_nb_pl, master_nb_delay, addr, data);
    }

    /** Function to write a byte into the system, going through the timing process
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    virtual void
    wr_b(uint32_t addr, uint32_t data)
    {
        ARM_TLM_DBG(2, "wr B addr=0x%08X data=0x%08X", addr, data);

        TLM_B_WR_BYTE(master_socket, master_nb_pl, master_nb_delay, addr, data);
    }

    /** Function to make a debug read access into the system
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    int
    gdb_rd(uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        TLM_DBG_RD(master_socket, master_nb_pl, addr, dataptr, len);

        ARM_TLM_DBG(2, "rd D addr=0x%08llX n_bytes=%d", addr, n_bytes);

        return n_bytes;
    }

    /** Function to make a debug write access into the system
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    int
    gdb_wr(uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        TLM_DBG_WR(master_socket, master_nb_pl, addr, dataptr, len);

        ARM_TLM_DBG(2, "wr D addr=0x%08llX n_bytes=%d", addr, n_bytes);

        return n_bytes;
    }

    /** Function to wait for some ARM cycles
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
        ARM_TLM_DBG(1, "WFI: enter %d", 0);

#if 0
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
#endif
        ARM_TLM_DBG(1, "WFI: exit %d", 0);
    }


    /** Callback to read a word from the system, going through timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to read from
     * @return The value read
     */
    static uint32_t
    rd_l_cb(void *obj, uint32_t addr);

    /** Callback to read a short from the system, going through timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to read from
     * @return The value read
     */
    static uint32_t
    rd_s_cb(void *obj, uint32_t addr);

    /** Callback to read a byte from the system, going through timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to read from
     * @return The value read
     */
    static uint32_t
    rd_b_cb(void *obj, uint32_t addr);

    /** Callback to write a long into the system, going through the timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    static void
    wr_l_cb(void *obj, uint32_t addr, uint32_t data);

    /** Callback to write a short into the system, going through the timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    static void
    wr_s_cb(void *obj, uint32_t addr, uint32_t data);

    /** Callback to write a byte into the system, going through the timing process
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to write to
     * @param[in] data Data to write
     */
    static void
    wr_b_cb(void *obj, uint32_t addr, uint32_t data);

    /** Callback to make a debug read access into the system
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to read at
     * @param[in, out] dataptr Pointer to the buffer to fill
     * @param[in] len Length of the data to read
     * @return The number of bytes actually read
     */
    static int
    gdb_rd_cb(void *obj, uint64_t addr, uint8_t* dataptr, uint32_t len);

    /** Callback to make a debug write access into the system
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to write at
     * @param[in] dataptr Pointer to the buffer to read from
     * @param[in] len Length of the data to write
     * @return The number of bytes actually written
     */
    static int
    gdb_wr_cb(void *obj, uint64_t addr, uint8_t* dataptr, uint32_t len);

protected:
    /** Load a register from the current mode bank
     * @param[in] reg Index of the register that must be returned
     * @return The value of the register
     */
    uint32_t load_reg(int reg)
    {
        if (reg == 15) {
            uint32_t addr;
            /* normally, since we updated PC, we need only to add one insn */
            if (this->thumb)
                addr = this->pc + 2;
            else
                addr = this->pc + 4;
            return addr;
        } else {
            return this->regs[reg];
        }
    }

    /// features supported by this core
    uint32_t features;

    /// Program Counter
    uint32_t pc;

    /// Processor is in thumb mode
    bool thumb;

private:
    /// Event used to wait for an interrupt
    sc_core::sc_event m_interrupt;
};
#endif


#endif /*ARM_H_*/
