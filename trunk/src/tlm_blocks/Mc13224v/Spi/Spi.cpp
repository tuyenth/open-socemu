#include "Spi.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_SPI 0

// Used to make direct register access
uint32_t* reg_spi;

// using this namespace to simplify streaming
using namespace std;

Spi::Spi(sc_core::sc_module_name name)
: reg_s_socket("reg_s_socket")
, int_m_socket("int_m_socket")
, m_free(true)
{
    // force the default values of the INT transaction
    int_pl.set_streaming_width(4);
    int_pl.set_byte_enable_ptr(0);
    int_pl.set_dmi_allowed(false);

    // initialized the register access
    reg_spi = &(m_reg[0]);

    // clear all the registers
    memset(m_reg, 0, sizeof(m_reg));

    // Register callbacks for incoming interface method calls
    reg_s_socket.register_b_transport(this, &Spi::reg_s_b_transport);
    reg_s_socket.register_nb_transport_fw(this, &Spi::reg_s_nb_transport_fw);
    reg_s_socket.register_transport_dbg(this, &Spi::reg_s_transport_dbg);

    // initialize the flash interface
    m_flash.active = false;

    // create threads
    SC_THREAD(thread_flash);
}

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

void Spi::reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_WORD_SANITY(trans);

    // retrieve the required parameters
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    if (trans.get_command() == tlm::TLM_READ_COMMAND)
    {
        *ptr = reg_rd(trans.get_address());
    }
    else
    {
        reg_wr(trans.get_address(), *ptr);
    }

    // there was no error in the processing
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

    return;
}

tlm::tlm_sync_enum Spi::reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}


unsigned int Spi::reg_s_transport_dbg(tlm::tlm_generic_payload& trans)
{
    // sanity check
    TLM_TRANS_SANITY(trans);

    // execute the debug command
    TLM_DBG_EXEC(trans, m_reg, sizeof(m_reg));
}

uint32_t Spi::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_SPI_COUNT);
    assert(m_free);

    // mark as busy
    m_free = false;

    // internal delay
    wait(100, sc_core::SC_NS);

    switch (index)
    {
    default:
        // read the register value
        result = this->m_reg[index];
        break;
    }

    // mark as free
    m_free = true;

    return result;
}

void Spi::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_SPI_COUNT);
    assert(m_free);

    // mark as busy
    m_free = false;

    // internal delay
    wait(100, sc_core::SC_NS);

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

    // mark as free
    m_free = true;
}

void Spi::check_int(void)
{
    if (spi_int_getf())
    {
        TLM_INT_SET(int_m_socket, int_pl, int_delay);
    }
    else
    {
        TLM_INT_CLR(int_m_socket, int_pl, int_delay);
    }
}
