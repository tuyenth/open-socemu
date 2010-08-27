#include "Spi.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_SPI 0

// Used to make direct register access
uint32_t* reg_spi;

// using this namespace to simplify streaming
using namespace std;

void Spi::thread_flash(void)
{
    while (true)
    {
        int32_t bitcount;

        // wait for the action on the radio controller to be over
        sc_core::wait(m_flash.event);

        // save that there is no more access
        m_flash.active = false;

        // compute the number of extra bits after TX
        bitcount = spi_sck_count_getf() - spi_data_length_getf();

        // depend on the command type
        switch (spi_tx_data_get() >> 24)
        {
        case 0x90:
        case 0xAB:
            if (spi_data_length_getf() != 32)
            {
                TLM_ERR("READ ID command not correctly written 0x%X",
                        spi_data_length_getf());
            }

            // add the corresponding bits
            spi_rx_data_set(0xBF02BF02 >> (31-bitcount));
            break;

        default:
            TLM_DBG("Unknown FLASH command 0x%02X", spi_tx_data_get() >> 28);
            break;
        }

        // set the SPI interrupt bit
        spi_int_setf(1);

        // check the interrupt status
        this->check_int();
    }
}

uint32_t
Spi::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_SPI_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    default:
        // read the register value
        result = m_reg[index];
        break;
    }
    return result;
}

void
Spi::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_SPI_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case SPI_RX_DATA_INDEX:
        TLM_ERR("Write to SPI R/O register 0x%02X, data = 0x%08X", offset, value);
        break;

    case SPI_CLK_CTRL_INDEX:
        // save the register value except the START bit
        m_reg[index] = value & ~(SPI_START_BIT);

        // check if there was a start request
        if (value & SPI_START_BIT)
        {
            // sanity check that there is no other ongoing access
            if (m_flash.active)
            {
                TLM_ERR("Starting SPI access while previous not over 0x%08X", value);
            }

            // make sure that the flash is selected (either forced low or auto low)
            if (spi_ss_setup_getf() != 1 && spi_ss_setup_getf() != 2)
            {
                TLM_ERR("Starting SPI access while FLASH not selected 0x%08X", value);
            }

            // make sure that the number of clock cycles is enough
            if ((spi_sck_count_getf()+1) < spi_data_length_getf())
            {
                TLM_ERR("Starting SPI access without enough CLK for the TX DATA %d < %d",
                        spi_sck_count_getf(), spi_data_length_getf());
            }

            // clear the pending interrupt done bit
            spi_int_setf(0);

            // save that there is an active access request
            m_flash.active = true;

            // set the event to wake up the thread at the end of the access
            m_flash.event.notify(spi_sck_count_getf() * 83, sc_core::SC_NS);
        }
        break;

    case SPI_STATUS_INDEX:
        // clear the pending status bits
        m_reg[index] &= ~(value & (SPI_INT_BIT | SPI_OVERFLOW_BIT));
        break;

    default:
        m_reg[index] = value;
        break;
    }

    // check the interrupt status
    this->check_int();

}

void Spi::check_int(void)
{
    if (spi_int_getf())
    {
        this->interrupt.set();
    }
    else
    {
        this->interrupt.clear();
    }
}
