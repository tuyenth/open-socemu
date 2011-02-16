#include "Pl190.h"
#include "utils.h"

/// Used to select debugging
#define DEBUG_PL190 0

// using this namespace to simplify streaming
using namespace std;

uint32_t
Pl190::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_PL190_COUNT);

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
Pl190::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_PL190_COUNT);

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

