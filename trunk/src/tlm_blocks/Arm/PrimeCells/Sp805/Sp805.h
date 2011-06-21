#ifndef SP805_H_
#define SP805_H_

/// ARM Watchdog IP

#include "Generic/Peripheral/Peripheral.h"
#include "utils.h"

/// Registers definition
enum
{
    REG_SP805_WDOGLOAD,
    REG_SP805_WDOGVALUE,
    REG_SP805_WDOGCONTROL,
    REG_SP805_WDOGINTCLR,
    REG_SP805_WDOGRIS,
    REG_SP805_WDOGMIS,
    REG_SP805_WDOGLOCK = 0xC00/4,
    REG_SP805_WDOGITCR = 0xF00/4,
    REG_SP805_WDOGITOP,
    REG_SP805_PERIPHID0 = 0xFE0/4,
    REG_SP805_PERIPHID1,
    REG_SP805_PERIPHID2,
    REG_SP805_PERIPHID3,
    REG_SP805_PCELLID0,
    REG_SP805_PCELLID1,
    REG_SP805_PCELLID2,
    REG_SP805_PCELLID3,
    REG_SP805_COUNT
};

struct Sp805 : Peripheral<REG_SP805_COUNT>
{
    // Module has a thread
    SC_HAS_PROCESS(Sp805);

    /// Constructor
    Sp805(sc_core::sc_module_name name)
    : Peripheral<REG_SP805_COUNT>(name)
    {
        // initialize the registers content
        m_reg[REG_SP805_WDOGLOAD] = 0xFFFFFFFF;
        m_reg[REG_SP805_WDOGVALUE] = 0xFFFFFFFF;
        m_reg[REG_SP805_PERIPHID0] = 0x5;
        m_reg[REG_SP805_PERIPHID1] = 0x18;
        m_reg[REG_SP805_PERIPHID2] = 0x14;
        m_reg[REG_SP805_PERIPHID3] = 0x0;
        m_reg[REG_SP805_PCELLID0] = 0x0D;
        m_reg[REG_SP805_PCELLID1] = 0xF0;
        m_reg[REG_SP805_PCELLID2] = 0x05;
        m_reg[REG_SP805_PCELLID3] = 0xB1;

        // create the module thread
        SC_THREAD(thread_process);

        // initialize the status
        m_locked = true;
    }

private:
    /// Module thread
    void
    thread_process()
    {

    }

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
        assert(index < REG_SP805_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        case REG_SP805_WDOGLOCK:
            if (m_locked) result = 1;
            else result = 0;
            break;
        case REG_SP805_WDOGINTCLR:
            // this does nothing but it is not an error
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
        assert(index < REG_SP805_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        case REG_SP805_WDOGCONTROL:
            // check if the interrupt is re-enabled
            if ((!BIT_SET(m_reg[index], 0)) && BIT_SET(value, 0))
            {
                
            }
            break;
        case REG_SP805_WDOGLOCK:
            if (value == 0x1ACCE551)
            {
                m_locked = false;
            }
            else
            {
                m_locked = true;
            }
            break;
        case REG_SP805_WDOGVALUE:
        case REG_SP805_PERIPHID0:
        case REG_SP805_PERIPHID1:
        case REG_SP805_PERIPHID2:
        case REG_SP805_PERIPHID3:
        case REG_SP805_PCELLID0:
        case REG_SP805_PCELLID1:
        case REG_SP805_PCELLID2:
        case REG_SP805_PCELLID3:
            TLM_ERR("write access to read-only register (%d)", index);
            break;
            
        default:
            TLM_DBG("m_reg(0x%X) = 0x%02X", offset, value);
            m_reg[index] = value;
            break;
        }
    }

    bool m_locked;
};

#endif /*SP805_H_*/
