#include "Mc13224v.h"
#include "elfreader.h"

#include "systemc.h"

#include <stdio.h>
#include <fcntl.h>

#define ROM_BASE_ADDR (0x00000000)
#define ROM_SIZE (0x14000)

#define SRAM_BASE_ADDR (0x00400000)
#define SRAM_SIZE (0x18000)

#define FLASH_SIZE (0x20000)

#define REG_UART1_BASE_ADDR 0x80005000
#define REG_UART2_BASE_ADDR 0x8000B000

Mc13224v::Mc13224v(sc_core::sc_module_name name, struct Parameters& Parameters)
{
    // create the address decoder instance
    this->addrdec = new AddrDec<9> ("addrdec");

    // ARM7TDMI CPU:
    //   - create instance
    this->cpu = new Cpu("cpu", "ARM7TDMI", Parameters.gdb_enabled, Parameters.gdb_wait);
    //   - bind interfaces (CPU access to address decoder)
    this->cpu->bus_m_socket.bind( this->addrdec->bus_s_socket );

    // ROM:
    //   - create instance
    {
        uint32_t* data;
        int fd;

        // allocate the memory needed
        data = (uint32_t*)malloc(ROM_SIZE);

        // open the ROM file specified
        fd = open(Parameters.romfile, O_RDONLY);
        if (fd == -1)
        {
            TLM_ERR("ROM file (%s) could not be read\n", Parameters.romfile);
        }

        read(fd, data, ROM_SIZE);

        close(fd);

        // create the memory
        this->rom = new Memory("rom", data, ROM_SIZE);
    }
    //   - bind interface (rom is hooked to the address decoder)
    if (this->addrdec->bind(&this->rom->socket, ROM_BASE_ADDR, ROM_BASE_ADDR+ROM_SIZE))
    {
        TLM_ERR("ROM address range wrong %d", 0);
        return;
    }

    // SRAM:
    //   - create instance
    {
        uint32_t* data;

        // allocate the memory needed
        data = (uint32_t*)malloc(SRAM_SIZE);

        // create the memory
        this->sram = new Memory("sram", data, SRAM_SIZE);
    }
    //   - bind interface (sram is hooked to the address decoder)
    if (this->addrdec->bind(&this->sram->socket, SRAM_BASE_ADDR, SRAM_BASE_ADDR+SRAM_SIZE))
    {
        TLM_ERR("SRAM address range wrong %d", 0);
        return;
    }

    // ITC:
    //   - create instance
    this->itc = new Itc("itc");
    //   - bind interfaces
    //      -> CPU IRQ and FIQ
    this->itc->irq_m_socket.bind(this->cpu->irq_s_socket);
    this->itc->fiq_m_socket.bind(this->cpu->fiq_s_socket);
    //      -> address decoder
    if (this->addrdec->bind(&this->itc->reg_s_socket, REG_ITC_BASE_ADDR,
        REG_ITC_BASE_ADDR + (REG_ITC_COUNT*4)))
    {
        TLM_ERR("ITC address range wrong %d", 0);
        return;
    }

    // CRM:
    //   - create instance
    this->crm = new Crm("crm");
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(&this->crm->reg_s_socket, REG_CRM_BASE_ADDR,
        REG_CRM_BASE_ADDR + (REG_CRM_COUNT*4)))
    {
        TLM_ERR("CRM address range wrong %d", 0);
        return;
    }
    //      -> interrupt hooked to ITC
    this->crm->int_m_socket.bind(*this->itc->int_s_socket[3]);

    // SPIF:
    //   - create instance
    {
        uint8_t* data;
        int fd;

        // allocate the memory needed
        data = (uint8_t*)malloc(FLASH_SIZE);

        // open the FLASH file specified
        fd = open(Parameters.flashfile, O_RDONLY);
        if (fd == -1)
        {
            TLM_ERR("FLASH file (%s) could not be read\n", Parameters.flashfile);
        }

        // load the FLASH content into the model
        read(fd, data, FLASH_SIZE);

        // if the SRAM was preloaded with the FLASH
        if (Parameters.flash_preloaded)
        {
            memcpy(this->sram->m_data, &(data[8]), U32(data[4]));

            // initialize the CRM
            this->crm->m_reg[STATUS_INDEX] = 3;
        }

        close(fd);

        this->spif = new Spif("spif", data, FLASH_SIZE);
    }
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(&this->spif->reg_s_socket, REG_SPIF_BASE_ADDR,
        REG_SPIF_BASE_ADDR + (REG_SPIF_COUNT*4)))
    {
        TLM_ERR("SPIF address range wrong %d", 0);
        return;
    }
    //      -> interrupt hooked to ITC
    this->spif->int_m_socket.bind(*this->itc->int_s_socket[6]);

    // SPI:
    //   - create instance
    this->spi = new Spi("spi");
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(&this->spi->reg_s_socket, REG_SPI_BASE_ADDR,
        REG_SPI_BASE_ADDR + (REG_SPI_COUNT*4)))
    {
        TLM_ERR("SPI address range wrong %d", 0);
        return;
    }
    //      -> interrupt hooked to ITC
    this->spi->int_m_socket.bind(*this->itc->int_s_socket[10]);

    // GPIO:
    //   - create instance
    this->gpio = new Gpio("gpio");
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(&this->gpio->reg_s_socket, REG_GPIO_BASE_ADDR,
        REG_GPIO_BASE_ADDR + (REG_GPIO_COUNT*4)))
    {
        TLM_ERR("GPIO address range wrong %d", 0);
        return;
    }

    // UART1:
    //   - create instance
    this->uart1 = new Uart(0, "uart1");
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(&this->uart1->reg_s_socket, REG_UART1_BASE_ADDR,
        REG_UART1_BASE_ADDR + (REG_UART_COUNT*4)))
    {
        TLM_ERR("UART1 address range wrong %d", 0);
        return;
    }
    //      -> interrupt hooked to ITC
    this->uart1->int_m_socket.bind(*this->itc->int_s_socket[1]);

    // UART2:
    //   - create instance
    this->uart2 = new Uart(1, "uart2");
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(&this->uart2->reg_s_socket, REG_UART2_BASE_ADDR,
        REG_UART2_BASE_ADDR + (REG_UART_COUNT*4)))
    {
        TLM_ERR("UART2 address range wrong %d", 0);
        return;
    }
    //      -> interrupt hooked to ITC
    this->uart2->int_m_socket.bind(*this->itc->int_s_socket[2]);

    // hook all the interrupt sources not connected to dummies
    for (int i = 0; i < Itc::NUM_INT; i++)
    {
        char txt[256];
        tlm_utils::simple_initiator_socket<Mc13224v>* dummy_m_socket;

        // check if the socket is already bound
        if (this->itc->int_s_socket[i]->get_base_port().get_interface() == NULL)
        {
            sprintf(txt, "dummy_itc_int_m_socket[%d]", i);
            dummy_m_socket = new tlm_utils::simple_initiator_socket<Mc13224v>(txt);
            dummy_m_socket->bind(*this->itc->int_s_socket[i]);
        }
    }

#if 0
    // create an instance of ElfReader
    CElfReader ElfReader;

    // open the ELF file
    ElfReader.Open(Parameters.elffile);

    // use a Segment pointer
    CSegment* Segment;

    // loop on all the segments and copy the loadables in memory
    while ((Segment = ElfReader.GetNextSegment()) != NULL)
    {
        // check if the segment is mapped to the sram
        if ((Segment->Address() >= SRAM_BASE_ADDR) &&
            ((Segment->Address()+Segment->Size()) <= (SRAM_SIZE)))
        {
            // then load the segment in the sram
            memcpy(&sram->m_data[Segment->Address()-SRAM_BASE_ADDR], Segment->Data(), Segment->Size());
            break;
        }
        else
        {
            SC_REPORT_FATAL("TLM-2", "ELF file can not be loaded");
        }
    }
#endif

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


