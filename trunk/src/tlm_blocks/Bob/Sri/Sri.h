#ifndef SRI_H_
#define SRI_H_

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_DC_SRI_IST_BIT,
    REG_DC_SRI_DS_BIT,
    REG_DC_SRI_IST0,
    REG_DC_SRI_IST1,
    REG_DC_SRI_DS0,
    REG_DC_SRI_DS1,
    REG_DC_SRI_DS2,
    REG_DC_SRI_DS3,
    REG_DC_SRI_JTAG_ACCESS,
    REG_DC_SRI_RF_CTL0,
    REG_DC_SRI_RF_CTL1,
    REG_DC_SRI_RSSI_VAL,
    REG_DC_SRI_PWR_CTL,
    REG_DC_T_SYNTH_PU,
    REG_DC_T_SL_CTL,
    REG_DC_T_PA_RAMP,
    REG_DC_T_PA_DOWN,
    REG_DC_T_TX_PU,
    REG_DC_T_RX_PU,
    REG_DC_AVAILABLE01,
    REG_DC_SRI_SCHP0_SEL,
    REG_DC_SRI_SCHP1_SEL,
    REG_DC_SRI_SCHP2_SEL,
    REG_DC_SRI_SCHP3_SEL,
    REG_SRI_COUNT
};

struct Sri : Peripheral<REG_SRI_COUNT>
{
    // Module has a thread
    SC_HAS_PROCESS(Sri);

    /// Constructor
    Sri(sc_core::sc_module_name name)
    : Peripheral<REG_SRI_COUNT>(name)
    {
        // initialize the registers content

        // create the module thread
        SC_THREAD(thread_process);
    }

private:
    /// Module thread
    void
    thread_process();

    /** Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset);

    /** Register write function
     * @param[in] offset Offset of the register to read
     * @param[in] offset Value to write in the register
     */
    void
    reg_wr(uint32_t offset, uint32_t value);

    /// Indicate if the current access is a write
    bool m_iswrite;

    /// Event used to wake up the thread
    sc_core::sc_event m_event;
};

#endif /*SRI_H_*/
