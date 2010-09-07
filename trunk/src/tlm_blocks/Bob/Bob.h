#ifndef BOB_H_
#define BOB_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious include
#include "systemc"

// command line parameters
#include "Parameters.h"

// necessary for system definition
#include "AddrDec.h"
#include "Cpu.h"
#include "Memory.h"
#include "Rom.h"
#include "Pmu/Pmu.h"

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
    /// BOBPMU instance pointer
    Pmu* pmu;

    /** Constructor of the top level module
     * @param[in] name Name of the module
     * @param[in] parameters Command line parameters
     * @param[in] config Parameters of the current block (and sub-blocks)
     */
    Bob(sc_core::sc_module_name name, Parameters& parameters, MSP& config);
};

#endif /*BOB_H_*/
