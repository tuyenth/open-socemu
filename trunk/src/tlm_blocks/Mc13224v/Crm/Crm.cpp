#include "Crm.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_CRM 0

// Used to make direct register access
uint32_t* reg_crm;

// using this namespace to simplify streaming
using namespace std;

unsigned int
Crm::reg_s_transport_dbg(tlm::tlm_generic_payload& trans)
{
    // sanity check
    TLM_TRANS_SANITY(trans);

    // execute the debug command
    TLM_DBG_EXEC_COPY_NORETURN(trans, m_reg, sizeof(m_reg));

    // check the interrupt status
    this->check_int();

    return __s;
}

uint32_t
Crm::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_CRM_COUNT);
    // internal delay
    this->delay();

    switch (index)
    {
    default:
        // read the register value
        result = m_reg[index];
        break;
    }

    // check the interrupt status
    this->check_int();

    return result;
}

void
Crm::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_CRM_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case MOD_STATUS_INDEX:
    case WU_COUNT_INDEX:
    case CAL_COUNT_INDEX:
        TLM_ERR("Write to CRM R/O register 0x%02X, data = 0x%08X", offset, value);
        break;
    case STATUS_INDEX:
    {
        uint32_t clearmask;
        clearmask = value&(CAL_DONE_BIT|EXT_WU_EVT_MASK|RTC_WU_EVT_BIT|DOZE_WU_EVT_BIT|
                HIB_WU_EVT_BIT|SLEEP_SYNC_BIT);

        // clear the bits set in the clear mask
        m_reg[index] &= ~clearmask;
        break;
    }

    default:
        m_reg[index] = value;
        break;
    }

    // check the interrupt status
    this->check_int();
}

void Crm::check_int(void)
{
    if (ext_wu_evt_getf() != 0)
    {
        this->interrupt.set();
    }
    else
    {
        this->interrupt.clear();
    }
}
