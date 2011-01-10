#include "Sri.h"
#include "utils.h"

/// Used to select debugging
#define DEBUG_SRI 0

// using this namespace to simplify streaming
using namespace std;

uint32_t
Sri::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_SRI_COUNT);

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
Sri::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_SRI_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case REG_DC_SRI_DS0:
        // check if there is an access ongoing
        if (m_reg[REG_DC_SRI_JTAG_ACCESS] & 1)
        {
            TLM_DBG("writing to REG_DC_SRI_DS0 while access ongoing 0x%x", value);
        }
        m_iswrite = true;
        m_reg[index] = value;
        break;
    case REG_DC_SRI_JTAG_ACCESS:
        // check if it is a start
        if (m_reg[REG_DC_SRI_JTAG_ACCESS] & 1)
        {
            TLM_DBG("writing to REG_DC_SRI_JTAG_ACCESS while access ongoing 0x%x", value);
        }
        else if (value & 1)
        {
            m_event.notify();
        }
        m_reg[index] = value;
        break;
    default:
        TLM_DBG("m_reg(0x%X) = 0x%02X", offset, value);
        m_reg[index] = value;
        break;
    }
}

void
Sri::thread_process()
{
    while (true)
    {
        // wait for an event
        sc_core::wait(m_event);

        // wait for some time
        sc_core::wait(10, sc_core::SC_US);
        m_iswrite = false;
        m_reg[REG_DC_SRI_JTAG_ACCESS] &= ~1;
    }
}
