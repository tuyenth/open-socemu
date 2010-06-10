#ifndef CPUBASE_H_
#define CPUBASE_H_

#include "SimpleMaster.h"

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

struct CpuBase: SimpleMaster
{
    /** Arm constructor
     * @param[in] name Name of the module
     * @param[in] gdbserver Specifies if the GDB remote connection must be supported
     * @param[in] gdbstart Specifies if ISS must wait for GDB remote connection before starting
     */
    CpuBase(sc_core::sc_module_name name)
    : SimpleMaster(name)
    {

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
    /// Program Counter
    uint32_t pc;

};

#endif /* CPUBASE_H_ */
