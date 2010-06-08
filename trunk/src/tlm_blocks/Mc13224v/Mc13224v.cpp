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

Mc13224v::Mc13224v(sc_core::sc_module_name name, Parameters &parameters, MSP &config)
{
    Parameter *cpu_parameter, *romfile, *flashfile, *srampreloadedwithflash;
    MSP *cpu_config;
    uint32_t *romdata, *sramdata, *flashdata;

    // sanity check
    if (config.count("cpu") != 1)
    {
        TLM_ERR("CPU definitions found: %d", parameters.config.count("cpu"));
        return;
    }
    if (config.count("romfile") != 1)
    {
        TLM_ERR("ROMFILE definitions found: %d\n", parameters.config.count("romfile"));
        return;
    }
    if (config.count("flashfile") != 1)
    {
        TLM_ERR("FLASHFILE definitions found: %d", parameters.config.count("flashfile"));
        return;
    }
    if (config.count("srampreloadedwithflash") != 1)
    {
        TLM_ERR("SRAMPRELOADEDWITHFLASH definitions found: %d", parameters.config.count("srampreloadedwithflash"));
        return;
    }
    cpu_parameter = config["cpu"];
    romfile = config["romfile"];
    flashfile = config["flashfile"];
    srampreloadedwithflash = config["srampreloadedwithflash"];
    cpu_config = cpu_parameter->get_config();

    // create the address decoder instance
    this->addrdec = new AddrDec<9> ("addrdec");

    // ARM7TDMI CPU:
    //   - create instance
    this->cpu = new Cpu("cpu", *cpu_parameter->get_string(), parameters, *cpu_config);
    //   - bind interfaces (CPU access to address decoder)
    this->cpu->bus_m_socket.bind( this->addrdec->bus_s_socket );

    // ROM:
    //   - create instance
    //       * allocate the memory needed
    romdata = (uint32_t*)malloc(ROM_SIZE);
    //       * create the memory
    this->rom = new Memory("rom", romdata, ROM_SIZE);
    //   - bind interface (ROM is hooked to the address decoder)
    if (this->addrdec->bind(&this->rom->slave_socket, ROM_BASE_ADDR, ROM_BASE_ADDR+ROM_SIZE))
    {
        TLM_ERR("ROM address range wrong %d", 0);
        return;
    }

    // SRAM:
    //   - create instance
    //       * allocate the memory needed
    sramdata = (uint32_t*)malloc(SRAM_SIZE);
    //       * create the memory
    this->sram = new Memory("sram", sramdata, SRAM_SIZE);
    //   - bind interface (sram is hooked to the address decoder)
    if (this->addrdec->bind(&this->sram->slave_socket, SRAM_BASE_ADDR, SRAM_BASE_ADDR+SRAM_SIZE))
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
    //       * allocate the memory needed
    flashdata = (uint32_t*)malloc(FLASH_SIZE);
    //       * create the flash controller
    this->spif = new Spif("spif", (uint8_t*)flashdata, FLASH_SIZE);
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

    {
        int fd;
        // open the ROM file specified
        fd = open(romfile->c_str(), O_RDONLY);
        if (fd == -1)
        {
            TLM_ERR("ROM file (%s) could not be read\n", romfile->c_str());
        }
        read(fd, romdata, ROM_SIZE);
        close(fd);

        // open the FLASH file specified
        fd = open(flashfile->c_str(), O_RDONLY);
        if (fd == -1)
        {
            TLM_ERR("FLASH file (%s) could not be read\n", flashfile->c_str());
        }

        // load the FLASH content into the model
        read(fd, flashdata, FLASH_SIZE);

        // if the SRAM was preloaded with the FLASH content
        if (srampreloadedwithflash->get_bool())
        {
            memcpy(sramdata, &(flashdata[8]), U32(flashdata[4]));

            // initialize the CRM
            this->crm->m_reg[STATUS_INDEX] = 3;
        }

        close(fd);

    }
}


