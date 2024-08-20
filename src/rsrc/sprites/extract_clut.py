import re
import sys

mode = 0

with open("sprite_clut.h", "w") as clut_file:
	clut_file.write("uint8_t sprite_clut[] = {\n")
	with open("arrow.txt", "r") as sprite_data:
		for line in sprite_data:
			if re.match(r'^begin palette', line):
				mode = 1

			elif re.match(r'end palette', line):
				break

			else:
				m = re.match(r'^(\d+)\s+(\d+)\s+(\d+)\s+', line)
				if m:
					red = int(m.group(1))
					green = int(m.group(2))
					blue = int(m.group(3))
					clut_line = "\t0x{0:02X}, 0x{1:02X}, 0x{2:02X}, 0xff,\n".format(blue, green, red)
					clut_file.write(clut_line)

	clut_file.write("};\n")
