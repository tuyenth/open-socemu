#include "B2070_Cr.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_B2070_CR 0

// using this namespace to simplify streaming
using namespace std;

B2070_Cr::B2070_Cr(sc_core::sc_module_name name) : SimpleSlave(name, m_reg, REG_CR_COUNT*4)
{
}

void B2070_Cr::slave_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
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


uint32_t B2070_Cr::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_CR_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    default:
        // read the register value
        result = this->m_reg[index];
        break;
    }

    return result;
}

void B2070_Cr::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_CR_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    default:
        m_reg[index] = value;
        break;
    }
}

