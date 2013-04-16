# make libs

CC ?= gcc
LD ?= ld

SRC = crc.c \
	crcmodel.c

OBJS = $(SRC:.c=.o)

all: lib.o

lib.o: $(OBJS)
	$(LD) -r $(OBJS) -o lib.o

%.o : %.c
	$(CC) -c $(CPFLAGS) -Wa,-ahlms=$(<:.c=.lst) -I . $(INCDIR) $< -o $@

libcrc.so: crc.c crcmodel.c crc_utest.c
	# shared lib for host side crc generation
	$(CC) -shared -fpic -olibcrc.so crc.c crcmodel.c
	$(CC) -o utest crc_utest.c -lcrc -L./ # dbg utest only

clean:
	-rm -f $(OBJS)
	-rm -f lib.o
	-rm -f libcrc.so

