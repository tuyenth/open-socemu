#include "Smc.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_SMC 0

// Used to make direct register access
uint32_t* reg_smc;

// using this namespace to simplify streaming
using namespace std;

/** Register read function
 * @param[in] offset Offset of the register to read
 * @return The value read
 */
uint32_t
Smc::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_SMC_COUNT);

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

/** Register write function
 * @param[in] offset Offset of the register to read
 */
void
Smc::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_SMC_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    default:
        m_reg[index] = value;
        break;
    }
}

void
Smc::bus_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
#if DEBUG_SMC
    cout << this->name() << ": bus_s_b_transport received addr=0x"<< hex << trans.get_address() << " at " << sc_core::sc_time_stamp() << endl;
#endif

    
    // Forward path
    sc_dt::uint64 address = trans.get_address();
    sc_dt::uint64 masked_address = address & 0x0FFFFFFF;
    
    // Modify address within transaction
    trans.set_address( masked_address );

    // Forward transaction to slave target
    bus_m_socket->b_transport(trans, delay);

    // Replace original address
    trans.set_address( address );
    
    return;
}

tlm::tlm_sync_enum
Smc::bus_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}


unsigned int
Smc::bus_s_transport_dbg(tlm::tlm_generic_payload& trans)
{
#if DEBUG_SMC
    cout << this->name() << ": bus_s_b_transport received addr=0x"<< hex << trans.get_address() << " at " << sc_core::sc_time_stamp() << endl;
#endif

    
    // Forward path
    sc_dt::uint64 address = trans.get_address();
    sc_dt::uint64 masked_address = address & 0x0FFFFFFF;
    
    // Modify address within transaction
    trans.set_address( masked_address );

    // Forward transaction to slave target
    return bus_m_socket->transport_dbg(trans);
}

