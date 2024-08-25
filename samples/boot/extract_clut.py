import re
import sys

mode = 0

with open("boot_clut.s", "w") as clut_file:
	clut_file.write("boot_icon_clut:\n")
	with open("BootIcon.txt", "r") as sprite_data:
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
					clut_line = "\t.byte ${0:02X}, ${1:02X}, ${2:02X}, $ff\n".format(blue, green, red)
					clut_file.write(clut_line)

