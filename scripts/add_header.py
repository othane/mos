#!/usr/bin/python
from intelhex import IntelHex
import struct
from collections import namedtuple
import sys
import os
try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'lib'))
import crc
from argparse import *

header_format = '<LLBLBLLH'
global header_addr

def ih2header(ih):
	''' convert a intel hex file object to a mos program header '''
	global header_addr
	header_raw = struct.unpack(header_format, ih.gets(header_addr, 24))
	header = namedtuple('header', 'crc len type isr_vector pid max_len key hw_id')
	header = header._asdict(header._make(header_raw))
	return header

def header2ih(ih, header):
	''' convert a intel hex file object to a mos program header '''
	global header_addr
	header_new = struct.pack(header_format, *header.values())
	ih.puts(header_addr, header_new)
	return ih
	
# check args
def auto_int(x):
    return int(x,0)
parser = ArgumentParser('add_header.py read a mos header, update it with given properties and length and checksum and write it back to stdout')
parser.add_argument('-p', '--pid', type=auto_int, help='option to change the pid for this program')
parser.add_argument('-k', '--key', type=auto_int, help='option to change the key for this program')
parser.add_argument('--hw', type=auto_int, help='option to change the hardware id for this program')
parser.add_argument('filename', nargs=1, help='mandatory hex file to read header data from')
args = parser.parse_args()
filename = args.filename[0]
if not os.path.exists(filename):
	sys.exit("Unable open %s" % filename)

# open the full program (open via StringIO so we can write back to filename if desired)
prog = IntelHex(StringIO(open(filename, "r").read()))
header_addr = prog.minaddr() # all programs require there header to be located at the start

# read out the header
header = ih2header(prog)

# optional updates
if args.pid != None and args.pid >= 0 and args.pid < 256:
	header['pid'] = args.pid
if args.key != None:
	header['key'] = args.key
if args.hw != None:
	header['hw_id'] = args.hw

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

