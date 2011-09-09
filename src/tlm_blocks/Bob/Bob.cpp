#include "Bob.h"

#define ROM_BASE_ADDR (0x00000000)
#define ROM_SIZE (352*1024)

#define SRAM_BASE_ADDR (0x00080000)
#define SRAM_SIZE (104*1024)

#define FLASH_BASE_ADDR (0xFC000000)
#define FLASH_SIZE (512*1024)

// peripherals base addresses
#define IC_BASE_ADDR    (0x000C0000)
#define DMAC_BASE_ADDR  (0x000C8000)
#define PRC_BASE_ADDR   (0x000D8000)
#define PTU_BASE_ADDR   (0x000E0000)
#define PHY_BASE_ADDR   (0x000E8000)
#define RBG_BASE_ADDR   (0x000E9A00)
#define RF_BASE_ADDR    (0x000EF800)
#define PMU_BASE_ADDR   (0x000F0000)
#define FM_BASE_ADDR    (0x000F2000)
#define RMP_BASE_ADDR   (0x000F8000)
#define WDOG_BASE_ADDR  (0x000F9000)
#define TIMER_BASE_ADDR (0x000FA000)
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
    
    // create the multi port arbiter
    this->mpa = new Mpa<2>("mpa");

    // create the address decoder instance
    this->addrdec = new AddrDec("addrdec");
    
    // CPU:
    //   - create instance
    this->cpu = new Cpu("cpu", *cpu_parameter->get_string(), parameters, *cpu_config);

    // ROM:
    //   - create the instance
    this->rom = new Rom("rom", ROM_SIZE);
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->rom, ROM_BASE_ADDR))
    {
        TLM_ERR("ROM address range wrong");
        return;
    }

    // SRAM:
    //   - create the instance
    this->sram = new Memory("sram", SRAM_SIZE);
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->sram, SRAM_BASE_ADDR))
    {
        TLM_ERR("SRAM address range wrong");
        return;
    }

    // FLASH:
    //   - create the instance
    this->flash = new Rom("flash", FLASH_SIZE);
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->flash, FLASH_BASE_ADDR))
    {
        TLM_ERR("FLASH address range wrong");
        return;
    }

    // PHY:
    //   - create the instance
    this->phy = new Phy("phy");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->phy, PHY_BASE_ADDR))
    {
        TLM_ERR("PHY address range wrong");
        return;
    }

    // PMU:
    //   - create the instance
    this->pmu = new Pmu("pmu");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->pmu, PMU_BASE_ADDR))
    {
        TLM_ERR("PMU address range wrong");
        return;
    }

    // GPIO:
    //   - create the instance
    this->gpio = new Pl061("gpio");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->gpio, GPIO_BASE_ADDR))
    {
        TLM_ERR("GPIO address range wrong");
        return;
    }

    // GPIO2:
    //   - create the instance
    this->gpio2 = new Pl061("gpio2");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->gpio2, GPIO2_BASE_ADDR))
    {
        TLM_ERR("GPIO2 address range wrong");
        return;
    }

    // RBG:
    //   - create the instance
    this->rbg = new Rbg("rbg");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->rbg, RBG_BASE_ADDR))
    {
        TLM_ERR("RBG address range wrong");
        return;
    }

    // PTU:
    //   - create the instance
    this->ptu = new Ptu("ptu");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->ptu, PTU_BASE_ADDR))
    {
        TLM_ERR("PTU address range wrong");
        return;
    }

    // PRC:
    //   - create the instance
    this->prc = new Prc("prc");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->prc, PRC_BASE_ADDR))
    {
        TLM_ERR("PRC address range wrong");
        return;
    }

    // FM:
    //   - create the instance
    this->fm = new Fm("fm");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->fm, FM_BASE_ADDR))
    {
        TLM_ERR("FM address range wrong");
        return;
    }

    // IC:
    //   - create the instance
    this->ic = new Pl190("ic");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->ic, IC_BASE_ADDR))
    {
        TLM_ERR("IC address range wrong");
        return;
    }

    // TIMER:
    //   - create the instance
    this->timer = new Sp804("timer");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->timer, TIMER_BASE_ADDR))
    {
        TLM_ERR("TIMER address range wrong");
        return;
    }
    this->timer->set_debug(false);

    // RMP:
    //   - create the instance
    this->rmp = new Rmp("rmp");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->rmp, RMP_BASE_ADDR))
    {
        TLM_ERR("RMP address range wrong");
        return;
    }

    // WDOG:
    //   - create the instance
    this->wdog = new Sp805("wdog");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->wdog, WDOG_BASE_ADDR))
    {
        TLM_ERR("WDOG address range wrong");
        return;
    }
    this->wdog->set_lock_mode(false);
    this->wdog->set_debug(false);

    // RF:
    //   - create the instance
    this->rf = new Rf("rf");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->rf, RF_BASE_ADDR))
    {
        TLM_ERR("RF address range wrong");
        return;
    }
    this->rf->set_debug(false);

    // DMAC:
    //   - create the instance
    this->dmac = new Pl081("dmac");
    //   - bind interface (hook to the address decoder)
    if (this->addrdec->bind(*this->dmac, DMAC_BASE_ADDR))
    {
        TLM_ERR("DMAC address range wrong");
        return;
    }
    this->dmac->set_debug(true);

    // hook the bus connections
    this->cpu->bind(*this->mpa->get_slave(0));
    this->dmac->bind(*this->mpa->get_slave(1));
    this->mpa->bind(*this->addrdec);

    // hook the interrupts
    this->dmac->intr.bind(*this->ic->vicintsource[5]);
    this->timer->t1int.bind(*this->ic->vicintsource[6]);
    this->timer->t2int.bind(*this->ic->vicintsource[7]);
    this->wdog->intsource.bind(*this->ic->vicintsource[14]);
    this->ic->fiq.bind(this->cpu->fiq);
    this->ic->irq.bind(this->cpu->irq);
    
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

    // create the module thread
    SC_THREAD(thread_process);
}

void
Bob::thread_process()
{
    while (true)
    {
        TLM_DBG("tick");

        // wait for 1 second
        sc_core::wait(sc_core::sc_time(50, sc_core::SC_MS));
    }
}


