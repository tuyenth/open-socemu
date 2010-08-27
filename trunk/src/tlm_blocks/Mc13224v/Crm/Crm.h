#ifndef CRM_H_
#define CRM_H_

// this is a peripheral
#include "Peripheral.h"

// with an interrupt
#include "IntMaster.h"

// include the registers definition
#include "reg_crm.h"

/// Interrupt Controller block model
struct Crm : Peripheral<REG_CRM_COUNT>
{
    /** Constructor
     * @param[in] name Name of the module
     */
    Crm(sc_core::sc_module_name name)
    : Peripheral<REG_CRM_COUNT>(name)
    {
        // initialized the register access
        reg_crm = &(m_reg[0]);
    }

    /** Set the status of the block
     * @param[in] status New status value
     */
    void
    set_status(uint32_t status)
    {
        m_reg[STATUS_INDEX] = status;
    }

    /// Interrupt source
    IntMaster interrupt;

private:
    /// Override the virtual function to be able to generate interrupt from debugger
    unsigned int
    Crm::reg_s_transport_dbg(tlm::tlm_generic_payload& trans);

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

    /// Check that interrupt status
    void check_int();
};

#endif /*CRM_H_*/
