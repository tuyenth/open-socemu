#include "B2070_Cr.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_B2070_CR 0

// using this namespace to simplify streaming
using namespace std;

uint32_t
B2070_Cr::reg_rd(uint32_t offset)
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

void
B2070_Cr::reg_wr(uint32_t offset, uint32_t value)
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

