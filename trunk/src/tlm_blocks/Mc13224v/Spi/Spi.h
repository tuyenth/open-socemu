#ifndef SPI_H_
#define SPI_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

// include the registers definition
#include "reg_spi.h"

/// Interrupt Controller block model
struct Spi : sc_core::sc_module
{
    /// TLM-2 slave socket to handle bus accesses
    tlm_utils::simple_target_socket<Spi> reg_s_socket;

    /// TLM-2 master socket to set/clear INT signal
    tlm_utils::simple_initiator_socket<Spi> int_m_socket;

    // Not necessary if this module does not have a thread
    SC_HAS_PROCESS(Spi);

    /// Constructor
    Spi(sc_core::sc_module_name name);

    /// Module thread (responds to read and writes to the FLASH through the SPI)
    void thread_flash(void);

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

    /// Check that interrupt status
    void check_int();

    /// Registers content
    uint32_t m_reg[REG_SPI_COUNT];

    /// Indicate if busy for sanity check
    bool m_free;

    /// Structure containing the information for the flash
    struct
    {
        /// Indicate that an access is ongoing
        bool active;
        /// Event used to hold the SPI thread
        sc_core::sc_event event;
    } m_flash;

    /// Generic payload transaction to use for INT requests
    tlm::tlm_generic_payload int_pl;
    /// Time object for delay to use for INT requests
    sc_core::sc_time int_delay;

};

#endif /*SPI_H_*/
