#include "B2070.h"

#define ROM_BASE_ADDR (0x00000000)
#define ROM_SIZE (352*1024)

#define SRAM_BASE_ADDR (0x00080000)
#define SRAM_SIZE (104*1024)

B2070::B2070(sc_core::sc_module_name name, Parameters& parameters, MSP& config)
{
    uint32_t *romdata, *sramdata;
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
    this->addrdec = new AddrDec<2> ("addrdec");

    // CPU:
    //   - create instance
    this->cpu = new Cpu("cpu", *cpu_parameter->get_string(), parameters, *cpu_config);
    //   - bind interfaces (CPU access to address decoder)
    this->cpu->bus_m_socket.bind(this->addrdec->slave_socket);

    // ROM:
    //   - allocate the memory needed
    romdata = (uint32_t*)malloc(ROM_SIZE);
    //   - create the memory instance
    this->rom = new Memory("rom", romdata, ROM_SIZE);
    //   - bind interface (rom is hooked to the address decoder)
    if (this->addrdec->bind(this->rom->slave_socket, ROM_BASE_ADDR, ROM_BASE_ADDR+ROM_SIZE))
    {
        TLM_ERR("ROM address range wrong %d", 0);
        return;
    }

    // SRAM:
    //   - create instance
    // allocate the memory needed
    sramdata = (uint32_t*)malloc(SRAM_SIZE);
    //   - create the memory instance
    this->sram = new Memory("sram", sramdata, SRAM_SIZE);
    //   - bind interface (sram is hooked to the address decoder)
    if (this->addrdec->bind(this->sram->slave_socket, SRAM_BASE_ADDR, SRAM_BASE_ADDR+SRAM_SIZE))
    {
        TLM_ERR("SRAM address range wrong %d", 0);
        return;
    }


    // hook the ARM input interrupts to dummies if not yet connected
    if (this->cpu->fiq_s_socket.get_base_port().get_interface() == NULL)
    {
        char txt[256];
        tlm_utils::simple_initiator_socket<B2070>* dummy_m_socket;

        sprintf(txt, "dummy_fiq_m_socket");
        dummy_m_socket = new tlm_utils::simple_initiator_socket<B2070>(txt);
        dummy_m_socket->bind(this->cpu->fiq_s_socket);
    }
    if (this->cpu->irq_s_socket.get_base_port().get_interface() == NULL)
    {
        char txt[256];
        tlm_utils::simple_initiator_socket<B2070>* dummy_m_socket;

        sprintf(txt, "dummy_irq_m_socket");
        dummy_m_socket = new tlm_utils::simple_initiator_socket<B2070>(txt);
        dummy_m_socket->bind(this->cpu->irq_s_socket);
    }
}


