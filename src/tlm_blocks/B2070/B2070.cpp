#include "B2070.h"
#include "elfreader.h"

#define ROM_BASE_ADDR (0x00000000)
#define ROM_SIZE (352*1024)

#define SRAM_BASE_ADDR (0x00080000)
#define SRAM_SIZE (104*1024)

B2070::B2070(sc_core::sc_module_name name, struct Parameters& Parameters)
{
    CElfReader ElfReader;
    CSegment* Segment;
    uint32_t* data;

    // create the address decoder instance
    this->addrdec = new AddrDec<2> ("addrdec");

    // ARM7TDMI CPU:
    //   - create instance
    this->cpu = new Cpu("cpu", "ARM7TDMI", Parameters.gdb_enabled, Parameters.gdb_wait);
    //   - bind interfaces (CPU access to address decoder)
    this->cpu->bus_m_socket.bind(this->addrdec->bus_s_socket);

    // ROM:
    //   - allocate the memory needed
    data = (uint32_t*)malloc(ROM_SIZE);
    //   - create the memory instance
    this->rom = new Memory("rom", data, ROM_SIZE);
    //   - bind interface (rom is hooked to the address decoder)
    if (this->addrdec->bind(&this->rom->socket, ROM_BASE_ADDR, ROM_BASE_ADDR+ROM_SIZE))
    {
        TLM_ERR("ROM address range wrong %d", 0);
        return;
    }

    // SRAM:
    //   - create instance
    // allocate the memory needed
    data = (uint32_t*)malloc(SRAM_SIZE);
    //   - create the memory instance
    this->sram = new Memory("sram", data, SRAM_SIZE);
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

    // open the ELF file
    ElfReader.Open(Parameters.elffile);

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
            memcpy(&this->rom->m_data[(Segment->Address()-ROM_BASE_ADDR)/4], Segment->Data(), Segment->Size());
        }
    }
}


