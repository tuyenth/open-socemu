/** @file cache.c
 * @brief Cache interface API implementation.
 *
 * @sa http://infocenter.arm.com/
 */
#include <assert.h>

#include "mmu.h"

int
mmu::mmu_cache_init(struct cache* cache_t, int width, int way, int set, enum write_mode w_mode)
{
    int i, j;
    struct cache_set *sets;
    struct cache_line *lines;

    // allocate cache set
    sets = NULL;
    lines = NULL;
    sets = (struct cache_set *) malloc(sizeof(struct cache_set) * set);
    assert(sets != NULL);
    cache_t->sets = sets;

    // init cache set
    for (i = 0; i < set; i++)
    {
        // alloc cache line
        lines = (struct cache_line *) malloc(sizeof(struct cache_line) * way);
        assert(lines != NULL);
        // init cache lines
        for (j = 0; j < way; j++)
        {
            // invalid tag
            lines[j].tag = 0;
            lines[j].data = (uint32_t *) malloc (width);
            assert(lines[j].data != NULL);
        }
        sets[i].lines = lines;
        sets[i].cycle = 0;
    }
    cache_t->width = width;
    cache_t->set = set;
    cache_t->way = way;
    cache_t->w_mode = w_mode;
    return 0;
}

void
mmu::mmu_cache_exit(struct cache * cache_t)
{
    int i, j;
    struct cache_set *sets, *set;
    struct cache_line *lines, *line;

    // free all set
    sets = cache_t->sets;
    for (set = sets, i = 0; i < cache_t->set; i++, set++)
    {
        // free all lines
        lines = set->lines;
        for (line = lines, j = 0; j < cache_t->way; j++, line++)
        {
            free(line->data);
        }
        free(lines);
    }
    free(sets);
}

struct mmu::cache_line*
mmu::mmu_cache_search(struct cache* cache_t, uint32_t va)
{
    int i;
    int set = va_cache_set(va, cache_t);
    uint32_t tag = va_cache_align(va, cache_t);
    struct cache_line *cache;

    struct cache_set *cache_set = cache_t->sets + set;
    for (i = 0, cache = cache_set->lines; i < cache_t->way; i++, cache++)
    {
        if ((cache->tag & TAG_VALID_FLAG) &&
            (tag == va_cache_align(cache->tag, cache_t)))
        {
            return cache;
        }
    }
    return NULL;
}

struct mmu::cache_line*
mmu::mmu_cache_search_by_index(struct cache* cache_t, uint32_t index)
{
    int way = cache_t->way;
    int set_v = index_cache_set(index, cache_t);
    int i = 0, index_v = 0;
    struct cache_set *set;

    while ((way >>= 1) >= 1)
    {
        i++;
    }
    index_v = index >> (32 - i);
    set = cache_t->sets + set_v;

    return set->lines + index_v;
}


struct mmu::cache_line *
mmu::mmu_cache_alloc(struct cache* cache_t, uint32_t va, uint32_t pa)
{
    struct cache_line *cache;
    struct cache_set *set;
    int i;

    va = va_cache_align(va, cache_t);
    pa = va_cache_align(pa, cache_t);

    set = &cache_t->sets[va_cache_set (va, cache_t)];

    // robin-round
    cache = &set->lines[set->cycle++];
    if (set->cycle == cache_t->way)
        set->cycle = 0;

    if (cache_t->w_mode == CACHE_WRITE_BACK)
    {
        uint32_t t;

        // if cache valid, try to write back
        if (cache->tag & TAG_VALID_FLAG)
        {
            mmu_cache_write_back(cache_t, cache);
        }
        // read in cache_line
        t = pa;
        for (i = 0; i < (cache_t->width >> WORD_SHIFT); i++, t += WORD_SIZE)
        {
            cache->data[i] = m_bus.rd_l(m_bus.obj, t);
        }
    }
    // store tag and pa
    cache->tag = va | TAG_VALID_FLAG;
    cache->pa = pa;

    return cache;
}

void
mmu::mmu_cache_write_back(struct cache* cache_t, struct cache_line * cache)
{
    uint32_t pa = cache->pa;
    int nw = cache_t->width >> WORD_SHIFT;
    uint32_t *data = cache->data;
    int i;

    if ((cache->tag & 1) == 0)
    {
        return;
    }

    switch (cache->tag & ~1 & (TAG_FIRST_HALF_DIRTY | TAG_LAST_HALF_DIRTY))
    {
    case 0:
        return;
    case TAG_FIRST_HALF_DIRTY:
        nw /= 2;
        break;
    case TAG_LAST_HALF_DIRTY:
        nw /= 2;
        pa += nw << WORD_SHIFT;
        data += nw;
        break;
    case TAG_FIRST_HALF_DIRTY | TAG_LAST_HALF_DIRTY:
        break;
    }
    for (i = 0; i < nw; i++, data++, pa += WORD_SIZE)
    {
        m_bus.wr_l(m_bus.obj, pa, *data);
    }

    cache->tag &= ~(TAG_FIRST_HALF_DIRTY | TAG_LAST_HALF_DIRTY);
}


void
mmu::mmu_cache_clean(struct cache* cache_t, uint32_t va)
{
    struct cache_line *cache;

    cache = mmu_cache_search(cache_t, va);
    if (cache)
    {
        mmu_cache_write_back(cache_t, cache);
    }
}

void
mmu::mmu_cache_clean_by_index(struct cache* cache_t, uint32_t index)
{
    struct cache_line *cache;

    cache = mmu_cache_search_by_index(cache_t, index);
    if (cache)
    {
        mmu_cache_write_back(cache_t, cache);
    }
}

void
mmu::mmu_cache_invalidate(struct cache* cache_t, uint32_t va)
{
    struct cache_line *cache;

    cache = mmu_cache_search(cache_t, va);
    if (cache)
    {
        mmu_cache_write_back(cache_t, cache);
        cache->tag = 0;
    }
}

void
mmu::mmu_cache_invalidate_by_index(struct cache* cache_t, uint32_t index)
{
    struct cache_line *cache;

    cache = mmu_cache_search_by_index(cache_t, index);
    if (cache)
    {
        mmu_cache_write_back(cache_t, cache);
        cache->tag = 0;
    }
}

void
mmu::mmu_cache_invalidate_all(struct cache* cache_t)
{
    int i, j;
    struct cache_set *set;
    struct cache_line *cache;

    set = cache_t->sets;
    for (i = 0; i < cache_t->set; i++, set++)
    {
        cache = set->lines;
        for (j = 0; j < cache_t->way; j++, cache++)
        {
            mmu_cache_write_back(cache_t, cache);
            cache->tag = 0;
        }
    }
};

void
mmu::mmu_cache_soft_flush(struct cache* cache_t, uint32_t pa)
{
    uint32_t set, way;
    struct cache_line *cache;
    pa = (pa / cache_t->width);
    way = pa & (cache_t->way - 1);
    set = (pa / cache_t->way) & (cache_t->set - 1);
    cache = &cache_t->sets[set].lines[way];

    mmu_cache_write_back(cache_t, cache);
    cache->tag = 0;
}
