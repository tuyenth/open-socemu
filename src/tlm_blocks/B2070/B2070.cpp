#include "B2070.h"
#include "elfreader.h"

#define ROM_BASE_ADDR (0x00000000)
#define ROM_SIZE (352*1024)

#define SRAM_BASE_ADDR (0x00080000)
#define SRAM_SIZE (104*1024)

B2070::B2070(sc_core::sc_module_name name, Parameters &parameters, MSP &config)
{
    uint32_t *romdata, *sramdata;
    Parameter *cpu_parameter;
    MSP *cpu_config;
    Parameter *elffile;
    std::string elffilepath;

    // sanity check: check parameters
    if (config.count("cpu") != 1)
    {
        TLM_ERR("CPU definitions found: %d", parameters.config.count("cpu"));
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
    this->cpu->bus_m_socket.bind(this->addrdec->bus_s_socket);

    // ROM:
    //   - allocate the memory needed
    romdata = (uint32_t*)malloc(ROM_SIZE);
    //   - create the memory instance
    this->rom = new Memory("rom", romdata, ROM_SIZE);
    //   - bind interface (rom is hooked to the address decoder)
    if (this->addrdec->bind(&this->rom->socket, ROM_BASE_ADDR, ROM_BASE_ADDR+ROM_SIZE))
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
    if (this->addrdec->bind(&this->sram->socket, SRAM_BASE_ADDR, SRAM_BASE_ADDR+SRAM_SIZE))
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

    // check if there is an elf file defined
    if (cpu_config->count("elffile") != 1)
    {
        TLM_ERR("elffile definitions found: %d", cpu_config->count("elffile"));
    }

    elffile = (*cpu_config)["elffile"];
    elffilepath = parameters.configpath + *elffile->get_string();

    // create an instance of ElfReader
    CElfReader ElfReader;

    // open the ELF file
    ElfReader.Open(elffilepath.c_str());

    // use a Segment pointer
    CSegment* Segment;

    // loop on all the segments and copy the loadables in memory
    while ((Segment = ElfReader.GetNextSegment()) != NULL)
    {
        TLM_DBG("Segment = 0x%x[%d]", Segment->Address(), Segment->Size());

        // empty segments are not taken into account
        if (Segment->Size())
        {
            // make sure that this fits in the ROM
            if ((Segment->Address() < ROM_BASE_ADDR) ||
                (Segment->Address()+Segment->Size()) > (ROM_BASE_ADDR + ROM_SIZE))
            {
                TLM_ERR("ELF segment (0x%x[%d]) does not fit in ROM (0x%x[%d])",
                        Segment->Address(), Segment->Size(), ROM_BASE_ADDR, ROM_SIZE);
            }
            memcpy(&romdata[(Segment->Address()-ROM_BASE_ADDR)/4], Segment->Data(), Segment->Size());
        }
    }
}


