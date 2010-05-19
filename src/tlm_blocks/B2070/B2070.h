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

/// B2070 level plateform module.
struct B2070 : sc_core::sc_module
{
    /// Address decoder
    AddrDec<2>* addrdec;
    Cpu* cpu;
    Memory* rom;
    Memory* sram;

    /** Constructor of the top level module.
     * @param[in] name Name of the module
     * @param[in] Parameters Parameters from the configuration file
     */
    B2070(sc_core::sc_module_name name, struct Parameters& Parameters);
};

#endif /*B2070_H_*/
