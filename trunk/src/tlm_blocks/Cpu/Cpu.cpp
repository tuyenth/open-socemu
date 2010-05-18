#include "Cpu.h"

// include the tools
#include "arm926ejs.h"
#include "arm7tdmi.h"

// using this namespace to simplify streaming
using namespace std;

uint64_t *g_numinstr;

Cpu::Cpu(sc_core::sc_module_name name, std::string cpuname, bool gdbserver, bool gdbstart)
: bus_m_socket("bus_m_socket")
, irq_s_socket("irq_s_socket")
, fiq_s_socket("fiq_s_socket")
{
    struct mmu::bus bus;

    // force the default values of the BUS transaction
    bus_pl.set_streaming_width(4);
    bus_pl.set_byte_enable_ptr(0);
    bus_pl.set_dmi_allowed(false);
    // register callbacks for incoming interface method calls
    bus_m_socket.register_nb_transport_bw(this, &Cpu::bus_m_nb_transport_bw);
    // only blocking calls supported by IRQ and FIQ sockets
    irq_s_socket.register_b_transport(this, &Cpu::irq_s_b_transport);
    fiq_s_socket.register_b_transport(this, &Cpu::fiq_s_b_transport);

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

    if (cpuname == "ARM926EJ-S")
    {
        this->m_arm = new arm926ejs(&bus, gdbserver, gdbstart);
    }
    else if (cpuname == "ARM7TDMI")
    {
        this->m_arm = new arm7tdmi(&bus, gdbserver, gdbstart);
    }
    else
    {
        TLM_ERR("Unsupported CPU name(%s)", cpuname.c_str());
    }

    // save the reference to the number of instructions executed
    g_numinstr = &(m_arm->m_NumInstrs);

    SC_THREAD(thread_process);
}

void Cpu::thread_process()
{
    // run armulator (should never return)
    this->m_arm->run();

    // should never reach here
    assert(0);
}

tlm::tlm_sync_enum Cpu::bus_m_nb_transport_bw( tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
    SC_REPORT_FATAL("TLM-2", "Non blocking not yet implemented");

    return tlm::TLM_ACCEPTED;
}

void Cpu::irq_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_INT_SANITY(trans);

    // retrieve the required parameters
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    // check if it is a set or clear command
    if (*ptr)
    {
        this->m_arm->irq_set();
        // notify the interrupt (after setting the interrupt in processor)
        m_interrupt.notify();
    }
    else
    {
        this->m_arm->irq_clear();
    }

    // there was no error in the processing of the transaction
    trans.set_response_status(tlm::TLM_OK_RESPONSE);

    return;
}

void Cpu::fiq_s_b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    TLM_INT_SANITY(trans);

    // retrieve the required parameters
    uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

    // check if it is a set or clear command
    if (*ptr)
    {
        this->m_arm->fiq_set();
        // notify the interrupt (after setting the interrupt in arm)
        m_interrupt.notify();
    }
    else
    {
        this->m_arm->fiq_clear();
    }

    // there was no error in the processing of the transaction
    trans.set_response_status(tlm::TLM_OK_RESPONSE);

    return;
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

