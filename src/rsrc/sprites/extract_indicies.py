import re
import sys

mode = 0

sprite_name = sys.argv[1]
input_file = "{}.txt".format(sprite_name)
out_file = "{}.h".format(sprite_name)

with open(out_file, "w") as index_file:
	index_file.write("uint8_t {0}_pixels[] = {1}\n".format(sprite_name, "{"))
	with open(input_file, "r") as sprite_data:
		for line in sprite_data:
			if re.match(r'^begin indices', line):
				mode = 1

			elif re.match(r'end indices', line):
				break

			elif mode == 1:
				indices = re.findall(r'\d+', line)
				index_file.write("\t")
				for index in indices:
					value = int(index)
					index_file.write("0x{0:02X}, ".format(value))
				index_file.write("\n")

	index_file.write("};\n")
