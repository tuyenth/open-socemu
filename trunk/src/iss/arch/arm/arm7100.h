#ifndef ARM7100_H_
#define ARM7100_H_

#include "mmu.h"

struct arm7100: public mmu
{
public:
    arm7100(struct bus *bus, bool gdbserver = false, bool gdbstart = false, bool bigendian = false);

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
    mmu_read(uint32_t virt_addr, uint32_t * data, enum arm_data_type datatype);
    enum fault_t
    mmu_write(uint32_t virt_addr, uint32_t data, enum arm_data_type datatype);


    struct cache cache;
    struct tlb tlb;
};

#endif	/*ARM7100_H_*/
