#ifndef MEMORY_H_
#define MEMORY_H_

#include "SimpleSlave.h"

/// Generic Memory TLM module, deriving the simple slave
struct Memory : SimpleSlave
{
    /** Memory class constructor
     * @param name Name of the module
     * @param[in] data Pointer to the allocated memory
     * @param[in] size Size of the memory module in bytes
     */
    Memory(sc_core::sc_module_name name, uint32_t* data, uint32_t size)
    : SimpleSlave(name, data, size)
    {
    }

    /** Memory class constructor
     * @param name Name of the module
     * @param[in] size Size of the memory module in bytes
     */
    Memory(sc_core::sc_module_name name, uint32_t size)
    : SimpleSlave(name, (uint32_t*)malloc(size), size)
    {
    }
};

#endif /*MEMORY_H_*/
