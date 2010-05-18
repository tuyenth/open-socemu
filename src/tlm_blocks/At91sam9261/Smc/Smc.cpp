#include "Smc.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_SMC 0

// Used to make direct register access
uint32_t* reg_smc;

// using this namespace to simplify streaming
using namespace std;

Smc::Smc(sc_core::sc_module_name name)
: reg_s_socket("reg_s_socket")
, bus_s_socket("bus_s_socket")
, m_free(true)
{
    // initialized the register access
    reg_smc = &(m_registers[0]);
    
    // clear all the registers
    memset(m_registers, 0, sizeof(m_registers));
    
    // Register callbacks for incoming interface method calls
    reg_s_socket.register_b_transport(this, &Smc::reg_s_b_transport);
    reg_s_socket.register_nb_transport_fw(this, &Smc::reg_s_nb_transport_fw);
    reg_s_socket.register_transport_dbg(this, &Smc::reg_s_transport_dbg);

    bus_s_socket.register_b_transport(this, &Smc::bus_s_b_transport);
    bus_s_socket.register_nb_transport_fw(this, &Smc::bus_s_nb_transport_fw);
    bus_s_socket.register_transport_dbg(this, &Smc::bus_s_transport_dbg);

}

void Smc::reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_WORD_SANITY(trans);

    // retrieve the required parameters
    sc_dt::uint64 index = trans.get_address()/4;
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    // sanity check
    assert(index < REG_SMC_COUNT);
    assert(m_free);

    // mark as busy
    m_free = false;
    
    // internal delay
    wait(100, sc_core::SC_NS);
    
    if (trans.get_command() == tlm::TLM_READ_COMMAND)
    {
        *ptr = m_registers[index];
    }
    else
    {
        m_registers[index] = *ptr;
    }

    // there was no error in the processing
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

    // mark as free
    m_free = true;

    return;
}

tlm::tlm_sync_enum Smc::reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}


unsigned int Smc::reg_s_transport_dbg(tlm::tlm_generic_payload& trans)
{
    // sanity check
    TLM_TRANS_SANITY(trans);

    // execute the debug command
    TLM_DBG_EXEC(trans, m_registers, REG_SMC_COUNT*4);
}

void Smc::bus_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
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

tlm::tlm_sync_enum Smc::bus_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}


unsigned int Smc::bus_s_transport_dbg(tlm::tlm_generic_payload& trans)
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

