.PHONY: clean all utest

include hal/hal.mk

OBJS =  hal/libhal.o \
		lib/lib.o \

all: libmos.o utest
	echo "libmos built"

libmos.o: $(OBJS)
	$(LD) -r $(OBJS) -o $@

hal/libhal.o:
	make -C hal

lib/lib.o:
	make -C lib lib.o

sched:
	#make -C sched

utest:
	make -C utest

clean:
	make -C hal clean
	make -C lib clean
	make -C utest clean
	-rm -f libmos.o

