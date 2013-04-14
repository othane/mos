#!/usr/bin/python
from intelhex import IntelHex
import struct
from collections import namedtuple
import copy
import sys
import os
import StringIO


def show_help():
	print("%s - Add a new program to the build" % sys.argv[0])
	print("(build starts with bootstrap.hex and is build up from there)\n")
	print("Usage: %s [-h] <build> <new_program>" % sys.argv[0])

# check args
if (len(sys.argv) == 2) and (sys.argv[1] == '-h'):
	show_help()
	sys.exit(0)
if len(sys.argv) != 3:
	show_help()
	sys.exit(1)
if not os.path.exists(sys.argv[1]):
	sys.exit("Unable open build %s" % sys.argv[1])
if not os.path.exists(sys.argv[2]):
	sys.exit("Unable open build %s" % sys.argv[2])
build_filename = sys.argv[1]
prog_filename = sys.argv[2]

# open the build and prog
# note open build via StringIO so we can add to it
build = IntelHex(StringIO.StringIO(open(build_filename, "r").read()))
prog = IntelHex(prog_filename)

# merge program into build
prog_header_addr = prog.minaddr()
prog.start_addr = build.start_addr # we need this to make the merge work smoothly
build.merge(prog)

# add pointer to program header to the bootstrap
header_tbl_addr = 0x08000200
header_tbl_len = 2
header_tbl_format = "<LL"
header_tbl = list(struct.unpack(header_tbl_format, build.gets(header_tbl_addr, header_tbl_len * 4)))
k = 0
while header_tbl[k] != 0:
	if k > header_tbl_len:
		sys.exit("bootstrap program table full [you have too many programs]!");
	k += 1
header_tbl[k] = prog_header_addr
build.puts(header_tbl_addr, struct.pack(header_tbl_format, *header_tbl))

# done
build.write_hex_file(build_filename)
