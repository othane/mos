# make libs

CRCLIBNAME=libcrc

CC ?= gcc
LD ?= ld

SRC = crc.c \
	crcmodel.c

OBJS = $(SRC:.c=.o)

all: lib.o

lib.o: $(CRCLIBNAME).o
	$(LD) -r $? -o lib.o

$(CRCLIBNAME).o: $(OBJS)
	# dont build a .a file as we cannot override weak functions in a .a
	# instead use LD to accumlate all .o files in to 1 which is basically 
	# a lib like .a
	$(LD) -r $(OBJS) -o $@

%.o : %.c
	$(CC) -c $(CPFLAGS) -Wa,-ahlms=$(<:.c=.lst) -I . $(INCDIR) $< -o $@

$(CRCLIBNAME).so: crc.c crcmodel.c
	# shared lib for host side crc generation (note compile all of it in one go
	# ie from all c lib so we can cross compile libcrc.o and host compile libcrc.so
	# in one build process easily
	$(CC) -shared -fpic -o$(CRCLIBNAME).so crc.c crcmodel.c

clean:
	-rm -f $(OBJS)
	-rm -f lib.o
	-rm -f $(CRCLIBNAME).so
	-rm -f $(OBJS:.o=.lst)

