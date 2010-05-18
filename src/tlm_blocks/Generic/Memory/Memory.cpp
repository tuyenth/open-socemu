#include "Memory.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_MEMORY 0

// using this namespace to simplify streaming
using namespace std;

/// Helps converting partial accesses to masks
static const uint32_t s_bytemasks[5] = {0, 0xFF, 0xFFFF, 0, 0xFFFFFFFF};

Memory::Memory(sc_core::sc_module_name name, uint32_t* data, uint32_t size)
: socket("socket")
, m_size(size)
, m_free(true)
{
    // allocate the memory (for test purposes it is not initialized)
    this->m_data = data;

    // register callbacks for incoming interface method calls
    socket.register_b_transport(this, &Memory::b_transport);
    socket.register_nb_transport_fw(this, &Memory::nb_transport_fw);
    socket.register_transport_dbg(  this, &Memory::transport_dbg);

}


// TLM-2 blocking transport method
void Memory::b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    // sanity check
    TLM_TRANS_SANITY(trans);

    // retrieve the required parameters
    uint32_t length = trans.get_data_length();
    sc_dt::uint64 addr = trans.get_address();
    sc_dt::uint64 index = addr/4;
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());
    uint32_t mask, shift;

    // sanity check
    assert(length <= 4);
    assert(length > 0);
    assert(index < m_size/4);
    assert(m_free);
    // check that the length does not go beyond the word boundary
    assert(length <= (4 - (addr & 3)));
    assert((length == 1) || (length == 2) || (length == 4));

    // convert the length into a mask
    mask = s_bytemasks[length];
    shift = (addr & 3)*8;
    
    #if DEBUG_MEMORY
    cout << this->name() << ": b_transport received addr=0x"<< hex << trans.get_address() << " at " << sc_core::sc_time_stamp() << endl;
    #endif

    // mark as busy
    m_free = false;
    
    // internal delay
    wait(100, sc_core::SC_NS);
    
    if (trans.get_command() == tlm::TLM_READ_COMMAND)
    {
        *ptr = (m_data[index] >> shift) & mask;
    }
    else
    {
        uint32_t data = (m_data[index] & (~(mask << shift)));

        // set the data
        data |= ((*ptr) & mask) << shift;

        m_data[index] = data;
    }
    
    // there was no error in the processing
    trans.set_response_status(tlm::TLM_OK_RESPONSE);

    // mark as free
    m_free = true;

    return;
}

/// TLM-2 non-blocking transport method.
tlm::tlm_sync_enum Memory::nb_transport_fw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");
    return tlm::TLM_COMPLETED;
}


unsigned int Memory::transport_dbg(tlm::tlm_generic_payload& trans)
{
    // sanity check
    TLM_TRANS_SANITY(trans);

    // execute the debug command
    TLM_DBG_EXEC(trans, m_data, m_size);
}
