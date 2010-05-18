#ifndef MMU_H_
#define MMU_H_

#include <iostream>
#include <stdint.h>

#include "arm.h"

/// Word shift index.
#define WORD_SHIFT (2)
/// Word size.
#define WORD_SIZE (1<<WORD_SHIFT)

/// The MMU is accessible with MCR and MRC operations to copro 15.
#define MMU_COPRO (15)


/* Bits in the control register */
#define CONTROL_MMU                     (1<<0)
#define CONTROL_ALIGN_FAULT             (1<<1)
#define CONTROL_CACHE                   (1<<2)
#define CONTROL_DATA_CACHE              (1<<2)
#define CONTROL_WRITE_BUFFER            (1<<3)
#define CONTROL_BIG_ENDIAN              (1<<7)
#define CONTROL_SYSTEM                  (1<<8)
#define CONTROL_ROM                     (1<<9)
#define CONTROL_UNDEFINED               (1<<10)
#define CONTROL_BRANCH_PREDICT          (1<<11)
#define CONTROL_INSTRUCTION_CACHE       (1<<12)
#define CONTROL_VECTOR                  (1<<13)
#define CONTROL_RR                      (1<<14)
#define CONTROL_L4                      (1<<15)

/* Macro defines for MMU state*/
#define MMU_CTL (this->control)
#define MMU_Enabled (MMU_CTL & CONTROL_MMU)
#define MMU_Disabled (!(MMU_Enabled))
#define MMU_AlignCheck (MMU_CTL & CONTROL_ALIGN_FAULT)

#define MMU_ICacheEnabled (MMU_CTL & CONTROL_INSTRUCTION_CACHE)
#define MMU_ICacheDisabled (!(MMU_ICacheDisabled))

#define MMU_DCacheEnabled (MMU_CTL & CONTROL_DATA_CACHE)
#define MMU_DCacheDisabled (!(MMU_DCacheEnabled))

#define MMU_CacheEnabled (MMU_CTL & CONTROL_CACHE)
#define MMU_CacheDisabled (!(MMU_CacheEnabled))

#define MMU_WBEnabled (MMU_CTL & CONTROL_WRITE_BUFFER)
#define MMU_WBDisabled (!(MMU_WBEnabled))

/// virt_addr exchange according to CP15.R13(process id virtual mapping)
#define MMU_FCSE_MASK (0xfe000000)
/** generate the Modified Virtual Address (includes the FCSE ID):
 * - check if the address is within 0 to 32 MB range
 * - if it is apply the translation with the current Process ID, otherwise to nothing
 */
#define mmu_va_to_mva(va)                                               \
    ((va)&MMU_FCSE_MASK)?(va):((va) | fcse_id)

/// Macro to check if the permission has C (cacheable) flag.
#define tlb_c_flag(tlb)                                                 \
    ((tlb)->perms & 0x8)

/// Macro to check if the permission has B (writeBack) flag.
#define tlb_b_flag(tlb)                                                 \
    ((tlb)->perms & 0x4)

/// Macro to translate virtual address to physical address.
#define tlb_va_to_pa(tlb, va)                                           \
    ((tlb->phys_addr & (tlb_masks[tlb->mapping])) | (va & ~(tlb_masks[tlb->mapping])))

/// Cache bit flag.
#define TAG_VALID_FLAG       (0x00000001)
/// Cache bit flag.
#define TAG_FIRST_HALF_DIRTY (0x00000002)
/// Cache bit flag.
#define TAG_LAST_HALF_DIRTY  (0x00000004)

/// Virtual address to cache set index.
#define va_cache_set(va, cache_t)                                       \
    (((va) / (cache_t)->width) & ((cache_t)->set - 1))
/// Virtual address to cache line aligned.
#define va_cache_align(va, cache_t)                                     \
        ((va) & ~((cache_t)->width - 1))
/// Virtual address to cache line word index.
#define va_cache_index(va, cache_t)                                     \
        (((va) & ((cache_t)->width - 1)) >> WORD_SHIFT)

/// Set/index format value to cache set value.
#define index_cache_set(index, cache_t)                                 \
    (((index) / (cache_t)->width) & ((cache_t)->set - 1))


/// MMU object, derivates from ARM object.
struct mmu: public arm
{
    /// Structure containing the bus interface for access
    struct bus
    {
        /// Main object reference
        void *obj;
        /// Read long word
        uint32_t (*rd_l)(void *obj, uint32_t addr);
        /// Read short word
        uint32_t (*rd_s)(void *obj, uint32_t addr);
        /// Read byte
        uint32_t (*rd_b)(void *obj, uint32_t addr);
        /// Write long word
        void (*wr_l)(void *obj, uint32_t addr, uint32_t data);
        /// Write short word
        void (*wr_s)(void *obj, uint32_t addr, uint32_t data);
        /// Write byte
        void (*wr_b)(void *obj, uint32_t addr, uint32_t data);
        /// Debug read
        int (*gdb_rd)(void *obj, uint64_t addr, uint8_t *buf, uint32_t len);
        /// Debug write
        int (*gdb_wr)(void *obj, uint64_t addr, uint8_t *buf, uint32_t len);
        /// Execution cycles elapsed
        void (*exec_cycles)(void *obj, int cycles);
        /// Wait for an interrupt
        void (*wfi)(void *obj);
    };
public:
    /** MMU Constructor.
     * @param[in, out] bus Bus callbacks
     * @param[in] gdbserver Indicate if ISS shall run gdbserver, support gdb connections.
     * @param[in] gdbstart Indicate if ISS shall wait for gdb connect at start.
     * @param[in] bigendian Indicate if ISS is little or big endian.
     */
    mmu(struct bus *bus,
        bool gdbserver = false, bool gdbstart = false, bool bigendian = false)
    : arm(gdbserver, gdbstart, bigendian)
    {
        m_bus = *bus;
    }

    /// Cache supported write modes.
    enum write_mode
    {
        /// Write back mode (write accesses are cached).
        CACHE_WRITE_BACK,
        /// Write through mode (write accesses are not cached).
        CACHE_WRITE_THROUGH
    };

    /// Cache configuration descriptor.
    struct cache_desc
    {
        /// Number of bytes in a cache line.
        int width;
        /// Way of set association.
        int way;
        /// Number of sets.
        int set;
        /// Write mode.
        enum write_mode w_mode;
    };

    /// Write buffers configuration.
    struct wb_desc
    {
        /// Number of write buffers.
        int num;
        /// Size in bytes of the write buffers.
        int nb;
    };

    /** Get the vector base address from the MMU coprocessor configuration.
     * @return The vector base address.
     */
    uint32_t mmu_vector_base(void)
    {
        if (this->control & CONTROL_VECTOR)
        {
            return 0xffff0000;
        }
        else
        {
            return 0;
        }
    }

    /** Set a fault status in the MMU coprocessor.
     * @param[in] fault Fault status of the failed access.
     * @param[in] address Address of the failed access.
     */
    void mmu_set_fault(uint32_t fault, uint32_t address)
    {
        fault_status = (fault | (last_domain << 4)) & 0xFF;
        fault_address = address;
    }



protected:
    /// Register numbers in the MMU coprocessor.
    enum mmu_regnum_t
    {
        MMU_ID = 0,
        MMU_CONTROL = 1,
        MMU_TRANSLATION_TABLE_BASE = 2,
        MMU_DOMAIN_ACCESS_CONTROL = 3,
        MMU_FAULT_STATUS = 5,
        MMU_FAULT_ADDRESS = 6,
        MMU_CACHE_OPS = 7,
        MMU_TLB_OPS = 8,
        MMU_CACHE_LOCKDOWN = 9,
        MMU_TLB_LOCKDOWN = 10,
        MMU_PID = 13,

        /* MMU_V4 */
        MMU_V4_CACHE_OPS = 7,
        MMU_V4_TLB_OPS = 8,

        /* MMU_V3 */
        MMU_V3_FLUSH_TLB = 5,
        MMU_V3_FLUSH_TLB_ENTRY = 6,
        MMU_V3_FLUSH_CACHE = 7,

        /* MMU Intel SA-1100 */
        MMU_SA_RB_OPS = 9,
        MMU_SA_DEBUG = 14,
        MMU_SA_CP15_R15 = 15,

    };

    /// Define ARM data width.
    enum arm_data_type
    {
        /// 8 bits access.
        ARM_BYTE_TYPE,
        /// 16 bits access.
        ARM_HALFWORD_TYPE,
        /// 32 bits access.
        ARM_WORD_TYPE
    };


    /// Page mapping type.
    enum tlb_mapping
    {
        TLB_INVALID = 0,
        TLB_SMALLPAGE = 1,
        TLB_LARGEPAGE = 2,
        TLB_SECTION = 3,
        TLB_ESMALLPAGE = 4,
        TLB_TINYPAGE = 5,
        TLB_MAPPING_MAX
    };

    /// TLB page masks, indexed by tlb_mapping.
    static const uint32_t tlb_masks[TLB_MAPPING_MAX];

    /// Translation Lookaside Buffer descriptor.
    struct tlb_entry
    {
        /// Base virtual address.
        uint32_t virt_addr;
        /// Base physical address.
        uint32_t phys_addr;
        /// Page permissions.
        uint32_t perms;
        /// Domain.
        uint32_t domain;
        /// Mapping type.
        enum tlb_mapping mapping;
    };

    /// Translation Lookaside Buffer list.
    struct tlb
    {
        /// Number of entries.
        int num;
        /// Current TLB cycle.
        int cycle;
        /// Array of TLBs.
        struct tlb_entry* entries;
    };

    /** Check if access is allowed.
     * @param[in] ap AP bits from the TLB entry.
     * @param[in] read Indicate if it is a read or write access.
     * @return 0 if access is not allowed, other than 0 if allowed.
     */
    bool
    check_perms(int ap, int read);

    /** Checks access rights.
     * @param[in] virt_addr Access virtual address.
     * @param[in] tlb_entry Pointer to the TLB element of the access.
     * @param[in] read Read access if 1.
     * @return The fault type if there is one.
     */
    fault_t
    check_access(uint32_t virt_addr, struct tlb_entry* tlb_entry, int read);

    /** Retrieves the Translation Lookaside Buffer for a given virtual address.
     * @param[in] virt_addr Access virtual address.
     * @param[in, out] tlb TLB list pointer.
     * @param[out] tlb_entry Pointer to a pointer to the TLB element of the access.
     * @return The fault type if there is one.
     */
    fault_t
    translate(uint32_t virt_addr, struct tlb* tlb, struct tlb_entry** tlb_entry);

    /** Initialize the TLB list and allocates the TLBs.
     * @param[in, out] tlb TLB list pointer.
     * @param[in] num Total number of TLBs in the list (will be allocated).
     * @return 0 if OK, different than 0 otherwise.
     */
    int
    mmu_tlb_init(struct tlb* tlb, int num);

    /** Free the TLB list array of TLBs.
     * @param[in, out] tlb TLB list pointer.
     */
    void
    mmu_tlb_exit(struct tlb* tlb);

    /** Invalidate all the TLBs in the TLB list.
     * @param[in, out] tlb TLB list pointer.
     */
    void
    mmu_tlb_invalidate_all(struct tlb* tlb);

    /** Invalidate the TLB for a given virtual address.
     * @param[in, out] tlb TLB list pointer.
     * @param[in] virt_addr Virtual address to invalidate.
     */
    void
    mmu_tlb_invalidate_entry(struct tlb* tlb, uint32_t virt_addr);

    /** Search for a TLB matching a given virtual address.
     * @param[in, out] tlb TLB list pointer.
     * @param[in] virt_addr Virtual address to search for.
     */
    struct tlb_entry*
    mmu_tlb_search(struct tlb* tlb, uint32_t virt_addr);



    /// Write buffer.
    struct wb_entry
    {
        /// Physical address of the write buffer.
        uint32_t pa;
        /// Pointer to the local data container (CPU cache memory).
        uint8_t *data;
        /// Number of bytes in the write buffer.
        int nb;
    };

    /// Write buffer list.
    struct wb
    {
        /// Number of entries.
        int num;
        /// Number of bytes in each write buffer.
        int nb;
        /// Index of the next write buffer to use.
        int first;
        /// Index of the oldest write buffer added.
        int last;
        /// Number of used buffers.
        int used;
        /// Pointer to the write buffers array.
        struct wb_entry* entries;
    } wb;

    /** @brief Allocates the write buffers in local memory (to simulate cache).
     * @param[in, out] wb Pointer to the write buffer list.
     * @param[in] num number of entries to allocate.
     * @param[in] nb number of bytes in each entry.
     * @return 0 if OK, different than 0 if error.
     */
    int
    mmu_wb_init(struct wb* wb, int num, int nb);


    /** @brief Frees the write buffers.
     * @param[in, out] wb Pointer to the write buffer list.
     */
    void
    mmu_wb_exit(struct wb* wb);


    /** Put bytes in a write buffer.
     * @param[in, out] wb Pointer to the write buffer list.
     * @param[in] pa physical address of the write.
     * @param[in] data Pointer to the data to write.
     * @param[in] n Number of bytes to write.
     *
     * @warning Write buffer merge is not implemented, can be done later.
     */
    void
    mmu_wb_write_bytes(struct wb* wb, uint32_t pa, uint8_t* data, int n);


    /** Flushes all write buffers.
     * @param[in, out] wb Pointer to the write buffer list.
     */
    void
    mmu_wb_drain_all(struct wb* wb);


    /// Read buffer types.
    enum rb_type_t
    {
        /// Invalid type.
        RB_INVALID = 0,
        /// Single word type.
        RB_1,
        /// Quad word type.
        RB_4,
        /// Octo word type.
        RB_8
    };

    /// Read buffer descriptor.
    struct rb_entry
    {
        /// Storage (maximum number of words in read block = 8).
        uint32_t data[8];
        /// Virtual address of the first word.
        uint32_t va;
        /// Type of the read buffer.
        int type;
        /// Potential fault.
        enum fault_t fault;
    };

    /// Read buffer list.
    struct rb
    {
        /// Number of read buffers.
        int num;
        /// Read buffers array.
        struct rb_entry* entries;
    };

    /** Initialize the read buffer list and allocate the read buffers.
     * @param[in, out] rb Pointer to the read buffer list.
     * @param[in] num Number of read buffers to allocate.
     * @return 0 if OK, different than 0 if error.
     */
    int
    mmu_rb_init(struct rb* rb, int num);

    /** Frees the read buffer list.
     * @param[in, out] rb Pointer to the read buffer list.
     */
    void
    mmu_rb_exit(struct rb* rb);

    /** Search for a read block matching a virtual address.
     * @param[in] rb Pointer to the read buffer list.
     * @param[in] num Number of read buffers to allocate.
     * @return The pointer to the read buffer descriptor or NULL if no match found.
     */
    struct rb_entry*
    mmu_rb_search(struct rb* rb, uint32_t va);

    /** Invalidate a read buffer.
     * @param[in, out] rb Pointer to the read buffer list.
     * @param[in] i Index of the read buffer to invalidate.
     */
    void
    mmu_rb_invalidate_entry(struct rb* rb, int i);

    /** Invalidate all read buffers.
     * @param[in, out] rb Pointer to the read buffer list.
     */
    void
    mmu_rb_invalidate_all(struct rb* rb);

    /** Invalidate all read buffers.
     * @param[in, out] rb Pointer to the read buffer list.
     * @param[in] tlb Pointer to the considered TLB list.
     * @param[in] i_rb Index of the read buffer to read into.
     * @param[in] type Type of the read buffer to read into.
     * @param[in] va Virtual address to load.
     */
    void
    mmu_rb_load(struct rb* rb, struct tlb* tlb, int i_rb, int type, uint32_t va);

    /// Cache line descriptor.
    struct cache_line
    {
        /** Cache line tag:
         *   - bit[31..3] : align address
         *   - bit[2]     : last half dirty
         *   - bit[1]     : first half dirty
         *   - bit[0]     : cache valid flag
         */
        uint32_t tag;
        /// Physical address.
        uint32_t pa;
        /// Cached data buffer.
        uint32_t* data;
    };

    /// Cache lines set descriptor.
    struct cache_set
    {
        /// Array of cache lines.
        struct cache_line* lines;
        /// Current cache set cycle.
        int cycle;
    };

    /// Full cache descriptor.
    struct cache
    {
        /// Number of bytes in a cache line.
        int width;
        /// Way of set association.
        int way;
        /// Number of sets.
        int set;
        /// Write mode.
        enum write_mode w_mode;
        /// Cache sets array.
        struct cache_set* sets;
    };


    int
    mmu_cache_init(struct cache* cache_t, int width, int way, int set, enum write_mode w_mode);

    void
    mmu_cache_exit(struct cache* cache_t);

    struct cache_line*
    mmu_cache_search(struct cache* cache_t, uint32_t va);

    struct cache_line*
    mmu_cache_search_by_index(struct cache* cache_t, uint32_t index);

    struct cache_line*
    mmu_cache_alloc(struct cache* cache_t, uint32_t va, uint32_t pa);

    void
    mmu_cache_write_back(struct cache* cache_t, struct cache_line* cache);

    void
    mmu_cache_clean(struct cache* cache_t, uint32_t va);

    void
    mmu_cache_clean_by_index(struct cache* cache_t, uint32_t index);

    void
    mmu_cache_invalidate(struct cache* cache_t, uint32_t va);

    void
    mmu_cache_invalidate_by_index(struct cache* cache_t, uint32_t index);

    void
    mmu_cache_invalidate_all(struct cache* cache_t);

    void
    mmu_cache_soft_flush(struct cache* cache_t, uint32_t pa);

    /// Implementation of virtual function
    void
    arm_exec_cycles(int cycles);


    /// MMU control register
    uint32_t control;
    /// MMU translation table base address
    uint32_t translation_table_base;
    /// MMU domain access control
    uint32_t domain_access_control;
    /// MMU fault status register
    uint32_t fault_status;
    /// MMU fault address register
    uint32_t fault_address;
    uint32_t last_domain;
    uint32_t fcse_id;
    uint32_t cache_locked_down;
    uint32_t tlb_locked_down;

    /// Bus interface
    struct bus m_bus;
};



#endif /*MMU_H_*/
