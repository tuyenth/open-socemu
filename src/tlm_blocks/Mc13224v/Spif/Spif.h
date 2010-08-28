#ifndef SPIF_H_
#define SPIF_H_

// this is a peripheral
#include "Peripheral.h"

// that generates an interrupt
#include "IntMaster.h"

// include the registers definition
#include "reg_spif.h"

/// Interrupt Controller block model
struct Spif : Peripheral<REG_SPIF_COUNT>
{
    // Not necessary if this module does not have a thread
    SC_HAS_PROCESS(Spif);

    /// Constructor
    Spif(sc_core::sc_module_name name, uint8_t *flashdata, uint32_t flashsize)
    : Peripheral<REG_SPIF_COUNT>(name)
    {
        // initialized the register access
        reg_spif = &(m_reg[0]);

        // initialize the flash
        m_flash.data = flashdata;
        m_flash.size = flashsize;
        m_flash.opcode = 0x00;
        m_flash.active = false;

        // create threads
        SC_THREAD(thread_flash);
    }

    /// Source interrupt
    IntMaster interrupt;

private:
    /// Structure containing the information for the flash
    struct
    {
        /// Size of the flash
        uint32_t size;
        /// Flash contents
        uint8_t* data;

        /// Indicate that an access is ongoing
        bool active;
        /// Indicate the last opcode received (0x00 when it was reset)
        uint8_t opcode;
        /// Event used to hold the SPIF thread
        sc_core::sc_event event;
        /// Current address
        uint32_t curaddr;
    } m_flash;

    /// Module thread (responds to read and writes to the FLASH through the SPIF)
    void thread_flash(void);

    /// Check that interrupt status
    void check_int();

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

#endif /*SPIF_H_*/
