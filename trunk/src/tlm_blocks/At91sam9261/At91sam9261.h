#ifndef AT91SAM9261_H_
#define AT91SAM9261_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious include
#include "systemc"

// command line parameters
#include "Parameters.h"

// generic blocks
#include "AddrDec.h"
#include "Cpu.h"
#include "Memory.h"
// system specific blocks
#include "Aic/Aic.h"
#include "Smc/Smc.h"

// necessary for dummies
#include "tlm_utils/simple_initiator_socket.h"

/// At91sam9261 level platform module
struct At91sam9261 : sc_core::sc_module
{
    /// Address decoder
    AddrDec<4>* addrdec;
    /// ARM 926EJ-S instruction simulation
    Cpu* cpu;
    /// Advanced Interrupt controller
    Aic* aic;
    /// Static Memory controller
    Smc* smc;
    /// Embedded SRAM
    Memory* sram;

    /** Constructor of the module
     * @param[in] name Name of the module
     * @param[in] parameters Command line parameters
     * @param[in] config Parameters of the current block (and sub-blocks)
     */
    At91sam9261(sc_core::sc_module_name name, Parameters& parameters, MSP& config);
};

#endif /*AT91SAM9261_H_*/
