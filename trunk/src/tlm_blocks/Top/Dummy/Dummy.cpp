#include "Dummy.h"
/** Register read function
 * @param[in] offset Offset of the register to read
 * @return The value read
 */
uint32_t
Dummy::reg_rd(uint32_t offset)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_DUMMY_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    default:
        return m_reg[index];
        break;
    }
}

/** Register write function
 * @param[in] offset Offset of the register to read
 */
void
Dummy::reg_wr(uint32_t offset, uint32_t value)
{
    // retrieve the required parameters
    uint32_t index = offset/4;

    // sanity check
    assert(index < REG_DUMMY_COUNT);

    // internal delay
    this->delay();

    switch (index)
    {
    case REG_DUMMY_COMMAND:
        m_int.set();
        break;
    case REG_DUMMY_ACK:
        m_int.clear();
        break;
    default:
       m_reg[index] = value;
        break;
    }
}
