#ifndef GPIO_H_
#define GPIO_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

// include the registers definition
#include "reg_gpio.h"

/// Interrupt Controller block model
struct Gpio : sc_core::sc_module
{
    /// TLM-2 slave socket to handle bus accesses
    tlm_utils::simple_target_socket<Gpio> reg_s_socket;

    // Not necessary if this module does not have a thread
//    SC_HAS_PROCESS(Gpio);

    /// Constructor
    Gpio(sc_core::sc_module_name name);

    /// TLM-2 socket blocking method
    virtual void reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// TLM-2 socket non blocking path
    virtual tlm::tlm_sync_enum reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 socket debug path
    virtual unsigned int reg_s_transport_dbg(tlm::tlm_generic_payload& trans);

    /// Read access to the registers
    uint32_t reg_rd(uint32_t offset);

    /// Write access to the registers
    void reg_wr(uint32_t offset, uint32_t value);

    /// Registers content
    uint32_t m_reg[REG_GPIO_COUNT];

    /// Indicate if busy for sanity check
    bool m_free;
};

#endif /*GPIO_H_*/
