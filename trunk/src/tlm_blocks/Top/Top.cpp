#include "Top.h"

const struct {
    uint32_t size;
    sc_dt::uint64 base;
    sc_dt::uint64 mask;
} Memories[TOP_NUM_MEMORIES] = {
        {65536, 0x00000000, 0xFFFFFFFFFF000000LL}
};

Top::Top(sc_core::sc_module_name name, Parameters &parameters, MSP &config)
{
    uint8_t i;
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

    // create the BUS instance
    bus = new Bus<2,TOP_NUM_MEMORIES+2> ("bus");

    // create the CPU instance
    cpu = new Cpu("cpu", *cpu_parameter->get_string(), parameters, *cpu_config);
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
        ( *(bus->init_socket[i]) ).bind( memory[i]->slave_socket );
        // specify the MEMORY address range from the BUS perspective
        if (bus->set_range(i, Memories[i].base, Memories[i].mask))
        {
            TLM_ERR("Memory address range wrong @0x%08llX mask=0x%08llX", Memories[i].base, Memories[i].mask);
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
}


