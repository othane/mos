LIBHAL = hal/libhal.o

.PHONY: clean all utest $(LIBHAL)

include hal/hal.mk

OBJS =  $(LIBHAL) \
		lib/lib.o \

all: utest libmos.o 
	echo "libmos built"

libmos.o: $(OBJS)
	$(LD) -r $(OBJS) -o $@

$(LIBHAL):
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

