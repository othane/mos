/**
 * @file crc.c
 *
 * @brief implement the crc module (this is hardware independent but can call hardware methods if available)
 *
 * @author OT
 *
 * @date March 2013
 *
 */


#include "crc.h"
#include <stdlib.h> // for NULL

#ifdef CRC_TRACE
#include <stdio.h>
#define TRACE printf("%s:%i\n", __FUNCTION__, __LINE__);
#else
#define TRACE
#endif


#ifndef weak
// note we assume gcc style if this is not defined
#define weak __attribute((weak))
#endif


ulong reflect(ulong v,int b); // grab the reflect method from the crcmodel lib (a bit naughty but it works for now)


// crc a 32bit word buffer via the table method
static uint32_t crc_32w_buf_table(struct crc_h *h, const void *buf, uint32_t len, bool reset)
{
	uint32_t *_buf = (uint32_t *)buf;
	uint32_t w;
	uint8_t b;
	TRACE;

	// init crc lib
	if (reset)
		cm_ini(&h->cm);

	while (1)
	{
		uint8_t k = 4;
		w = *_buf++;
		while (k--)
		{
			b = (uint8_t)((w & 0xFF000000) >> 24);
			w <<= 8;
			//b = (uint8_t)(w & 0x000000FF);
			//w >>= 8;
			b = (h->cm.cm_refin)? reflect(b, 8): b;
			h->cm.cm_reg = ((uint32_t *)h->table)[((h->cm.cm_reg>>24) ^ b) & 0xFFL] ^ (h->cm.cm_reg << 8);
			if (--len == 0)
				goto done;
		} 
	}

done:
	return cm_crc(&h->cm);
}


// crc a 8bit word buffer via the table method
static uint8_t crc_8w_buf_table(struct crc_h *h, const void *buf, uint32_t len, bool reset)
{
	uint32_t *_buf = (uint32_t *)buf;
	uint32_t w;
	uint8_t b;
	uint8_t _crc = h->cm.cm_reg;
	TRACE;

	if (reset)
		_crc = (uint8_t)h->cm.cm_init;

	while (1)
	{
		uint8_t k = 4;
		w = *_buf++;
		while (k--)
		{
			b = (uint8_t)((w & 0xFF000000) >> 24);
			w <<= 8;
			//b = (uint8_t)(w & 0x000000FF);
			//w >>= 8;
			b = (h->cm.cm_refin)? reflect(b, 8): b;
			_crc = ((uint8_t *)h->table)[_crc ^ b];
			if (--len == 0)
				goto done;
		} 
	}

done:
	h->cm.cm_reg = _crc;
	return cm_crc(&h->cm);
}


// crc anything using the soft method
static uint32_t crc_buf_soft(struct crc_h *h, const void *buf, uint32_t len, bool reset)
{
	uint32_t *_buf = (uint32_t *)buf;
	uint32_t w;
	uint8_t b;
	TRACE;

	// init crc lib
	if (reset)
		cm_ini(&h->cm);

	while (1)
	{
		uint8_t k = 4;
		w = *_buf++;
		while (k--)
		{
			b = (uint8_t)((w & 0xFF000000) >> 24);
			w <<= 8;
			//b = (uint8_t)(w & 0x000000FF);
			//w >>= 8;
			cm_nxt(&h->cm, b);
			if (--len == 0)
				goto done;
		} 
	}

done:
	return cm_crc(&h->cm);
}


// default handler (this should be overridden if possible)
weak uint32_t crc_buf_hard(struct crc_h *h, const void *buf, uint32_t len, bool reset)
{
	TRACE;
	return 0;
}


// default handler (this should be overridden if possible)
weak bool crc_init_hard(struct crc_h *h)
{
	TRACE;
	return false;
}


// generate a 32 bit crc table (4bytes*256 = 1KB of RAM needed)
static void gen_table32(struct crc_h *h)
{
	unsigned int i;
	uint32_t *tbl = h->table;
	TRACE;

	for (i=0; i < 256; i++)
		tbl[i] = (uint32_t)cm_tab(&h->cm, i);
}


// generate a 8 bit crc table (1byte*256 = 1/4KB of ram needed)
static void gen_table8(struct crc_h *h)
{
	unsigned int i;
	uint8_t *tbl = h->table;
	TRACE;

	for (i=0; i < 256; i++)
		tbl[i] = (uint8_t)cm_tab(&h->cm, i);
}


static bool crc_init_table(struct crc_h *h)
{
	TRACE;

	// if the caller has not given space for a table then give up
	if (h->table == NULL)
		return false;

	// we only support certain predefined (and tested) table layouts
	if (h->cm.cm_width == 32 && h->table_size >= 256*sizeof(uint32_t))
	{
		uint32_t refin = h->cm.cm_refin;
		h->cm.cm_refin = false;
		h->method = CRC_METHOD_TABLE_32W;
		gen_table32(h);
		h->cm.cm_refin = refin;
		return true;
	}
	if (h->cm.cm_width == 8 && h->table_size >= 256*sizeof(uint8_t))
	{
		uint32_t refin = h->cm.cm_refin;
		h->cm.cm_refin = false;
		h->method = CRC_METHOD_TABLE_8W;
		gen_table8(h);
		h->cm.cm_refin = refin;
		return true;
	}

	// others are not supported by table method atm so will have to use soft method
	return false;
}


// this is slow but it should work for anything
static bool crc_init_soft(struct crc_h *h)
{
	TRACE;
	h->method = CRC_METHOD_SOFT;
	return true;
}


bool crc_init(struct crc_h *h)
{
	TRACE;

	// reset the cm (only needed for table and soft but it doesnt hurt for hard)
	cm_ini(&h->cm);

	// try hardware method first as it is fastest
	if (h->method == CRC_METHOD_HARD || 
		h->method == CRC_METHOD_BEST)
		if (crc_init_hard(h))
			return true;

	// try table method next as it is still quite fast
	if (h->method == CRC_METHOD_TABLE_8W || 
		h->method == CRC_METHOD_TABLE_32W || 
		h->method == CRC_METHOD_BEST)
		if(crc_init_table(h))
			return true;

	// only option is full software method (quite slow)
	return crc_init_soft(h);
}


uint32_t crc_buf(struct crc_h *h, const void *buf, uint32_t len, bool reset)
{
	switch (h->method)
	{
		case CRC_METHOD_SOFT:
			return crc_buf_soft(h, buf, len, reset);
		case CRC_METHOD_HARD:
			return crc_buf_hard(h, buf, len, reset);
		case CRC_METHOD_TABLE_8W:
			return crc_8w_buf_table(h, buf, len, reset);
		case CRC_METHOD_TABLE_32W:
			return crc_32w_buf_table(h, buf, len, reset);
		default:
			///@todo error !!
			return -1;
	}
}


