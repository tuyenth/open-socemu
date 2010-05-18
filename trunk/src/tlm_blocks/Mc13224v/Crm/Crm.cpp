#include "Crm.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_CRM 0

// Used to make direct register access
uint32_t* reg_crm;

// using this namespace to simplify streaming
using namespace std;

Crm::Crm(sc_core::sc_module_name name)
: reg_s_socket("reg_s_socket")
, int_m_socket("int_m_socket")
, m_free(true)
{
    // force the default values of the INT transaction
    int_pl.set_streaming_width(4);
    int_pl.set_byte_enable_ptr(0);
    int_pl.set_dmi_allowed(false);

    // initialized the register access
    reg_crm = &(m_reg[0]);

    // clear all the registers
    memset(m_reg, 0, sizeof(m_reg));

    // Register callbacks for incoming interface method calls
    reg_s_socket.register_b_transport(this, &Crm::reg_s_b_transport);
    reg_s_socket.register_nb_transport_fw(this, &Crm::reg_s_nb_transport_fw);
    reg_s_socket.register_transport_dbg(this, &Crm::reg_s_transport_dbg);

}

void Crm::reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_WORD_SANITY(trans);

    // retrieve the required parameters
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    if (trans.get_command() == tlm::TLM_READ_COMMAND)
    {
        *ptr = reg_rd(trans.get_address());
    }
    else
    {
        reg_wr(trans.get_address(), *ptr);
    }

    // there was no error in the processing
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

    return;
}

tlm::tlm_sync_enum Crm::reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}


unsigned int Crm::reg_s_transport_dbg(tlm::tlm_generic_payload& trans)
{
    // sanity check
    TLM_TRANS_SANITY(trans);

    // execute the debug command
    TLM_DBG_EXEC_COPY_NORETURN(trans, m_reg, sizeof(m_reg));

    // check the interrupt status
    this->check_int();

    return __s;
}

uint32_t Crm::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_CRM_COUNT);
    assert(m_free);

    // mark as busy
    m_free = false;

    // internal delay
    wait(100, sc_core::SC_NS);

    switch (index)
    {
    default:
        // read the register value
        result = this->m_reg[index];
        break;
    }

    // check the interrupt status
    this->check_int();

    // mark as free
    m_free = true;

    return result;
}

void Crm::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_CRM_COUNT);
    assert(m_free);

    // mark as busy
    m_free = false;

    // internal delay
    wait(100, sc_core::SC_NS);

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

    // mark as free
    m_free = true;
}

void Crm::check_int(void)
{
    if (ext_wu_evt_getf() != 0)
    {
        TLM_INT_SET(int_m_socket, int_pl, int_delay);
    }
    else
    {
        TLM_INT_CLR(int_m_socket, int_pl, int_delay);
    }
}
