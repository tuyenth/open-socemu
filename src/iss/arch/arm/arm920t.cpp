#include <assert.h>
#include <string.h>

#include "arm920t.h"

#define ARM920T_I_TLB() (&this->i_tlb)
#define ARM920T_I_CACHE() (&this->i_cache)

#define ARM920T_D_TLB() (&this->d_tlb)
#define ARM920T_D_CACHE() (&this->d_cache)
#define ARM920T_WB() (&this->wb)

typedef struct arm920t_desc_s
{
    int i_tlb;
    struct mmu::cache_desc i_cache;

    int d_tlb;
    struct mmu::cache_desc d_cache;
    struct mmu::wb_desc wb;
} arm920t_desc_t;

static arm920t_desc_t
arm920t_desc = {
    64,
    /* 32 bytes per line
     * 64 way
     * 8 set
     * */
    {32, 64, 8, mmu::CACHE_WRITE_BACK},

    64,
    {32, 64, 8, mmu::CACHE_WRITE_BACK},
    {8, 8}			//for byte size
};

arm920t::arm920t(struct bus *bus, bool gdbserver, bool gdbstart, bool bigendian)
: mmu(bus, gdbserver, gdbstart, bigendian)
{
    arm920t_desc_t *desc;
    struct cache_desc *c_desc;

    // configure the ARM
    m_DeviceId = 0x41009200;
    m_IsV4 = true;
    m_IsV5 = false;
    m_IsV5e = false;
    m_IsBaseUpdatedAbort = false;

    // configure the MMU
    control = 0x70;
    translation_table_base = 0xDEADC0DE;
    domain_access_control = 0xDEADC0DE;
    fault_status = 0;
    fault_address = 0;
    fcse_id = 0;

    desc = &arm920t_desc;
    assert(mmu_tlb_init(ARM920T_I_TLB(), desc->i_tlb) == 0);

    c_desc = &desc->i_cache;
    assert(mmu_cache_init(ARM920T_I_CACHE(), c_desc->width, c_desc->way,
            c_desc->set, c_desc->w_mode) == 0);

    assert(mmu_tlb_init(ARM920T_D_TLB(), desc->d_tlb) == 0);

    c_desc = &desc->d_cache;
    assert(mmu_cache_init(ARM920T_D_CACHE(), c_desc->width, c_desc->way,
            c_desc->set, c_desc->w_mode) == 0);

    assert(mmu_wb_init(ARM920T_WB(), desc->wb.num, desc->wb.nb) == 0);
}

enum arm::fault_t
arm920t::mmu_load_instr(uint32_t va, uint32_t * instr)
{
    fault_t fault;
    struct tlb_entry *tlb;
    struct cache_line *cache;
    // cache bit
    int c;
    // modified addresses
    uint32_t mva, pa;

    static int debug_count = 0; //used for debug

    mva = mmu_va_to_mva(va);
    if (MMU_Enabled)
    {
        // align check
        if (mva & (WORD_SIZE - 1))
        {
            ARM_WARN("Unaligned instruction load @0x%08X", mva);
            if (MMU_AlignCheck)
            {
                return ALIGNMENT_FAULT;
            }
            mva &= ~(WORD_SIZE - 1);
        }

        // translate TLB
        fault = translate(mva, ARM920T_I_TLB(), &tlb);
        if (fault)
        {
            ARM_WARN("instruction VA to TLB translation failed @0x%08X", mva);
            return fault;
        }

        // check access
        fault = check_access(mva, tlb, 1);
        if (fault)
        {
            ARM_WARN("instruction TLB check access failed @0x%08X", mva);
            return fault;
        }
    }

    // search cache no matter MMU enabled/disabled
    cache = mmu_cache_search(ARM920T_I_CACHE(), mva);
    if (cache)
    {
        *instr = cache->data[va_cache_index(mva, ARM920T_I_CACHE())];
        return NO_FAULT;
    }

    // if MMU disabled or C flag is set alloc cache
    if (MMU_Disabled)
    {
        c = 1;
        pa = mva;
    }
    else
    {
        c = tlb_c_flag(tlb);
        pa = tlb_va_to_pa(tlb, mva);
    }

    if (c)
    {
        int index;

        debug_count++;
        cache = mmu_cache_alloc(ARM920T_I_CACHE(), mva, pa);
        index = va_cache_index(mva, ARM920T_I_CACHE());
        *instr = cache->data[va_cache_index(mva, ARM920T_I_CACHE())];
    }
    else
    {
        *instr = m_bus.rd_l(m_bus.obj, pa);
    }

    return NO_FAULT;
}



enum arm::fault_t
arm920t::mmu_read_byte(uint32_t va, uint32_t * data)
{
    fault_t fault;
    fault = mmu_read(va, data, ARM_BYTE_TYPE);
    return fault;
}

enum arm::fault_t
arm920t::mmu_read_halfword(uint32_t va, uint32_t * data)
{
    fault_t fault;
    fault = mmu_read(va, data, ARM_HALFWORD_TYPE);
    return fault;
}

enum arm::fault_t
arm920t::mmu_read_word(uint32_t va, uint32_t * data)
{
    return mmu_read(va, data, ARM_WORD_TYPE);
}

enum arm::fault_t
arm920t::mmu_read(uint32_t va, uint32_t * data, enum arm_data_type datatype)
{
    enum fault_t fault;
    struct tlb_entry *tlb;
    struct cache_line *cache;
    uint32_t mva, pa, temp, offset;

    // remap address (depending on process ID if enabled)
    mva = mmu_va_to_mva(va);

    // if MMU disabled, memory_read
    if (MMU_Disabled)
    {
        if (datatype == ARM_BYTE_TYPE)
            *data = m_bus.rd_b(m_bus.obj, mva);
        else if (datatype == ARM_HALFWORD_TYPE)
            *data = m_bus.rd_s(m_bus.obj, mva);
        else if (datatype == ARM_WORD_TYPE)
            *data = m_bus.rd_l(m_bus.obj, mva);
        else
        {
            assert(0);
        }

        return NO_FAULT;
    }

    // align check
    if (((mva & 3) && (datatype == ARM_WORD_TYPE)) ||
        ((mva & 1) && (datatype == ARM_HALFWORD_TYPE)))
    {
        // information even if not data abort
        ARM_WARN("read alignment fault -> @0x%08X, size 2**%d", mva, datatype);
        if (MMU_AlignCheck)
        {
            return ALIGNMENT_FAULT;
        }
    }

    // align address on word
    mva &= ~(WORD_SIZE - 1);

    // translate VA to TLB
    fault = translate(mva, ARM920T_D_TLB(), &tlb);
    if (fault)
    {
        ARM_WARN("read VA to TLB translation failed @0x%08X", mva);
        return fault;
    }
    // check access permission
    fault = check_access(mva, tlb, 1);
    if (fault)
    {
        ARM_WARN("read TLB check access failed @0x%08X", mva);
        return fault;
    }
    // search main cache
    cache = mmu_cache_search(ARM920T_D_CACHE(), mva);
    if (cache)
    {
        *data = cache->data[va_cache_index(mva, ARM920T_D_CACHE())];
        goto datatrans;
        //return 0;
    }

    // get phy_addr
    pa = tlb_va_to_pa(tlb, mva);
    if ((pa >= 0xe0000000) && (pa < 0xe8000000))
    {
        if (tlb_c_flag(tlb))
        {
            //if (tlb_b_flag(tlb)){
            //      mmu_cache_soft_flush(ARM920T_D_CACHE(), pa);
            //}
            mmu_cache_soft_flush(ARM920T_D_CACHE(), pa);
        }
        return NO_FAULT;
    }

    // if Buffer, drain Write Buffer first
    if (tlb_b_flag(tlb))
    {
        mmu_wb_drain_all(ARM920T_WB());
    }

    // alloc cache or mem_read
    if (tlb_c_flag(tlb) && MMU_DCacheEnabled)
    {
        struct cache *cache_t;

        //if (tlb_b_flag(tlb))
        cache_t = ARM920T_D_CACHE();
        cache = mmu_cache_alloc(cache_t, mva, pa);
        *data = cache->data[va_cache_index(mva, cache_t)];
    }
    else
    {
        //*data = mem_read_word(pa);
        if (datatype == ARM_BYTE_TYPE)
            *data = m_bus.rd_b(m_bus.obj, pa | (va & 3));
        else if (datatype == ARM_HALFWORD_TYPE)
            *data = m_bus.rd_s(m_bus.obj, pa | (va & 2));
        else if (datatype == ARM_WORD_TYPE)
            *data = m_bus.rd_l(m_bus.obj, pa);
        else
        {
            assert(0);
        }
        return NO_FAULT;
    }


datatrans:
    if (datatype == ARM_HALFWORD_TYPE)
    {
        temp = *data;
        // bit offset into the word
        offset = (((uint32_t)m_IsBigEndian * 2) ^ (va & 2)) << 3;
        *data = (temp >> offset) & 0xffff;
    }
    else if (datatype == ARM_BYTE_TYPE)
    {
        temp = *data;
        // bit offset into the word
        offset = (((uint32_t)m_IsBigEndian * 3) ^ (va & 3)) << 3;
        *data = (temp >> offset & 0xffL);
    }
    return NO_FAULT;
}


enum arm::fault_t
arm920t::mmu_write_byte(uint32_t va, uint32_t data)
{
    return mmu_write(va, data, ARM_BYTE_TYPE);
}

enum arm::fault_t
arm920t::mmu_write_halfword(uint32_t va, uint32_t data)
{
    return mmu_write(va, data, ARM_HALFWORD_TYPE);
}

enum arm::fault_t
arm920t::mmu_write_word(uint32_t va, uint32_t data)
{
    return mmu_write(va, data, ARM_WORD_TYPE);
}


enum arm::fault_t
arm920t::mmu_write(uint32_t va, uint32_t data, enum arm_data_type datatype)
{
    struct tlb_entry *tlb;
    struct cache_line *cache;
    int b;
    uint32_t mva, pa;
    enum fault_t fault;

    // remap address (depending on process ID if enabled)
    mva = mmu_va_to_mva(va);

    // search instruction cache
    cache = mmu_cache_search(ARM920T_I_CACHE(), mva);
    if (cache)
    {
        update_cache(mva, data, datatype, cache, ARM920T_I_CACHE(), va);
    }

    if (MMU_Disabled)
    {
        // mem_write_word(mva, data);
        if (datatype == ARM_BYTE_TYPE)
            m_bus.wr_b(m_bus.obj, mva, data);
        else if (datatype == ARM_HALFWORD_TYPE)
            m_bus.wr_s(m_bus.obj, mva, data);
        else if (datatype == ARM_WORD_TYPE)
            m_bus.wr_l(m_bus.obj, mva, data);
        else
        {
            assert(0);
        }

        return NO_FAULT;
    }
    // align check
    if (((mva & 3) && (datatype == ARM_WORD_TYPE)) ||
        ((mva & 1) && (datatype == ARM_HALFWORD_TYPE)))
    {
        // information even if not data abort
        ARM_WARN("write alignment fault -> @0x%08X, size 2**%d", mva, datatype);
        if (MMU_AlignCheck)
        {
            return ALIGNMENT_FAULT;
        }
    }
    mva &= ~(WORD_SIZE - 1);

    // TLB translate
    fault = translate(mva, ARM920T_D_TLB(), &tlb);
    if (fault)
    {
        ARM_WARN("write VA to TLB translation failed @0x%08X", mva);
        return fault;
    }
    // TLB check access
    fault = check_access(mva, tlb, 0);
    if (fault)
    {
        ARM_WARN("write TLB check access failed @0x%08X", mva);
        return fault;
    }

    // search main cache
    cache = mmu_cache_search(ARM920T_D_CACHE(), mva);
    if (cache)
    {
        update_cache(mva, data, datatype, cache, ARM920T_D_CACHE(), va);
    }

    if (!cache)
    {
        b = tlb_b_flag(tlb);
        pa = tlb_va_to_pa(tlb, mva);
        if (b)
        {
            if (MMU_WBEnabled)
            {
                if (datatype == ARM_WORD_TYPE)
                    mmu_wb_write_bytes(ARM920T_WB(), pa, (uint8_t*)&data, 4);
                else if (datatype == ARM_HALFWORD_TYPE)
                    mmu_wb_write_bytes(ARM920T_WB(), (pa |(va & 2)), (uint8_t*)&data, 2);
                else if (datatype == ARM_BYTE_TYPE)
                    mmu_wb_write_bytes(ARM920T_WB(), (pa | (va & 3)), (uint8_t*)&data, 1);

            }
            else
            {
                if (datatype == ARM_WORD_TYPE)
                    m_bus.wr_l(m_bus.obj, pa, data);
                else if (datatype == ARM_HALFWORD_TYPE)
                    m_bus.wr_s(m_bus.obj, (pa | (va & 2)), data);
                else if (datatype == ARM_BYTE_TYPE)
                    m_bus.wr_b(m_bus.obj, (pa | (va & 3)), data);
            }
        }
        else
        {
            mmu_wb_drain_all(ARM920T_WB());

            if (datatype == ARM_WORD_TYPE)
                m_bus.wr_l(m_bus.obj, pa, data);
            else if (datatype == ARM_HALFWORD_TYPE)
                m_bus.wr_s(m_bus.obj, (pa | (va & 2)), data);
            else if (datatype == ARM_BYTE_TYPE)
                m_bus.wr_b(m_bus.obj, (pa | (va & 3)), data);
        }
    }
    return NO_FAULT;
}

enum arm::fault_t
arm920t::update_cache(uint32_t mva, uint32_t data, uint32_t datatype,
        struct cache_line* cache, struct cache * cache_t, uint32_t va)
{
    uint32_t temp, offset;

    uint32_t index = va_cache_index(mva, cache_t);

    if (datatype == ARM_WORD_TYPE)
        cache->data[index] = data;
    else if (datatype == ARM_HALFWORD_TYPE)
    {
        temp = cache->data[index];
        // bit offset into the word
        offset = (((uint32_t)m_IsBigEndian * 2) ^ (va & 2)) << 3;
        cache->data[index] = (temp & ~(0xffffL << offset)) | ((data & 0xffffL) << offset);
    }
    else if (datatype == ARM_BYTE_TYPE)
    {
        temp = cache->data[index];
        // bit offset into the word
        offset = (((uint32_t)m_IsBigEndian * 3) ^ (va & 3)) << 3;
        cache->data[index] = (temp & ~(0xffL << offset)) | ((data & 0xffL) << offset);
    }

    if (index < (cache_t->width >> (WORD_SHIFT + 1)))
        cache->tag |= TAG_FIRST_HALF_DIRTY;
    else
        cache->tag |= TAG_LAST_HALF_DIRTY;

    return NO_FAULT;
}

int
arm920t::mmu_mrc(uint32_t instr, uint32_t * value)
{
    mmu_regnum_t creg = (mmu_regnum_t)(BITS(16, 19) & 0xf);
    int OPC_2 = BITS(5, 7) & 0x7;
    uint32_t data;

    switch (creg) {
    case MMU_ID:
        if (OPC_2 == 0)
        {
            data = m_DeviceId;
        }
        else if (OPC_2 == 1)
        {
            /* Cache type:
             * 000 0110 1 000 101 110 0 10 000 101 110 0 10
             * */
            data = 0x0D172172;
        }
        else
        {
            ARM_WARN("arm920t: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
            return -1;
        }
        break;
    case MMU_CONTROL:
        /*
         * 6:3          should be 1.
         * 11:10        should be 0
         * */
        data = (control | 0x78) & 0xFFFFF3FF;;
        break;
    case MMU_TRANSLATION_TABLE_BASE:
        data = translation_table_base;
        break;
    case MMU_DOMAIN_ACCESS_CONTROL:
        data = domain_access_control;
        break;
    case MMU_FAULT_STATUS:
        /* OPC_2 = 0: data FSR value
         * */
        if (OPC_2 == 0)
        {
            data = fault_status;
        }
        else
        {
            ARM_WARN("arm920t: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
            return -1;
        }
        break;
    case MMU_FAULT_ADDRESS:
        data = fault_address;
        break;
    case MMU_PID:
        data = fcse_id;
        break;
    default:
        ARM_WARN("arm920t: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
        return -1;
        break;
    }
    *value = data;
    return 0;
}

void
arm920t::mmu_cache_ops(uint32_t instr, uint32_t value)
{
    int CRm, OPC_2;

    CRm = BITS(0, 3);
    OPC_2 = BITS(5, 7);

    if (OPC_2 == 0 && CRm == 7) {
        mmu_cache_invalidate_all(ARM920T_I_CACHE());
        mmu_cache_invalidate_all(ARM920T_D_CACHE());
        return;
    }

    if (OPC_2 == 0 && CRm == 5) {
        mmu_cache_invalidate_all(ARM920T_I_CACHE());
        return;
    }
    /*Invalidate ICache single entry
     **/
    if (OPC_2 == 1 && CRm == 5) {
        mmu_cache_invalidate(ARM920T_I_CACHE(), value);
        return;
    }
    /* FIXME: should complete
     * Prefetch ICache line (using MVA)
     * */
    if (OPC_2 == 1 && CRm == 13) {
        //mmu_cache_invalidate(ARM920T_I_CACHE(), value);
        return;
    }

    if (OPC_2 == 0 && CRm == 6) {
        mmu_cache_invalidate_all(ARM920T_D_CACHE());
        return;
    }

    /* Invalidate DCache single entry (using MVA)
     * */
    if (OPC_2 == 1 && CRm == 6) {
        mmu_cache_invalidate(ARM920T_D_CACHE(), value);
        return;
    }
    /* Clean DCache single entry (using MVA)
     * */
    if (OPC_2 == 1 && CRm == 0xa) {
        mmu_cache_clean(ARM920T_D_CACHE(), value);
        return;
    }
    /* Clean and Invalidate DCache entry (using MVA)
     * */
    if (OPC_2 == 1 && CRm == 14) {
        mmu_cache_clean(ARM920T_D_CACHE(), value);
        mmu_cache_invalidate(ARM920T_D_CACHE(), value);
        return;
    }
    /* Clean DCache single entry (using index)
     * */
    if (OPC_2 == 2 && CRm == 0xa) {
        mmu_cache_clean_by_index(ARM920T_D_CACHE(), value);
        return;
    }
    /* Clean and Invalidate DCache entry (using index)
     * */
    if (OPC_2 == 2 && CRm == 14) {
        mmu_cache_clean_by_index(ARM920T_D_CACHE(), value);
        mmu_cache_invalidate_by_index(ARM920T_D_CACHE(), value);
        return;
    }

    /* Drain write buffer
     * */
    if (OPC_2 == 4 && CRm == 0xa) {
        mmu_wb_drain_all(ARM920T_WB());
        return;
    }
    /* FIXME: how to do a waiting operation?
     * Wait for a interrupt
     * */
    if (OPC_2 == 4 && CRm == 0) {
        return;
    }
    assert(0);
}

void
arm920t::mmu_tlb_ops(uint32_t instr, uint32_t value)
{
    int CRm, OPC_2;

    CRm = BITS(0, 3);
    OPC_2 = BITS(5, 7);


    if (OPC_2 == 0 && CRm == 0x7) {
        mmu_tlb_invalidate_all(ARM920T_I_TLB());
        mmu_tlb_invalidate_all(ARM920T_D_TLB());
        return;
    }

    if (OPC_2 == 0 && CRm == 0x5) {
        mmu_tlb_invalidate_all(ARM920T_I_TLB());
        return;
    }

    if (OPC_2 == 1 && CRm == 0x5) {
        mmu_tlb_invalidate_entry(ARM920T_I_TLB(), value);
        return;
    }

    if (OPC_2 == 0 && CRm == 0x6) {
        mmu_tlb_invalidate_all(ARM920T_D_TLB());
        return;
    }

    if (OPC_2 == 1 && CRm == 0x6) {
        mmu_tlb_invalidate_entry(ARM920T_D_TLB(), value);
        return;
    }

    assert(0);
}

int
arm920t::mmu_mcr(uint32_t instr, uint32_t value)
{
    mmu_regnum_t creg = (mmu_regnum_t)(BITS(16, 19) & 0xF);
    int OPC_2 = BITS(5, 7) & 0x7;

    switch (creg)
    {
    case MMU_CONTROL:
        control = (value | 0x78) & 0xFFFFF3FF;
        break;
    case MMU_TRANSLATION_TABLE_BASE:
        translation_table_base = value & 0xFFFFC000;
        break;
    case MMU_DOMAIN_ACCESS_CONTROL:
        domain_access_control = value;
        break;

    case MMU_FAULT_STATUS:
        if (OPC_2 == 0)
        {
            fault_status = value & 0xFF;
        }
        else
        {
            ARM_WARN("arm920t: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
            return -1;
        }
        break;
    case MMU_FAULT_ADDRESS:
        fault_address = value;
        break;

    case MMU_CACHE_OPS:
        mmu_cache_ops(instr, value);
        break;
    case MMU_TLB_OPS:
        mmu_tlb_ops(instr, value);
        break;
    case MMU_CACHE_LOCKDOWN:
        ARM_WARN("arm920t: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
        return -1;
        break;
    case MMU_TLB_LOCKDOWN:
        ARM_WARN("arm920t: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
        return -1;
        break;
    case MMU_PID:
        // 0:24 should be zero.
        fcse_id = value & MMU_FCSE_MASK;
        break;

    default:
        ARM_WARN("arm920t: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
        return -1;
        break;
    }
    return 0;
}

int
arm920t::gdb_read(uint64_t addr, uint8_t* dataptr, uint32_t len)
{
    // if MMU disabled, do quick access
    if (MMU_Disabled)
    {
        return m_bus.gdb_rd(m_bus.obj, addr, dataptr, len);
    }
    else
    {
        ARM_WARN("arm920t: gdb access when cache enabled not supported yet @0x%08llX", addr);
        return 0;
    }
}

int
arm920t::gdb_write(uint64_t addr, uint8_t* dataptr, uint32_t len)
{
    // if MMU disabled, do quick access
    if (MMU_Disabled)
    {
        return m_bus.gdb_wr(m_bus.obj, addr, dataptr, len);
    }
    else
    {
        ARM_WARN("arm920t: gdb access when cache enabled not supported yet @0x%08llX", addr);
        return 0;
    }
}
