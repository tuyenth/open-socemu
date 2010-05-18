#ifndef SPIF_H_
#define SPIF_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

// not so obvious inclusions
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

// include the registers definition
#include "reg_spif.h"

/// Interrupt Controller block model
struct Spif : sc_core::sc_module
{
    /// TLM-2 slave socket to handle bus accesses.
    tlm_utils::simple_target_socket<Spif> reg_s_socket;

    /// TLM-2 master socket to set/clear INT signal
    tlm_utils::simple_initiator_socket<Spif> int_m_socket;

    // Not necessary if this module does not have a thread
    SC_HAS_PROCESS(Spif);

    /// Constructor.
    Spif(sc_core::sc_module_name name, uint8_t *flashdata, uint32_t flashsize);

    /// Module thread (responds to read and writes to the FLASH through the SPIF)
    void thread_flash(void);

    /// TLM-2 socket blocking method.
    virtual void reg_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// TLM-2 socket non blocking path.
    virtual tlm::tlm_sync_enum reg_s_nb_transport_fw( tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase, sc_core::sc_time& delay );

    /// TLM-2 socket debug path.
    virtual unsigned int reg_s_transport_dbg(tlm::tlm_generic_payload& trans);

    /// Read access to the registers
    uint32_t reg_rd(uint32_t offset);

    /// Write access to the registers
    void reg_wr(uint32_t offset, uint32_t value);

    /// Check that interrupt status.
    void check_int();

    /// Registers content.
    uint32_t m_reg[REG_SPIF_COUNT];

    /// Indicate if busy for sanity check
    bool m_free;

    /// Structure containing the information for the flash
    struct
    {
        /// Size of the flash
        uint32_t size;
        /// Flash contents
        uint8_t* data;

        /// Indicate that an access is ongoing
        bool active;
        /// Indicate the last opcode received (0x00 when it was reset)
        uint8_t opcode;
        /// Event used to hold the SPIF thread
        sc_core::sc_event event;
        /// Current address
        uint32_t curaddr;
    } m_flash;

    /// Generic payload transaction to use for INT requests
    tlm::tlm_generic_payload int_pl;
    /// Time object for delay to use for INT requests
    sc_core::sc_time int_delay;

};

#endif /*SPIF_H_*/
