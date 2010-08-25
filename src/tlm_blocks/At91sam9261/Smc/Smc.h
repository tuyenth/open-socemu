#ifndef SMC_H_
#define SMC_H_

// by default, this is a simple bus slave
#include "SimpleSlave.h"

// includes also interrupt masters
#include "tlm_utils/simple_initiator_socket.h"

// include the registers definition
#include "reg_smc.h"

/// Static Memory Controller block model
struct Smc : SimpleSlave
{
    /// Constructor
    Smc(sc_core::sc_module_name name)
        : SimpleSlave(name, m_reg, sizeof(m_reg))
        , bus_s_socket("bus_s_socket")
    {
        // initialized the register access
        reg_smc = &(m_reg[0]);

        // clear all the registers
        memset(m_reg, 0, sizeof(m_reg));

        bus_s_socket.register_b_transport(this, &Smc::bus_s_b_transport);
        bus_s_socket.register_nb_transport_fw(this, &Smc::bus_s_nb_transport_fw);
        bus_s_socket.register_transport_dbg(this, &Smc::bus_s_transport_dbg);

    }

    /// Override the virtual function
    void
    reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
    {
        TLM_WORD_SANITY(trans);

        // retrieve the required parameters
        uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

        // sanity check
        #if SIMPLESLAVE_DEBUG
        assert(m_free);
        #endif

        // mark as busy
        #if SIMPLESLAVE_DEBUG
        m_free = false;
        #endif

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

        // mark as free
        #if SIMPLESLAVE_DEBUG
        m_free = true;
        #endif

        return;

    }

    /// TLM-2 socket blocking path
    virtual void
    bus_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// TLM-2 socket non blocking path
    virtual tlm::tlm_sync_enum
    bus_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 socket debug path
    virtual unsigned int
    bus_s_transport_dbg(tlm::tlm_generic_payload& trans);

    /// TLM-2 slave socket to receive bus accesses
    tlm_utils::simple_target_socket<Smc> bus_s_socket;

protected:
    /** Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset);

    /** Register write function
     * @param[in] offset Offset of the register to read
     */
    void
    reg_wr(uint32_t offset, uint32_t value);

    /// TLM-2 master socket to forward bus accesses
    tlm_utils::simple_initiator_socket<Smc> bus_m_socket;

    /// Registers content
    uint32_t m_reg[REG_SMC_COUNT];
};

#endif /*SMC_H_*/
