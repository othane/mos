
#include <stdio.h>
#include "crc.h"


#define STR_EXP(x) #x
#define STR(x) STR_EXP(x)


#define TYPE uint32_t
TYPE tbl[256];
struct crc_h crc_cfg = 
{
	.cm =
	{
		.cm_width = sizeof(TYPE) * 8,
		.cm_poly = 0x04C11DB7,
		.cm_init = 0xFFFFFFFF,
		.cm_refin = false,
		.cm_refot = false,
		.cm_xorot = 0x00000000,
	},
	.table = tbl,
	.table_size = 256 * sizeof(TYPE),
	.method = CRC_METHOD_BEST
};
//char msg[32] = "Hello world!";
char msg[] = "Hello world this is a message to crc";


void run()
{
	uint32_t crc;

	crc_init(&crc_cfg);
	if (crc_cfg.method == CRC_METHOD_TABLE_8W || 
		crc_cfg.method == CRC_METHOD_TABLE_32W)
	{
		printf("table = { \n");
		int i;
		for (i=0; i < 256; i++)
		{
			printf("0x%.8X,  ", tbl[i]);
			if (i % 4 == 3)
				printf("\n");
		}
		printf("};\n");
	}

	crc = crc_buf(&crc_cfg, msg, sizeof(msg) - 1);

	printf("crc(msg) = 0x%.8X, %d bytes\n", crc, sizeof(msg)-1);
}


void main(void)
{
	run();
	
	crc_cfg.method = CRC_METHOD_SOFT;
	run();
}
