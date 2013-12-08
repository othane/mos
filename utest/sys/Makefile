# build the sys unit test

.PHONY: all clean

PRJ = sys_utest
PRJ_FULL = $(PRJ).hex

include ../../hal/hal.mk

export CPFLAGS += -DDEBUG_MCO

SRC = sys_utest.c 
SRC += hw.c

OBJS = $(SRC:.c=.o)

INCDIR += ../../hal/
INC = $(patsubst %,-I%,$(INCDIR))

LDSCRIPT = ./../../hal/$(ARCH)/utest.ld
LDFLAGS += -T$(LDSCRIPT)

all: $(PRJ_FULL)
	echo $(PRJ_FULL)

$(PRJ).elf: ../../hal/libhal.o $(OBJS) $(LDSCRIPT)
	$(CC) $(OBJS) ../../hal/libhal.o -Wl,-Map=$(PRJ).map $(LDFLAGS) -o $@

../../hal/libhal.o:
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
	
