#include "Bob.h"

#define ROM_BASE_ADDR (0x00000000)
#define ROM_SIZE (352*1024)

#define SRAM_BASE_ADDR (0x00080000)
#define SRAM_SIZE (104*1024)

#define FLASH_BASE_ADDR (0xFC000000)
#define FLASH_SIZE (512*1024)

// peripherals base addresses
#define IC_BASE_ADDR    (0x000C0000)
#define PRC_BASE_ADDR   (0x000D8000)
#define PTU_BASE_ADDR   (0x000E0074)
#define DC_BASE_ADDR    (0x000E8024)
#define SRI_BASE_ADDR   (0x000E8510)
#define RBG_BASE_ADDR   (0x000E9A00)
#define PMU_BASE_ADDR   (0x000F0000)
#define FM_BASE_ADDR    (0x000F2000)
#define GPIO2_BASE_ADDR (0x000FB000)
#define GPIO_BASE_ADDR  (0x000FC000)

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

    // PTU:
    //   - create the instance
    this->ptu = new Ptu("ptu");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->ptu, PTU_BASE_ADDR))
    {
        TLM_ERR("PTU address range wrong %d", 0);
        return;
    }

    // PRC:
    //   - create the instance
    this->prc = new Prc("prc");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->prc, PRC_BASE_ADDR))
    {
        TLM_ERR("PRC address range wrong %d", 0);
        return;
    }

    // SRI:
    //   - create the instance
    this->sri = new Sri("sri");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->sri, SRI_BASE_ADDR))
    {
        TLM_ERR("SRI address range wrong %d", 0);
        return;
    }

    // DC:
    //   - create the instance
    this->dc = new Dc("dc");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->dc, DC_BASE_ADDR))
    {
        TLM_ERR("DC address range wrong %d", 0);
        return;
    }

    // FM:
    //   - create the instance
    this->fm = new Fm("fm");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->fm, FM_BASE_ADDR))
    {
        TLM_ERR("FM address range wrong %d", 0);
        return;
    }

    // IC:
    //   - create the instance
    this->ic = new Pl190("ic");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->ic, IC_BASE_ADDR))
    {
        TLM_ERR("IC address range wrong %d", 0);
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


