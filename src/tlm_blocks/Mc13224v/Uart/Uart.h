#ifndef UART_H_
#define UART_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

// include the registers definition
#include "reg_uart.h"

// include the queue element
#include <queue>

/// Interrupt Controller block model
struct Uart : sc_core::sc_module
{
    /// TLM-2 slave socket to handle bus accesses
    tlm_utils::simple_target_socket<Uart> reg_s_socket;

    /// TLM-2 master socket to set/clear INT signal
    tlm_utils::simple_initiator_socket<Uart> int_m_socket;

    // Not necessary if this module does not have a thread
    SC_HAS_PROCESS(Uart);

    /** Constructor.
     * @param instance Number of the instance (since block can be multiple)
     * @param name Name of the instance
     */

    Uart(int instance, sc_core::sc_module_name name);

    /// Module threads
    void thread_tx(void);
    void thread_rx(void);

    /// TLM-2 socket blocking method
    virtual void reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// TLM-2 socket non blocking path
    virtual tlm::tlm_sync_enum reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 socket debug path
    virtual unsigned int reg_s_transport_dbg(tlm::tlm_generic_payload& trans);

    /// Read access to the registers
    uint32_t reg_rd(uint32_t offset);

    /// Write access to the registers
    void reg_wr(uint32_t offset, uint32_t value);

    /// Check the interrupt status
    void check_int();

    /// Instance number
    int instance;

    /// Registers content
    uint32_t m_reg[REG_UART_COUNT];

    /// Indicate if busy for sanity check
    bool m_free;

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


    /// Generic payload transaction to use for INT requests
    tlm::tlm_generic_payload int_pl;
    /// Time object for delay to use for INT requests
    sc_core::sc_time int_delay;

};

#endif /*UART_H_*/
