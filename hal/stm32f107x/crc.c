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


#include <crc.h>
#include <stm32f10x_conf.h>
#include "hal.h"


// this setup mimics the stm32 32bit crc hardware
// see: https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=https%3a%2f%2fmy%2est%2ecom%2fpublic%2fSTe2ecommunities%2fmcu%2fLists%2fcortex_mx_stm32%2fCRC%20calculation%20in%20software&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=3822
const static cm_t cm_stm32f107x = 
{
	32, 			// width
	0x04C11DB7, 	// poly
	0xFFFFFFFF, 	// init
	FALSE, 			// refin
	FALSE, 			// refot
	0, 				// xorot

	0,				// working reg (irgnore this in cmp)
};


static bool clk_running = false;


#define cm_t_elem_compare(a, b, elem) (a->elem == b->elem)
bool cm_t_compare(const cm_t *a, const cm_t *b)
{
	if (!cm_t_elem_compare(a, b, cm_width))
		return false;
	if (!cm_t_elem_compare(a, b, cm_poly))
		return false;
	if (!cm_t_elem_compare(a, b, cm_init))
		return false;
	if (!cm_t_elem_compare(a, b, cm_refin))
		return false;
	if (!cm_t_elem_compare(a, b, cm_refot))
		return false;
	if (!cm_t_elem_compare(a, b, cm_xorot))
		return false;
	// note do not compare cm_reg (not setup just working reg)

	return true;
}


uint32_t crc_buf_hard(struct crc_h *h, const void *buf, uint32_t len)
{
	uint32_t r; 

	// reset
	CRC_ResetDR();

	// mangle length so it is 4 byte aligned as hard requires this
	if (len & 0x03)
		len = (len >> 2) + 1;
	else
		len = (len >> 2);

	// crc buf
	r = CRC_CalcBlockCRC((uint32_t *)buf, len);

	// return result
	return r;
}


bool crc_init_hard(struct crc_h *h)
{
	bool result = false;

	// if the cm config matches the hardware we are good to go !!
	result = cm_t_compare(&h->cm, &cm_stm32f107x);

	// start crc hw clock if needed
	if (result && !clk_running)
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	clk_running = true;

	// mark as hard
	if (result)
		h->method = CRC_METHOD_HARD;

	return result;
}

