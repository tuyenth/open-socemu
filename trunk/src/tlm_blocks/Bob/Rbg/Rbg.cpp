#include "Rbg.h"
#include "utils.h"

/// Used to select debugging
#define DEBUG_RBG 0

// using this namespace to simplify streaming
using namespace std;

uint32_t
Rbg::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_RBG_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case REG_RBG_RANDOM_NUM:
        // check if the peripheral was enabled
        if (!(m_reg[REG_RBG_CONTROL] & 1))
        {
            TLM_DBG("WARNING: reading out random number while peripheral disabled %d",
                    m_reg[REG_RBG_CONTROL]);
            return 0;
        }
        if (GETF(m_reg[REG_RBG_STATUS], 0xFFFFFF, 0) != 0xFFFFF)
        {
            TLM_DBG("WARNING: reading out random number while count warmup not complete (0x%X)",
                    m_reg[REG_RBG_STATUS]);
            return 0;
        }
        if (GETF(m_reg[REG_RBG_STATUS], 0xFF000000, 24) != 32)
        {
            TLM_DBG("WARNING: reading out random number while count did not reach 32 (0x%X)",
                    GETF(m_reg[REG_RBG_STATUS], 0xFF000000, 24));
            return 0;
        }
        // indicate to re-count the bit generation
        m_event.notify();
        // return a random number from the sequence
        return m_last_random++;
    default:
        // read the register value
        result = m_reg[index];
        break;
    }

    return result;
}

void
Rbg::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_RBG_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case REG_RBG_CONTROL:
        // check if there is an extra bit set
        if (value & ~3)
        {
            TLM_DBG("Extra unused bits (0x%X)", value);
            // clearing out unused bits
            value &= 3;
        }
        // check if the peripheral currently disabled
        if ((m_reg[REG_RBG_CONTROL] & 1) == 0)
        {
            // check if this is an enable command
            if (value & 1)
            {
                m_event.notify();
            }
        }
        else
        {
            // check if this is a disable command
            if ((value & 1) == 0)
            {
                if (GETF(m_reg[REG_RBG_STATUS], 0xFF000000, 24) != 32)
                {
                    TLM_DBG("WARNING: disabling peripheral while count did not reach 32 (0x%X)",
                            GETF(m_reg[REG_RBG_STATUS], 0xFF000000, 24));
                }
                m_event.notify();
            }
        }
        m_reg[index] = value;
        break;
    default:
        m_reg[index] = value;
        break;
    }
}

void
Rbg::thread_process()
{
    while (true)
    {
        // here we should be in disabled
        // wait for the event to start downcounting the warmup time
        sc_core::wait(m_event);

        // update the register status
        m_reg[REG_RBG_STATUS] = 0x40000;

        // count the warmup time
        while (m_reg[REG_RBG_STATUS] != 0xFFFFF)
        {
            sc_core::wait(635, sc_core::SC_NS);
            m_reg[REG_RBG_STATUS]++;
        }

        // as long as the peripheral is enabled
        while (m_reg[REG_RBG_CONTROL] & 1)
        {
            // start counting the bits
            m_reg[REG_RBG_STATUS] = 0xFFFFF;
            while (m_reg[REG_RBG_STATUS] != 0x200FFFFF)
            {
                sc_core::wait(1, sc_core::SC_US);
                m_reg[REG_RBG_STATUS] += 0x01000000;
            }
            // wait for an event
            sc_core::wait(m_event);
        }
    }
}
