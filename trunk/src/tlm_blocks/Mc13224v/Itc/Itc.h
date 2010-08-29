#ifndef ITC_H_
#define ITC_H_

// this is a peripheral
#include "Peripheral.h"

// with slave and master interrupts
#include "IntMaster.h"
#include "IntSlave.h"

// include the registers definition
#include "reg_itc.h"

/// Interrupt Controller block model
struct Itc : Peripheral<REG_ITC_COUNT>
{
    enum
    {
        NUM_INT = 16
    };
    /// Constructor
    Itc(sc_core::sc_module_name name)
    : Peripheral<REG_ITC_COUNT>(name)
    {
        // initialized the register access
        reg_itc = &(m_reg[0]);

        for (int i = 0; i < NUM_INT; i++)
        {
            this->interrupts[i].init(this, &Itc::interrupt_set, &Itc::interrupt_clr, (void*)i);
        }
    }

    /// Slave interrupts array
    IntSlave<Itc> interrupts[NUM_INT];

    /// IRQ interrupt
    IntMaster irq;

    /// FIQ interrupt
    IntMaster fiq;

private:
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

    /// Check that interrupt status
    void
    check_int();

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

    /// End of elaboration:  unhooked interrupt should be connected
    void
    end_of_elaboration()
    {
        // initialize the slave interrupt
        for (int i = 0; i < NUM_INT; i++)
        {
            if (!this->interrupts[i].is_bound())
            {
                char txt[256];
                tlm_utils::simple_initiator_socket<Itc>* dummy_int_m_socket;

                sprintf(txt, "itc_dummy_int[%d]", i);
                dummy_int_m_socket = new tlm_utils::simple_initiator_socket<Itc>(txt);
                dummy_int_m_socket->bind(this->interrupts[i]);
            }
        }
    }
};
#endif /*ITC_H_*/
