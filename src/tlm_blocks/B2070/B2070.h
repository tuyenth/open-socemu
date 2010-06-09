#ifndef B2070_H_
#define B2070_H_

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

/// B2070 level platform module
struct B2070 : sc_core::sc_module
{
    /// Address decoder instance pointer
    AddrDec<2>* addrdec;
    /// CPU instance pointer
    Cpu* cpu;
    /// ROM instance pointer
    Memory* rom;
    /// SRAM instance pointer
    Memory* sram;

    /** Constructor of the top level module
     * @param[in] name Name of the module
     * @param[in] parameters Command line parameters
     * @param[in] config Parameters of the current block (and sub-blocks)
     */
    B2070(sc_core::sc_module_name name, Parameters &parameters, MSP &config);
};

#endif /*B2070_H_*/