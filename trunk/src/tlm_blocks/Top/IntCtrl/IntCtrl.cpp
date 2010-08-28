#include "IntCtrl.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_INTCTRL 0

// using this namespace to simplify streaming
using namespace std;


void
IntCtrl::interrupt_set(void* opaque)
{
    m_reg[REG_INTCTRL_IRQST_RAW] |= 1;
    if (m_reg[REG_INTCTRL_IRQST_RAW] != 0)
    {
        this->irq.set();
    }
}

void
IntCtrl::interrupt_clr(void* opaque)
{
    m_reg[REG_INTCTRL_IRQST_RAW] &= ~1;
    if (m_reg[REG_INTCTRL_IRQST_RAW] == 0)
    {
        this->irq.clear();
    }
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
        result = m_reg[index];
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
