/** @file wb.cpp
 * @brief Write Blocks interface implementation
 *
 * @sa http://infocenter.arm.com/
 */

#include <assert.h>

#include "mmu.h"

int
mmu::mmu_wb_init(struct wb* wb, int num, int nb)
{
    int i;
    struct wb_entry *wb_entries, *wb_entry;

    // allocate the array of entries
    wb_entries = (struct wb_entry *) malloc(sizeof(*wb_entries) * num);
    assert(wb_entries != NULL);

    // init entryes one by one
    for (wb_entry = wb_entries, i = 0; i < num; i++, wb_entry++)
    {
        /*chy 2004-06-06, fix bug found by wenye@cs.ucsb.edu */
        //wb->data = (uint32_t *)malloc(sizeof(uint32_t) * nb);
        wb_entry->data = (uint8_t*) malloc(nb);
        assert(wb_entry->data != NULL);
    }

    wb->first = wb->last = wb->used = 0;
    wb->num = num;
    wb->nb = nb;
    wb->entries = wb_entries;
    return 0;

};

void
mmu::mmu_wb_exit(struct wb* wb)
{
    uint32_t i;
    struct wb_entry *wb_entry;

    wb_entry = wb->entries;
    for (i = 0; i < wb->num; i++, wb_entry++)
    {
        free (wb_entry->data);
    }
    free (wb->entries);
};

void
mmu::mmu_wb_write_bytes(struct wb* wb, uint32_t pa, uint8_t* data, uint32_t n)
{
    uint32_t i;
    struct wb_entry *wb_entry;

    // loop as long as there are bytes to write
    while (n)
    {
        // check if all write buffers are in use
        if (wb->num == wb->used)
        {
            // flush the last write buffer entry
            uint32_t t;

            // retrieve the last element
            wb_entry = &wb->entries[wb->last];
            // get its physical address
            t = wb_entry->pa;

            // write all the bytes in memory
            for (i = 0; i < wb_entry->nb; i++)
            {
                m_bus.wr_b(m_bus.obj, t, wb_entry->data[i]);
                t++;
            }
            // increment the last element pointer
            wb->last++;

            // wrap around
            if (wb->last == wb->num)
            {
                wb->last = 0;
            }

            // decrement the used counter
            wb->used--;
        }

        // retrieve the pointer to the first available entry
        wb_entry = &wb->entries[wb->first];

        // retrieve the number of writable bytes in the entry
        i = (n < wb->nb) ? n : wb->nb;

        // save the physical address
        wb_entry->pa = pa;
        // increment for next entry allocation
        pa += i;

        // save the number of written bytes
        wb_entry->nb = i;
        // copy the data
        memcpy(wb_entry->data, data, i);
        // increment the data pointer
        data += i;
        // decrement the number of bytes remaining to store
        n -= i;
        // increment the first
        wb->first++;
        // wrap around
        if (wb->first == wb->num)
        {
            wb->first = 0;
        }
        // increment the number of used wb
        wb->used++;
    };
}

void
mmu::mmu_wb_drain_all(struct wb* wb)
{
    uint32_t pa;
    struct wb_entry *wb_entry;
    uint32_t i;

    // loop on all used entries
    while (wb->used)
    {
        // retrieve the entry
        wb_entry = &wb->entries[wb->last];
        // get the physical address
        pa = wb_entry->pa;
        // loop on all entry bytes
        for (i = 0; i < wb_entry->nb; i++)
        {
            m_bus.wr_b(m_bus.obj, pa, wb_entry->data[i]);
            pa++;
        }
        // increment the last added wb
        wb->last++;
        // wrap around
        if (wb->last == wb->num)
        {
            wb->last = 0;
        }
        // decrement the number of used buffer
        wb->used--;
    };
}
