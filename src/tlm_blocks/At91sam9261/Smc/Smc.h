#ifndef SMC_H_
#define SMC_H_

// this is a peripheral
#include "Generic/Peripheral/Peripheral.h"

// with a bus master
#include "tlm_utils/simple_initiator_socket.h"

// include the registers definition
#include "reg_smc.h"

/// Static Memory Controller block model
struct Smc : Peripheral<REG_SMC_COUNT>
{
    /// Constructor
    Smc(sc_core::sc_module_name name)
    : Peripheral<REG_SMC_COUNT>(name)
    , bus_s_socket("bus_s_socket")
    {
        // initialized the register access
        reg_smc = &(m_reg[0]);

        bus_s_socket.register_b_transport(this, &Smc::bus_s_b_transport);
        bus_s_socket.register_nb_transport_fw(this, &Smc::bus_s_nb_transport_fw);
        bus_s_socket.register_transport_dbg(this, &Smc::bus_s_transport_dbg);
    }

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
     * @param[in] offset Value to write in the register
     */
    void
    reg_wr(uint32_t offset, uint32_t value);

    /// TLM-2 master socket to forward bus accesses
    tlm_utils::simple_initiator_socket<Smc> bus_m_socket;

    /// TLM-2 socket blocking path
    void
    bus_s_b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);

    /// TLM-2 socket non blocking path
    tlm::tlm_sync_enum
    bus_s_nb_transport_fw(tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay);

    /// TLM-2 socket debug path
    unsigned int
    bus_s_transport_dbg(tlm::tlm_generic_payload& trans);


};

#endif /*SMC_H_*/
