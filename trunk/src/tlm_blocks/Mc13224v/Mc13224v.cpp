#include "Mc13224v.h"

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

Mc13224v::Mc13224v(sc_core::sc_module_name name, Parameters& parameters, MSP& config)
{
    Parameter *cpu_parameter, *romfile, *flashfile, *srampreloadedwithflash;
    MSP *cpu_config;
    uint32_t* flashdata;

    // sanity check
    if (config.count("cpu") != 1)
    {
        TLM_ERR("CPU definitions found: %d", config.count("cpu"));
        return;
    }
    if (config.count("romfile") != 1)
    {
        TLM_ERR("ROMFILE definitions found: %d\n", config.count("romfile"));
        return;
    }
    if (config.count("flashfile") != 1)
    {
        TLM_ERR("FLASHFILE definitions found: %d", config.count("flashfile"));
        return;
    }
    if (config.count("srampreloadedwithflash") != 1)
    {
        TLM_ERR("SRAMPRELOADEDWITHFLASH definitions found: %d", config.count("srampreloadedwithflash"));
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
    this->cpu->bind(*this->addrdec);

    // ROM:
    //   - create instance
    this->rom = new Memory("rom", ROM_SIZE);
    //   - bind interface (ROM is hooked to the address decoder)
    if (this->addrdec->bind(*this->rom, ROM_BASE_ADDR, ROM_BASE_ADDR+this->rom->get_size()))
    {
        TLM_ERR("ROM address range wrong %d", 0);
        return;
    }

    // SRAM:
    //   - create instance
    this->sram = new Memory("sram", SRAM_SIZE);
    //   - bind interface (sram is hooked to the address decoder)
    if (this->addrdec->bind(*this->sram, SRAM_BASE_ADDR, SRAM_BASE_ADDR+this->sram->get_size()))
    {
        TLM_ERR("SRAM address range wrong %d", 0);
        return;
    }

    // ITC:
    //   - create instance
    this->itc = new Itc("itc");
    //   - bind interfaces
    //      -> CPU IRQ and FIQ
    this->itc->irq.bind(this->cpu->irq);
    this->itc->fiq.bind(this->cpu->fiq);
    //      -> address decoder
    if (this->addrdec->bind(*this->itc, REG_ITC_BASE_ADDR))
    {
        TLM_ERR("ITC address range wrong %d", 0);
        return;
    }

    // CRM:
    //   - create instance
    this->crm = new Crm("crm");
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(*this->crm, REG_CRM_BASE_ADDR))
    {
        TLM_ERR("CRM address range wrong %d", 0);
        return;
    }
    //      -> interrupt hooked to ITC
    this->crm->interrupt.bind(this->itc->interrupts[3]);

    // SPIF:
    //   - create instance
    //       * allocate the memory needed
    flashdata = (uint32_t*)malloc(FLASH_SIZE);
    //       * create the flash controller
    this->spif = new Spif("spif", (uint8_t*)flashdata, FLASH_SIZE);
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(*this->spif, REG_SPIF_BASE_ADDR))
    {
        TLM_ERR("SPIF address range wrong %d", 0);
        return;
    }
    //      -> interrupt hooked to ITC
    this->spif->interrupt.bind(*this->itc->interrupts[6]);

    // SPI:
    //   - create instance
    this->spi = new Spi("spi");
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(*this->spi, REG_SPI_BASE_ADDR))
    {
        TLM_ERR("SPI address range wrong %d", 0);
        return;
    }
    //      -> interrupt hooked to ITC
    this->spi->interrupt.bind(*this->itc->interrupts[10]);

    // GPIO:
    //   - create instance
    this->gpio = new Gpio("gpio");
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(*this->gpio, REG_GPIO_BASE_ADDR))
    {
        TLM_ERR("GPIO address range wrong %d", 0);
        return;
    }

    // UART1:
    //   - create instance
    this->uart1 = new Uart("uart1", 0);
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(*this->uart1, REG_UART1_BASE_ADDR))
    {
        TLM_ERR("UART1 address range wrong %d", 0);
        return;
    }
    //      -> interrupt hooked to ITC
    this->uart1->interrupt.bind(*this->itc->interrupts[1]);

    // UART2:
    //   - create instance
    this->uart2 = new Uart("uart2", 1);
    //   - bind interfaces
    //      -> address decoder
    if (this->addrdec->bind(*this->uart2, REG_UART2_BASE_ADDR))
    {
        TLM_ERR("UART2 address range wrong %d", 0);
        return;
    }
    //      -> interrupt hooked to ITC
    this->uart2->interrupt.bind(*this->itc->interrupts[2]);

    {
        int fd;
        // open the ROM file specified
        fd = open(romfile->c_str(), O_RDONLY);
        if (fd == -1)
        {
            TLM_ERR("ROM file (%s) could not be read\n", romfile->c_str());
        }
        read(fd, this->rom->get_data(), ROM_SIZE);
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
            memcpy(this->sram->get_data(), &(flashdata[8]), U32(flashdata[4]));

            // initialize the CRM
            this->crm->set_status(3);
        }
        close(fd);
    }
}


