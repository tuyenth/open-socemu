#ifndef SPI_H_
#define SPI_H_

// this is a peripheral
#include "Generic/Peripheral/Peripheral.h"

// that generates an interrupt
#include "Generic/IntMaster/IntMaster.h"

// include the registers definition
#include "reg_spi.h"

/// Interrupt Controller block model
struct Spi : Peripheral<REG_SPI_COUNT>
{
    // Not necessary if this module does not have a thread
    SC_HAS_PROCESS(Spi);

    /// Constructor
    Spi(sc_core::sc_module_name name)
    : Peripheral<REG_SPI_COUNT>(name)
    {
        // initialized the register access
        reg_spi = &(m_reg[0]);

        // initialize the flash interface
        m_flash.active = false;

        // create threads
        SC_THREAD(thread_flash);
    }

    /// Interrupt
    IntMaster interrupt;

private:
    /// Structure containing the information for the flash
    struct
    {
        /// Indicate that an access is ongoing
        bool active;
        /// Event used to hold the SPI thread
        sc_core::sc_event event;
    } m_flash;

    /// Module thread (responds to read and writes to the FLASH through the SPI)
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

#endif /*SPI_H_*/
