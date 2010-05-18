#include <assert.h>
#include <string.h>

#include "arm7100.h"

arm7100::arm7100(struct bus *bus, bool gdbserver, bool gdbstart, bool bigendian)
: mmu(bus, gdbserver, gdbstart, bigendian)
{
    // configure the ARM
    m_DeviceId = 0x41007100;
    m_IsV4 = false;
    m_IsV5 = false;
    m_IsV5e = false;
    m_IsBaseUpdatedAbort = true;

    // configure the MMU
    control = 0x70;
    translation_table_base = 0xDEADC0DE;
    domain_access_control = 0xDEADC0DE;
    fault_status = 0;
    fault_address = 0;
    assert(mmu_cache_init(&this->cache, 16, 4, 128, CACHE_WRITE_THROUGH) == 0);
    assert(mmu_tlb_init(&this->tlb, 64) == 0);
}

enum arm::fault_t
arm7100::mmu_load_instr(uint32_t va, uint32_t * instr)
{
    return mmu_read_word(va, instr);
}

enum arm::fault_t
arm7100::mmu_read_byte(uint32_t va, uint32_t * data)
{
    fault_t fault;
    fault = mmu_read(va, data, ARM_BYTE_TYPE);
    return fault;
}

enum arm::fault_t
arm7100::mmu_read_halfword(uint32_t va, uint32_t * data)
{
    fault_t fault;
    fault = mmu_read(va, data, ARM_HALFWORD_TYPE);
    return fault;
}

enum arm::fault_t
arm7100::mmu_read_word(uint32_t va, uint32_t * data)
{
    return mmu_read(va, data, ARM_WORD_TYPE);
}

enum arm::fault_t
arm7100::mmu_read(uint32_t va, uint32_t * data, enum mmu::arm_data_type datatype)
{
    struct tlb_entry *tlb;
    uint32_t pa;
    uint32_t temp, offset;
    enum fault_t fault;

    // check alignment error
    if (((va & 3) && (datatype == ARM_WORD_TYPE)) ||
        ((va & 1) && (datatype == ARM_HALFWORD_TYPE)))
    {
        // information even if not data abort
        ARM_WARN("read alignment fault -> @0x%08lX, size 2**%d", va, datatype);
    }

    // check if there is no MMU
    if (MMU_Disabled)
    {
        switch(datatype)
        {
        case ARM_BYTE_TYPE:
            *data = m_bus.rd_b(m_bus.obj, va);
            break;

        case ARM_HALFWORD_TYPE:
            *data = m_bus.rd_s(m_bus.obj, va);
            break;

        case ARM_WORD_TYPE:
            *data = m_bus.rd_l(m_bus.obj, va);
            break;

        default:
            assert(0);
            std::exit(-1);
            break;
        }
        return NO_FAULT;
    }

    // check alignment error
    if (((va & 3) && (datatype == ARM_WORD_TYPE)) ||
        ((va & 1) && (datatype == ARM_HALFWORD_TYPE)))
    {
        if (control & CONTROL_ALIGN_FAULT)
        {
            return ALIGNMENT_FAULT;
        }
    }

    // check if there is a cache
    if (MMU_CacheEnabled)
    {
        struct cache_line *cache;
        cache = mmu_cache_search(&this->cache, va);
        if (cache)
        {
            switch(datatype)
            {
            case ARM_WORD_TYPE:
                *data = cache->data[(va >> 2) & 3];
                break;

            case ARM_HALFWORD_TYPE:
                temp = cache->data[(va >> 2) & 3];
                // bit offset into the word
                offset = (((uint32_t) m_IsBigEndian * 2) ^ (va & 2)) << 3;
                *data = (temp >> offset) & 0xffff;
                break;

            case ARM_BYTE_TYPE:
                temp = cache->data[(va >> 2) & 3];
                // bit offset into the word
                offset = (((uint32_t) m_IsBigEndian * 3) ^ (va & 3)) << 3;
                *data = (temp >> offset & 0xffL);
                break;

            default:
                assert(0);
                break;
            }
            return NO_FAULT;
        }
    }
    fault = translate(va, &this->tlb, &tlb);
    if (fault)
    {
        ARM_WARN("read VA to TLB translation failed @0x%08lX", va);
        return fault;
    }
    fault = check_access(va, tlb, 1);
    if (fault)
    {
        ARM_WARN("read TLB check access failed @0x%08lX", va);
        return fault;
    }
    pa = (tlb->phys_addr & tlb_masks[tlb->mapping]) | (va & ~tlb_masks[tlb->mapping]);

    // allocate to the cache if cacheable
    if ((tlb->perms & 0x08) && (MMU_CacheEnabled))
    {
        struct cache_line *cache;
        uint32_t fetch;
        int i;
        cache = mmu_cache_alloc(&this->cache, va, 0);
        fetch = pa & 0xFFFFFFF0;
        for (i = 0; i < 4; i++)
        {
            cache->data[i] = m_bus.rd_l(m_bus.obj, fetch);
            fetch += 4;
        }
        cache->tag = va_cache_align(va, &this->cache) | TAG_VALID_FLAG;

        switch(datatype)
        {
        case ARM_WORD_TYPE:
            *data = cache->data[(va >> 2) & 3];
            break;

        case ARM_HALFWORD_TYPE:
            temp = cache->data[(va >> 2) & 3];
            // bit offset into the word
            offset = (((uint32_t)m_IsBigEndian * 2) ^ (va & 2)) << 3;
            *data = (temp >> offset) & 0xffff;
            break;

        case ARM_BYTE_TYPE:
            temp = cache->data[(va >> 2) & 3];
            // bit offset into the word
            offset = (((uint32_t)m_IsBigEndian * 3) ^ (va & 3)) << 3;
            *data = (temp >> offset & 0xffL);
            break;

        default:
            assert(0);
            break;
        }
        return NO_FAULT;
    }
    else
    {
        switch(datatype)
        {
        case ARM_BYTE_TYPE:
            *data = m_bus.rd_b(m_bus.obj, pa);
            break;

        case ARM_HALFWORD_TYPE:
            *data = m_bus.rd_s(m_bus.obj, pa);
            break;

        case ARM_WORD_TYPE:
            *data = m_bus.rd_l(m_bus.obj, pa);
            break;

        default:
            assert(0);
            break;
        }
        return NO_FAULT;
    }
}
enum arm::fault_t
arm7100::mmu_write_byte(uint32_t va, uint32_t data)
{
    return mmu_write(va, data, ARM_BYTE_TYPE);
}

enum arm::fault_t
arm7100::mmu_write_halfword(uint32_t va, uint32_t data)
{
    return mmu_write(va, data, ARM_HALFWORD_TYPE);
}

enum arm::fault_t
arm7100::mmu_write_word(uint32_t va, uint32_t data)
{
    return mmu_write(va, data, ARM_WORD_TYPE);
}

enum arm::fault_t
arm7100::mmu_write(uint32_t va, uint32_t data, enum mmu::arm_data_type datatype)
{
    struct tlb_entry* tlb;
    uint32_t pa;
    enum fault_t fault;
    uint32_t temp, offset;

    // check alignment
    if (((va & 3) && (datatype == ARM_WORD_TYPE)) ||
        ((va & 1) && (datatype == ARM_HALFWORD_TYPE)))
    {
        // information even if not data abort
        ARM_WARN("write alignment fault -> @0x%08lX, size 2**%d", va, datatype);
    }

    // if MMU is not present
    if (MMU_Disabled)
    {
        switch(datatype)
        {
        case ARM_BYTE_TYPE:
            m_bus.wr_b(m_bus.obj, va, data);
            break;

        case ARM_HALFWORD_TYPE:
            m_bus.wr_s(m_bus.obj, va, data);
            break;

        case ARM_WORD_TYPE:
            m_bus.wr_l(m_bus.obj, va, data);
            break;

        default:
            assert(0);
            break;
        }
        return NO_FAULT;
    }

    // check alignment
    if (((va & 3) && (datatype == ARM_WORD_TYPE)) ||
        ((va & 1) && (datatype == ARM_HALFWORD_TYPE)))
    {
        if (control & CONTROL_ALIGN_FAULT)
        {
            return ALIGNMENT_FAULT;
        }
    }

    // check if there is cache
    if (MMU_CacheEnabled)
    {
        struct cache_line *cache;
        cache = mmu_cache_search(&this->cache, va);
        if (cache)
        {
            switch(datatype)
            {
            case ARM_WORD_TYPE:
                cache->data[(va >> 2) & 3] = data;
                break;

            case ARM_HALFWORD_TYPE:
                temp = cache->data[(va >> 2) & 3];
                // bit offset into the word
                offset = (((uint32_t)m_IsBigEndian * 2) ^ (va & 2)) << 3;
                cache->data[(va >> 2) & 3] = (temp & ~(0xffffL << offset)) |
                    ((data & 0xffffL) << offset);
                break;

            case ARM_BYTE_TYPE:
                temp = cache->data[(va >> 2) & 3];
                // bit offset into the word
                offset = (((uint32_t)m_IsBigEndian * 3) ^ (va & 3)) << 3;
                cache->data[(va >> 2) & 3] = (temp & ~(0xffL << offset)) |
                    ((data & 0xffL) << offset);
                break;

            default:
                assert(0);
                break;
            }
        }
    }
    fault = translate(va, &this->tlb, &tlb);
    if (fault)
    {
        ARM_WARN("write VA to TLB translation failed @0x%08lX", va);
        return fault;
    }
    fault = check_access(va, tlb, 0);
    if (fault)
    {
        ARM_WARN("write TLB check access failed @0x%08lX", va);
        return fault;
    }
    pa = (tlb->phys_addr & tlb_masks[tlb->mapping]) |
    (va & ~tlb_masks[tlb->mapping]);

    switch(datatype)
    {
    case ARM_BYTE_TYPE:
        m_bus.wr_b(m_bus.obj, pa, data);
        break;

    case ARM_HALFWORD_TYPE:
        m_bus.wr_s(m_bus.obj, pa, data);
        break;

    case ARM_WORD_TYPE:
        m_bus.wr_l(m_bus.obj, pa, data);
        break;

    default:
        assert(0);
        break;
    }
    return NO_FAULT;
}

int
arm7100::mmu_mrc(uint32_t instr, uint32_t * value)
{
    mmu_regnum_t creg = (mmu_regnum_t)(BITS(16, 19) & 0xF);
    uint32_t data;
    switch (creg)
    {
    case MMU_ID:
        data = m_DeviceId;
        break;
    case MMU_CONTROL:
        data = control;
        break;
    case MMU_TRANSLATION_TABLE_BASE:
        data = translation_table_base;
        break;
    case MMU_DOMAIN_ACCESS_CONTROL:
        data = domain_access_control;
        break;
    case MMU_FAULT_STATUS:
        data = fault_status;
        break;
    case MMU_FAULT_ADDRESS:
        data = fault_address;
        break;
    default:
        ARM_WARN("arm7100: MMU operation not implemented reg=%d", creg);
        return -1;
        break;
    }
    *value = data;

    return 0;
}

int
arm7100::mmu_mcr(uint32_t instr, uint32_t value)
{
    mmu_regnum_t creg = (mmu_regnum_t)(BITS(16, 19) & 0xF);
    switch (creg)
    {
    case MMU_CONTROL:
        control = (value | 0x70) & 0xFFFF;
        break;
    case MMU_TRANSLATION_TABLE_BASE:
        translation_table_base = value & 0xFFFFC000;
        break;
    case MMU_DOMAIN_ACCESS_CONTROL:
        domain_access_control = value;
        break;
    case MMU_FAULT_STATUS:
        fault_status = value & 0xFF;
        break;
    case MMU_FAULT_ADDRESS:
        fault_address = value;
        break;
    case MMU_V4_CACHE_OPS:
        if ((BITS(5, 7) & 7) == 0)
        {
            mmu_cache_invalidate_all(&this->cache);
        }
        else
        {
            ARM_WARN("arm7100: MMU operation not implemented reg=%d", creg);
            return -1;
        }
        break;
    case MMU_V4_TLB_OPS:
        switch (BITS(5, 7) & 7)
        {
        case 0:
            mmu_tlb_invalidate_all(&this->tlb);
            break;
        case 1:
            mmu_tlb_invalidate_entry(&this->tlb, value);
            break;
        default:
            ARM_WARN("arm7100: MMU operation not implemented reg=%d", creg);
            return -1;
            break;
        }
        break;
    default:
        ARM_WARN("arm7100: MMU operation not implemented reg=%d", creg);
        return -1;
        break;
    }
    return 0;
}

int
arm7100::gdb_read(uint64_t addr, uint8_t* dataptr, uint32_t len)
{
    // if Cache disabled, do quick access
    if (MMU_Disabled || MMU_CacheDisabled)
    {
        return m_bus.gdb_rd(m_bus.obj, addr, dataptr, len);
    }
    else
    {
        ARM_WARN("arm7100: gdb access when cache enabled not supported yet @0x%08llX", addr);
        return 0;
    }
}

int
arm7100::gdb_write(uint64_t addr, uint8_t* dataptr, uint32_t len)
{
    // if Cache disabled, do quick access
    if (MMU_Disabled || MMU_CacheDisabled)
    {
        return m_bus.gdb_wr(m_bus.obj, addr, dataptr, len);
    }
    else
    {
        ARM_WARN("arm7100: gdb access when cache enabled not supported yet @0x%08llX", addr);
        return 0;
    }
}
