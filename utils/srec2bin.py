#
# Convert Motorola SREC to binary ROM files (both a 512KB main file and 8KB sector files for non-empty sectors)
#
# usage: python srec2bin.py
# 

import re

def srec_add_data(rom, address, data):
	"""Add a line of data from an SREC file (hex digits) to the binary image at the given address"""
	data_bytes = bytearray.fromhex(data)
	for i in range(len(data_bytes)):
		rom[address + i] = data_bytes[i]

def srec_process(rom, line):
	"""Process a line of a Motorola SREC file"""

	m = re.match(r'^S(\d)([0-9a-fA-F]{2})([0-9a-fA-F]+)$', line)
	if m:
		type = int(m.group(1), 10)
		size = int(m.group(2), 16)
		addr_data_chk = m.group(3)

		if type == 3:
			m2 = re.match(r'^([0-9a-fA-F]{8})([0-9a-fA-F]+)([0-9a-fA-F]{2})$', addr_data_chk)
			if m2:
				address = int(m2.group(1),16)
				data = m2.group(2)
				chksum = m2.group(3)
				if address >= rom_base:
					srec_add_data(rom, address - rom_base, data)

def bank_has_data(rom, bank):
	"""Check to see if a bank is empty (all 0xFF)"""
	start = bank * 8192
	end = start + 8192

	for i in range(start, end):
		if rom[i] != 0xff:
			# Return True if we find a non-blank entry
			return True
	
	return False

rom_size = 512 * 1024
rom_base = 0xf80000

input_file = "toolbox.s37"
output_file = "toolbox.bin"

rom_image = bytearray(rom_size)
for i in range(rom_size):
	rom_image[i] = 0xff

with open(input_file, "r") as input:
	for line in input:
		srec_process(rom_image, line)

with open("toolbox.bin", "wb") as output:
	output.write(rom_image)

for bank in range(0, 64):
	if bank_has_data(rom_image, bank):
		output_name = "toolbox-{:02X}.bin".format(bank)
		with open(output_name, "wb") as output:
			output.write(rom_image[bank * 8192:bank * 8192 + 8192])