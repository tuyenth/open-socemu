#include "Pmu.h"
#include "utils.h"

/// Used to select debugging
#define DEBUG_PMU 0

// using this namespace to simplify streaming
using namespace std;

uint32_t
Pmu::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_PMU_COUNT);

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
Pmu::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_PMU_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case REG_CR_ERR_EN:
        m_reg[index] = value;
        if (value == 1)
        {
            m_reg[REG_CR_ERR_RESULT] = 0;
        }
        else if (value == 0)
        {
            m_reg[REG_CR_ERR_EN] = 1;
        }
        else
        {
            TLM_ERR("Unsupported value in CR_ERR_EN %d", value);
        }
        m_event.notify();
        break;
    default:
        TLM_DBG("m_reg(0x%X) = 0x%02X", offset, value);
        m_reg[index] = value;
        break;
    }
}

void
Pmu::thread_process()
{
    while (true)
    {
        // wait for an event
        sc_core::wait(m_event);

        if (m_reg[REG_CR_ERR_EN] == 1)
        {
            // start measurement
            // wait for some time
            sc_core::wait(10, sc_core::SC_US);
            m_reg[REG_CR_ERR_RESULT] = 1000;
        }
        else if (m_reg[REG_CR_ERR_EN] == 0)
        {
            // prepare for next measurement
            // wait for some time
            sc_core::wait(10, sc_core::SC_US);
            m_reg[REG_CR_ERR_EN] = 1;
        }
    }
}
