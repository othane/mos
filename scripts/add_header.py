#!/usr/bin/python
from intelhex import IntelHex
import struct
from collections import namedtuple
import sys
import os
import StringIO
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'lib'))
import crc

header_format = '<LLBLB'
global header_addr

def ih2header(ih):
	''' convert a intel hex file object to a mos program header '''
	global header_addr
	header_raw = struct.unpack(header_format, ih.gets(header_addr, 14))
	header = namedtuple('header', 'crc len type isr_vector pid')
	header = header._asdict(header._make(header_raw))
	return header

def header2ih(ih, header):
	''' convert a intel hex file object to a mos program header '''
	global header_addr
	header_new = struct.pack(header_format, *header.values())
	ih.puts(header_addr, header_new)
	return ih
	
def show_help():
	print("Usage: %s [-h|<filename>]" % sys.argv[0])

# check args
if len(sys.argv) != 2:
	show_help()
	sys.exit(1)
elif sys.argv[1] == '-h':
	show_help()
	sys.exit(0)
elif not os.path.exists(sys.argv[1]):
	sys.exit("Unable open %s" % sys.argv[1])
filename = sys.argv[1]


# open the full program (open via StringIO so we can write back to filename if desired)
prog = IntelHex(StringIO.StringIO(open(filename, "r").read()))
header_addr = prog.minaddr() # all programs require there header to be located at the start

# read out the header
header = ih2header(prog)

# debug
import copy
header_orig = copy.deepcopy(header)

# update the header len
l = prog.maxaddr() - prog.minaddr() + 1
header['len'] = l
prog = header2ih(prog, header) # put the length back into the intel hex file for crc

# update the header crc (note we do no include the CRC word in the CRC)
crc_start_addr = prog.minaddr() + 4
prog.padding = 0xff
buf = prog.tobinstr(start=crc_start_addr)
crc.crc_init(crc.stm32f10x_crc_h)
crc = crc.crc_buf(crc.stm32f10x_crc_h, buf, len(buf)) & 0xffffffff
header['crc'] = crc
prog = header2ih(prog, header) # put the crc back into the intel hex file so we can write it out

# write header back to hex file
prog.write_hex_file(sys.stdout)

