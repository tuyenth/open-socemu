#include "Cpu/Cpu.h"

// include the tools
#include "ElfReader/ElfReader.h"
#include "arm926ejs.h"
#include "arm7tdmi.h"

// using this namespace to simplify streaming
using namespace std;

uint64_t *g_numinstr;

Cpu::Cpu(sc_core::sc_module_name name, const std::string& cpuname, Parameters& parameters, MSP& config)
: BusMaster(name)
{
    // structure whose definition belongs to a specific class
    struct mmu::bus bus;
    Parameter* gdbserver;
    Parameter* elffile;

    // sanity check
    if (config.count("gdbserver") != 1)
    {
        TLM_ERR("CPU definitions found: %d", config.count("cpu"));
    }
    // check if there is an elf file defined
    if (config.count("elffile") == 0)
    {
        m_elfpath = NULL;
    }
    else
    {
        elffile = config["elffile"];
        elffile->add_path(parameters.configpath);
        // copy the path into an internal variable
        m_elfpath = elffile->get_string();
    }
    gdbserver = config["gdbserver"];

    // only blocking calls supported by IRQ and FIQ sockets
    this->irq.init(this, &Cpu::interrupt_set, &Cpu::interrupt_clr, (void*)0);
    this->fiq.init(this, &Cpu::interrupt_set, &Cpu::interrupt_clr, (void*)1);

    bus.obj = (void*)this;
    bus.rd_l = &rd_l_cb;
    bus.rd_s = &rd_s_cb;
    bus.rd_b = &rd_b_cb;
    bus.wr_l = &wr_l_cb;
    bus.wr_s = &wr_s_cb;
    bus.wr_b = &wr_b_cb;
    bus.gdb_rd = &gdb_rd_cb;
    bus.gdb_wr = &gdb_wr_cb;
    bus.exec_cycles = &exec_cycles_cb;
    bus.wfi = &wfi_cb;

    TLM_DBG("CPU: gdbserver = %s", gdbserver->get_bool()?"TRUE":"FALSE");
    TLM_DBG("CPU: gdbwait = %s", parameters.gdb_wait.get_bool()?"TRUE":"FALSE");
    if (cpuname == "ARM926EJ-S")
    {
        m_arm = new arm926ejs(&bus, gdbserver->get_bool(), parameters.gdb_wait.get_bool());
    }
    else if (cpuname == "ARM7TDMI")
    {
        m_arm = new arm7tdmi(&bus, gdbserver->get_bool(), parameters.gdb_wait.get_bool());
    }
    else
    {
        TLM_ERR("Unsupported CPU name(%s)", cpuname.c_str());
    }

    // save the reference to the number of instructions executed
    g_numinstr = &(m_arm->m_NumInstrs);

}

void
Cpu::thread_process()
{
    // create an instance of ElfReader
    ElfReader ElfReader;

    // check if there was an ELF file specified for this CPU
    if (m_elfpath != NULL)
    {
        // open the ELF file
        ElfReader.Open(m_elfpath->c_str());

        // use a Segment pointer
        Segment* Segment;

        // loop on all the segments and copy the loadables in memory
        while ((Segment = ElfReader.GetNextSegment()) != NULL)
        {
            TLM_DBG_WR(this->master_socket, this->master_b_pl, Segment->Address(), Segment->Data(), Segment->Size());
        }
    }

    // run armulator (should never return)
    m_arm->run();

    // should never reach here
    assert(0);
}

void
Cpu::interrupt_set(void* opaque)
{
    if (opaque == 0)
    {
        m_arm->irq_set();
    }
    else
    {
        m_arm->fiq_set();
    }
    // notify the interrupt (after setting the interrupt in processor)
    m_interrupt.notify();
}

void
Cpu::interrupt_clr(void* opaque)
{
    if (opaque == 0)
    {
        m_arm->irq_clear();
    }
    else
    {
        m_arm->fiq_clear();
    }
    // notify the interrupt (after setting the interrupt in processor)
    m_interrupt.notify();
}

uint32_t
Cpu::rd_l_cb(void *obj, uint32_t addr)
{
    struct Cpu* myself = (struct Cpu*)obj;
    return myself->rd_l(addr);
}

uint32_t
Cpu::rd_s_cb(void *obj, uint32_t addr)
{
    struct Cpu* myself = (struct Cpu*)obj;
    return myself->rd_s(addr);
}

uint32_t
Cpu::rd_b_cb(void *obj, uint32_t addr)
{
    struct Cpu* myself = (struct Cpu*)obj;
    return myself->rd_b(addr);
}

void
Cpu::wr_l_cb(void *obj, uint32_t addr, uint32_t data)
{
    struct Cpu* myself = (struct Cpu*)obj;
    myself->wr_l(addr, data);
}

void
Cpu::wr_s_cb(void *obj, uint32_t addr, uint32_t data)
{
    struct Cpu* myself = (struct Cpu*)obj;
    myself->wr_s(addr, data);
}

void
Cpu::wr_b_cb(void *obj, uint32_t addr, uint32_t data)
{
    struct Cpu* myself = (struct Cpu*)obj;
    myself->wr_b(addr, data);
}

int
Cpu::gdb_rd_cb(void *obj, uint64_t addr, uint8_t* dataptr, uint32_t len)
{
    struct Cpu* myself = (struct Cpu*)obj;
    return myself->gdb_rd(addr, dataptr, len);
}

int
Cpu::gdb_wr_cb(void *obj, uint64_t addr, uint8_t* dataptr, uint32_t len)
{
    struct Cpu* myself = (struct Cpu*)obj;
    return myself->gdb_wr(addr, dataptr, len);
}

void
Cpu::exec_cycles_cb(void *obj, int cycles)
{
    struct Cpu* myself = (struct Cpu*)obj;
    myself->exec_cycles(cycles);
}

void
Cpu::wfi_cb(void *obj)
{
    struct Cpu* myself = (struct Cpu*)obj;
    myself->wfi();
}

