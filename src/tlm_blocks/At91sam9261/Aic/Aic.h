#ifndef AIC_H_
#define AIC_H_

// by default, this is a simple bus slave
#include "SimpleSlave.h"

// includes also interrupt masters
#include "tlm_utils/simple_initiator_socket.h"

// include the registers definition
#include "reg_aic.h"

/// Advanced Interrupt Controller block model
struct Aic : SimpleSlave
{
    /// Constructor
    Aic(sc_core::sc_module_name name)
        : SimpleSlave(name, m_reg, REG_AIC_COUNT*4)
        , irq_m_socket("irq_m_socket")
        , fiq_m_socket("fiq_m_socket")
    {
        // force the default values of the FIQ transaction
        fiq_pl.set_streaming_width(4);
        fiq_pl.set_byte_enable_ptr(0);
        fiq_pl.set_dmi_allowed(false);
        // force the default values of the IRQ transaction
        irq_pl.set_streaming_width(4);
        irq_pl.set_byte_enable_ptr(0);
        irq_pl.set_dmi_allowed(false);
        // initialize the register access
        reg_aic = &(m_reg[0]);

        // clear all the registers
        memset(m_reg, 0, sizeof(m_reg));

        for (uint8_t i = 0; i < 32; i++)
        {
            char txt[20];
            sprintf(txt, "int_s_socket_%d", i);
            int_s_socket[i] = new tlm_utils::simple_target_socket_tagged<Aic>(txt);
            // only blocking method is supported
            int_s_socket[i]->register_b_transport(this, &Aic::int_s_b_transport, i);
        }
    }

    /// TLM-2 slave sockets for interrupt sources (tagged to use only one callback)
    tlm_utils::simple_target_socket_tagged<Aic>* int_s_socket[32];

    /// TLM-2 master socket to set/clear IRQ signal
    tlm_utils::simple_initiator_socket<Aic> irq_m_socket;

    /// TLM-2 master socket to set/clear FIQ signal
    tlm_utils::simple_initiator_socket<Aic> fiq_m_socket;

    /// Override the virtual function
    void slave_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
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
    virtual void int_s_b_transport( int id, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay );

    /// Check the pending interrupts
    void check_int();

    /// Registers content
    uint32_t m_reg[REG_AIC_COUNT];

    /// Generic payload transaction to use for FIQ requests
    tlm::tlm_generic_payload fiq_pl;
    /// Time object for delay to use for FIQ requests
    sc_core::sc_time fiq_delay;
    /// Generic payload transaction to use for IRQ requests
    tlm::tlm_generic_payload irq_pl;
    /// Time object for delay to use for IRQ requests
    sc_core::sc_time irq_delay;

protected:
    /**
     * Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset);

    /**
     * Register write function
     * @param[in] offset Offset of the register to read
     */
    void
    reg_wr(uint32_t offset, uint32_t value);

};

#endif /*AIC_H_*/
