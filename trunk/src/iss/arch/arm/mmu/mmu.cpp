/** @file mmu.cpp
 *
 * MMU definition file.
 */

#include "mmu.h"

void
mmu::arm_exec_cycles(int cycles)
{
    // call the CPU callback function
    m_bus.exec_cycles(m_bus.obj, cycles);
}
