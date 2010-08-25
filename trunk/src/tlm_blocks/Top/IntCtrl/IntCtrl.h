#ifndef INTCTRL_H_
#define INTCTRL_H_

#include "Peripheral.h"

#include "tlm_utils/simple_initiator_socket.h"

    /// Registers definition
    enum
    {
        REG_INTCTRL_IRQEN_SET  = 0,
        REG_INTCTRL_IRQEN_CLR  = 1,
        REG_INTCTRL_IRQST_RAW  = 2,
        REG_INTCTRL_FIQST_RAW  = 3,
        REG_INTCTRL_WFI        = 4,

        REG_INTCTRL_COUNT      = 256
    };

struct IntCtrl : Peripheral<REG_INTCTRL_COUNT>
{
    /// Constructor
    IntCtrl(sc_core::sc_module_name name)
        : Peripheral<REG_INTCTRL_COUNT>(name)
        , int_socket("int_socket")
        , irq_socket("irq_socket")
        , fiq_socket("fiq_socket")
    {
        // force the default values of the FIQ transaction
        fiq_pl.set_streaming_width(4);
        fiq_pl.set_byte_enable_ptr(0);
        fiq_pl.set_dmi_allowed(false);
        // force the default values of the IRQ transaction
        irq_pl.set_streaming_width(4);
        irq_pl.set_byte_enable_ptr(0);
        irq_pl.set_dmi_allowed(false);
        // Register callbacks for incoming interface method calls
        int_socket.register_b_transport(this, &IntCtrl::int_b_transport);
    }

    // TLM-2 blocking transport method
    void
    int_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// Interrupt socket (only one interrupt source)
    tlm_utils::simple_target_socket<IntCtrl> int_socket;

    /// IRQ socket to set/clear in IRQ signal
    tlm_utils::simple_initiator_socket<IntCtrl> irq_socket;

    /// IRQ socket to set/clear in FIQ signal
    tlm_utils::simple_initiator_socket<IntCtrl> fiq_socket;

    /// Generic payload transaction to use for interrupt requests
    tlm::tlm_generic_payload fiq_pl;
    /// Time object for delay to use for interrupt requests
    sc_core::sc_time fiq_delay;
    /// Generic payload transaction to use for interrupt requests
    tlm::tlm_generic_payload irq_pl;
    /// Time object for delay to use for interrupt requests
    sc_core::sc_time irq_delay;

protected:
    /** Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset);

    /** Register write function
     * @param[in] offset Offset of the register to read
     */
    void
    reg_wr(uint32_t offset, uint32_t value);
};

#endif /*INTCTRL_H_*/
