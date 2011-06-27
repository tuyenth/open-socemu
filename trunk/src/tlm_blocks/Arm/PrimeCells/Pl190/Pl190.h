#ifndef PL190_H_
#define PL190_H_

/// ARM Interrupt Controller IP

#include "Generic/Peripheral/Peripheral.h"
#include "Generic/IntSlave/IntSlave.h"
#include "Generic/IntMaster/IntMaster.h"

/// Registers definition
enum
{
    REG_PL190_IRQSTATUS,
    REG_PL190_FIQSTATUS,
    REG_PL190_RAWINTR,
    REG_PL190_INTSELECT,
    REG_PL190_INTENABLE,
    REG_PL190_INTENCLEAR,
    REG_PL190_SOFTINT,
    REG_PL190_SOFTINTCLEAR,
    REG_PL190_PROTECTION,
    REG_PL190_VECTADDR = 0x30/4,
    REG_PL190_DEFVECTADDR,
    REG_PL190_VECTADDR0 = 0x100/4,
    REG_PL190_VECTCNTL0 = 0x200/4,
    REG_PL190_ITCR = 0x300/4,
    REG_PL190_ITIP1,
    REG_PL190_ITIP2,
    REG_PL190_ITOP1,
    REG_PL190_ITOP2,
    REG_PL190_PERIPHID0 = 0xFE0/4,
    REG_PL190_PERIPHID1,
    REG_PL190_PERIPHID2,
    REG_PL190_PERIPHID3,
    REG_PL190_PCELLID0,
    REG_PL190_PCELLID1,
    REG_PL190_PCELLID2,
    REG_PL190_PCELLID3,
    REG_PL190_COUNT
};

struct Pl190 : Peripheral<REG_PL190_COUNT>
{
    enum
    {
        NUM_INT = 32
    };

    /// Constructor
    Pl190(sc_core::sc_module_name name)
    : Peripheral<REG_PL190_COUNT>(name),
      m_vicintsource(0)
    {
        // create all the interrupts
        for (int i = 0; i < NUM_INT; i++)
        {
            this->vicintsource[i].init(this, &Pl190::int_set_cb, &Pl190::int_clr_cb, (void*)i);
        }

        // initialize the registers content
        m_reg[REG_PL190_PERIPHID0] = 0x90;
        m_reg[REG_PL190_PERIPHID1] = 0x11;
        m_reg[REG_PL190_PERIPHID2] = 0x10;
        m_reg[REG_PL190_PERIPHID3] = 0x0;
        m_reg[REG_PL190_PCELLID0] = 0x0D;
        m_reg[REG_PL190_PCELLID1] = 0xF0;
        m_reg[REG_PL190_PCELLID2] = 0x05;
        m_reg[REG_PL190_PCELLID3] = 0xB1;
    }

    /// Slave interrupts array
    IntSlave<Pl190> vicintsource[NUM_INT];

    /// IRQ interrupt
    IntMaster irq;

    /// FIQ interrupt
    IntMaster fiq;

private:
    /** Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset)
    {
        uint32_t result;
        // retrieve the required parameters
        uint32_t index = offset/4;

        // sanity check
        assert(index < REG_PL190_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        case REG_PL190_INTENCLEAR:
        case REG_PL190_SOFTINTCLEAR:
            TLM_ERR("read access to write-only register (%d)", index);
            break;
            
        default:
            // read the register value
            result = m_reg[index];
            break;
        }

        return result;
    }

    /** Register write function
     * @param[in] offset Offset of the register to read
     * @param[in] offset Value to write in the register
     */
    void
    reg_wr(uint32_t offset, uint32_t value)
    {
        // retrieve the required parameters
        uint32_t index = offset/4;

        // sanity check
        assert(index < REG_PL190_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        case REG_PL190_INTENCLEAR:
            m_reg[REG_PL190_INTENABLE] &= ~value;
            break;

        case REG_PL190_SOFTINTCLEAR:
            m_reg[REG_PL190_SOFTINT] &= ~value;
            break;

        case REG_PL190_IRQSTATUS:
        case REG_PL190_FIQSTATUS:
        case REG_PL190_RAWINTR:
        case REG_PL190_PERIPHID0:
        case REG_PL190_PERIPHID1:
        case REG_PL190_PERIPHID2:
        case REG_PL190_PERIPHID3:
        case REG_PL190_PCELLID0:
        case REG_PL190_PCELLID1:
        case REG_PL190_PCELLID2:
        case REG_PL190_PCELLID3:
            TLM_ERR("write access to read-only register (%d)", index);
            break;
            
        default:
            TLM_DBG("m_reg(0x%X) = 0x%02X", offset, value);
            m_reg[index] = value;
            break;
        }
        // always update the interrupts after a write access
        this->update_int();
    }

    /// Interrupt set callback
    /// @param[in] opaque Pointer passed in parameter when registering
    void
    int_set_cb(void* opaque)
    {
        int i = (int)opaque;
        
        // set the interrupt
        m_vicintsource |= 1 << i;
        
        // update the interrupt
        this->update_int();
    }

    /// Interrupt clear callback
    /// @param[in] opaque Pointer passed in parameter when registering
    void
    int_clr_cb(void* opaque)
    {
        int i = (int)opaque;

        // clear the interrupt
        m_vicintsource &= ~(1 << i);

        // update the interrupt
        this->update_int();
    }
    
    void
    update_int(void)
    {
        int i;
        
        // OR with the soft interrupt sources
        m_reg[REG_PL190_RAWINTR] = m_vicintsource | m_reg[REG_PL190_SOFTINT];
        
        // set IRQ or FOQ if enabled
        m_reg[REG_PL190_IRQSTATUS] = m_reg[REG_PL190_RAWINTR] & m_reg[REG_PL190_INTENABLE] & 
                (~m_reg[REG_PL190_INTSELECT]);
        m_reg[REG_PL190_FIQSTATUS] = m_reg[REG_PL190_RAWINTR] & m_reg[REG_PL190_INTENABLE] & 
                m_reg[REG_PL190_INTSELECT];
        
        // Vector block operation
        for (i = 0; i < 16; i++)
        {
            // check if it is enabled
            if (m_reg[REG_PL190_VECTCNTL0 + i] & (1 << 5))
            {
                // check if the selected interrupt is set
                if (m_reg[REG_PL190_IRQSTATUS] & (1 << (m_reg[REG_PL190_VECTCNTL0 + i] & 0x1F)))
                {
                    break;
                }
            }
        }

        // check if a vectored interrupt was found
        if (i < 16)
        {
            m_reg[REG_PL190_VECTADDR] = m_reg[REG_PL190_VECTADDR0 + i];
        }
        else
        {
            m_reg[REG_PL190_VECTADDR] = m_reg[REG_PL190_DEFVECTADDR];
        }
        
        // set interrupt levels
        if (m_reg[REG_PL190_FIQSTATUS])
        {
            this->fiq.set();
        }
        else
        {
            this->fiq.clear();
        }
        if (m_reg[REG_PL190_IRQSTATUS])
        {
            this->irq.set();
        }
        else
        {
            this->irq.clear();
        }
    }

    /// End of elaboration: unhooked interrupt should be connected
    void
    end_of_elaboration()
    {
        // initialize the slave interrupt
        for (int i = 0; i < NUM_INT; i++)
        {
            if (!this->vicintsource[i].is_bound())
            {
                char txt[256];
                tlm_utils::simple_initiator_socket<Pl190>* dummy_int_m_socket;

                sprintf(txt, "itc_dummy_int[%d]", i);
                dummy_int_m_socket = new tlm_utils::simple_initiator_socket<Pl190>(txt);
                dummy_int_m_socket->bind(this->vicintsource[i]);
            }
        }
    }
    
    uint32_t m_vicintsource;
};

#endif /*PL190_H_*/
