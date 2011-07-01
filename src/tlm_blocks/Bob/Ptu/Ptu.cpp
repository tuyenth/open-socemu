#include "Ptu.h"
#include "utils.h"

/// Used to select debugging
#define DEBUG_PTU 0

// using this namespace to simplify streaming
using namespace std;

uint32_t
Ptu::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_PTU_COUNT);

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
Ptu::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_PTU_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    default:
        m_reg[index] = value;
        break;
    }
}

