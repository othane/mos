# build the crc unit test

.PHONY: all clean

include ../../hal/stm32f107x/stm32f107x.mk

PRJ = crc_utest

SRC = hw.c \
	crc_utest.c 

OBJS = $(SRC:.c=.o)

INCDIR = ./../../hal/stm32f107x/CMSIS/CM3/CoreSupport/ \
	./../../hal/stm32f107x/CMSIS/CM3/DeviceSupport/ST/STM32F10x/ \
	./../../hal/stm32f107x/STM32F10x_StdPeriph_Driver/inc/ \
	./../../hal/stm32f107x/STM32F10x_StdPeriph_Driver/inc/ \
	./../../lib \
	./../../hal/stm32f107x \

INC = $(patsubst %,-I%,$(INCDIR))

LDSCRIPT = stm32f107x_flash_cl.ld

export CC
export LD
export ASFLAGS
export CPFLAGS 

all: $(PRJ).hex

$(PRJ).elf: ../../hal/libhal.o ../../lib/lib.o $(OBJS) $(LDSCRIPT)
	$(CC) $(OBJS) ../../hal/libhal.o ../../lib/lib.o -T$(LDSCRIPT) -Wl,-Map=$(PRJ).map $(LDFLAGS) -o $@

../../hal/libhal.o:
	make -C ../../hal

../../lib/lib.o:
	make -C ../../lib

%.hex: %.elf
	$(BIN) $< $@

%.o : %.c
	$(CC) -c $(CPFLAGS) -DEMBEDDED -Wa,-ahlms=$(<:.c=.lst) -I . $(INC) $< -o $@

flash:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_FLASH_CMDS)

debug:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init -c 'reset halt'

clean:
	-rm -f $(OBJS)
	-rm -f $(PRJ).elf
	-rm -f $(PRJ).hex
	make -C ../../hal clean
	make -C ../../lib clean
	
