# build the sched unit test
export HALCFG := $(shell pwd)/config

LIBMOS = ../../libmos.o

.PHONY: all clean $(LIBMOS)

PRJ = sched_utest
PRJ_FULL = $(PRJ).hex

include ../../hal/hal.mk

SRC = sched_utest.c 
SRC += hw.c

OBJS = $(SRC:.c=.o)

INCDIR += ../..
INC = $(patsubst %,-I%,$(INCDIR))

LDSCRIPT = ./../../hal/$(ARCH)/utest.ld
LDFLAGS += -T$(LDSCRIPT)

all: $(PRJ_FULL)
	echo $(PRJ_FULL)

$(PRJ).elf: $(LIBMOS) $(OBJS) $(LDSCRIPT)
	$(CC) $(OBJS) $(LIBMOS) -Wl,-Map=$(PRJ).map $(LDFLAGS) -o $@

$(LIBMOS):
	make -C ../.. $(notdir $(LIBMOS))

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
	
