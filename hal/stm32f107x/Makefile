# make libs

include stm32f107x.mk

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
SRC  = ./CMSIS/CM3/CoreSupport/core_cm3.c \
	./CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c \
	./STM32F10x_StdPeriph_Driver/src/misc.c \
	./STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c \
	./STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c \
	./STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
	./STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
	./STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c \
	./STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c \
	./STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c \
	./STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c \
	./STM32F10x_StdPeriph_Driver/src/stm32f10x_crc.c \
	./syscalls.c \
	./nvm.c \
	./crc.c \
	./bootstrap.c \
	./sys.c

# these files are only included when configured ... this is because they implement isr's and gcc cannot 
# know that you aren't going to call these so it includes everything, and this can blow your code size ...
# so by using the flags you can remove the overhead easily
SRC-$(CONFIG_GPIO) += ./gpio.c
SRC-$(CONFIG_SPIS) += ./spis.c

ASRC = ./CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_cl.s

OBJS = $(ASRC:.s=.o) $(SRC:.c=.o) $(SRC-y:.c=.o)

INCDIR += ../../lib/ \
	./

INC = $(patsubst %,-I%,$(INCDIR))

all: $(LIBHAL)

$(LIBHAL): $(OBJS)
	$(LD) -r $(OBJS) -o $@

%.o : %.c
	$(CC) -c $(CPFLAGS) -I . $(INC) $< -o $@

%.o : %.s
	$(AS) -c $(ASFLAGS) $< -o $@

clean:
	-rm -f $(OBJS)
	-rm -f $(LIBHAL)

