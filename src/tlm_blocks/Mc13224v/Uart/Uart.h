#ifndef UART_H_
#define UART_H_

// this is a peripheral
#include "Generic/Peripheral/Peripheral.h"

// that generates an interrupt
#include "Generic/IntMaster/IntMaster.h"

// include the registers definition
#include "reg_uart.h"

// include the queue element
#include <queue>

/// Interrupt Controller block model
struct Uart : Peripheral<REG_UART_COUNT>
{
    // Not necessary if this module does not have a thread
    SC_HAS_PROCESS(Uart);

    /** Constructor
     * @param instance Number of the instance (since block can be multiple)
     * @param name Name of the instance
     */

    Uart(sc_core::sc_module_name name, int instance)
    : Peripheral<REG_UART_COUNT>(name)
    , interrupt("interrupt")
    , instance(instance)
    , m_rx_eq("rx_eq")
    {
        // create threads
        SC_THREAD(thread_tx);
        SC_THREAD(thread_rx);
    }

    /// Source interrupt
    IntMaster interrupt;

private:
    /// Instance number
    int instance;

    /// Structure containing the information for the TX path
    struct
    {
        /// Event used to hold the UART TX thread
        sc_core::sc_event event;

        /// FIFO
        std::queue<uint8_t> fifo;
    } m_tx;

    /// Structure containing the information for the RX path
    struct
    {
        /// FIFO
        std::queue<uint8_t> fifo;
    } m_rx;

    /// Event queue containing the received chars
    tlm_utils::peq_with_get<uint8_t> m_rx_eq;

    /// Module threads
    void thread_tx(void);
    void thread_rx(void);

    /// Check the interrupt status
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

#endif /*UART_H_*/
