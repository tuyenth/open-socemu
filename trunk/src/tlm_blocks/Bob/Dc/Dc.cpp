#include "Dc.h"
#include "utils.h"

/// Used to select debugging
#define DEBUG_DC 0

// using this namespace to simplify streaming
using namespace std;

uint32_t
Dc::reg_rd(uint32_t offset)
{
    sc_core::sc_time sctime;
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_DC_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case REG_DC_NBTC_CLK:
        sctime = sc_core::sc_time_stamp();
        result = (sctime.value() / 312500) & 0x0FFFFFFF;
        TLM_DBG("nbtc_clk = 0x%08X", result);
        break;
    case REG_DC_NBTC_PCLK:
        sctime = sc_core::sc_time_stamp();
        result = ((sctime.value() % 312500) / 1000 ) & 0x1FF;
        TLM_DBG("nbtc_pclk = 0x%08X", result);
        break;
    default:
        // read the register value
        result = m_reg[index];
        TLM_DBG("m_reg[%d] = 0x%08X", index, result);
        break;
    }

    return result;
}

void
Dc::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_DC_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    default:
        TLM_DBG("m_reg(0x%X) = 0x%02X", offset, value);
        m_reg[index] = value;
        break;
    }
}

