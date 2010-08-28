#include "Uart.h"
#include "utils.h"

/// debug level
#define UART_DEBUG_LEVEL 0

/// Macro to print debug messages
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define UART_TLM_DBG(__l, __f, ...)                                                     \
    do {                                                                                \
        if (UART_DEBUG_LEVEL >= __l) {                                                  \
            TLM_DBG(__f, __VA_ARGS__);                                                  \
        }                                                                               \
    } while (false)

// using this namespace to simplify streaming
using namespace std;

char const*const uart_regnames[] =
{
    "UCON",
    "USTAT",
    "UDATA",
    "URxCON",
    "UTxCON",
    "UCTS",
    "UBR"
};

void
Uart::thread_tx(void)
{
    while (true)
    {
        uint32_t timeout;
        uint8_t c;

        // wait for the TX event
        sc_core::wait(m_tx.event);

        // sanity check: there must be elements to send
        assert(m_tx.fifo.size() != 0);

        // check: is the output enabled
        if (m_reg[UCON_INDEX] & TX_OEN_B_BIT)
            UART_TLM_DBG(0, "WARNING: character TX'd but TXD wire output disabled %d", 0);

        // pop the first element from the queue
        c = m_tx.fifo.front();
        m_tx.fifo.pop();

        if ((m_reg[UBR_INDEX] & UBRMOD_MASK) == 0)
            TLM_ERR("MOD=0, invalid UBR=0x%08X", m_reg[UBR_INDEX]);

        // wait for the time to TX the char over the wire
        if (m_reg[UCON_INDEX] & XTIM_BIT)
            timeout = (1000 * ((m_reg[UBR_INDEX] & UBRMOD_MASK) >> UBRMOD_LSB))/(3 * (((m_reg[UBR_INDEX] & UBRINC_MASK) >> UBRINC_LSB)+1));
        else
            timeout = (2000 * ((m_reg[UBR_INDEX] & UBRMOD_MASK) >> UBRMOD_LSB))/(3 * (((m_reg[UBR_INDEX] & UBRINC_MASK) >> UBRINC_LSB)+1));

        sc_core::wait(timeout, sc_core::SC_NS);

        // print character sent
        UART_TLM_DBG(0, "TX (%c)", c);

        // check the interrupt status
        this->check_int();

        // check if there are more bytes to send and if TX is enabled
        if (m_tx.fifo.size() && (m_reg[UCON_INDEX] & TXE_BIT))
            m_tx.event.notify(sc_core::SC_ZERO_TIME);
    }
}

void
Uart::thread_rx(void)
{
    while (true)
    {
        uint32_t timeout;
        uint8_t *c;

        // check if there are pending chars
        c = m_rx_eq.get_next_transaction();

        if (c != 0)
        {
            // check if the reception is enabled
            if (m_reg[UCON_INDEX] & RXE_BIT)
            {
                // wait for the time to RX the char over the wire
                if (m_reg[UCON_INDEX] & XTIM_BIT)
                    timeout = (1000 * ((m_reg[UBR_INDEX] & UBRMOD_MASK) >> UBRMOD_LSB))/(3 * (((m_reg[UBR_INDEX] & UBRINC_MASK) >> UBRINC_LSB)+1));
                else
                    timeout = (2000 * ((m_reg[UBR_INDEX] & UBRMOD_MASK) >> UBRMOD_LSB))/(3 * (((m_reg[UBR_INDEX] & UBRINC_MASK) >> UBRINC_LSB)+1));

                sc_core::wait(timeout, sc_core::SC_NS);

                // print character received
                UART_TLM_DBG(0, "RX (%c)", *c);

                // add the character to the reception FIFO
                m_rx.fifo.push(*c);

                // check the interrupt status
                this->check_int();
            }
            else
            {
                UART_TLM_DBG(0, "RX (%c): but RX not enabled", *c);
            }

            free(c);
        }
        else
        {
            sc_core::wait(m_rx_eq.get_event());
        }
    }
}

uint32_t
Uart::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_UART_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case UDATA_INDEX:
        if (!m_rx.fifo.empty())
        {
            result = (uint32_t)m_rx.fifo.front();
            m_rx.fifo.pop();
        }
        else
        {
            UART_TLM_DBG(0, "Reading UDATA while RX FIFO empty %d", 0);
            result = 0;
        }
        break;
    case URXCON_INDEX:
        // read the number of used bytes in the RX FIFO
        result = this->m_rx.fifo.size();
        break;
    case UTXCON_INDEX:
        // read the number of free bytes in the TX FIFO
        result = 32-this->m_tx.fifo.size();
        break;
    default:
        // read the register value
        result = this->m_reg[index];
        break;
    }

    UART_TLM_DBG(2, "RD(%s) : 0x%08X", uart_regnames[index], result);

    return result;
}

void
Uart::reg_wr(uint32_t offset, uint32_t value)
{
    double baudrate;
    uint32_t inc;
    uint32_t mod;

    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_UART_COUNT);

    // internal delay
    this->delay();

    UART_TLM_DBG(2, "WR(%s) : 0x%08X", uart_regnames[index], value);
    switch (index)
    {
    case UCON_INDEX:
        m_reg[index] = value;

        // check TX path
        if ((value & TXE_BIT) && m_tx.fifo.size())
            m_tx.event.notify();

        inc = (m_reg[UBR_INDEX] & UBRINC_MASK) >> UBRINC_LSB;
        mod = (m_reg[UBR_INDEX] & UBRMOD_MASK) >> UBRMOD_LSB;

        if (mod != 0)
        {
            // wait for the time to TX the char over the wire
            if (m_reg[UCON_INDEX] & XTIM_BIT)
                baudrate = (3000000 * (inc+1)) / mod;
            else
                baudrate = (1500000 * (inc+1)) / mod;

            UART_TLM_DBG(1, "writing UCON(0x%08X): UBR=0x%08X INC=0x%04X MOD=0x%04X -> baudrate=%f",
                    m_reg[UCON_INDEX], m_reg[UBR_INDEX], inc, mod, baudrate);
        }
        else
        {
            UART_TLM_DBG(1, "writing UCON(0x%08X): UBR=0x%08X MOD=0, baudrate not valid yet",
                    m_reg[UCON_INDEX], m_reg[UBR_INDEX]);
        }

        // check not supported features
        if (value & CONTX_BIT)
            UART_TLM_DBG(0, "Continuous TX feature not implemented 0x%08X", value);
        if (value & TST_BIT)
            UART_TLM_DBG(0, "TST feature not supported 0x%08X", value);
        break;
    case USTAT_INDEX:
        TLM_ERR("Write to UART R/O register 0x%02X, data = 0x%08X", offset, value);
        break;

    case UDATA_INDEX:
        if (!m_tx.fifo.size() < 32)
        {
            m_tx.fifo.push((uint8_t)(value & 0xFF));

            // check if TX is enabled
            if (m_reg[UCON_INDEX] & TXE_BIT)
                m_tx.event.notify();
        }
        else
            UART_TLM_DBG(0, "Pushing UDATA while TX FIFO full %d", 0);
        break;
    case URXCON_INDEX:
        // sanity check
        if (value > 31)
            TLM_ERR("Write to UART URXCON register 0x%02X, invalid data = 0x%08X", offset, value);
        m_reg[index] = value;
        break;
    case UTXCON_INDEX:
        // sanity check
        if (value > 31)
            TLM_ERR("Write to UART UTXCON register 0x%02X, invalid data = 0x%08X", offset, value);
        m_reg[index] = value;
        break;
    case UBR_INDEX:
        m_reg[index] = value;

        inc = (m_reg[UBR_INDEX] & UBRINC_MASK) >> UBRINC_LSB;
        mod = (m_reg[UBR_INDEX] & UBRMOD_MASK) >> UBRMOD_LSB;

        if (mod != 0)
        {
            // wait for the time to TX the char over the wire
            if (m_reg[UCON_INDEX] & XTIM_BIT)
                baudrate = (3000000 * (inc+1)) / mod;
            else
                baudrate = (1500000 * (inc+1)) / mod;

            UART_TLM_DBG(1, "writing UBR(0x%08X): UCON=0x%08X INC=0x%04X MOD=0x%04X -> baudrate %f",
                    m_reg[UBR_INDEX], m_reg[UCON_INDEX], inc, mod, baudrate);
        }
        else
        {
            TLM_ERR("writing UBR(0x%08X): setting MOD to 0, invalid", m_reg[UBR_INDEX]);
        }
        break;

    default:
        m_reg[index] = value;
        break;
    }

    // check the interrupt status
    this->check_int();
}

void
Uart::check_int(void)
{

    // check TX interrupt
    if (m_reg[UCON_INDEX] & MTXR_BIT)
    {
        if ((32-m_tx.fifo.size()) >= m_reg[UTXCON_INDEX])
        {
            this->interrupt.set();
            return;
        }
    }

    // check RX interrupt
    if (m_reg[UCON_INDEX] & MRXR_BIT)
    {
        if (m_rx.fifo.size() >= m_reg[URXCON_INDEX])
        {
            this->interrupt.set();
            return;
        }
    }
    // if we reached here it means that there are no interrupts pending
    this->interrupt.clear();
}
