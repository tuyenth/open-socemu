#ifndef CPUBASE_H_
#define CPUBASE_H_

#include "SimpleMaster.h"

#include "Parameters.h"

/// debug level
#define CPUBASE_DEBUG 0

/// Macro to print debug messages
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define CPUBASE_TLM_DBG(__l, __f, ...)                                                  \
    do {                                                                                \
        if (CPUBASE_DEBUG >= __l) {                                                     \
            TLM_DBG(__f, __VA_ARGS__);                                                  \
        }                                                                               \
    } while (false)

/** Base CPU class, all other CPU definition should derive from this one
 * This base class derives the SimpleMaster because a CPU is mainly a main connection to the
 * system bus.  But it also is a template of a GDB connection.  The GDB typename should
 * derive GdbServerNone to make sure it implements all the required methods.  If
 * GdbServerNone is used as is, then no gdb server is running.
 */
template<typename GDB>
struct CpuBase: SimpleMaster
{
    // Module has a thread
    SC_HAS_PROCESS(CpuBase);

    /** CpuBase constructor
     * @param[in] name Name of the module
     * @param[in] parameters Command line parameters
     * @param[in] config Parameters of the current block (and sub-blocks)
     */
    CpuBase(sc_core::sc_module_name name, Parameters& parameters, MSP& config)
    : SimpleMaster(name)
    {
        Parameter* elffile;
        // structure whose definition belongs to a specific class
        struct GDB::GdbCb callbacks;


        // set the default callbacks
        callbacks.gdb_set_pc_cb = &(this->gdb_set_pc_cb);
        callbacks.gdb_read_registers_cb = &(this->gdb_read_registers_cb);
        callbacks.gdb_write_registers_cb = &(this->gdb_write_registers_cb);
        callbacks.gdb_rd_cb = &(this->gdb_rd_cb);
        callbacks.gdb_wr_cb = &(this->gdb_wr_cb);
        gdbserver.set_callbacks(callbacks);

        // check if there is an elf file defined
        if (config.count("elffile") == 0)
        {
            this->elfpath = NULL;
        }
        else
        {
            elffile = config["elffile"];
            elffile->add_path(parameters.configpath);
            // copy the path into an internal variable
            this->elfpath = elffile->get_string();
        }

        // create the module thread
        SC_THREAD(thread_process);
    }

    /// Main module thread, runs the CPU startup and instruction loop
    virtual void
    thread_process()
    {
        // wait forever
        sc_core::wait();
    }

    /** Function to read an 32 bit word from the system, going through timing process
     * @param[in] addr Address to read the instruction at
     * @return The value read
     */
    virtual uint32_t
    rd_l_unaligned(uint32_t addr)
    {
        uint32_t data;

        CPUBASE_TLM_DBG(3, "rd L unaligned addr=0x%08X", addr);

        // sanity check: byte aligned accesses not supported
        assert((addr & 1) == 0);

        // check if this is an unaligned access
        if (unlikely(addr & 2))
        {
            uint32_t datal, datah, addrm;

            addrm = addr & (~3);
            TLM_B_RD_WORD(master_socket, master_b_pl, master_b_delay, addrm, datal);
            TLM_B_RD_WORD(master_socket, master_b_pl, master_b_delay, addrm + 4, datah);
            data = (datah << 16) | (datal >> 16);
        }
        else
        {
            TLM_B_RD_WORD(master_socket, master_b_pl, master_b_delay, addr, data);
        }

        CPUBASE_TLM_DBG(2, "rd L unaligned addr=0x%08X data=0x%08X", addr, data);
        return data;
    }

    /** Function to read an 32 bit word from the system, going through timing process
     * @param[in] addr Address to read the instruction at
     * @return The value read
     */
    virtual uint32_t
    rd_l_aligned(uint32_t addr)
    {
        uint32_t data;

        CPUBASE_TLM_DBG(3, "rd L aligned addr=0x%08X", addr);

        // sanity check: byte aligned accesses not supported
        assert((addr & 3) == 0);
        TLM_B_RD_WORD(master_socket, master_b_pl, master_b_delay, addr, data);

        CPUBASE_TLM_DBG(2, "rd L aligned addr=0x%08X data=0x%08X", addr, data);
        return data;
    }

    /** Change the program counter location
     * @param[in] pc New program counter value to set
     */
    virtual void
    gdb_set_pc(uint64_t pc)
    {
        this->pc = (uint32_t)pc;
    }

    /** Callback to change the program counter location
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] pc New program counter value to set
     */
    static void
    gdb_set_pc_cb(void *obj, uint64_t pc)
    {
        struct CpuBase* myself = (struct CpuBase*)obj;
        return myself->gdb_set_pc(pc);
    }

    /** Read the CPU registers and fill the buffer
     * @param[out] mem_buf Destination buffer for the registers content
     * @return Number of bytes written in the buffer
     * @warning There is no check for the buffer boundaries.  It is expected to be
     * large enough to hold all the registers.
     */
    virtual int
    gdb_read_registers(uint8_t *mem_buf)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
        return 0;
    }

    /** Callback to read the CPU registers and fill the buffer
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[out] mem_buf Destination buffer for the registers content
     * @return Number of bytes written in the buffer
     * @warning There is no check for the buffer boundaries.  It is expected to be
     * large enough to hold all the registers.
     */
    static int
    gdb_read_registers_cb(void *obj, uint8_t *mem_buf)
    {
        struct CpuBase* myself = (struct CpuBase*)obj;
        return myself->gdb_read_registers(mem_buf);
    }

    /** Write the CPU registers
     * @param[in] mem_buf Buffer containing the registers content
     * @param[in] size Size of the buffer
     */
    virtual void
    gdb_write_registers(uint8_t *mem_buf, int size)
    {
        std::cout << "ERROR: virtual function '" << __FUNCTION__ << "' undefined" << std::endl;
    }

    /** Callback to write the CPU registers
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] mem_buf Buffer containing the registers content
     * @param[in] size Size of the buffer
     */
    static void
    gdb_write_registers_cb(void *obj, uint8_t *mem_buf, int size)
    {
        struct CpuBase* myself = (struct CpuBase*)obj;
        return myself->gdb_write_registers(mem_buf, size);
    }

    /** Make a debug read access into the system
     * @param[in] addr Address to read at
     * @param[in, out] dataptr Pointer to the buffer to fill
     * @param[in] len Length of the data to read
     * @return The number of bytes actually read
     */
    virtual int
    gdb_rd(uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        TLM_DBG_RD(master_socket, master_b_pl, addr, dataptr, len);

        CPUBASE_TLM_DBG(2, "rd D addr=0x%08llX n_bytes=%d", addr, n_bytes);

        return n_bytes;
    }

    /** Callback to make a debug read access into the system
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to read at
     * @param[in, out] dataptr Pointer to the buffer to fill
     * @param[in] len Length of the data to read
     * @return The number of bytes actually read
     */
    static int
    gdb_rd_cb(void *obj, uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        struct CpuBase* myself = (struct CpuBase*)obj;
        return myself->gdb_rd(addr, dataptr, len);
    }

    /** Make a debug write access into the system
     * @param[in] addr Address to write at
     * @param[in] dataptr Pointer to the buffer to read from
     * @param[in] len Length of the data to write
     * @return The number of bytes actually written
     */
    virtual int
    gdb_wr(uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        TLM_DBG_WR(master_socket, master_b_pl, addr, dataptr, len);

        CPUBASE_TLM_DBG(2, "wr D addr=0x%08llX n_bytes=%d", addr, n_bytes);

        return n_bytes;
    }

    /** Callback to make a debug write access into the system
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] addr Address to write at
     * @param[in] dataptr Pointer to the buffer to read from
     * @param[in] len Length of the data to write
     * @return The number of bytes actually written
     */
    static int
    gdb_wr_cb(void *obj, uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        struct CpuBase* myself = (struct CpuBase*)obj;
        return myself->gdb_wr(addr, dataptr, len);
    }

protected:
    /// GDB connection
    GDB gdbserver;

    /// Program Counter
    uint32_t pc;

    /// ELF file name and path
    std::string* elfpath;

};

#endif /* CPUBASE_H_ */
