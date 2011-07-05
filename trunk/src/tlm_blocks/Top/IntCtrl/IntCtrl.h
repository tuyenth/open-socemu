#ifndef INTCTRL_H_
#define INTCTRL_H_

// this is  a peripheral
#include "Generic/Peripheral/Peripheral.h"

// with master and slave interrupts
#include "Generic/IntMaster/IntMaster.h"
#include "Generic/IntSlave/IntSlave.h"

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
    , fiq("fiq")
    , irq("irq")
    {
        // save the interrupt callbacks
        this->interrupt.init(this, &IntCtrl::interrupt_set, &IntCtrl::interrupt_clr, NULL);
    }

    /// Source interrupt
    IntSlave<IntCtrl> interrupt;

    /// FIQ
    IntMaster fiq;

    /// IRQ
    IntMaster irq;

private:
    /** Interrupt set handler
     * @param[in] opaque Pointer passed in parameter when registering
     */
    void
    interrupt_set(void* opaque);

    /** Interrupt clear handler
     * @param[in] opaque Pointer passed in parameter when registering
     */
    void
    interrupt_clr(void* opaque);

    /** Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset);

    /** Register write function
     * @param[in] offset Offset of the register to read
     * @param[in] offset Value to write in the register
     */
    void
    reg_wr(uint32_t offset, uint32_t value);
};

#endif /*INTCTRL_H_*/
