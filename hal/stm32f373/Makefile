# make libs

include stm32f373.mk

CPFLAGS += -DNOHW_H 

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
	./syscalls.c \
	./sys.c \
	./gpio.c \
	./nvm.c \
	./crc.c \
	./bootstrap.c \
	#./spis.c

ASRC = ./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/CMSIS/Device/ST/STM32F37x/Source/Templates/gcc_ride7/startup_stm32f37x.s

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

