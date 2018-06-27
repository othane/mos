# make lib

include stm32f4.mk

ifneq ($(HALCFG),)
include $(HALCFG)
$(info using HALCFG file $(HALCFG))
else
$(warning no HALCFG file used $(HALCFG))
endif

LIBHAL?=libhal.o
.PHONY: clean $(LIBHAL)

CPFLAGS += -DNOHW_H 

# system source files required for all systems using the stm32f4
SRC  = ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c \
	./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c \
	./STM32_USB-Host-Device_Lib_V2.2.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_core.c \
	./STM32_USB-Host-Device_Lib_V2.2.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_ioreq.c \
	./STM32_USB-Host-Device_Lib_V2.2.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_req.c \
	./STM32_USB-Host-Device_Lib_V2.2.0/Libraries/STM32_USB_OTG_Driver/src/usb_core.c \
	./STM32_USB-Host-Device_Lib_V2.2.0/Libraries/STM32_USB_OTG_Driver/src/usb_dcd.c	\
	./STM32_USB-Host-Device_Lib_V2.2.0/Libraries/STM32_USB_OTG_Driver/src/usb_dcd_int.c \
	./syscalls.c \
	./nvm.c \
	./crc.c \
	./bootstrap.c \
	./spi.c \
	./sys.c

ifneq (,$(filter $(SUB_ARCH), STM32F427_437xx STM32F429_439xx STM32F446xx STM32F469_479xx))
SRC += fmc.c
endif

	
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
SRC-$(CONFIG_USB) += ./usb.c
SRC-$(CONFIG_I2C) += ./i2c.c

ASRC = ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40xx.s

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

