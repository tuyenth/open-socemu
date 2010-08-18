#ifndef ARM920T_H_
#define ARM920T_H_

#include "mmu.h"

struct arm920t: public mmu
{
public:
    arm920t(struct bus *bus, bool gdbserver = false, bool gdbstart = false, bool bigendian = false);

    /// Read byte data
    enum fault_t
    mmu_read_byte(uint32_t va, uint32_t* data);

    /// Write byte data
    enum fault_t
    mmu_write_byte(uint32_t va, uint32_t data);

    /// Read halfword data 
    enum fault_t
    mmu_read_halfword(uint32_t va, uint32_t* data);

    /// Write halfword data
    enum fault_t
    mmu_write_halfword(uint32_t va, uint32_t data);

    /// Read word data
    enum fault_t
    mmu_read_word(uint32_t va, uint32_t* data);

    /// Write word data
    enum fault_t
    mmu_write_word(uint32_t va, uint32_t data);

    /// Load instruction
    enum fault_t
    mmu_load_instr(uint32_t va, uint32_t * instr);

    /// MCR
    int
    mmu_mcr(uint32_t instr, uint32_t val);

    /// MRC
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


    struct tlb i_tlb;
    struct cache i_cache;

    struct tlb d_tlb;
    struct cache d_cache;
    struct wb wb;
};

#endif /*ARM920T_H_*/
