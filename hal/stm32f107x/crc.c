/**
 * @file crc.c
 *
 * @brief implement the crc module for the stm32107
 *
 * @author OT
 *
 * @date March 2013
 *
 */


#include <stdint.h>


// setup the defines to select which CRC method (hardware or software) to use
#if defined(CRC_HARD) && defined(CRC_SOFT)
#warning "Both hard and soft crc's present (this should only be done for testing) !"
#elif  !defined(CRC_HARD) && !defined(CRC_SOFT)
#define CRC_SOFT // default to soft crc unless told we have a hard crc available
#endif


#ifdef CRC_SOFT 

// we use the crc lib embedded in http://www.ross.net/crc/download/crc_v3.txt setup to mimic the stm32 HW see:
// https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=https%3a%2f%2fmy%2est%2ecom%2fpublic%2fSTe2ecommunities%2fmcu%2fLists%2fcortex_mx_stm32%2fCRC%20calculation%20in%20software&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=3822
#include <crcmodel.h>
#include <crctable.h>


// this setup mimics the stm32 32bit crc hardware
#define CRC_INI
static cm_t _cm = 
{
	32, 			// width
	0x04C11DB7, 	// poly
	0xFFFFFFFF, 	// init
	FALSE, 			// refin
	FALSE, 			// refot
	0x00000000, 	// xorot
};


void _stm32_crc_blk_tbl(cm_t *cm, void *buf, uint32_t len)
{
	uint32_t words = len / 4;
	uint32_t *_buf = (uint32_t *)buf;

	while (words--)
	{
		int i;
		uint32_t w = *_buf++;
		uint8_t b;
		for (i = 0; i < sizeof(w); i++)
		{
			if (cm->cm_refin == FALSE)
			{
				b = (uint8_t)((w & 0xFF000000) >> 24);
				w <<= 8;
			}
			else
			{
				b = (uint8_t)(w & 0x000000FF);
				w >>= 8;
			}
			cm->cm_reg = crctable[((cm->cm_reg>>24) ^ b) & 0xFFL] ^ (cm->cm_reg << 8);
		}
	}
}


uint32_t _crc_soft(void *buf, uint32_t len)
{
	// reset
	cm_ini(&_cm);

	// crc buf using the special stm32 crc method
	_stm32_crc_blk_tbl(&_cm, buf, len);

	// return result
	return cm_crc(&_cm);
}

#endif


#ifdef CRC_HARD 

// the stm32f1x series comes with a hardware crc unit the has a fixed polynomial of
// 0x04C11DB7

#include <stm32f10x_conf.h>
#include "hal.h"

static bool clk_running = false;

uint32_t _crc_hard(void *buf, uint32_t len)
{
	uint32_t r; 

	// init if needed
	if (!clk_running)
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	clk_running = true;

	// reset
	CRC_ResetDR();

	// crc buf
	r = CRC_CalcBlockCRC((uint32_t *)buf, len >> 2);

	// return result
	return r;
}

#endif


// wrapper to choose crc method
static uint32_t crc(void *buf, uint32_t len)
{
	// prefer hardware crc if available
	#ifdef CRC_HARD
	return _crc_hard(buf, len);
	#else
	return _crc_soft(buf, len);
	#endif
}


uint8_t crc8(void *buf, uint32_t len)
{
	///@todo can we just take the MSByte of a crc32 for this ?
	return 0;
}


uint16_t crc16(void *buf, uint32_t len)
{
	///@todo can we just take the MSWord of a crc32 for this ?
	return 0;
}


uint32_t crc32(void *buf, uint32_t len)
{
	return crc(buf, len);
}



