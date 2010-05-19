#ifndef TOP_H_
#define TOP_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious include
#include "systemc"

// command line parameters
#include "Parameters.h"

// necessary for system definition
#include "Bus.h"
#include "Cpu.h"
#include "IntCtrl.h"
#include "Dummy.h"
#include "Memory.h"

// necessary for dummy target
#include "tlm_utils/simple_target_socket.h"

// Definition
#define TOP_NUM_MEMORIES 1

/// Top level plateform module.
struct Top : sc_core::sc_module
{
    Bus<2,TOP_NUM_MEMORIES+2>* bus;
    Cpu* cpu;
    IntCtrl* intctrl;
    Dummy* dummy;
    Memory* memory[TOP_NUM_MEMORIES];

    /** Constructor of the top level module.
     * @param[in] name Name of the module
     * @param[in] Parameters Parameters from the configuration file
     */
    Top(sc_core::sc_module_name name, struct Parameters& Parameters);
};

#endif /*TOP_H_*/
