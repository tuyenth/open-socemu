#include <assert.h>
#include <string.h>

#include "arm926ejs.h"

#define ARM926EJS_MAIN_TLB() (&main_tlb)
#define ARM926EJS_I_CACHE() (&i_cache)

#define ARM926EJS_LOCKDOWN_TLB() (&lockdown_tlb)
#define ARM926EJS_D_CACHE() (&d_cache)
#define ARM926EJS_WB() (&wb)

typedef struct arm926ejs_desc_s
{
    int main_tlb;
    int lockdown_tlb;
    struct mmu::cache_desc i_cache;
    struct mmu::cache_desc d_cache;
    struct mmu::wb_desc wb;
} arm926ejs_desc_t;

static arm926ejs_desc_t arm926ejs_desc =
{
    // (2 way *32) entry main TLB
    64,
    // 8 entry locked down TLB
    8,
    /* 32 bytes per line
     * 4 way
     * 64 set
     *
     * 8K i cache
     * 4K d cache
     * */
    {32, 4, 64, mmu::CACHE_WRITE_BACK},

    {32, 4, 32, mmu::CACHE_WRITE_BACK},
    {8, 8} //for byte size
};

arm926ejs::arm926ejs(struct bus* bus, bool gdbserver, bool gdbstart, bool bigendian)
: mmu(bus, gdbserver, gdbstart, bigendian)
{
    arm926ejs_desc_t *desc;
    struct cache_desc *c_desc;

    // configure the ARM
    m_DeviceId = 0x41069260;
    m_IsV4 = true;
    m_IsV5 = true;
    m_IsV5e = true;
    m_IsBaseUpdatedAbort = false;

    // configure the MMU
    control = 0x50078;
    translation_table_base = 0x0;
    domain_access_control = 0xDEADC0DE;
    fault_status = 0;
    fault_address = 0;
    fcse_id = 0;

    desc = &arm926ejs_desc;
    assert(mmu_tlb_init(ARM926EJS_MAIN_TLB(), desc->main_tlb) == 0);

    c_desc = &desc->i_cache;
    assert(mmu_cache_init(ARM926EJS_I_CACHE(), c_desc->width, c_desc->way,
            c_desc->set, c_desc->w_mode) == 0);

    assert(mmu_tlb_init(ARM926EJS_LOCKDOWN_TLB(), desc->lockdown_tlb) == 0);

    c_desc = &desc->d_cache;
    assert(mmu_cache_init(ARM926EJS_D_CACHE(), c_desc->width, c_desc->way,
            c_desc->set, c_desc->w_mode) == 0);

    assert(mmu_wb_init(ARM926EJS_WB(), desc->wb.num, desc->wb.nb) == 0);
}

enum arm::fault_t
arm926ejs::mmu_load_instr(uint32_t va, uint32_t* instr)
{
    fault_t fault;
    struct tlb_entry* tlb;
    struct cache_line* cache;
    uint32_t mva, pa;

    // generate modified VA (integrate process ID)
    mva = mmu_va_to_mva(va);

    // align check
    if (mva & (WORD_SIZE - 1))
    {
        ARM_WARN("Unaligned instruction load @0x%08X", mva);
        if (MMU_AlignCheck)
        {
            assert(false);
            return ALIGNMENT_FAULT;
        }
        // otherwise, align the instruction address
        mva &= ~(WORD_SIZE - 1);
    }


    // check if MMU instruction cache is not enabled
    if (!MMU_ICacheEnabled)
    {
        *instr = m_bus.rd_l(m_bus.obj, mva);
        return NO_FAULT;
    }
    // search cache no matter MMU enabled/disabled
    cache = mmu_cache_search(ARM926EJS_I_CACHE(), mva);

    // if cache was hit, then return immediately the data
    if (cache)
    {
        *instr = cache->data[va_cache_index(mva, ARM926EJS_I_CACHE())];
        return NO_FAULT;
    }

    // if reached here, then cache did not contain the data

    // check if MMU is enabled
    if (MMU_Enabled)
    {
        // translate VA to TLB
        fault = translate(mva, ARM926EJS_MAIN_TLB(), &tlb);
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

        // translate VA in PA
        pa = tlb_va_to_pa(tlb, mva);

        // check if region is cacheable
        if (tlb_c_flag(tlb))
        {
            int index;

            cache = mmu_cache_alloc(ARM926EJS_I_CACHE(), mva, pa);
            index = va_cache_index(mva, ARM926EJS_I_CACHE());
            *instr = cache->data[va_cache_index(mva, ARM926EJS_I_CACHE())];
        }
        else
        {
            *instr = m_bus.rd_l(m_bus.obj, pa);
        }
    }
    else
    {
        int index;
        pa = mva;

        cache = mmu_cache_alloc(ARM926EJS_I_CACHE(), mva, pa);
        index = va_cache_index(mva, ARM926EJS_I_CACHE());
        *instr = cache->data[va_cache_index(mva, ARM926EJS_I_CACHE())];
    }

    return NO_FAULT;
}



enum arm::fault_t
arm926ejs::mmu_read_byte(uint32_t va, uint32_t * data)
{
    fault_t fault;
    fault = mmu_read(va, data, ARM_BYTE_TYPE);
    return fault;
}

enum arm::fault_t
arm926ejs::mmu_read_halfword(uint32_t va, uint32_t * data)
{
    fault_t fault;
    fault = mmu_read(va, data, ARM_HALFWORD_TYPE);
    return fault;
}

enum arm::fault_t
arm926ejs::mmu_read_word(uint32_t va, uint32_t * data)
{
    return mmu_read(va, data, ARM_WORD_TYPE);
}


enum arm::fault_t
arm926ejs::mmu_read(uint32_t va, uint32_t * data, enum arm_data_type datatype)
{
    fault_t fault;
    struct tlb_entry *tlb;
    struct cache_line *cache;
    uint32_t mva, pa, temp, offset;

    // generate modified VA (integrate process ID)
    mva = mmu_va_to_mva(va);

    // alignment check
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

    // if MMU Data cache disabled, memory_read directly using VA
    if (MMU_DCacheDisabled)
    {
        if (datatype == ARM_BYTE_TYPE)
        {
            *data = m_bus.rd_b(m_bus.obj, mva);
        }
        else if (datatype == ARM_HALFWORD_TYPE)
        {
            *data = m_bus.rd_s(m_bus.obj, mva);
        }
        else if (datatype == ARM_WORD_TYPE)
        {
            *data = m_bus.rd_l(m_bus.obj, mva);
        }
        else
        {
            assert(0);
        }

        return NO_FAULT;
    }

    // align virtual address on word boundary
    mva &= ~(WORD_SIZE - 1);

    // translate VA to TLB
    fault = translate(mva, ARM926EJS_MAIN_TLB(), &tlb);
    if (fault)
    {
        ARM_WARN("read VA to TLB translation failed @0x%08X", mva);
        return fault;
    }
    // check access permission
    fault = check_access(mva, tlb, 1);
    if (fault)
    {
        ARM_WARN("read check access failed @0x%08X", mva);
        return fault;
    }
    // search main cache
    cache = mmu_cache_search(ARM926EJS_D_CACHE(), mva);
    if (cache)
    {
        // data found in cache
        *data = cache->data[va_cache_index(mva, ARM926EJS_D_CACHE())];
    }
    else
    {
        // data not in cache

        // get phy_addr
        pa = tlb_va_to_pa(tlb, mva);
        if ((pa >= 0xe0000000) && (pa < 0xe8000000))
        {
            if (tlb_c_flag(tlb))
            {
                //if (tlb_b_flag(tlb)){
                //      mmu_cache_soft_flush(ARM926EJS_D_CACHE(), pa);
                //}
                mmu_cache_soft_flush(ARM926EJS_D_CACHE(), pa);
            }
            return NO_FAULT;
        }

        // if Buffer, drain Write Buffer first
        if (tlb_b_flag(tlb))
        {
            mmu_wb_drain_all(ARM926EJS_WB());
        }

        // allocate cache or mem_read
        if (tlb_c_flag(tlb) && MMU_DCacheEnabled)
        {
            struct cache *cache_t;

            //if (tlb_b_flag(tlb))
            cache_t = ARM926EJS_D_CACHE();
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
    }
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
arm926ejs::mmu_write_byte(uint32_t va, uint32_t data)
{
    return mmu_write(va, data, ARM_BYTE_TYPE);
}

enum arm::fault_t
arm926ejs::mmu_write_halfword(uint32_t va, uint32_t data)
{
    return mmu_write(va, data, ARM_HALFWORD_TYPE);
}

enum arm::fault_t
arm926ejs::mmu_write_word(uint32_t va, uint32_t data)
{
    return mmu_write(va, data, ARM_WORD_TYPE);
}



enum arm::fault_t
arm926ejs::mmu_write(uint32_t va, uint32_t data, enum arm_data_type datatype)
{
    struct tlb_entry *tlb;
    struct cache_line *cache;
    int b;
    uint32_t mva, pa;
    fault_t fault;

    // generate modified VA (integrate process ID)
    mva = mmu_va_to_mva(va);

    // alignment check
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

    // search instruction cache
    cache = mmu_cache_search(ARM926EJS_I_CACHE(), mva);
    if (cache)
    {
        update_cache(mva, data, datatype, cache, ARM926EJS_I_CACHE(), va);
    }

    if (MMU_DCacheDisabled)
    {
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

    // mask lower bits of address
    mva &= ~(WORD_SIZE - 1);

    // TLB translate
    fault = translate(mva, ARM926EJS_MAIN_TLB(), &tlb);
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
    cache = mmu_cache_search(ARM926EJS_D_CACHE(), mva);
    if (cache)
    {
        update_cache(mva, data, datatype, cache, ARM926EJS_D_CACHE(), va);
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
                    mmu_wb_write_bytes(ARM926EJS_WB(), pa, (uint8_t*)&data, 4);
                else if (datatype == ARM_HALFWORD_TYPE)
                    mmu_wb_write_bytes(ARM926EJS_WB(), (pa | (va & 2)), (uint8_t*)&data, 2);
                else if (datatype == ARM_BYTE_TYPE)
                    mmu_wb_write_bytes(ARM926EJS_WB(), (pa | (va & 3)), (uint8_t*)&data, 1);

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
            mmu_wb_drain_all(ARM926EJS_WB());

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
arm926ejs::update_cache(uint32_t mva, uint32_t data, uint32_t datatype,
        struct cache_line* cache, struct cache* cache_t, uint32_t va)
{
    uint32_t temp, offset;

    uint32_t index = va_cache_index(mva, cache_t);

    //cache->data[index] = data;

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
arm926ejs::mmu_mrc(uint32_t instr, uint32_t * value)
{
    mmu_regnum_t creg = (mmu_regnum_t)(BITS(16, 19) & 0xf);
    int OPC_2 = BITS(5, 7) & 0x7;
    uint32_t data;

    switch (creg)
    {
    case MMU_ID:
        if (OPC_2 == 0)
        {
            data = m_DeviceId;
        }
        else if (OPC_2 == 1)
        {
            /* ARM926EJS Cache type:
             *            Ctype     S               Dsize            Isize
             *                                 Size assoc M Len        Size assoc M Len
             *NS9750: 000 1110      1       00 0011 010   0 10      00 0100 010   0 10
             *see ARM926ejs TRM P2-8
             * */
            data = 0x1D0D2112;
        }
        else if (OPC_2 == 2)
        {
            // TCM status
            data = 0x0;
        }
        else
        {
            ARM_WARN("arm926ej-s: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
            return -1;
        }
        break;
    case MMU_CONTROL:
        /*
         * 6:3          should be 1.
         * 11:10        should be 0
         * 18,16        should be 1
         * 17           should be 0
         * 31:19        should be 0
         * */
        data = (control | 0x50078) & 0x0005F3FF;;
        break;
    case MMU_TRANSLATION_TABLE_BASE:
        data = translation_table_base & 0xFFFFC000;
        break;
    case MMU_DOMAIN_ACCESS_CONTROL:
        data = domain_access_control;
        break;
    case MMU_FAULT_STATUS:
        /* OPC_2 = 0: data FSR value
         * */
        if (OPC_2 == 0)
        {
            data = fault_status & 0x0FF;
        }
        else
        {
            ARM_WARN("arm926ej-s: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
            return -1;
        }
        break;
    case MMU_FAULT_ADDRESS:
        data = fault_address;
        break;
    case MMU_CACHE_OPS:
        ARM_WARN("arm926ej-s: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
        return -1;
        break;
    case MMU_TLB_LOCKDOWN:
        ARM_WARN("arm926ej-s: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
        return -1;
        break;
    case MMU_PID:
        data = fcse_id;
        break;
    default:
        ARM_WARN("arm926ej-s: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
        return -1;
        break;
    }
    *value = data;
    return 0;
}

void
arm926ejs::mmu_cache_ops(uint32_t instr, uint32_t value)
{
    int CRm, OPC_2;

    CRm = BITS(0, 3);
    OPC_2 = BITS(5, 7);

    //printf("%s - CRm: %d, OPC_2: %d\n", __FUNCTION__, CRm, OPC_2);

    // Invalidate ICache and DCache
    if (OPC_2 == 0 && CRm == 7)
    {
        mmu_cache_invalidate_all(ARM926EJS_I_CACHE());
        mmu_cache_invalidate_all(ARM926EJS_D_CACHE());
        return;
    }
    // Invalidate ICache
    if (OPC_2 == 0 && CRm == 5)
    {
        mmu_cache_invalidate_all(ARM926EJS_I_CACHE());
        return;
    }
    // Invalidate ICache single entry (MVA)
    if (OPC_2 == 1 && CRm == 5)
    {
        mmu_cache_invalidate(ARM926EJS_I_CACHE(), value);
        return;
    }
    // Invalidate ICache single entry (Set/Way)
    if (OPC_2 == 2 && CRm == 5)
    {
        mmu_cache_invalidate_by_index(ARM926EJS_I_CACHE(), value);
        return;
    }
    /// @todo Prefetch ICache line (MVA)
    if (OPC_2 == 1 && CRm == 13)
    {
        //mmu_cache_invalidate(ARM926EJS_I_CACHE(), value);
        return;
    }
    // Invalidate DCache
    if (OPC_2 == 0 && CRm == 6)
    {
        mmu_cache_invalidate_all(ARM926EJS_D_CACHE());
        return;
    }
    // Invalidate DCache single entry (MVA)
    if (OPC_2 == 1 && CRm == 6)
    {
        mmu_cache_invalidate(ARM926EJS_D_CACHE(), value);
        return;
    }
    // Invalidate DCache single entry (Set/Way)
    if (OPC_2 == 2 && CRm == 6)
    {
        mmu_cache_invalidate_by_index(ARM926EJS_D_CACHE(), value);
        return;
    }
    // Clean DCache single entry (MVA)
    if (OPC_2 == 1 && CRm == 10)
    {
        mmu_cache_clean(ARM926EJS_D_CACHE(), value);
        return;
    }
    // Clean DCache single entry (Set/Way)
    if (OPC_2 == 2 && CRm == 10)
    {
        mmu_cache_clean_by_index(ARM926EJS_D_CACHE(), value);
        return;
    }
    /// @todo Test and clean DCache
    if (OPC_2 == 3 && CRm == 10)
    {
        return;
    }
    // Clean and invalidate DCache entry (MVA)
    if (OPC_2 == 1 && CRm == 14)
    {
        mmu_cache_clean(ARM926EJS_D_CACHE(), value);
        mmu_cache_invalidate(ARM926EJS_D_CACHE(), value);
        return;
    }
    // Clean and invalidate DCache entry (Set/Way)
    if (OPC_2 == 2 && CRm == 14)
    {
        mmu_cache_clean_by_index(ARM926EJS_D_CACHE(), value);
        mmu_cache_invalidate_by_index(ARM926EJS_D_CACHE(), value);
        return;
    }
    /// @todo Test, clean, and invalidate DCache
    if (OPC_2 == 3 && CRm == 14)
    {
        return;
    }
    // Drain write buffer
    if (OPC_2 == 4 && CRm == 10)
    {
        mmu_wb_drain_all(ARM926EJS_WB());
        return;
    }
    // Wait for interrupt
    if (OPC_2 == 4 && CRm == 0)
    {
        mmu_wb_drain_all(ARM926EJS_WB());
        m_bus.wfi(m_bus.obj);
        return;
    }
    assert(0);
}

void
arm926ejs::mmu_tlb_ops(uint32_t instr, uint32_t value)
{
    int CRm, OPC_2;

    CRm = BITS(0, 3);
    OPC_2 = BITS(5, 7);

    if (OPC_2 == 0 && (CRm == 0x7) || (CRm == 0x6) || (CRm == 0x5))
    {
        mmu_tlb_invalidate_all(ARM926EJS_MAIN_TLB());
        //mmu_tlb_invalidate_all(ARM926EJS_LOCKDOWN_TLB());
        return;
    }

    if (OPC_2 == 1 && (CRm == 0x5 || (CRm == 0x7) || (CRm == 0x6)))
    {
        mmu_tlb_invalidate_entry(ARM926EJS_MAIN_TLB(), value);
        return;
    }
    assert(0);
}

int
arm926ejs::mmu_mcr(uint32_t instr, uint32_t value)
{
    mmu_regnum_t creg = (mmu_regnum_t)(BITS(16, 19) & 0xF);
    int OPC_2 = BITS(5, 7) & 0x7;

    switch (creg)
    {
    case MMU_CONTROL:
        control = (value | 0x50078) & 0x0005F3FF;
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
        else if (OPC_2 == 1)
        {
            ARM_WARN("arm926ej-s: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
        }
        else
        {
            ARM_WARN("arm926ej-s: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
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
        ARM_WARN("arm926ej-s: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
        break;
    case MMU_TLB_LOCKDOWN:
        ARM_WARN("arm926ej-s: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
        return -1;
        break;
    case MMU_PID:
        if (OPC_2 == 0)
        {
            // FCSE PID register
            // 24:0 SBZ
            fcse_id = value & MMU_FCSE_MASK;
        }
        else
        {
            // Context ID register
            ARM_WARN("arm926ej-s: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
            return -1;
        }
        break;

    default:
        ARM_WARN("arm926ej-s: MMU operation not implemented reg=%d opc2=%d", creg, OPC_2);
        return -1;
        break;
    }
    return 0;
}


int
arm926ejs::gdb_read(uint64_t addr, uint8_t* dataptr, uint32_t len)
{
    // if DCache disabled, do quick access
    if (MMU_Disabled || MMU_DCacheDisabled)
    {
        return m_bus.gdb_rd(m_bus.obj, addr, dataptr, len);
    }
    else
    {
        ARM_WARN("arm926ej-s: gdb access when cache enabled not supported yet @0x%08llX", addr);
        return 0;
    }
}

int
arm926ejs::gdb_write(uint64_t addr, uint8_t* dataptr, uint32_t len)
{
    // if DCache disabled, do quick access
    if (MMU_Disabled || MMU_DCacheDisabled)
    {
        return m_bus.gdb_wr(m_bus.obj, addr, dataptr, len);
    }
    else
    {
        ARM_WARN("arm926ej-s: gdb access when cache enabled not supported yet @0x%08llX", addr);
        return 0;
    }
}



