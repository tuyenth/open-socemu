#ifndef SP805_H_
#define SP805_H_

/// ARM Watchdog IP

#include "utils.h"
#include "Generic/Peripheral/Peripheral.h"
#include "Generic/IntMaster/IntMaster.h"

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
    , intsource("intsource")
    , m_ckperiod(sc_core::sc_time(100, sc_core::SC_NS))
    , m_lock_enabled(true)
    , m_inted(false)
    , m_stopped(true)
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

        // initialize the status
        m_locked = true;

        // create the module thread
        SC_THREAD(thread_process);

    }

    /** Set the watchdog counter period
     * @param[in] period Watchdog timer counter period
     */
    void
    set_clock_period(sc_core::sc_time period)
    {
        this->m_ckperiod = period;
    }
    
    /** Enable/disable the watchdog lock mode
     * @param[in] enable Watchdog timer counter period
     */
    void
    set_lock_mode(bool enable)
    {
        m_lock_enabled = enable;
    }

    /// Interrupt source
    IntMaster intsource;

private:
    /// Check if there is a change in the interrupt state
    void
    update_int()
    {
        if (BIT_SET(m_reg[REG_SP805_WDOGCONTROL], 0) && 
            BIT_SET(m_reg[REG_SP805_WDOGRIS], 0))
        {
            intsource.set();
        }
        else
        {
            intsource.clear();
        }
    }
    
    /// Reload the counter
    /// @param[in] reset True to fully reset the mechanism
    void
    reload_counter(bool reset)
    {
        // reset the starttime
        m_starttime = sc_core::sc_time_stamp();
        
        // cancel the event
        m_event.cancel();
        
        if (reset)
        {
            m_inted = false;
            m_stopped = false;
        }
        
        // restart the event
        m_event.notify(m_ckperiod * m_reg[REG_SP805_WDOGLOAD]);
    }

    /// Stop the counter
    void
    stop_counter()
    {
        // mark stopped
        m_stopped = true;

        // cancel the current event
        m_event.cancel();

        // compute the counter value
        m_reg[REG_SP805_WDOGVALUE] = m_reg[REG_SP805_WDOGLOAD] - 
                (uint32_t)((sc_core::sc_time_stamp() - m_starttime)/m_ckperiod);
    }

    /// Module thread
    void
    thread_process()
    {
        while (true)
        {
            // wait for an event
            sc_core::wait(m_event);

            // check if this is the first time
            if (!m_inted)
            {
                m_inted = true;

                // set the raw interrupt
                m_reg[REG_SP805_WDOGRIS] |= 1;
                
                // reload the counter
                this->reload_counter(false);
            }
            else
            {
                if (BIT_SET(m_reg[REG_SP805_WDOGCONTROL], 1))
                {
                    TLM_ERR("WATCHDOG expired -> reset");
                }
                else
                {
                    TLM_DBG("WATCHDOG expired -> reset masked out");
                }
            }
            
            // update interrupt
            this->update_int();
        }
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
        case REG_SP805_WDOGVALUE:
            if (m_stopped)
            {
                // read the register value
                result = m_reg[index];
            }
            else
            {
                result = m_reg[REG_SP805_WDOGLOAD] - 
                        (uint32_t)((sc_core::sc_time_stamp() - m_starttime)/m_ckperiod);
            }
            break;
        case REG_SP805_WDOGINTCLR:
            // this does nothing but it is not an error
            break;
        case REG_SP805_WDOGLOCK:
            if (m_locked) result = 1;
            else result = 0;
            break;
        case REG_SP805_WDOGITOP:
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
        case REG_SP805_WDOGLOAD:
            if (m_lock_enabled && m_locked)
            {
                TLM_ERR("Accessing register %d while locked", index);
                break;
            }
            if (!value)
            {
                TLM_ERR("Writing 0 into SP805 LOAD register");
                break;
            }
            m_reg[index] = value;
            this->reload_counter(true);
            break;
            
        case REG_SP805_WDOGCONTROL:
            if (m_lock_enabled && m_locked)
            {
                TLM_ERR("Accessing register %d while locked", index);
                break;
            }
            // check if the interrupt is re-enabled
            if ((!BIT_SET(m_reg[index], 0)) && BIT_SET(value, 0))
            {
                this->reload_counter(true);
            }
            // check if the interrupt is disabled
            if (BIT_SET(m_reg[index], 0) && (!BIT_SET(value, 0)))
            {
                this->stop_counter();
            }
            
            m_reg[index] = value;
            break;
            
        case REG_SP805_WDOGINTCLR:
            if (m_lock_enabled && m_locked)
            {
                TLM_ERR("Accessing register %d while locked", index);
                break;
            }
            
            // clear the pending interrupts
            m_reg[REG_SP805_WDOGRIS] &= ~1;

            // reset entirely the counting mechanism
            this->reload_counter(true);
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
        case REG_SP805_WDOGRIS:
        case REG_SP805_WDOGMIS:
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
            m_reg[index] = value;
            break;
        }
        
        // update interrupt
        this->update_int();
    }

    /// Increment clock period
    sc_core::sc_time m_ckperiod;
    /// Indicate if the lock mechanism is enabled
    bool m_lock_enabled;
    /// Indicate if the registers are locked or not
    bool m_locked;
    /// Event used to wake up the thread
    sc_core::sc_event m_event;
    /// Indicate if the interrupt was already triggered
    bool m_inted;
    /// Indicate that currently, the timer is stopped
    bool m_stopped;
    /// Counter start time
    sc_core::sc_time m_starttime;
};

#endif /*SP805_H_*/
