#ifndef CR_H_
#define CR_H_

#include "SimpleSlave.h"

struct Cr : sc_core::sc_module
{
    /// Constructor
    Cr(sc_core::sc_module_name name);

    /// Registers definition
    enum
    {
        REG_INTCTRL_IRQEN_SET  = 0,
        REG_INTCTRL_IRQEN_CLR  = 1,
        REG_INTCTRL_IRQST_RAW  = 2,
        REG_INTCTRL_FIQST_RAW  = 3,
        REG_INTCTRL_WFI        = 4,

        REG_SIZE               = 256
    };

    /// Registers content
    uint32_t m_registers[REG_SIZE];

};

#endif /*CR_H_*/
