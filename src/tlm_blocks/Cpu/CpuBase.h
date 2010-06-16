#ifndef CPUBASE_H_
#define CPUBASE_H_

#include "SimpleMaster.h"

#include "ElfReader.h"
#include "Parameters.h"

/// debug level
#define CPUBASE_DEBUG_LEVEL 0

/// Macro to print debug messages
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define CPUBASE_TLM_DBG(__l, __f, ...)                                                  \
    do {                                                                                \
        if (CPUBASE_DEBUG_LEVEL >= __l) {                                               \
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
    /** CpuBase constructor
     * @param[in] name Name of the module
     * @param[in] parameters Command line parameters
     * @param[in, out] config Parameters of the current block (and sub-blocks)
     */
    CpuBase(sc_core::sc_module_name name, Parameters& parameters, MSP& config)
    : SimpleMaster(name)
    , gdbserver(parameters, config)
    {
        Parameter* elffile;
        // structure whose definition belongs to a specific class
        struct GDB::GdbCb callbacks;

        // set the gdbserver callbacks
        callbacks.obj = (void*)this;
        callbacks.gdb_set_pc_cb = &(this->gdb_set_pc_cb);
        callbacks.gdb_rd_reg_cb = &(this->gdb_rd_reg_cb);
        callbacks.gdb_wr_reg_cb = &(this->gdb_wr_reg_cb);
        callbacks.gdb_rd_mem_cb = &(this->gdb_rd_mem_cb);
        callbacks.gdb_wr_mem_cb = &(this->gdb_wr_mem_cb);
        gdbserver.set_callbacks(callbacks);

        // check if there is an elf file defined
        if (config.count("elffile") == 0)
        {
            CPUBASE_TLM_DBG(1, "No ELF file found %d", config.count("elffile"));
            this->elfpath = NULL;
        }
        else
        {
            elffile = config["elffile"];
            elffile->add_path(parameters.configpath);
            // copy the path into an internal variable
            this->elfpath = elffile->get_string();
            CPUBASE_TLM_DBG(1, "ELF file found %s", this->elfpath->c_str());
        }
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

    /// Load the ELF file using the debug transport from the CPU point of view
    virtual void
    load_elf(void)
    {
        // create an instance of ElfReader
        ElfReader ElfReader;
        // use a Segment pointer
        Segment* Segment;

        // check if there was an ELF file specified for this CPU
        if (this->elfpath != NULL)
        {
            // open the ELF file
            ElfReader.Open(this->elfpath->c_str());

            // loop on all the segments and copy the loadables in memory
            while ((Segment = ElfReader.GetNextSegment()) != NULL)
            {
                // do a debug write operation
                TLM_DBG_WR(this->master_socket, this->master_b_pl, Segment->Address(), Segment->Data(), Segment->Size());
            }
        }
    }

    /// Fetch the next instruction
    virtual void
    fetch_insn()
    {
    }

    /// Execute a single instruction
    virtual void
    execute_insn()
    {
        // increment the PC
        this->pc += 4;
    }

    /// Main module thread, runs the CPU startup and instruction loop
    virtual void
    thread_process(void)
    {
        // load the elf file
        this->load_elf();

        // reset the CPU
        this->reset();

        // start the GDB server
        this->gdbserver.start();

        while (true)
        {
            // fetch the next instruction
            this->fetch_insn();

            // check if there is a pending exception
            if (unlikely(this->handle_exception()))
            {
                continue;
            }

            // check if the debugger wants to halt and if it wants to execute the current instruction
            if (unlikely(!this->gdbserver.after_ins_fetch(this->pc)))
                continue;

            // execute the instruction fetched
            this->execute_insn();
        }
    }

    /** Function to read an 32 bit word from the system, going through timing process
     * @param[in] addr Address to read the instruction at
     * @return The value read
     */
    virtual uint32_t
    rd_l_unaligned(uint64_t addr)
    {
        uint32_t data;

        CPUBASE_TLM_DBG(3, "rd L unaligned addr=0x%08llX", addr);

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

        CPUBASE_TLM_DBG(2, "rd L unaligned addr=0x%08llX data=0x%08X", addr, data);
        return data;
    }

    /** Function to read an 32 bit word from the system, going through timing process
     * @param[in] addr Address to read the instruction at
     * @return The value read
     */
    virtual uint32_t
    rd_l_aligned(uint64_t addr)
    {
        uint32_t data;

        CPUBASE_TLM_DBG(3, "rd L aligned addr=0x%08llX", addr);

        // sanity check: byte aligned accesses not supported
        assert((addr & 3) == 0);
        TLM_B_RD_WORD(master_socket, master_b_pl, master_b_delay, addr, data);

        CPUBASE_TLM_DBG(2, "rd L aligned addr=0x%08llX data=0x%08X", addr, data);
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
    gdb_rd_reg(uint8_t *mem_buf)
    {
        int i;
        uint8_t *ptr;

        ptr = mem_buf;

        // 15 core integer registers (4 bytes each)
        for (i = 0; i < 15; i++)
        {
            *(uint32_t *)ptr = i;
            ptr += 4;
        }

        // R15 = Program Counter register
        *(uint32_t *)ptr = this->pc;
        ptr += 4;

        // 8 FPA registers (12 bytes each), FPS (4 bytes), not implemented
        memset (ptr, 0, (8 * 12) + 4);
        ptr += 8 * 12 + 4;
        /* CPSR (4 bytes).  */
        *(uint32_t *)ptr = 0x10;
        ptr += 4;

        // return the size of the buffer filled
        return ptr - mem_buf;
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
    gdb_rd_reg_cb(void *obj, uint8_t *mem_buf)
    {
        struct CpuBase* myself = (struct CpuBase*)obj;
        return myself->gdb_rd_reg(mem_buf);
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

    /** Callback to write the CPU registers
     * @warning This function is static because it is used as a callback
     * @param[in, out] obj Pointer to the instance to use
     * @param[in] mem_buf Buffer containing the registers content
     * @param[in] size Size of the buffer
     */
    static void
    gdb_wr_reg_cb(void *obj, uint8_t *mem_buf, int size)
    {
        struct CpuBase* myself = (struct CpuBase*)obj;
        return myself->gdb_wr_reg(mem_buf, size);
    }

    /** Make a debug read access into the system
     * @param[in] addr Address to read at
     * @param[in, out] dataptr Pointer to the buffer to fill
     * @param[in] len Length of the data to read
     * @return The number of bytes actually read (-1 in case of error)
     */
    virtual int
    gdb_rd_mem(uint64_t addr, uint8_t* dataptr, uint32_t len)
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
     * @return The number of bytes actually read (-1 in case of error)
     */
    static int
    gdb_rd_mem_cb(void *obj, uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        struct CpuBase* myself = (struct CpuBase*)obj;
        return myself->gdb_rd_mem(addr, dataptr, len);
    }

    /** Make a debug write access into the system
     * @param[in] addr Address to write at
     * @param[in] dataptr Pointer to the buffer to read from
     * @param[in] len Length of the data to write
     * @return The number of bytes actually written (-1 in case of error)
     */
    virtual int
    gdb_wr_mem(uint64_t addr, uint8_t* dataptr, uint32_t len)
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
     * @return The number of bytes actually written (-1 in case of error)
     */
    static int
    gdb_wr_mem_cb(void *obj, uint64_t addr, uint8_t* dataptr, uint32_t len)
    {
        struct CpuBase* myself = (struct CpuBase*)obj;
        return myself->gdb_wr_mem(addr, dataptr, len);
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
