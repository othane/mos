
#include <stdio.h>
#include "crc.h"


uint8_t tbl[256];
struct crc_h crc_cfg = 
{
	.cm =
	{
		.cm_width = 8,
		.cm_poly = 0x07,
		.cm_init = 0xffffffff,
		.cm_refin = true,
		.cm_refot = true,
		.cm_xorot = false,
	},
	.table = tbl,
	.table_size = 256,
	.method = CRC_METHOD_NONE
};
char msg[] = "Hello world!";


void run()
{
	uint8_t crc;

	crc_init(&crc_cfg);
	if (crc_cfg.method == CRC_METHOD_TABLE_8W || 
		crc_cfg.method == CRC_METHOD_TABLE_32W)
	{
		printf("table = { \n");
		int i;
		for (i=0; i < 256; i++)
		{
			printf("0x%.2X,  ", tbl[i]);
			if (i % 4 == 3)
				printf("\n");
		}
		printf("};\n");
	}

	crc = crc_buf(&crc_cfg, msg, sizeof(msg) - 1);

	printf("crc(msg) = 0x%.2X, %d bytes\n", crc, sizeof(msg)-1);
}


void main(void)
{
	run();
	
	crc_cfg.method = CRC_METHOD_SOFT,
	run();
}
