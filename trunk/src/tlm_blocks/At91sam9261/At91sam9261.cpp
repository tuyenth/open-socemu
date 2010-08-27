#include "At91sam9261.h"

#include "systemc.h"

#define SRAM_BASE_ADDR (0x00000000)
#define SRAM_SIZE (0x28000)

At91sam9261::At91sam9261(sc_core::sc_module_name name, Parameters& parameters, MSP& config)
{
    Parameter *cpu_parameter;

    // sanity check: check parameters
    if (config.count("cpu") != 1)
    {
        TLM_ERR("CPU definitions found: %d", config.count("cpu"));
        return;
    }
    cpu_parameter = config["cpu"];

    // create the address decoder instance
    this->addrdec = new AddrDec<4> ("addrdec");

    // CPU:
    //   - create instance
    this->cpu = new Cpu("cpu", *cpu_parameter, parameters, *cpu_parameter);
    //   - bind interfaces
    this->cpu->bind(*this->addrdec);

    // SRAM:
    //   - create instance
    this->sram = new Memory("sram", SRAM_SIZE);
    //   - set range
    if (this->addrdec->bind(*this->sram, SRAM_BASE_ADDR, (SRAM_BASE_ADDR+this->sram->get_size())))
    {
        TLM_ERR("SRAM address range wrong %d", 0);
        return;
    }

    // SMC:
    //   - create instance
    this->smc = new Smc("smc");
    //   - set range (registers map)
    if (this->addrdec->bind(*this->smc, REG_SMC_BASE_ADDR, REG_SMC_BASE_ADDR+this->smc->get_size()))
    {
        TLM_ERR("SMC registers address range wrong %d", 0);
        return;
    }
    //   - set range (memory map)
    if (this->addrdec->bind(this->smc->bus_s_socket, 0x10000000, 0x90000000))
    {
        TLM_ERR("SMC memory address range wrong %d", 0);
        return;
    }

    // AIC:
    //   - create instance
    this->aic = new Aic("aic");
    //   - set range
    if (this->addrdec->bind(*this->aic, REG_AIC_BASE_ADDR, REG_AIC_BASE_ADDR+this->aic->get_size()))
    {
        TLM_ERR("AIC address range wrong %d", 0);
        return;
    }
    //   - bind interfaces
    this->aic->irq_m_socket.bind(this->cpu->irq);
    this->aic->fiq_m_socket.bind(this->cpu->fiq);
    //     + by default, hook all the interrupt sources to dummies
    for (int i = 0; i < 32; i++)
    {
        if ((i != 0) && (i != 29))
        {
            char txt[256];
            tlm::tlm_initiator_socket<>* dummy_int_m_socket;

            sprintf(txt, "dummy_int_m_socket[%d]", i);
            dummy_int_m_socket = new tlm::tlm_initiator_socket<>(txt);
            dummy_int_m_socket->bind(*this->aic->int_s_socket[i]);
        }
    }

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


