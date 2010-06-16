#include "Top.h"
#include "GdbServerTcp.h"
#include "Arm32.h"

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
    tlm_utils::simple_target_socket<Cpu>* irq;
    /// Socket to receive FIQ set and clear commands
    tlm_utils::simple_target_socket<Cpu>* fiq;

    // sanity check: check parameters
    if (config.count("cpu") != 1)
    {
        TLM_ERR("CPU definitions found: %d", config.count("cpu"));
        return;
    }
    cpu_parameter = config["cpu"];

    // create the BUS instance (1 masters, memories+2 slaves))
    bus = new Bus<1,TOP_NUM_MEMORIES+2> ("bus");

    // check if the CPU requested is the cpubase
    if (*cpu_parameter == "CpuBase")
    {
        cpubase = new Arm32<GdbServerTcp>("cpu", parameters, *cpu_parameter);
        cpubase->bind(*(bus->targ_socket[0]));
        irq = new tlm_utils::simple_target_socket<Cpu>("false_irq");
        fiq = new tlm_utils::simple_target_socket<Cpu>("false_fiq");
    }
    else
    {
        // old style CPU
        // create the CPU instance
        cpu = new Cpu("cpu", *cpu_parameter, parameters, *cpu_parameter);
        // bind the CPU socket to the first targ socket of the BUS
        cpu->bind(*(bus->targ_socket[0]));

        irq = &cpu->irq_s_socket;
        fiq = &cpu->fiq_s_socket;
    }
    for (i = 0; i < sizeof(Memories)/sizeof(Memories[0]); i++)
    {
        uint32_t* data;

        // allocate the memory
        data = (uint32_t*)malloc(Memories[i].size);
        // create the MEMORY instance with specific size
        memory[i] = new Memory("memory", data, Memories[i].size);
        // bind the init port of the BUS to the MEMORY
        ( *(bus->init_socket[i]) ).bind(*memory[i]);
        // specify the MEMORY address range from the BUS perspective
        if (bus->set_range(i, Memories[i].base, Memories[i].mask))
        {
            TLM_ERR("Memory address range wrong @0x%08llX mask=0x%08llX", Memories[i].base, Memories[i].mask);
            return;
        }
    }

    intctrl = new IntCtrl("intctrl");
    // bind the init port of the BUS to the INTCTRL
    (*(bus->init_socket[TOP_NUM_MEMORIES])).bind(intctrl->reg_socket);
    intctrl->irq_socket.bind(*irq);
    intctrl->fiq_socket.bind(*fiq);

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
    (*(bus->init_socket[TOP_NUM_MEMORIES+1])).bind(*dummy);
    // specify the MAC address range from the BUS perspective
    if (bus->set_range(TOP_NUM_MEMORIES+1, 0x02000000, 0xFFFFFFFFFF000000LL))
    {
        TLM_ERR("Dummy address range is wrong %d", -1);
        return;
    }
}


