address = 0xffe000

# Read in the list of system calls ("sys_")
with open("addresses.csv", "w") as addresses:
	with open("syscalls.txt", "r") as call_names:
		for line in call_names:
			# Remove comments
			index = line.find("#")
			if index == 0:
				line = ""
			elif index > 0:
				line = line[index - 1:]

			line = line.strip()
			elements = line.split(",")
			if len(elements) > 1:
				line = elements[0].strip()

			# Process only actual names
			if len(line) > 0:
				addresses.write("\"sys_{0}\",\"{1:06X}\"\n".format(line, address))
				address = address + 4
