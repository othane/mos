# build the crc unit test

.PHONY: all clean

ifdef EMBEDDED
include ../../hal/stm32f107x/stm32f107x.mk
endif

PRJ = crc_utest

SRC = crc_utest.c 
ifdef EMBEDDED
SRC += hw.c
endif

OBJS = $(SRC:.c=.o)

ifdef EMBEDDED
CPFLAGS += -DEMBEDDED
else
CPFLAGS += -DPRINT_RESULT
endif

INCDIR = ./../../lib
ifdef EMBEDDED
INCDIR += ./../../hal/stm32f107x/CMSIS/CM3/CoreSupport/ \
	./../../hal/stm32f107x/CMSIS/CM3/DeviceSupport/ST/STM32F10x/ \
	./../../hal/stm32f107x/STM32F10x_StdPeriph_Driver/inc/ \
	./../../hal/stm32f107x/STM32F10x_StdPeriph_Driver/inc/ \
	./../../hal/stm32f107x
endif

INC = $(patsubst %,-I%,$(INCDIR))

ifdef EMBEDDED
LDSCRIPT = stm32f107x_flash_cl.ld
LDFLAGS += -T$(LDSCRIPT)
endif

ifdef EMBEDDED
PRJ_FULL = $(PRJ).hex
else
PRJ_FULL = $(PRJ)
endif

export CC
export LD
export ASFLAGS
export CPFLAGS 

all: $(PRJ_FULL)
	echo $(PRJ_FULL)

$(PRJ): ../../lib/libcrc.so $(OBJS)
	echo $(PRJ_FULL)
	$(CC) $(OBJS) -L../../lib -lcrc -o $@

$(PRJ).elf: ../../hal/libhal.o ../../lib/lib.o $(OBJS) $(LDSCRIPT)
	$(CC) $(OBJS) ../../hal/libhal.o ../../lib/lib.o -Wl,-Map=$(PRJ).map $(LDFLAGS) -o $@

../../lib/libcrc.so:
	make -C ../../lib/ libcrc.so

../../lib/lib.o:
	make -C ../../lib

../../hal/libhal.o:
	make -C ../../hal

%.hex: %.elf
	$(BIN) $< $@

%.o : %.c
	$(CC) -c $(CPFLAGS) -Wa,-ahlms=$(<:.c=.lst) -I . $(INC) $< -o $@

flash:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_FLASH_CMDS)

debug:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init -c 'reset halt'

clean:
	-rm -f $(OBJS)
	-rm -f $(PRJ).elf
	-rm -f $(PRJ_FULL)
	make -C ../../hal clean
	make -C ../../lib clean
	
