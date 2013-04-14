#!/usr/bin/python
from intelhex import IntelHex
import struct
from collections import namedtuple
import sys
import os
import StringIO


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
prog_full = IntelHex(StringIO.StringIO(open(filename, "r").read()))

# read out the header
header_format = '<LLBLB'
header_addr = prog_full.minaddr() # all programs require there header to be located at the start
header_raw = struct.unpack(header_format, prog_full.gets(header_addr, 14))
header = namedtuple('header', 'crc len type isr_vector pid')
header = header._asdict(header._make(header_raw))

# debug
import copy
header_orig = copy.deepcopy(header)

# update the header
header['len'] = prog_full.maxaddr() - prog_full.minaddr()
header['crc'] = 0 #@todo

# write header back to hex file
prog_full.puts(header_addr, struct.pack(header_format, *header.values()))
prog_full.write_hex_file(sys.stdout)

