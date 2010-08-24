#ifndef MC13224V_H_
#define MC13224V_H_

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
#include "Itc/Itc.h"
#include "Crm/Crm.h"
#include "Spif/Spif.h"
#include "Spi/Spi.h"
#include "Gpio/Gpio.h"
#include "Uart/Uart.h"

// necessary for dummies
#include "tlm_utils/simple_initiator_socket.h"

/// Mc13224v level platform module
struct Mc13224v : sc_core::sc_module
{
    /// Address decoder
    AddrDec<9>* addrdec;
    /// ARM7TDMI-S instruction simulation
    Cpu* cpu;
    /// Embedded ROM
    Memory* rom;
    /// Embedded SRAM
    Memory* sram;
    /// Interrupt Controller
    Itc* itc;
    /// Clock and Reset Manager
    Crm* crm;
    /// SPI for internal FLASH
    Spif* spif;
    /// External peripheral SPI
    Spi* spi;
    /// GPIO
    Gpio* gpio;
    /// UART1
    Uart* uart1;
    /// UART2
    Uart* uart2;

    // Not necessary if this module does not have a thread
    //SC_HAS_PROCESS(Mc13224v);

    /** Constructor of the module
     * @param[in] name Name of the module
     * @param[in] parameters Command line parameters
     * @param[in] config Parameters of the current block (and sub-blocks)
     */
    Mc13224v(sc_core::sc_module_name name, Parameters& parameters, MSP& config);
};

#endif /*MC13224V_H_*/
