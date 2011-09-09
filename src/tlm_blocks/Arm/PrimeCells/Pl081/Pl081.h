#ifndef PL081_H_
#define PL081_H_

/// ARM DMA controller IP

#include "utils.h"
#include "Generic/Peripheral/Peripheral.h"
#include "Generic/IntMaster/IntMaster.h"

/// Registers definition
enum
{
    REG_PL081_DMACINTSTAT,
    REG_PL081_DMACINTTCSTAT,
    REG_PL081_DMACINTTCCLR,
    REG_PL081_DMACINTERRSTAT,
    REG_PL081_DMACINTERRCLR,
    REG_PL081_DMACRAWINTC,
    REG_PL081_DMACRAWINTERR,
    REG_PL081_DMACENBLDCHNS,
    REG_PL081_DMACSOFTBREQ,
    REG_PL081_DMACSOFTSREQ,
    REG_PL081_DMACSOFTLBREQ,
    REG_PL081_DMACSOFTLSREQ,
    REG_PL081_DMACCONFIG,
    REG_PL081_DMACSYNC,
    REG_PL081_DMACC0SRCADDR = 0x100/4,
    REG_PL081_DMACC0DESTADDR,
    REG_PL081_DMACC0LLIREG,
    REG_PL081_DMACC0CONTROL,
    REG_PL081_DMACC0CONFIG,
    REG_PL081_DMACC1SRCADDR = 0x120/4,
    REG_PL081_DMACC1DESTADDR,
    REG_PL081_DMACC1LLIREG,
    REG_PL081_DMACC1CONTROL,
    REG_PL081_DMACC1CONFIG,
    REG_PL081_DMACC2SRCADDR = 0x140/4,
    REG_PL081_DMACC2DESTADDR,
    REG_PL081_DMACC2LLIREG,
    REG_PL081_DMACC2CONTROL,
    REG_PL081_DMACC2CONFIG,
    REG_PL081_DMACC3SRCADDR = 0x160/4,
    REG_PL081_DMACC3DESTADDR,
    REG_PL081_DMACC3LLIREG,
    REG_PL081_DMACC3CONTROL,
    REG_PL081_DMACC3CONFIG,
    REG_PL081_DMACC4SRCADDR = 0x180/4,
    REG_PL081_DMACC4DESTADDR,
    REG_PL081_DMACC4LLIREG,
    REG_PL081_DMACC4CONTROL,
    REG_PL081_DMACC4CONFIG,
    REG_PL081_DMACC5SRCADDR = 0x1a0/4,
    REG_PL081_DMACC5DESTADDR,
    REG_PL081_DMACC5LLIREG,
    REG_PL081_DMACC5CONTROL,
    REG_PL081_DMACC5CONFIG,
    REG_PL081_DMACC6SRCADDR = 0x1c0/4,
    REG_PL081_DMACC6DESTADDR,
    REG_PL081_DMACC6LLIREG,
    REG_PL081_DMACC6CONTROL,
    REG_PL081_DMACC6CONFIG,
    REG_PL081_DMACC7SRCADDR = 0x1e0/4,
    REG_PL081_DMACC7DESTADDR,
    REG_PL081_DMACC7LLIREG,
    REG_PL081_DMACC7CONTROL,
    REG_PL081_DMACC7CONFIG,
    REG_PL081_DMACTCR = 0x500/4,
    REG_PL081_DMACITOP1,
    REG_PL081_DMACITOP2,
    REG_PL081_DMACITOP3,
    REG_PL081_DMACPERIPHID0 = 0xfe0/4,
    REG_PL081_DMACPERIPHID1,
    REG_PL081_DMACPERIPHID2,
    REG_PL081_DMACPERIPHID3,
    REG_PL081_DMACPCELLID0,
    REG_PL081_DMACPCELLID1,
    REG_PL081_DMACPCELLID2,
    REG_PL081_DMACPCELLID3,
    REG_PL081_COUNT
};

struct Pl081 : Peripheral<REG_PL081_COUNT>
{
    /// Constructor
    Pl081(sc_core::sc_module_name name)
    : Peripheral<REG_PL081_COUNT>(name)
      , inttc("inttc")
      , interr("interr")
      , intr("intr")
    , master_socket("master_socket")
    {
        // force the default values of the BUS transaction
        master_b_pl.set_streaming_width(4);
        master_b_pl.set_byte_enable_ptr(0);
        master_b_pl.set_dmi_allowed(false);
        // register callbacks for incoming interface method calls
        master_socket.register_nb_transport_bw(this, &Pl081::master_nb_transport_bw);
        master_socket.register_invalidate_direct_mem_ptr(this, &Pl081::master_invalidate_direct_mem_ptr);

        // initialize the registers content
        m_reg[REG_PL081_DMACPERIPHID0] = 0x81;
        m_reg[REG_PL081_DMACPERIPHID1] = 0x10;
        m_reg[REG_PL081_DMACPERIPHID2] = 0x05;
        m_reg[REG_PL081_DMACPERIPHID3] = 0x00;
        m_reg[REG_PL081_DMACPCELLID0] = 0x0D;
        m_reg[REG_PL081_DMACPCELLID1] = 0xF0;
        m_reg[REG_PL081_DMACPCELLID2] = 0x05;
        m_reg[REG_PL081_DMACPCELLID3] = 0xB1;
    }

    /** Bind a slave socket to the local master socket
     * @param[in, out] slave_socket TLM-2 slave socket to bind to the master socket of the peripheral
     */
    void
    bind(tlm::tlm_target_socket<>& slave_socket)
    {
        // hook the slave socket
        this->master_socket.bind(slave_socket);
    }

    /// Terminal count interrupt
    IntMaster inttc;
    /// Error interrupt
    IntMaster interr;
    /// Combined Terminal count and error interrupt
    IntMaster intr;

private:
    /// Update the interrupts
    void
    update_int(void)
    { 
        // generate the interrupts from the raw status
        m_reg[REG_PL081_DMACINTTCSTAT] = m_reg[REG_PL081_DMACRAWINTC];
        if (m_reg[REG_PL081_DMACINTTCSTAT]) inttc.set();
        else                                inttc.clear();
        m_reg[REG_PL081_DMACINTERRSTAT] = m_reg[REG_PL081_DMACRAWINTERR];
        if (m_reg[REG_PL081_DMACINTERRSTAT]) interr.set();
        else                                 interr.clear();
        m_reg[REG_PL081_DMACINTSTAT] = m_reg[REG_PL081_DMACINTTCSTAT] | m_reg[REG_PL081_DMACINTERRSTAT];
        if (m_reg[REG_PL081_DMACINTSTAT]) intr.set();
        else                              intr.clear();
    }
    
    /// TLM-2 master socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_initiator_socket<Pl081> master_socket;

    /** Generic payload transaction to use for master blocking requests.  This is used
     * to speed up the simulation by not allocating dynamically a payload for
     * each blocking transaction.
     * @warn This prevents can only be used for blocking accesses
     */
    tlm::tlm_generic_payload master_b_pl;
    
    /** Time object for delay to use for master blocking requests.  This is used
     * to speed up the simulation by not allocating dynamically a time object for
     * each blocking transaction.
     * @warn This can only be used for blocking accesses
     */
    sc_core::sc_time master_b_delay;

    /** Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset)
    {
        uint32_t result;
        // retrieve the required parameters
        uint32_t index = offset/4;

        // sanity check
        assert(index < REG_PL081_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        case REG_PL081_DMACINTTCCLR:
        case REG_PL081_DMACINTERRCLR:
            TLM_ERR("read access to write-only register (%d)", index);
            break;
            
        default:
            // read the register value
            result = m_reg[index];
            break;
        }

        return result;
    }


    /** Register write function
     * @param[in] offset Offset of the register to read
     * @param[in] offset Value to write in the register
     */
    void
    reg_wr(uint32_t offset, uint32_t value)
    {
        // retrieve the required parameters
        uint32_t index = offset/4;

        // sanity check
        assert(index < REG_PL081_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        case REG_PL081_DMACINTTCCLR:
            m_reg[REG_PL081_DMACRAWINTC] &= ~value;
            break;
            
        case REG_PL081_DMACINTERRCLR:
            m_reg[REG_PL081_DMACRAWINTERR] &= ~value;
            break;
            
        case REG_PL081_DMACINTSTAT:
        case REG_PL081_DMACINTTCSTAT:
        case REG_PL081_DMACINTERRSTAT:
        case REG_PL081_DMACRAWINTC:
        case REG_PL081_DMACRAWINTERR:
        case REG_PL081_DMACENBLDCHNS:
        case REG_PL081_DMACPERIPHID0:
        case REG_PL081_DMACPERIPHID1:
        case REG_PL081_DMACPERIPHID2:
        case REG_PL081_DMACPERIPHID3:
        case REG_PL081_DMACPCELLID0:
        case REG_PL081_DMACPCELLID1:
        case REG_PL081_DMACPCELLID2:
        case REG_PL081_DMACPCELLID3:
            TLM_ERR("write access to read-only register (%d)", index);
            break;

        default:
            m_reg[index] = value;
            break;
        }
        this->update_int();
    }

    /** slave_socket non-blocking forward transport method (default behavior, can be overridden)
     * @param[in, out] trans Transaction payload object, allocated here, filled by target
     * @param[in, out] phase Phase payload object, allocated here
     * @param[in, out] delay Time object, allocated here, filled by target
     */
    virtual tlm::tlm_sync_enum
    master_nb_transport_bw(tlm::tlm_generic_payload& trans,
                           tlm::tlm_phase& phase, sc_core::sc_time& delay)
    {
        SC_REPORT_FATAL("TLM-2", "Non blocking not implemented");
        return tlm::TLM_COMPLETED;
    }

    /** master_socket tagged non-blocking forward transport method
     * @param[in] start_range Start address of the memory invalidate command
     * @param[in] end_range End address of the memory invalidate command
     */
    virtual void
    master_invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                     sc_dt::uint64 end_range)
    {
        SC_REPORT_FATAL("TLM-2", "DMI not implemented");
    }
};

#endif /*PL081_H_*/
