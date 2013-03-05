all: crc_table crc_utest

crc_utest: crc_utest.c crc.c crcmodel.c
	gcc $^ -I./ -DCRC_SOFT -DPRINT_RESULT -o $@

crc_table: crctable.c crcmodel.c 
	gcc $^ -I./ -o $@
	./$@

