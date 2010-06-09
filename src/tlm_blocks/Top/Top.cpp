#include "Top.h"

const struct {
    uint32_t size;
    sc_dt::uint64 base;
    sc_dt::uint64 mask;
} Memories[TOP_NUM_MEMORIES] = {
        {65536, 0x00000000, 0xFFFFFFFFFF000000LL}
};

Top::Top(sc_core::sc_module_name name, Parameters& parameters, MSP& config)
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

    // create the BUS instance (1 masters, memories+2 slaves))
    bus = new Bus<1,TOP_NUM_MEMORIES+2> ("bus");

    // create the CPU instance
    cpu = new Cpu("cpu", *cpu_parameter->get_string(), parameters, *cpu_config);
    // bind the CPU socket to the first targ socket of the BUS
    cpu->bind(*(bus->targ_socket[0]));

    for (i = 0; i < sizeof(Memories)/sizeof(Memories[0]); i++)
    {
        uint32_t* data;

        // allocate the memory
        data = (uint32_t*)malloc(Memories[i].size);
        // create the MEMORY instance with specific size
        memory[i] = new Memory("memory", data, Memories[i].size);
        // bind the init port of the BUS to the MEMORY
        ( *(bus->init_socket[i]) ).bind(memory[i]->slave_socket);
        // specify the MEMORY address range from the BUS perspective
        if (bus->set_range(i, Memories[i].base, Memories[i].mask))
        {
            TLM_ERR("Memory address range wrong @0x%08llX mask=0x%08llX", Memories[i].base, Memories[i].mask);
            return;
        }
    }

    intctrl = new IntCtrl("intctrl");
    // bind the init port of the BUS to the INTCTRL
    ( *(bus->init_socket[TOP_NUM_MEMORIES]) ).bind(intctrl->reg_socket);
    intctrl->irq_socket.bind(cpu->irq_s_socket);
    intctrl->fiq_socket.bind(cpu->fiq_s_socket);

    // specify the INTCTRL address range from the BUS perspective
    if (bus->set_range(TOP_NUM_MEMORIES, 0x01000000, 0xFFFFFFFFFF000000LL))
    {
        printf("IntCtrl address range wrong\n");
        return;
    }

    // create the dummy module
    dummy = new Dummy("dummy");
    // bind the MAC socket to the second targ socket of the BUS
    dummy->bind(&intctrl->int_socket);
    // bind the init port of the BUS to the MAC
    ( *(bus->init_socket[TOP_NUM_MEMORIES+1]) ).bind(dummy->slave_socket);
    // specify the MAC address range from the BUS perspective
    if (bus->set_range(TOP_NUM_MEMORIES+1, 0x02000000, 0xFFFFFFFFFF000000LL))
    {
        TLM_ERR("Dummy address range is wrong %d", -1);
        return;
    }
}


