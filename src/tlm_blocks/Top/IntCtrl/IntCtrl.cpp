#include "IntCtrl.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_INTCTRL 0

// using this namespace to simplify streaming
using namespace std;

void
IntCtrl::int_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_INT_SANITY(trans);

    // retrieve the required parameters
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    if (*ptr == 1)
    {
        this->m_reg[REG_INTCTRL_IRQST_RAW] |= 1;
        if (this->m_reg[REG_INTCTRL_IRQST_RAW] != 0)
        {
            TLM_INT_SET(irq_socket, irq_pl, irq_delay);
        }
    }
    else
    {
        this->m_reg[REG_INTCTRL_IRQST_RAW] &= ~1;
        if (this->m_reg[REG_INTCTRL_IRQST_RAW] == 0)
        {
            TLM_INT_CLR(irq_socket, irq_pl, irq_delay);
        }
    }

    // there was no error in the processing
    trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

uint32_t
IntCtrl::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_INTCTRL_COUNT);

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

void
IntCtrl::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_INTCTRL_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case REG_INTCTRL_WFI:
        // WFI
        wait();
        break;
    default:
        m_reg[index] = value;
        break;
    }
}
