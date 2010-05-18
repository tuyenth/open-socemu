#include "Dummy.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_DUMMY 0

// using this namespace to simplify streaming
using namespace std;

Dummy::Dummy(sc_core::sc_module_name name)
: init_socket("init_socket")
, targ_socket("targ_socket")
, int_socket("int_socket")
, m_free(true)
{
    // force the default values of the INT transaction
    int_pl.set_streaming_width(4);
    int_pl.set_byte_enable_ptr(0);
    int_pl.set_dmi_allowed(false);

    // register callbacks for incoming interface method calls
    init_socket.register_nb_transport_bw(this, &Dummy::nb_transport_bw);

    // register callbacks for incoming interface method calls
    targ_socket.register_b_transport(this, &Dummy::b_transport);
    targ_socket.register_nb_transport_fw(this, &Dummy::nb_transport_fw);
    targ_socket.register_transport_dbg(  this, &Dummy::transport_dbg);

    // create thread
    SC_THREAD(thread_process);
}

void Dummy::thread_process()
{
    while (1)
    {
        // wait for an event
        wait();
    }
}


// TLM-2 blocking transport method
void Dummy::b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_WORD_SANITY(trans);

    // retrieve the required parameters
    sc_dt::uint64 index = trans.get_address()/4;
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    // sanity check
    assert(index < REG_SIZE);
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
        switch (index)
        {
        case REG_COMMAND:
            TLM_INT_SET(int_socket, int_pl, int_delay);
            break;
        case REG_ACK:
            TLM_INT_CLR(int_socket, int_pl, int_delay);
            break;
        }
    }
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

    // mark as free
    m_free = true;

    return;
}

/// TLM-2 non-blocking transport method.
tlm::tlm_sync_enum Dummy::nb_transport_fw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}

unsigned int Dummy::transport_dbg(tlm::tlm_generic_payload& trans)
{
    // sanity check
    TLM_TRANS_SANITY(trans);

    // execute the debug command
    TLM_DBG_EXEC(trans, m_registers, sizeof(m_registers));
}

tlm::tlm_sync_enum Dummy::nb_transport_bw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}


