#ifndef AIC_H_
#define AIC_H_

// this is a peripheral
#include "Peripheral.h"

// with 2 interrupt masters
#include "IntMaster.h"

// and 32 int slaves
#include "IntSlave.h"

// include the registers definition
#include "reg_aic.h"

/// Advanced Interrupt Controller block model
struct Aic : Peripheral<REG_AIC_COUNT>
{
    /// Constructor
    Aic(sc_core::sc_module_name name)
    : Peripheral<REG_AIC_COUNT>(name)
    {
        // initialize the register access
        reg_aic = &(m_reg[0]);

        // initialize the slave interrupt
        for (int i = 0; i < 32; i++)
        {
            interrupts[i].init(this, &Aic::interrupt_set, &Aic::interrupt_clr, (void*)i);
        }
    }

    /// TLM-2 slave sockets for interrupt sources (tagged to use only one callback)
    tlm_utils::simple_target_socket_tagged<Aic>* int_s_socket[32];

    /// IRQ interrupt
    IntMaster irq;

    /// FIQ interrupt
    IntMaster fiq;

    /// FIQ interrupt
    IntSlave<Aic> interrupts[32];

    /// Check the pending interrupts
    void check_int();

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
        for (int i = 0; i < 32; i++)
        {
            if (!this->interrupts[i].is_bound())
            {
                char txt[256];
                tlm_utils::simple_initiator_socket<Aic>* dummy_int_m_socket;

                sprintf(txt, "aic_dummy_int[%d]", i);
                dummy_int_m_socket = new tlm_utils::simple_initiator_socket<Aic>(txt);
                dummy_int_m_socket->bind(this->interrupts[i]);
            }
        }
    }
};

#endif /*AIC_H_*/
