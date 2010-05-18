#include "Gpio.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_GPIO 0

// Used to make direct register access
uint32_t* reg_gpio;

// using this namespace to simplify streaming
using namespace std;

Gpio::Gpio(sc_core::sc_module_name name)
: reg_s_socket("reg_s_socket")
, m_free(true)
{
    // initialized the register access
    reg_gpio = &(m_reg[0]);

    // clear all the registers
    memset(m_reg, 0, sizeof(m_reg));

    // Register callbacks for incoming interface method calls
    reg_s_socket.register_b_transport(this, &Gpio::reg_s_b_transport);
    reg_s_socket.register_nb_transport_fw(this, &Gpio::reg_s_nb_transport_fw);
    reg_s_socket.register_transport_dbg(this, &Gpio::reg_s_transport_dbg);

}

void Gpio::reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
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

tlm::tlm_sync_enum Gpio::reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}


unsigned int Gpio::reg_s_transport_dbg(tlm::tlm_generic_payload& trans)
{
    // sanity check
    TLM_TRANS_SANITY(trans);

    // execute the debug command
    TLM_DBG_EXEC(trans, m_reg, sizeof(m_reg));
}

uint32_t Gpio::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_GPIO_COUNT);
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

void Gpio::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_GPIO_COUNT);
    assert(m_free);

    // mark as busy
    m_free = false;

    // internal delay
    wait(100, sc_core::SC_NS);

    switch (index)
    {
    default:
        m_reg[index] = value;
        break;
    }

    // mark as free
    m_free = true;
}

