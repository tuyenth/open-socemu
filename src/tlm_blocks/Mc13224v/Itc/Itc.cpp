#include "Itc.h"
#include "utils.h"

/// Used to select debugging (0 = OFF, 1 = ON)
#define DEBUG_ITC 0

// Used to make direct register access
uint32_t* reg_itc;

// using this namespace to simplify streaming
using namespace std;

uint32_t
Itc::reg_rd(uint32_t offset)
{
    uint32_t result;
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_ITC_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case INTENNUM_INDEX:
    case INTDISNUM_INDEX:
        TLM_ERR("Read to ITC W/O register %02X", offset);
        result = 0;
        break;

    default:
        // read the register value
        result = m_reg[index];
        break;
    }

    return result;
}

void
Itc::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_ITC_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case NIVECTOR_INDEX:
    case FIVECTOR_INDEX:
    case INTSRC_INDEX:
    case NIPEND_INDEX:
    case FIPEND_INDEX:
        TLM_ERR("Write to ITC R/O register %02X, data = 0x%08X", offset, value);
        break;
    case INTENNUM_INDEX:
        // enable a single interrupt in the enable mask
        intenable_set(intenable_get() | (1 << value));
        break;

    case INTDISNUM_INDEX:
        // disable a single interrupt in the enable mask
        intenable_set(intenable_get() & (~(1 << value)));
        break;

    default:
        m_reg[index] = value;
        break;
    }

    // after every write, check if there is an interrupt to generate or clear
    this->check_int();
}

void
Itc::check_int()
{
    uint32_t pending_fiq;
    uint32_t pending_irq;

    // get the enabled FIQ and IRQ
    pending_fiq = intsrc_get() & intenable_get() &   inttype_get();
    pending_irq = intsrc_get() & intenable_get() & (~inttype_get());

    // mask the normal interrupt below the nimask
    if (nimask_get() < NUM_INT)
    {
        pending_irq &= ~((1 << (nimask_get() + 1) ) - 1);
    }

    // set the pending interrupt registers
    fipend_set(pending_fiq);
    nipend_set(pending_irq);

    // set the vector registers
    if (pending_irq != 0)
    {
        for (uint8_t i=10; i != 0; i--)
        {
            if (pending_irq & (1<<i))
            {
                nivector_set(i);
                break;
            }
        }
    }
    else
    {
        // set the vector to an invalid value
        nivector_set(0xF);
    }

    if (pending_fiq != 0)
    {
        for (uint8_t i=10; i != 0; i--)
        {
            if (pending_fiq & (1<<i))
            {
                fivector_set(i);
                break;
            }
        }
    }
    else
    {
        // set the vector to an invalid value
        fivector_set(0xF);
    }

    // check if there are pending interrupts
    if (fipend_get() != 0)
    {
        this->fiq.set();
        this->irq.clear();
    }
    else
    {
        this->fiq.clear();

        if (nipend_get() != 0)
        {
            this->irq.set();
        }
        else
        {
            this->irq.clear();
        }
    }
}

void
Itc::interrupt_set(void* opaque)
{
    int id = (int)opaque;

    // sanity check
    assert(id < NUM_INT);

    // set the register value
    intsrc_set(intsrc_get() | (1 << id));

    // check if interrupt configuration has changed
    check_int();
}

void
Itc::interrupt_clr(void* opaque)
{
    int id = (int)opaque;

    // sanity check
    assert(id < NUM_INT);

    // clear the register value
    intsrc_set(intsrc_get() & (~(1 << id)));

    // check if interrupt configuration has changed
    check_int();
}
