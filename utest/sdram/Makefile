# build the sdram unit test
export HALCFG := $(shell pwd)/config
export SUB_ARCH ?= STM32F429_439xx

LIBHAL = ../../hal/libhal.o

.PHONY: all clean $(LIBHAL)

PRJ = sdram_utest
PRJ_FULL = $(PRJ).hex

include ../../hal/hal.mk

SRC = sdram_utest.c
SRC += hw.c

OBJS = $(SRC:.c=.o)

INCDIR += ../../hal/
INC = $(patsubst %,-I%,$(INCDIR))

LDSCRIPT = sdram_utest.ld
LDFLAGS += -T$(LDSCRIPT)

all: $(PRJ_FULL)
	echo $(PRJ_FULL)

$(PRJ).elf: $(LIBHAL) $(OBJS) $(LDSCRIPT)
	$(CC) $(OBJS) $(LIBHAL) -Wl,-Map=$(PRJ).map $(LDFLAGS) -o $@

$(LIBHAL):
	make -C ../../hal

%.hex: %.elf
	$(BIN) $< $@

%.o : %.c
	$(CC) -c $(CPFLAGS) -Wa,-ahlms=$(<:.c=.lst) -I . $(INC) $< -o $@

clean:
	-rm -f $(OBJS)
	-rm -f $(OBJS:.o=.lst)
	-rm -f $(PRJ).lst
	-rm -f $(PRJ).map
	-rm -f $(PRJ).elf
	-rm -f $(PRJ_FULL)
	make -C ../../hal clean

