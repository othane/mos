# make libs

include stm32f107x.mk

CPFLAGS += -DNOHW_H 

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
	./gpio.c \
	./nvm.c \
	./spis.c \
	./crc.c \
	./sys.c \
	./syscalls.c \
	./bootstrap.c

ASRC = ./CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_cl.s

OBJS = $(ASRC:.s=.o) $(SRC:.c=.o)

INCDIR += ../../lib/ \
	./

INC = $(patsubst %,-I%,$(INCDIR))

all: libhal.o

libhal.o: $(OBJS)
	$(LD) -r $(OBJS) -o $@

%.o : %.c
	$(CC) -c $(CPFLAGS) -I . $(INC) $< -o $@

%.o : %.s
	$(AS) -c $(ASFLAGS) $< -o $@

clean:
	-rm -f $(OBJS)
	-rm -f libhal.o

