#include "Phy.h"
#include "utils.h"

/// Used to select debugging
#define DEBUG_PHY 0

// using this namespace to simplify streaming
using namespace std;

uint32_t
Phy::reg_rd(uint32_t offset)
{
    sc_core::sc_time sctime;
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_PHY_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case REG_PHY_DC_NBTC_CLK:
        sctime = sc_core::sc_time_stamp();
        result = (sctime.value() / 312500) & 0x0FFFFFFF;
        TLM_DBG("nbtc_clk = 0x%08X", result);
        break;
    case REG_PHY_DC_NBTC_PCLK:
        sctime = sc_core::sc_time_stamp();
        result = ((sctime.value() % 312500) / 1000 ) & 0x1FF;
        TLM_DBG("nbtc_pclk = 0x%08X", result);
        break;
    default:
        // read the register value
        result = m_reg[index];
        break;
    }

    return result;
}

void
Phy::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_PHY_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case REG_PHY_DC_SRI_DS0:
        // check if there is an access ongoing
        if (m_reg[REG_PHY_DC_SRI_JTAG_ACCESS] & 1)
        {
            TLM_DBG("writing to DC_SRI_DS0 while access ongoing 0x%x", value);
        }
        m_sriwrite = true;
        m_reg[index] = value;
        break;
    case REG_PHY_DC_SRI_JTAG_ACCESS:
        // check if it is a start
        if (m_reg[REG_PHY_DC_SRI_JTAG_ACCESS] & 1)
        {
            TLM_DBG("writing to DC_SRI_JTAG_ACCESS while access ongoing 0x%x", value);
        }
        else if (value & 1)
        {
            m_srievent.notify();
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
Phy::sri_process()
{
    while (true)
    {
        // wait for an event
        sc_core::wait(m_srievent);

        // wait for some time
        sc_core::wait(10, sc_core::SC_US);
        m_sriwrite = false;
        m_reg[REG_PHY_DC_SRI_JTAG_ACCESS] &= ~1;
    }
}
