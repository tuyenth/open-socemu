#include "At91sam9261.h"

#include "systemc.h"

#define SRAM_BASE_ADDR (0x00000000)
#define SRAM_SIZE (0x28000)

At91sam9261::At91sam9261(sc_core::sc_module_name name, Parameters& parameters, MSP& config)
{
    uint8_t slave_id = 0;
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
    this->addrdec = new AddrDec<4> ("addrdec");

    // CPU:
    //   - create instance
    this->cpu = new Cpu("cpu", *cpu_parameter->get_string(), parameters, *cpu_config);
    //   - bind interfaces
    this->cpu->bus_m_socket.bind(this->addrdec->slave_socket);

    // SRAM:
    //   - create instance
    {
        uint32_t* data;

        // allocate the memory needed
        data = (uint32_t*)malloc(SRAM_SIZE);

        // create the memory
        this->sram = new Memory("sram", data, SRAM_SIZE);
    }
    //   - set range
    if (this->addrdec->set_range(slave_id, SRAM_BASE_ADDR, SRAM_BASE_ADDR+SRAM_SIZE))
    {
        TLM_ERR("SRAM address range wrong %d", 0);
        return;
    }
    //   - bind interfaces
    ( *(this->addrdec->bus_m_socket[slave_id]) ).bind(this->sram->slave_socket);
    //   - increment the address decoder slave id
    slave_id++;

    // SMC:
    //   - create instance
    this->smc = new Smc("smc");
    //   - set range (registers map)
    if (this->addrdec->set_range(slave_id, REG_SMC_BASE_ADDR, REG_SMC_BASE_ADDR+(REG_SMC_COUNT*4)))
    {
        TLM_ERR("SMC registers address range wrong %d", 0);
        return;
    }
    //   - bind interfaces
    ( *(this->addrdec->bus_m_socket[slave_id]) ).bind(this->smc->reg_s_socket);
    //   - increment the address decoder slave id
    slave_id++;
    //   - set range (memory map)
    if (this->addrdec->set_range(slave_id, 0x10000000, 0x90000000))
    {
        TLM_ERR("SMC memory address range wrong %d", 0);
        return;
    }
    //   - bind interfaces
    ( *(this->addrdec->bus_m_socket[slave_id]) ).bind(this->smc->bus_s_socket);
    //   - increment the address decoder slave id
    slave_id++;

    // AIC:
    //   - create instance
    this->aic = new Aic("aic");
    //   - set range
    if (this->addrdec->set_range(slave_id, REG_AIC_BASE_ADDR, REG_AIC_BASE_ADDR+(REG_AIC_COUNT*4)))
    {
        TLM_ERR("AIC address range wrong %d", 0);
        return;
    }
    //   - bind interfaces
    ( *(this->addrdec->bus_m_socket[slave_id]) ).bind(this->aic->reg_s_socket);
    this->aic->irq_m_socket.bind(this->cpu->irq_s_socket);
    this->aic->fiq_m_socket.bind(this->cpu->fiq_s_socket);
    //     + by default, hook all the interrupt sources to dummies
    for (int i = 0; i < 32; i++)
    {
        if ( (i != 0) && (i != 29))
        {
            char txt[256];
            tlm_utils::simple_initiator_socket<At91sam9261>* dummy_int_m_socket;

            sprintf(txt, "dummy_int_m_socket[%d]", i);
            dummy_int_m_socket = new tlm_utils::simple_initiator_socket<At91sam9261>(txt);
            dummy_int_m_socket->bind(*this->aic->int_s_socket[i]);
        }
    }
    //   - increment the address decoder slave id
    slave_id++;

#if 0
    // create a trace file
    sc_trace_file* Tf;
    Tf = sc_create_vcd_trace_file("traces");
    sc_trace(Tf, this->cpu->m_currentaddr, "CPUADDR");
    sc_trace(Tf, this->cpu->m_currentdata, "CPUDATA");
    sc_trace(Tf, this->cpu->m_currentrw, "CPURW");
    sc_trace(Tf, this->cpu->m_currentirq, "CPUIRQ");
    sc_trace(Tf, this->cpu->m_currentfiq, "CPUFIQ");
#endif
}


