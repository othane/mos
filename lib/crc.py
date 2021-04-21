#!/usr/bin/env python

# This script wraps the generic crc library used in mos

import sys
import ctypes
import math
import os
import inspect


# load library
if sys.platform == "win32":
	libname = "crc.dll"
else:
	libname = "libcrc.so"
	# below is just hacks to get around LD_LIBRARY_PATH stuff
	thisfile = inspect.getfile(inspect.currentframe())
	thisdir = os.path.dirname(os.path.abspath(thisfile))
	thisdir = thisdir + '/'
	if os.path.exists(thisdir + libname):
		libname = thisdir + libname
libcrc = ctypes.cdll.LoadLibrary(libname)

# basic type alias'
uint8_t = ctypes.c_uint8
uint16_t = ctypes.c_uint16
uint32_t = ctypes.c_uint32
ulong = ctypes.c_ulong

# crc model (from 3rd party lib)
class cm_t(ctypes.Structure):
	_fields_ = [('cm_width', ctypes.c_int),
				('cm_poly', ctypes.c_ulong),
				('cm_init', ctypes.c_ulong),
				('cm_refin', ctypes.c_uint),
				('cm_refot', ctypes.c_uint),
				('cm_xorot', ctypes.c_ulong),
				('cm_reg', ctypes.c_ulong)]

# crc handle
CRC_METHOD_BEST = 0
class crc_h(ctypes.Structure):
	_fields_ = [('cm', cm_t),
				('table', ctypes.c_void_p),
				('table_size', ctypes.c_uint16),
				('method', ctypes.c_int)]

# models the stm32f10x hardware crc module
stm32f10x_crc_h = crc_h(
	cm_t(32, 0x04C11DB7, 0xFFFFFFFF, 0, 0, 0, 0), 
	None, 0, CRC_METHOD_BEST)

# bool crc_init(struct crc_h *h)
def crc_init(h):
	return libcrc.crc_init(ctypes.byref(h))

# uint32_t crc_buf(struct crc_h *h, const void *buf, uint32_t len); 
def crc_buf(h, buf, len, reset=True):
	return libcrc.crc_buf(ctypes.byref(h), buf, len, reset)

# unit test
if __name__ == "__main__":
	h = stm32f10x_crc_h
	crc_init(h)

	msg = "Hello world this is a message to crc\x00\x00\x00\x00"
	msg_crc = 0xb3c8cbe8;
	l = int(math.ceil(len(msg) / 4.0) * 4)
	crc = crc_buf(h, msg, l) & 0xffffffff

	print("crc = " + hex(crc & 0xffffffff))
	if crc == msg_crc:
		print("unit test passed")
	else:
		print("unit test failed")

