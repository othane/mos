/**
 * @file crc.h
 *
 * @brief interface to the crc module of the hal
 *
 * @author OT
 *
 * @date March 2013
 *
 */


#ifndef __CRC__
#define __CRC__


// we use the crc lib embedded in http://www.ross.net/crc/download/crc_v3.txt
#include "crcmodel.h"
#include <stdint.h>
#include <stdbool.h>


struct crc_h
{
	// setup
	cm_t cm;				// setup struct from our crc lib

	// table stuff
	void *table;			// pointer the table method can use
	uint16_t table_size;	// size in bytes of above table

	// which method
	enum
	{
		CRC_METHOD_BEST=0,
		CRC_METHOD_HARD,
		CRC_METHOD_TABLE_8W,
		CRC_METHOD_TABLE_32W,
		CRC_METHOD_SOFT,
	} method;
};


bool crc_init(struct crc_h *h);


uint32_t crc_buf(struct crc_h *h, const void *buf, uint32_t len);


#endif

