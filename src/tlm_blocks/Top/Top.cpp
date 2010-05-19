#include "Top.h"
#include "elfreader.h"

const struct {
    uint32_t size;
    sc_dt::uint64 base;
    sc_dt::uint64 mask;
} Memories[TOP_NUM_MEMORIES] = {
        {65536, 0x00000000, 0xFFFFFFFFFF000000LL}
};



Top::Top(sc_core::sc_module_name name, struct Parameters& Parameters)
{
    uint8_t i;
    CElfReader ElfReader;
    CSegment* Segment;

    // create the BUS instance
    bus = new Bus<2,TOP_NUM_MEMORIES+2> ("bus");

    // create the CPU instance
    cpu = new Cpu("cpu", "ARM7TDMI", Parameters.gdb_enabled, Parameters.gdb_wait);
    // bind the CPU socket to the first targ socket of the BUS
    cpu->bus_m_socket.bind( *(bus->targ_socket[0]) );

    for (i = 0; i < sizeof(Memories)/sizeof(Memories[0]); i++)
    {
        uint32_t* data;

        // allocate the memory
        data = (uint32_t*)malloc(Memories[i].size);
        // create the MEMORY instance with specific size
        memory[i] = new Memory("memory", data, Memories[i].size);
        // bind the init port of the BUS to the MEMORY
        ( *(bus->init_socket[i]) ).bind( memory[i]->socket );
        // specify the MEMORY address range from the BUS perspective
        if (bus->set_range(i, Memories[i].base, Memories[i].mask))
        {
            printf("Memory address range wrong\n");
            return;
        }
    }

    intctrl = new IntCtrl("intctrl");
    // bind the init port of the BUS to the INTCTRL
    ( *(bus->init_socket[TOP_NUM_MEMORIES]) ).bind( intctrl->reg_socket );
    intctrl->irq_socket.bind(cpu->irq_s_socket);
    intctrl->fiq_socket.bind(cpu->fiq_s_socket);

    // specify the INTCTRL address range from the BUS perspective
    if (bus->set_range(TOP_NUM_MEMORIES, 0x01000000, 0xFFFFFFFFFF000000LL))
    {
        printf("IntCtrl address range wrong\n");
        return;
    }

    dummy = new Dummy("dummy");
    // bind the MAC socket to the second targ socket of the BUS
    dummy->init_socket.bind( *(bus->targ_socket[1]) );
    dummy->int_socket.bind(intctrl->int_socket);
    // bind the init port of the BUS to the MAC
    ( *(bus->init_socket[TOP_NUM_MEMORIES+1]) ).bind(dummy->targ_socket);
    // specify the MAC address range from the BUS perspective
    if (bus->set_range(TOP_NUM_MEMORIES+1, 0x02000000, 0xFFFFFFFFFF000000LL))
    {
        printf("Dummy address range wrong\n");
        return;
    }

    // open the ELF file
    ElfReader.Open(Parameters.elffile);

    // loop on all the segments and copy the loadables in memory
    while ((Segment = ElfReader.GetNextSegment()) != NULL)
    {
        // look for the corresponding memory
        for (i = 0; i < sizeof(Memories)/sizeof(Memories[0]); i++)
        {
            if ((Segment->Address() >= Memories[i].base) &&
                ((Segment->Address()+Segment->Size()) <= (Memories[i].base+Memories[i].size)))
            {
                memcpy(&memory[i]->m_data[Segment->Address()-Memories[i].base], Segment->Data(), Segment->Size());
                break;
            }
        }
        // check if memory was found
        if (i >= sizeof(Memories)/sizeof(Memories[0]))
        {
            SC_REPORT_FATAL("TLM-2", "ELF file can not be loaded");
        }
    }

}

