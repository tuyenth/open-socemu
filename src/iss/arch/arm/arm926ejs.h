#ifndef ARM926EJS_H_
#define ARM926EJS_H_

#include "mmu.h"

struct arm926ejs: public mmu
{
public:
    arm926ejs(struct bus* bus, bool gdbserver = false, bool gdbstart = false, bool bigendian = false);

    /// Virtual function implementation.
    enum fault_t
    mmu_read_byte(uint32_t va, uint32_t* data);

    /// Write byte data.
    enum fault_t
    mmu_write_byte(uint32_t va, uint32_t data);

    /// Read halfword data .
    enum fault_t
    mmu_read_halfword(uint32_t va, uint32_t* data);

    /// Write halfword data.
    enum fault_t
    mmu_write_halfword(uint32_t va, uint32_t data);

    /// Read word data.
    enum fault_t
    mmu_read_word(uint32_t va, uint32_t* data);

    /// Write word data.
    enum fault_t
    mmu_write_word(uint32_t va, uint32_t data);

    /// Load instruction.
    enum fault_t
    mmu_load_instr(uint32_t va, uint32_t * instr);

    /// MCR.
    int
    mmu_mcr(uint32_t instr, uint32_t val);

    /// MRC.
    int
    mmu_mrc(uint32_t instr, uint32_t* val);

    /// Implementation of virtual function
    int
    gdb_read(uint64_t addr, uint8_t* dataptr, uint32_t len);

    /// Implementation of virtual function
    int
    gdb_write(uint64_t addr, uint8_t* dataptr, uint32_t len);

private:
    enum fault_t
    mmu_write(uint32_t va, uint32_t data, enum arm_data_type datatype);
    enum fault_t
    mmu_read(uint32_t va, uint32_t * data, enum arm_data_type datatype);
    enum fault_t
    update_cache(uint32_t mva, uint32_t data,
            uint32_t datatype, struct cache_line* cache,
            struct cache* cache_t, uint32_t va);
    void
    mmu_cache_ops(uint32_t instr, uint32_t value);
    void
    mmu_tlb_ops(uint32_t instr, uint32_t value);


    struct tlb main_tlb;
    struct tlb lockdown_tlb;

    struct cache i_cache;
    struct cache d_cache;
    struct wb wb;
};

#endif /*ARM926EJS_H_*/
