
###
### Generate client bindings for the Toolbox
###

# with open("toolbox_bindings.s", "w") as output:

table_entry_size = 4
table_address = 0xffe000
syscalls = {}

with open("..\..\src\C256\syscalls.txt", "r") as input:
	for line in input:
		# Remove comments
		index = line.find("#")
		if index == 0:
			line = ""
		elif index > 0:
			line = line[index - 1:]

		line = line.strip()

		if line != "":
			name = "sys_{}".format(line)
			syscalls[name] = table_address
			table_address += table_entry_size

with open("bindings.s", "w") as output:
	output.write(";;;\n;;; Bindings for the Foenix Toolbox public calls\n;;;\n\n")

	for name in syscalls.keys():
		output.write("\t.public {}\n".format(name))

	output.write("\n")

	for name in syscalls.keys():
		address = syscalls[name]
		output.write("{0:<30} .equlab 0x{1:06X}\n".format(name + ":", address))