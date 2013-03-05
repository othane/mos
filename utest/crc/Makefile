all: pc

pc:
	gcc -g crc_utest.c ../../hal/stm32f107x/crc.c ../../hal/stm32f107x/crcmodel.c -I../../hal/stm32f107x/ -DCRC_SOFT -DPRINT_RESULT -o crc_utest
