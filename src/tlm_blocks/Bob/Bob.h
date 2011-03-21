#ifndef BOB_H_
#define BOB_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious include
#include "systemc"

// command line parameters
#include "Parameters.h"

// necessary for system definition
#include "Generic/AddrDec/AddrDec.h"
#include "Cpu/Cpu.h"
#include "Generic/Memory/Memory.h"
#include "Generic/Rom/Rom.h"
#include "Pmu/Pmu.h"
#include "Arm/PrimeCells/Sp804/Sp804.h"
#include "Arm/PrimeCells/Sp805/Sp805.h"
#include "Arm/PrimeCells/Pl061/Pl061.h"
#include "Arm/PrimeCells/Pl190/Pl190.h"
#include "Bob/Phy/Phy.h"
#include "Bob/Prc/Prc.h"
#include "Bob/Rbg/Rbg.h"
#include "Bob/Ptu/Ptu.h"
#include "Bob/Fm/Fm.h"
#include "Bob/Rmp/Rmp.h"
#include "Bob/Rf/Rf.h"

/// Bob level platform module
struct Bob : sc_core::sc_module
{
    /// Address decoder instance pointer
    AddrDec* addrdec;
    /// CPU instance pointer
    Cpu* cpu;
    /// ROM instance pointer
    Rom* rom;
    /// SRAM instance pointer
    Memory* sram;
    /// FLASH instance pointer
    Rom* flash;
    /// PHY instance pointer
    Phy* phy;
    /// PMU instance pointer
    Pmu* pmu;
    /// Gpio peripheral
    Pl061* gpio;
    /// Gpio2 peripheral
    Pl061* gpio2;
    /// RBG peripheral
    Rbg* rbg;
    /// PTU peripheral
    Ptu* ptu;
    /// PRC peripheral
    Prc* prc;
    /// FM peripheral
    Fm* fm;
    /// IC peripheral
    Pl190* ic;
    /// Dual timer peripheral
    Sp804* timer;
    /// Rmp peripheral
    Rmp* rmp;
    /// Watchdog peripheral
    Sp805* wdog;
    /// RF frontend peripheral
    Rf* rf;

    /** Constructor of the top level module
     * @param[in] name Name of the module
     * @param[in] parameters Command line parameters
     * @param[in] config Parameters of the current block (and sub-blocks)
     */
    Bob(sc_core::sc_module_name name, Parameters& parameters, MSP& config);
};

#endif /*BOB_H_*/
