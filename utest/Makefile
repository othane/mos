.PHONY: clean all sys gpio nvm spis crc

all: sys gpio nvm spis crc bootstrap

sys:
	make -C sys

gpio:
	make -C gpio

nvm:
	make -C nvm

spis:
	make -C spis

crc:
	make -C crc EMBEDDED=1

bootstrap:
	make -C bootstrap

clean:
	make -C sys clean
	make -C gpio clean
	make -C nvm clean
	make -C spis clean
	make -C crc EMBEDDED=1 clean
	make -C bootstrap clean

