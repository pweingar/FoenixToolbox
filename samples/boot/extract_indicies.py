import re
import sys

mode = 0

input_file = "BootIcon.txt"
out_file = "boot_icon.s"

with open(out_file, "w") as index_file:
	index_file.write("boot_icon_pixels:\n")
	with open(input_file, "r") as sprite_data:
		for line in sprite_data:
			if re.match(r'^begin indices', line):
				mode = 1

			elif re.match(r'end indices', line):
				break

			elif mode == 1:
				indices = re.findall(r'\d+', line)
				index_file.write("\t.byte ")
				first = True
				for index in indices:
					value = int(index)
					if first:
						index_file.write("${0:02X}".format(value))	
						first = False
					else:
						index_file.write(", ${0:02X}".format(value))
				index_file.write("\n")

