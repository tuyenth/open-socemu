#include "Bob.h"

#define ROM_BASE_ADDR (0x00000000)
#define ROM_SIZE (352*1024)

#define SRAM_BASE_ADDR (0x00080000)
#define SRAM_SIZE (104*1024)

#define FLASH_BASE_ADDR (0xFC000000)
#define FLASH_SIZE (512*1024)

// peripherals base addresses
#define PMU_BASE_ADDR (0x000F0000)
#define GPIO2_BASE_ADDR (0x000FB000)
#define GPIO_BASE_ADDR (0x000FC000)
#define RBG_BASE_ADDR (0x000E9A00)

Bob::Bob(sc_core::sc_module_name name, Parameters& parameters, MSP& config)
{
    Parameter *cpu_parameter;
    MSP *cpu_config;

    // sanity check: check parameters
    if (config.count("cpu") != 1)
    {
        TLM_ERR("CPU definitions found: %d", config.count("cpu"));
        return;
    }
    cpu_parameter = config["cpu"];
    cpu_config = cpu_parameter->get_config();

    // create the address decoder instance
    this->addrdec = new AddrDec("addrdec");

    // CPU:
    //   - create instance
    this->cpu = new Cpu("cpu", *cpu_parameter->get_string(), parameters, *cpu_config);
    //   - bind interfaces (CPU access to address decoder)
    this->cpu->bind(*this->addrdec);

    // ROM:
    //   - create the instance
    this->rom = new Rom("rom", ROM_SIZE);
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->rom, ROM_BASE_ADDR))
    {
        TLM_ERR("ROM address range wrong %d", 0);
        return;
    }

    // SRAM:
    //   - create the instance
    this->sram = new Memory("sram", SRAM_SIZE);
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->sram, SRAM_BASE_ADDR))
    {
        TLM_ERR("SRAM address range wrong %d", 0);
        return;
    }

    // FLASH:
    //   - create the instance
    this->flash = new Rom("flash", FLASH_SIZE);
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->flash, FLASH_BASE_ADDR))
    {
        TLM_ERR("FLASH address range wrong %d", 0);
        return;
    }

    // PMU:
    //   - create the instance
    this->pmu = new Pmu("pmu");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->pmu, PMU_BASE_ADDR))
    {
        TLM_ERR("PMU address range wrong %d", 0);
        return;
    }

    // GPIO:
    //   - create the instance
    this->gpio = new Pl061("gpio");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->gpio, GPIO_BASE_ADDR))
    {
        TLM_ERR("GPIO address range wrong %d", 0);
        return;
    }

    // GPIO2:
    //   - create the instance
    this->gpio2 = new Pl061("gpio2");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->gpio2, GPIO2_BASE_ADDR))
    {
        TLM_ERR("GPIO2 address range wrong %d", 0);
        return;
    }

    // RBG:
    //   - create the instance
    this->rbg = new Rbg("rbg");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->rbg, RBG_BASE_ADDR))
    {
        TLM_ERR("RBG address range wrong %d", 0);
        return;
    }

    // hook the ARM input interrupts to dummies if not yet connected
    if (!this->cpu->fiq.is_bound())
    {
        tlm_utils::simple_initiator_socket<Bob>* dummy_m_socket;

        dummy_m_socket = new tlm_utils::simple_initiator_socket<Bob>("dummy_fiq_m_socket");
        dummy_m_socket->bind(this->cpu->fiq);
    }
    if (!this->cpu->irq.is_bound())
    {
        tlm_utils::simple_initiator_socket<Bob>* dummy_m_socket;

        dummy_m_socket = new tlm_utils::simple_initiator_socket<Bob>("dummy_irq_m_socket");
        dummy_m_socket->bind(this->cpu->irq);
    }
}


