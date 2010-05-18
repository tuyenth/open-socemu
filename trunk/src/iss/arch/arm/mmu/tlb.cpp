/** @file tlb.cpp
 * @brief Translation Lookaside Buffer interface implementation.
 *
 * @sa http://infocenter.arm.com/
 */
#include <assert.h>

#include "mmu.h"

const uint32_t mmu::tlb_masks[6] =
{
    // TLB_INVALID
    0x00000000,
    // TLB_SMALLPAGE
    0xFFFFF000,
    // TLB_LARGEPAGE
    0xFFFF0000,
    // TLB_SECTION
    0xFFF00000,
    // TLB_ESMALLPAGE, have TEX attirbute, only for XScale
    0xFFFFF000,
    // TLB_TINYPAGE
    0xFFFFFC00
};



bool
mmu::check_perms(int ap, int read)
{
    int s, r, user;

    // retrieve if it is controlled
    s = control & CONTROL_SYSTEM;
    // retrieve if it is a ROM
    r = control & CONTROL_ROM;

    // indicates user mode
    fprintf(stderr, "fix this problem, state is not known here\n");
    //user = (state->Mode == USER32MODE) || (state->Mode == USER26MODE);
    user = false;

    // depending on the AP bits, s, r and read/write request
    switch (ap)
    {
    case 0:
        // ro plus s&r checks
        return read && ((s && !user) || r);
    case 1:
        // rw for privileged, no access for user
        return !user;
    case 2:
        // rw for privileged, ro for user
        return read || !user;
    case 3:
        // rw for everyone
        return 1;
    }
    return 0;
}

enum mmu::fault_t
mmu::check_access(uint32_t virt_addr, struct tlb_entry* tlb_entry, int read)
{
    int access;

    // save in the MMU the last domain used
    last_domain = tlb_entry->domain;
    // retrieve the access type (client or manager) in the domain access control
    access = (domain_access_control >> (tlb_entry->domain * 2)) & 3;
    if ((access == 0) || (access == 2))
    {
        /* It's unclear from the documentation whether this
           should always raise a section domain fault, or if
           it should be a page domain fault in the case of an
           L1 that describes a page table.  In the ARM710T
           datasheets, "Figure 8-9: Sequence for checking faults"
           seems to indicate the former, while "Table 8-4: Priority
           encoding of fault status" gives a value for FS[3210] in
           the event of a domain fault for a page.  Hmm. */
        return SECTION_DOMAIN_FAULT;
    }
    if (access == 1)
    {
        // client access - check perms
        int subpage, ap;

        // depending on the type of page
        switch (tlb_entry->mapping)
        {
        /* only for xscale */
        case TLB_ESMALLPAGE:
            subpage = 0;
            break;
        case TLB_TINYPAGE:
            subpage = 0;
            break;
        case TLB_SMALLPAGE:
            subpage = (virt_addr >> 10) & 3;
            break;
        case TLB_LARGEPAGE:
            subpage = (virt_addr >> 14) & 3;
            break;
        case TLB_SECTION:
            subpage = 3;
            break;
        default:
            assert(0);
            subpage = 0;
        }
        // retrieve the AP bits at the corresponding location
        ap = (tlb_entry->perms >> (subpage * 2 + 4)) & 3;
        // check if the access is allowed
        if (!check_perms(ap, read))
        {
            if (tlb_entry->mapping == TLB_SECTION)
            {
                return SECTION_PERMISSION_FAULT;
            }
            else
            {
                return SUBPAGE_PERMISSION_FAULT;
            }
        }
    }
    else
    {
        // access == 3
        // manager access - don't check perms
    }
    return NO_FAULT;
}

enum mmu::fault_t
mmu::translate(uint32_t virt_addr, struct tlb* tlb, struct tlb_entry** tlb_entry)
{
    // check if the corresponding TLB is already loaded
    *tlb_entry = mmu_tlb_search(tlb, virt_addr);

    // if the entry is not found, then fetch it
    if (!*tlb_entry)
    {
        // walk the translation tables to fill with the new request
        uint32_t l1addr, l1desc;
        struct tlb_entry entry;

        // retrieve the first level descriptor
        l1addr = translation_table_base & 0xFFFFC000;
        l1addr = (l1addr | (virt_addr >> 18)) & ~3;
        l1desc = m_bus.rd_l(m_bus.obj, l1addr);

        // check the page size
        switch (l1desc & 3)
        {
        case 0:
            return PAGE_TRANSLATION_FAULT;
        case 3:
        {
            // fine page table
            uint32_t l2addr, l2desc;

            // retrieve the second level descriptor
            l2addr = l1desc & 0xFFFFF000;
            l2addr = (l2addr |
                    ((virt_addr & 0x000FFC00) >> 8)) &
                    ~3;
            l2desc = m_bus.rd_l(m_bus.obj, l2addr);

            // save the temporary entry with the information available
            entry.virt_addr = virt_addr;
            entry.phys_addr = l2desc;
            entry.perms = l2desc & 0x00000FFC;
            entry.domain = (l1desc >> 5) & 0x0000000F;

            // check the page size
            switch (l2desc & 3)
            {
            case 0:
                last_domain = entry.domain;
                return PAGE_TRANSLATION_FAULT;
            case 1:
                entry.mapping = TLB_LARGEPAGE;
                break;
            case 2:
                entry.mapping = TLB_SMALLPAGE;
                break;
            case 3:
                entry.mapping = TLB_TINYPAGE;
                break;
            }
        }
        break;
        case 1:
        {
            // coarse page table
            uint32_t l2addr, l2desc;

            // retrieve the second level descriptor
            l2addr = l1desc & 0xFFFFFC00;
            l2addr = (l2addr |
                    ((virt_addr & 0x000FF000) >> 10)) &
                    ~3;
            l2desc = m_bus.rd_l(m_bus.obj, l2addr);

            // save the temporary entry with the information available
            entry.virt_addr = virt_addr;
            entry.phys_addr = l2desc;
            entry.perms = l2desc & 0x00000FFC;
            entry.domain = (l1desc >> 5) & 0x0000000F;

            // check the page size
            switch (l2desc & 3)
            {
            case 0:
                last_domain = entry.domain;
                return PAGE_TRANSLATION_FAULT;
            case 3:
                last_domain = entry.domain;
                return PAGE_TRANSLATION_FAULT;
            case 1:
                entry.mapping = TLB_LARGEPAGE;
                break;
            case 2:
                entry.mapping = TLB_SMALLPAGE;
                break;
            }
        }
        break;
        case 2:
            // section
            entry.virt_addr = virt_addr;
            entry.phys_addr = l1desc;
            entry.perms = l1desc & 0x00000C0C;
            entry.domain = (l1desc >> 5) & 0x0000000F;
            entry.mapping = TLB_SECTION;
            break;
        }

        // mask the valid bits in the virtual address and physical address
        entry.virt_addr &= tlb_masks[entry.mapping];
        entry.phys_addr &= tlb_masks[entry.mapping];

        // place entry in the tlb
        *tlb_entry = &tlb->entries[tlb->cycle];
        tlb->cycle = (tlb->cycle + 1) % tlb->num;
        **tlb_entry = entry;
    }
    // save the last domain used in the MMU
    last_domain = (*tlb_entry)->domain;
    return NO_FAULT;
}

int
mmu::mmu_tlb_init(struct tlb* tlb, int num)
{
    struct tlb_entry* tlb_entry;
    int i;

    // allocate an array of TLBs
    tlb_entry = (struct tlb_entry*) malloc(sizeof (*tlb_entry) * num);
    if (tlb_entry == NULL)
    {
        printf("malloc size %d\n", sizeof (*tlb_entry) * num);
        assert(0);
        goto tlb_malloc_error;
    }
    // save the array in the TLB list
    tlb->entries = tlb_entry;

    // initialize all the TLBs
    for (i = 0; i < num; i++, tlb_entry++)
    {
        tlb_entry->mapping = TLB_INVALID;
    }
    // save the TLB list
    tlb->cycle = 0;
    tlb->num = num;
    return 0;

tlb_malloc_error:
    return -1;
}

void
mmu::mmu_tlb_exit(struct tlb* tlb)
{
    free(tlb->entries);
}

void
mmu::mmu_tlb_invalidate_all(struct tlb* tlb)
{
    int entry;

    for (entry = 0; entry < tlb->num; entry++)
    {
        tlb->entries[entry].mapping = TLB_INVALID;
    }
    tlb->cycle = 0;
}

void
mmu::mmu_tlb_invalidate_entry(struct tlb* tlb, uint32_t virt_addr)
{
    struct tlb_entry*tlb_entry;

    // search for a TLB matching the virtual address
    tlb_entry = mmu_tlb_search(tlb, virt_addr);
    // if found, invalidate it
    if (tlb_entry)
    {
        tlb_entry->mapping = TLB_INVALID;
    }
}

struct mmu::tlb_entry*
mmu::mmu_tlb_search(struct tlb* tlb, uint32_t virt_addr)
{
    int entry;

    for (entry = 0; entry < tlb->num; entry++)
    {
        struct tlb_entry*tlb_entry;
        uint32_t mask;

        tlb_entry = &(tlb->entries[entry]);
        if (tlb_entry->mapping == TLB_INVALID)
        {
            continue;
        }
        mask = tlb_masks[tlb_entry->mapping];

        // check if the TLB matches the virtual address
        if ((virt_addr & mask) == (tlb_entry->virt_addr & mask))
        {
            return tlb_entry;
        }
    }
    return NULL;
}
