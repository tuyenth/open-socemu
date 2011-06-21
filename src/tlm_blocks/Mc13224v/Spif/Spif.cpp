#include "Spif.h"
#include "utils.h"

// for the htonl byte swapping operation
#include <arpa/inet.h>


/// Used to select debugging
#define SPIF_DEBUG_LEVEL 0

/// Macro to print debug messages
/// @param __l level of debug message (0 means always printed)
/// @param __f format of the debug string
/// @param ... variable arguments
#define SPIF_TLM_DBG(__l, __f, ...)                                                     \
    do {                                                                                \
        if (SPIF_DEBUG_LEVEL >= __l) {                                                  \
            TLM_DBG(__f, ##__VA_ARGS__);                                                \
        }                                                                               \
    } while (false)

// Used to make direct register access
uint32_t* reg_spif;

// using this namespace to simplify streaming
using namespace std;


void
Spif::thread_flash(void)
{
    while (true)
    {
        int32_t bitcount;

        // wait for the action on the radio controller to be over
        sc_core::wait(m_flash.event);

        // check if the opcode was previously received
        if (m_flash.opcode == 0)
        {
            // retrieve the opcode
            m_flash.opcode = (uint8_t)(spif_tx_data_get() >> 24);

            SPIF_TLM_DBG(2, "new flash opcode received: 0x%02X", m_flash.opcode);
            // depending on the opcode, the verification is not the same
            switch (m_flash.opcode)
            {
            case 0x90:
            case 0xAB:
                // check that the right number of bits were transfered for the opcode
                if (spif_data_length_getf() != 32)
                {
                    TLM_ERR("READ ID command not correctly sent 0x%X",
                            spif_data_length_getf());
                }
                SPIF_TLM_DBG(1, "flash read ID");
                break;

            case 0x03:
                // check that the right number of bits were transfered for the opcode
                if (spif_data_length_getf() != 32)
                {
                    TLM_ERR("READ command not correctly written 0x%X",
                            spif_data_length_getf());
                }
                m_flash.curaddr = spif_tx_data_get() & 0x00FFFFFF;

                if (m_flash.curaddr & 3)
                {
                    TLM_ERR("Read address not aligned 0x%08X", m_flash.curaddr);
                }
                SPIF_TLM_DBG(1, "flash read from address 0x%08X", m_flash.curaddr);
                break;
            default:
                TLM_DBG("Unknown FLASH opcode 0x%02X", m_flash.opcode);
                break;
            }
            // compute the number of extra bits after TX
            bitcount = spif_sck_count_getf() + 1 - spif_data_length_getf();
        }
        else
        {
            bitcount = spif_sck_count_getf() + 1;
        }

        if (bitcount > 32)
        {
            TLM_ERR("RX bitcount larger than 32 (%u)", bitcount);
        }

        SPIF_TLM_DBG(1, "flash access bitcount = %u", bitcount);

        // depend on the command type
        switch (m_flash.opcode)
        {
        case 0x90:
        case 0xAB:
            // set the response
            spif_rx_data_set(0xBF02BF02 >> (32-bitcount));
            break;

        case 0x03:
            // set the response (since architecture is LE, swap the word to start with
            // the lowest address
            spif_rx_data_set((bitcount==0)?0:htonl(U32(m_flash.data[m_flash.curaddr])) >> (32-bitcount));
            SPIF_TLM_DBG(2, "read @%06X -> 0x%08X", m_flash.curaddr, spif_rx_data_get());
            m_flash.curaddr += bitcount/8;
            break;

        default:
            TLM_DBG("Unsupported access (opcode=0x%02X)", m_flash.opcode);
            break;
        }

        // save that there is no more access
        m_flash.active = false;

        // if the slave is automatically unselected, it falls back in reset
        if (spif_ss_setup_getf() < 2)
        {
            m_flash.opcode = 0;
        }

        // set the SPI interrupt bit
        spif_int_setf(1);

        // check the interrupt status
        this->check_int();
    }
}

uint32_t
Spif::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_SPIF_COUNT);

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
Spif::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_SPIF_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case SPIF_RX_DATA_INDEX:
        TLM_ERR("Write to SPIF R/O register 0x%02X, data = 0x%08X", offset, value);
        break;

    case SPIF_CLK_CTRL_INDEX:
        // save the register value except the START bit
        m_reg[index] = value & ~(SPIF_START_BIT);

        // check if there was a start request
        if (value & SPIF_START_BIT)
        {
            // sanity check that there is no other ongoing access
            if (m_flash.active)
            {
                TLM_ERR("Starting SPIF access while previous not over 0x%08X", value);
            }

            // make sure that the flash is selected (either forced low or auto low)
            if (spif_ss_setup_getf() != 1 && spif_ss_setup_getf() != 2)
            {
                TLM_ERR("Starting SPIF access while FLASH not selected 0x%08X", value);
            }

            // make sure that the number of clock cycles is enough
            if ((spif_sck_count_getf()+1) < spif_data_length_getf())
            {
                TLM_ERR("Starting SPIF access without enough CLK for the TX DATA %d < %d",
                        spif_sck_count_getf(), spif_data_length_getf());
            }

            // clear the pending interrupt done bit
            spif_int_setf(0);

            // save that there is an active access request
            m_flash.active = true;

            // set the event to wake up the thread at the end of the access
            m_flash.event.notify(spif_sck_count_getf() * 83, sc_core::SC_NS);
        }
        break;

    case SPIF_SETUP_INDEX:
        m_reg[index] = value;

        // if the SS is forced deasserted, the opcode is reset
        if (spif_ss_setup_getf() == 3)
        {
            m_flash.opcode = 0;
        }
        break;

    case SPIF_STATUS_INDEX:
        // clear the pending status bits
        m_reg[index] &= ~(value & (SPIF_INT_BIT | SPIF_OVERFLOW_BIT));
        break;

    default:
        m_reg[index] = value;
        break;
    }

    // check the interrupt status
    this->check_int();

}

void
Spif::check_int(void)
{
    if (spif_int_getf())
    {
        this->interrupt.set();
    }
    else
    {
        this->interrupt.clear();
    }
}
