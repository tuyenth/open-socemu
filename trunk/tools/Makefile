# the following are derived from the user base address
REG_XLS2H=../tools/reg_xls2h

regs= \
	../src/tlm_blocks/At91sam9261/Smc/reg_smc.h \
	../src/tlm_blocks/At91sam9261/Aic/reg_aic.h \
	../src/tlm_blocks/Mc13224v/Itc/reg_itc.h \
	../src/tlm_blocks/Mc13224v/Crm/reg_crm.h \
	../src/tlm_blocks/Mc13224v/Reserved/reg_reserved.h \
	../src/tlm_blocks/Mc13224v/Spif/reg_spif.h \
	../src/tlm_blocks/Mc13224v/Spi/reg_spi.h \
	../src/tlm_blocks/Mc13224v/Gpio/reg_gpio.h \
	../src/tlm_blocks/Mc13224v/Uart/reg_uart.h \
	

.PHONY: all clean
.SILENT: all clean
all: $(regs)
	echo "... Finished building ..."

clean:
	rm -vf $(regs)

../src/tlm_blocks/At91sam9261/Smc/reg_smc.h: ../src/tlm_blocks/At91sam9261/Smc/SMC.xls
	python $(REG_XLS2H)/reg_xls2h.py --simu                         $< "" 0xFFFFEC00 $@

../src/tlm_blocks/At91sam9261/Aic/reg_aic.h: ../src/tlm_blocks/At91sam9261/Aic/AIC.xls
	python $(REG_XLS2H)/reg_xls2h.py --simu                         $< "" 0xFFFFF000 $@

../src/tlm_blocks/Mc13224v/Itc/reg_itc.h: ../src/tlm_blocks/Mc13224v/Itc/ITC.xls
	python $(REG_XLS2H)/reg_xls2h.py --simu --short                 $< "" 0x80020000 $@

../src/tlm_blocks/Mc13224v/Crm/reg_crm.h: ../src/tlm_blocks/Mc13224v/Crm/CRM.xls
	python $(REG_XLS2H)/reg_xls2h.py --simu --short                 $< "" 0x80003000 $@

../src/tlm_blocks/Mc13224v/Spif/reg_spif.h: ../src/tlm_blocks/Mc13224v/Spif/SPIF.xls
	python $(REG_XLS2H)/reg_xls2h.py --simu --short                 $< "" 0x8000C000 $@

../src/tlm_blocks/Mc13224v/Spi/reg_spi.h: ../src/tlm_blocks/Mc13224v/Spi/SPI.xls
	python $(REG_XLS2H)/reg_xls2h.py --simu --short                 $< "" 0x80002000 $@

../src/tlm_blocks/Mc13224v/Gpio/reg_gpio.h: ../src/tlm_blocks/Mc13224v/Gpio/GPIO.xls
	python $(REG_XLS2H)/reg_xls2h.py --simu --short                 $< "" 0x80000000 $@

../src/tlm_blocks/Mc13224v/Uart/reg_uart.h: ../src/tlm_blocks/Mc13224v/Uart/UART.xls
	python $(REG_XLS2H)/reg_xls2h.py --simu --short                 $< "" 0x80005000 $@

../src/tlm_blocks/Mc13224v/Reserved/reg_reserved.h: ../src/tlm_blocks/Mc13224v/Reserved/RESERVED.xls
	python $(REG_XLS2H)/reg_xls2h.py --simu --short                 $< "" 0x8000C000 $@


