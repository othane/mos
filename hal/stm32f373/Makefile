# make lib

include stm32f373.mk

ifneq ($(HALCFG),)
include $(HALCFG)
$(info using HALCFG file $(HALCFG))
else
$(warning no HALCFG file used $(HALCFG))
endif

LIBHAL?=libhal.o
.PHONY: clean $(LIBHAL)

CPFLAGS += -DNOHW_H 

# system source files required for all systems using the stm32f373
SRC  = ./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/CMSIS/Device/ST/STM32F37x/Source/Templates/system_stm32f37x.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_misc.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_exti.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_flash.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_gpio.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_rcc.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_spi.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_usart.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_pwr.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_rtc.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_crc.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_syscfg.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_dma.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_sdadc.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_tim.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_dbgmcu.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_dac.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_usart.c \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/src/stm32f37x_i2c.c \
	./syscalls.c \
	./nvm.c \
	./crc.c \
	./bootstrap.c \
	./spi.c \
	./i2c.c \
	./sys.c

# these files are only included when configured ... this is because they implement isr's and gcc cannot 
# know that you aren't going to call these so it includes everything, and this can blow your code size ...
# so by using the flags you can remove the overhead easily
SRC-$(CONFIG_DMA) += ./dma.c
SRC-$(CONFIG_GPIO) += ./gpio.c
SRC-$(CONFIG_SPIS) += ./spis.c
SRC-$(CONFIG_SPIM) += ./spim.c
SRC-$(CONFIG_ADC) += ./adc.c ./dma.c
SRC-$(CONFIG_TMR) += ./tmr.c
SRC-$(CONFIG_PWM) += ./pwm.c ./tmr.c
SRC-$(CONFIG_PPM) += ./ppm.c ./tmr.c
SRC-$(CONFIG_UART) += ./uart.c
SRC-$(CONFIG_I2C) += ./i2c.c

ASRC = ./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/CMSIS/Device/ST/STM32F37x/Source/Templates/gcc_ride7/startup_stm32f37x.s

# turns the src into object, then runs sort to prune any duplicates
OBJS = $(sort $(ASRC:.s=.o) $(SRC:.c=.o) $(SRC-y:.c=.o))

INCDIR += ../../lib/ \
	./

INC = $(patsubst %,-I%,$(INCDIR))

all: $(LIBHAL)

$(LIBHAL): $(OBJS) $(HALCFG)
	$(LD) -r $(OBJS) -o $@

%.o : %.c
	$(CC) -c $(CPFLAGS) -I . $(INC) $< -o $@

%.o : %.s
	$(AS) -c $(ASFLAGS) $< -o $@

clean:
	-rm -f $(OBJS)
	-rm -f $(LIBHAL)

