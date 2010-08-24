/** @file rb.cpp
 * @brief Read Buffers interface implementation
 *
 * @sa http://infocenter.arm.com/
 */
#include <assert.h>

#include "mmu.h"

/// Defines the number of bytes for various read buffer types
static const uint32_t rb_masks[] =
{
    /// RB_INVALID
    0,
    /// RB_1
    4,
    /// RB_4
    16,
    /// RB_8
    32
};

int
mmu::mmu_rb_init(struct rb* rb, int num)
{
    int i;
    struct rb_entry* rb_entries;

    // allocate the read buffers
    rb_entries = (struct rb_entry*) malloc(sizeof (*rb_entries) * num);
    assert(rb_entries != NULL);

    // initialize the read buffers with default values
    for (i = 0; i < num; i++)
    {
        rb_entries[i].type = RB_INVALID;
        rb_entries[i].fault = NO_FAULT;
    }

    // save the read buffers list
    rb->entries = rb_entries;
    rb->num = num;
    return 0;
}

void
mmu::mmu_rb_exit(struct rb* rb)
{
    free(rb->entries);
}

struct mmu::rb_entry*
mmu::mmu_rb_search(struct rb* rb, uint32_t va)
{
    uint32_t i;
    struct rb_entry* rb_entry = rb->entries;

    // loop on all the read buffer entries
    for (i = 0; i < rb->num; i++, rb_entry++)
    {
        // check if the read buffer is valid
        if (rb_entry->type)
        {
            // check if the virtual address matches the read buffer
            if ((va >= rb_entry->va) &&
                (va < (rb_entry->va + rb_masks[rb_entry->type])))
            {
                return rb_entry;
            }
        }
    }
    return NULL;
}

void
mmu::mmu_rb_invalidate_entry(struct rb* rb, int i)
{
    rb->entries[i].type = RB_INVALID;
}

void
mmu::mmu_rb_invalidate_all(struct rb* rb)
{
    uint32_t i;

    for (i = 0; i < rb->num; i++)
    {
        mmu_rb_invalidate_entry(rb, i);
    }
}

void
mmu::mmu_rb_load(struct rb* rb, struct tlb* tlb, int i_rb, int type, uint32_t va)
{
    struct rb_entry* rb_entry;
    uint32_t i;
    uint32_t max_start, min_end;
    fault_t fault;
    struct tlb_entry* tlb_entry;

    // align va according to type
    va &= ~(rb_masks[type] - 1);

    // invalidate all RB matching [va, va + rb_masks[type]]
    for (rb_entry = rb->entries, i = 0; i < rb->num; i++, rb_entry++)
    {
        // check if entry is valid
        if (rb_entry->type)
        {
            max_start = arm_max(va, rb_entry->va);
            min_end = arm_min(va + rb_masks[type],
                    rb_entry->va + rb_masks[rb_entry->type]);
            if (max_start < min_end)
            {
                rb_entry->type = RB_INVALID;
            }
        }
    }
    // load word into the referenced read buffer
    rb_entry = &rb->entries[i_rb];
    rb_entry->type = type;

    // translate the address into tlb
    fault = translate(va, tlb, &tlb_entry);
    if (fault)
    {
        rb_entry->fault = fault;
        return;
    }
    // check the access rights
    fault = check_access(va, tlb_entry, 1);
    if (fault)
    {
        rb_entry->fault = fault;
        return;
    }
    rb_entry->fault = NO_FAULT;

    // translate the virtual address into physical address
    va = tlb_va_to_pa(tlb_entry, va);

    for (i = 0; i < (rb_masks[type] / 4); i++, va += WORD_SIZE)
    {
        rb_entry->data[i] = m_bus.rd_l(m_bus.obj, va);
    }
}


