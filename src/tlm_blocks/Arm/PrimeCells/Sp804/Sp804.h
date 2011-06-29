#ifndef SP804_H_
#define SP804_H_

/// ARM Dual Timer IP

#include "utils.h"
#include "Generic/Peripheral/Peripheral.h"
#include "Generic/IntMaster/IntMaster.h"

/// Registers definition
enum
{
    REG_SP804_TIMER1_LOAD,
    REG_SP804_TIMER1_VALUE,
    REG_SP804_TIMER1_CONTROL,
    REG_SP804_TIMER1_INTCLR,
    REG_SP804_TIMER1_RIS,
    REG_SP804_TIMER1_MIS,
    REG_SP804_TIMER1_BGLOAD,
    REG_SP804_TIMER2_LOAD = 0x20/4,
    REG_SP804_TIMER2_VALUE,
    REG_SP804_TIMER2_CONTROL,
    REG_SP804_TIMER2_INTCLR,
    REG_SP804_TIMER2_RIS,
    REG_SP804_TIMER2_MIS,
    REG_SP804_TIMER2_BGLOAD,
    REG_SP804_TIMER_ITCR = 0xF00/4,
    REG_SP804_TIMER_ITOP,
    REG_SP804_PERIPHID0 = 0xFE0/4,
    REG_SP804_PERIPHID1,
    REG_SP804_PERIPHID2,
    REG_SP804_PERIPHID3,
    REG_SP804_PCELLID0,
    REG_SP804_PCELLID1,
    REG_SP804_PCELLID2,
    REG_SP804_PCELLID3,
    REG_SP804_COUNT
};

struct Sp804 : Peripheral<REG_SP804_COUNT>
{
    // Module has a thread
    SC_HAS_PROCESS(Sp804);

    /// Constructor
    Sp804(sc_core::sc_module_name name)
    : Peripheral<REG_SP804_COUNT>(name),
      m_t1ckperiod(sc_core::sc_time(100, sc_core::SC_NS)),
      m_t2ckperiod(sc_core::sc_time(100, sc_core::SC_NS)),
      m_t1stopped(true),
      m_t2stopped(true)
    {
        // initialize the registers content
        m_reg[REG_SP804_TIMER1_VALUE] = 0xFFFFFFFF;
        m_reg[REG_SP804_TIMER2_VALUE] = 0xFFFFFFFF;
        m_reg[REG_SP804_TIMER1_CONTROL] = 0x20;
        m_reg[REG_SP804_TIMER2_CONTROL] = 0x20;
        m_reg[REG_SP804_PERIPHID0] = 0x04;
        m_reg[REG_SP804_PERIPHID1] = 0x18;
        m_reg[REG_SP804_PERIPHID2] = 0x14;
        m_reg[REG_SP804_PERIPHID3] = 0x00;
        m_reg[REG_SP804_PCELLID0] = 0x0D;
        m_reg[REG_SP804_PCELLID1] = 0xF0;
        m_reg[REG_SP804_PCELLID2] = 0x05;
        m_reg[REG_SP804_PCELLID3] = 0xB1;

        // create the module threads
        SC_THREAD(t1_thread_process);
        SC_THREAD(t2_thread_process);
    }
    
    /// Set the timer 1 counter period
    void
    set_t1_clock_period(sc_core::sc_time period)
    {
        this->m_t1ckperiod = period;
    }

    /// Set the timer 2 counter period
    void
    set_t2_clock_period(sc_core::sc_time period)
    {
        this->m_t2ckperiod = period;
    }

    /// Timer N interrupt
    IntMaster intsource[2];
    

private:
    
    /// Update the interrupts
    void
    update_int(void)
    {
        // check if timer1 is enabled
        if ((m_reg[REG_SP804_TIMER1_CONTROL] & (1 << 5)) && 
            (m_reg[REG_SP804_TIMER1_CONTROL] & (1 << 7)))
        {
            m_reg[REG_SP804_TIMER1_MIS] = m_reg[REG_SP804_TIMER1_RIS];
        }
        else
        {
            m_reg[REG_SP804_TIMER1_MIS] = 0;
        }
        // check if timer2 is enabled
        if ((m_reg[REG_SP804_TIMER2_CONTROL] & (1 << 5)) && 
            (m_reg[REG_SP804_TIMER2_CONTROL] & (1 << 7)))
        {
            m_reg[REG_SP804_TIMER2_MIS] = m_reg[REG_SP804_TIMER1_RIS];
        }
        else
        {
            m_reg[REG_SP804_TIMER2_MIS] = 0;
        }
        
        if (m_reg[REG_SP804_TIMER1_MIS])
        {
            intsource[0].set();
        }
        else
        {
            intsource[0].clear();
        }
        if (m_reg[REG_SP804_TIMER2_MIS])
        {
            intsource[1].set();
        }
        else
        {
            intsource[1].clear();
        }
    }
    
    /// Start timer 1 counter
    void
    start_t1_counter(void)
    {
        // cancel the current event
        m_t1event.cancel();

        // check if timer is enabled
        if (BIT_SET(m_reg[REG_SP804_TIMER1_CONTROL], 7))
        {
            if (!BIT_SET(m_reg[REG_SP804_TIMER1_CONTROL], 1))
            {
                m_reg[REG_SP804_TIMER1_VALUE] &= 0xFFFF;
            }
            
            // reset the starttime
            m_t1starttime = sc_core::sc_time_stamp();
            
            // restart the event
            m_t1event.notify(m_t1ckperiod * m_reg[REG_SP804_TIMER1_VALUE] * 
                    (1 << (GETF(m_reg[REG_SP804_TIMER1_CONTROL], 0xc, 2) * 4)));
        }
    }
    
    /// Stop timer 1 counter
    void
    stop_t1_counter(void)
    {
        // mark stopped
        m_t1stopped = true;

        // cancel the current event
        m_t1event.cancel();

        // compute the counter value
        m_reg[REG_SP804_TIMER1_VALUE] = m_reg[REG_SP804_TIMER1_VALUE] - 
                (uint32_t)((sc_core::sc_time_stamp() - m_t1starttime)/
                (m_t1ckperiod * (1 << (GETF(m_reg[REG_SP804_TIMER1_CONTROL], 0xc, 2) * 4))));
    }
    
    /// Start timer 2 counter
    void
    start_t2_counter(void)
    {
        // cancel the current event
        m_t2event.cancel();

        // check if timer is enabled
        if (BIT_SET(m_reg[REG_SP804_TIMER2_CONTROL], 7))
        {
            if (!BIT_SET(m_reg[REG_SP804_TIMER2_CONTROL], 1))
            {
                m_reg[REG_SP804_TIMER2_VALUE] &= 0xFFFF;
            }
            
            // reset the starttime
            m_t2starttime = sc_core::sc_time_stamp();
            
            // restart the event
            m_t2event.notify(m_t2ckperiod * m_reg[REG_SP804_TIMER2_VALUE] * 
                    (1 << (GETF(m_reg[REG_SP804_TIMER2_CONTROL], 0xc, 2) * 4)));
        }
    }
    
    /// Stop timer 2 counter
    void
    stop_t2_counter(void)
    {
        // mark stopped
        m_t2stopped = true;

        // cancel the current event
        m_t2event.cancel();

        // compute the counter value
        m_reg[REG_SP804_TIMER2_VALUE] = m_reg[REG_SP804_TIMER2_VALUE] - 
                (uint32_t)((sc_core::sc_time_stamp() - m_t2starttime)/
                (m_t2ckperiod * (1 << (GETF(m_reg[REG_SP804_TIMER2_CONTROL], 0xc, 2) * 4))));
    }
    
    /// Module thread
    void
    t1_thread_process()
    {
        while (true)
        {
            // wait for an event
            sc_core::wait(m_t1event);
            
            // interrupt
            m_reg[REG_SP804_TIMER1_RIS] = 1;
            this->update_int();
            
            // [6] TimerMode
            // [0] OneShot
            if (BIT_SET(m_reg[REG_SP804_TIMER1_CONTROL], 0))
            {
                // one shot
                this->stop_t1_counter();
            }
            else if (BIT_SET(m_reg[REG_SP804_TIMER1_CONTROL], 6))
            {
                // free running
                m_reg[REG_SP804_TIMER1_VALUE] = 0xFFFFFFFF;
                this->start_t1_counter();
            }
            else
            {
                // periodic
                m_reg[REG_SP804_TIMER1_VALUE] = m_reg[REG_SP804_TIMER1_BGLOAD];
                this->start_t1_counter();
            }
        }
    }
    
    /// Module thread
    void
    t2_thread_process()
    {
        while (true)
        {
            // wait for an event
            sc_core::wait(m_t2event);

            // interrupt
            m_reg[REG_SP804_TIMER2_RIS] = 1;
            this->update_int();
            
            // [6] TimerMode
            // [0] OneShot
            if (BIT_SET(m_reg[REG_SP804_TIMER2_CONTROL], 0))
            {
                // one shot
                this->stop_t2_counter();
            }
            else if (BIT_SET(m_reg[REG_SP804_TIMER2_CONTROL], 6))
            {
                // free running
                m_reg[REG_SP804_TIMER2_VALUE] = 0xFFFFFFFF;
                this->start_t2_counter();
            }
            else
            {
                // periodic
                m_reg[REG_SP804_TIMER2_VALUE] = m_reg[REG_SP804_TIMER2_BGLOAD];
                this->start_t2_counter();
            }
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
        assert(index < REG_SP804_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        case REG_SP804_TIMER1_VALUE:
            if (m_t1stopped)
            {
                // read the register value
                result = m_reg[index];
            }
            else
            {
                result = m_reg[REG_SP804_TIMER1_VALUE] - 
                        (uint32_t)((sc_core::sc_time_stamp() - m_t1starttime)/
                        (m_t1ckperiod * (1 << (GETF(m_reg[REG_SP804_TIMER1_CONTROL], 0xc, 2) * 4))));
            }
            break;

        case REG_SP804_TIMER2_VALUE:
            if (m_t2stopped)
            {
                // read the register value
                result = m_reg[index];
            }
            else
            {
                result = m_reg[REG_SP804_TIMER2_VALUE] - 
                        (uint32_t)((sc_core::sc_time_stamp() - m_t2starttime)/
                        (m_t2ckperiod * (1 << (GETF(m_reg[REG_SP804_TIMER2_CONTROL], 0xc, 2) * 4))));
            }
            break;

        case REG_SP804_TIMER1_LOAD:
            result = m_reg[REG_SP804_TIMER1_BGLOAD];
            break;

        case REG_SP804_TIMER2_LOAD:
            result = m_reg[REG_SP804_TIMER2_BGLOAD];
            break;

        case REG_SP804_TIMER1_INTCLR:
        case REG_SP804_TIMER2_INTCLR:
        case REG_SP804_TIMER_ITOP:
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
        uint32_t prev;

        // sanity check
        assert(index < REG_SP804_COUNT);

        // internal delay
        this->delay();

        switch (index)
        {
        case REG_SP804_TIMER1_CONTROL:
            prev = m_reg[index];
            m_reg[index] = value;
            // check if modified while enabled
            if (BIT_SET(prev, 7) && BIT_SET(value, 7))
            {
                if ((prev & 0x4f) != (value & 0x4f))
                {
                    TLM_ERR("Modifying control while timer enabled");
                }
            }
            // check if the timer is re-enabled
            if ((!BIT_SET(prev, 7)) && BIT_SET(value, 7))
            {
                // detect unsupported values
                if ((value & 0xc) == 0xc)
                {
                    TLM_ERR("Writing unsupported prescaler value");
                }
                // check if the value enabled is 16 bit
                this->start_t1_counter();
            }
            // check if the interrupt is disabled
            if (BIT_SET(prev, 7) && (!BIT_SET(value, 7)))
            {
                this->stop_t1_counter();
            }
            break;
            
        case REG_SP804_TIMER2_CONTROL:
            prev = m_reg[index];
            m_reg[index] = value;
            // check if modified while enabled
            if (BIT_SET(prev, 7) && BIT_SET(value, 7))
            {
                if ((prev & 0x4f) != (value & 0x4f))
                {
                    TLM_ERR("Modifying control while timer enabled");
                }
            }
            // check if the timer is re-enabled
            if ((!BIT_SET(prev, 7)) && BIT_SET(value, 7))
            {
                // detect unsupported values
                if ((value & 0xc) == 0xc)
                {
                    TLM_ERR("Writing unsupported prescaler value");
                }
                this->start_t2_counter();
            }
            // check if the interrupt is disabled
            if (BIT_SET(prev, 7) && (!BIT_SET(value, 7)))
            {
                this->stop_t2_counter();
            }
            break;
            
        case REG_SP804_TIMER1_INTCLR:
            m_reg[REG_SP804_TIMER1_RIS] = 0;
            break;
            
        case REG_SP804_TIMER2_INTCLR:
            m_reg[REG_SP804_TIMER2_RIS] = 0;
            break;
            
        case REG_SP804_TIMER1_LOAD:
            m_reg[REG_SP804_TIMER1_VALUE] = value;
            this->start_t1_counter();
            break;

        case REG_SP804_TIMER2_LOAD:
            m_reg[REG_SP804_TIMER2_VALUE] = value;
            this->start_t2_counter();
            break;

        case REG_SP804_TIMER1_BGLOAD:
            // write the load value without reloading immediately the counter
            m_reg[REG_SP804_TIMER1_BGLOAD] = value;
            break;

        case REG_SP804_TIMER2_BGLOAD:
            // write the load value without reloading immediately the counter
            m_reg[REG_SP804_TIMER2_BGLOAD] = value;
            break;

        case REG_SP804_TIMER1_VALUE:
        case REG_SP804_TIMER2_VALUE:
        case REG_SP804_TIMER1_RIS:
        case REG_SP804_TIMER2_RIS:
        case REG_SP804_TIMER1_MIS:
        case REG_SP804_TIMER2_MIS:
        case REG_SP804_PERIPHID0:
        case REG_SP804_PERIPHID1:
        case REG_SP804_PERIPHID2:
        case REG_SP804_PERIPHID3:
        case REG_SP804_PCELLID0:
        case REG_SP804_PCELLID1:
        case REG_SP804_PCELLID2:
        case REG_SP804_PCELLID3:
            TLM_ERR("write access to read-only register (%d)", index);
            break;
            
        default:
            m_reg[index] = value;
            break;
        }
        
        this->update_int();
    }

    /// Timer 1 clock period
    sc_core::sc_time m_t1ckperiod;

    /// Timer 2 clock period
    sc_core::sc_time m_t2ckperiod;

    /// Event used to wake up the timer 1 thread
    sc_core::sc_event m_t1event;

    /// Event used to wake up the timer 2 thread
    sc_core::sc_event m_t2event;

    /// Indicate that currently, the timer 1 is stopped
    bool m_t1stopped;

    /// Indicate that currently, the timer 2 is stopped
    bool m_t2stopped;

    /// Timer 1 counter start time
    sc_core::sc_time m_t1starttime;

    /// Timer 2 counter start time
    sc_core::sc_time m_t2starttime;
};

#endif /*SP804_H_*/
