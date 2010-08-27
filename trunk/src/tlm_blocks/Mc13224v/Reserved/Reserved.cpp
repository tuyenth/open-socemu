#include "Reserved.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_RESERVED 0

// Used to make direct register access
uint32_t* reg_reserved;

// using this namespace to simplify streaming
using namespace std;

uint32_t
Reserved::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_RESERVED_COUNT);

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
Reserved::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_RESERVED_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    default:
        m_reg[index] = value;
        break;
    }
}

