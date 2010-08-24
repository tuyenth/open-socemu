#ifndef ROM_H_
#define ROM_H_

#include "SimpleSlave.h"

/// Generic Rom TLM module, deriving the simple slave
struct Rom : SimpleSlave
{
    /** Rom class constructor
     * @param name Name of the module
     * @param[in] data Pointer to the allocated memory
     * @param[in] size Size of the memory module in bytes
     */
    Rom(sc_core::sc_module_name name, uint32_t* data, uint32_t size)
    : SimpleSlave(name, data, size)
    {
    }

    /** Rom class constructor - allocating locally the memory
     * @param name Name of the module
     * @param[in] size Size of the memory module in bytes
     */
    Rom(sc_core::sc_module_name name, uint32_t size)
    : SimpleSlave(name, (uint32_t*)malloc(size), size)
    {
    }

    /// Override the virtual function
    void
    slave_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
    {
        if (likely(trans.get_command() == tlm::TLM_READ_COMMAND))
        {
            SimpleSlave::slave_b_transport(trans, delay);
        }
        else
        {
            TLM_ERR("Non read access to ROM (%d)", trans.get_command());
        }
        return;
    }

};

#endif /*ROM_H_*/
