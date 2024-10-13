#
# Generate the assembly files for both the system call library and the kernel jump tabls
#

sys_call_names = []
internal_names = []

# Read in the list of system calls ("sys_")
with open("syscalls.txt", "r") as call_names:
	for line in call_names:
		# Remove comments
		index = line.find("#")
		if index == 0:
			line = ""
		elif index > 0:
			line = line[index - 1:]

		line = line.strip()

		# Skip blank lines
		if len(line) > 0:
			elements = line.split(",")
			if len(elements) == 1:
				sys_call_names.append(line)
				internal_names.append(line)
			else:
				sys_call_names.append(elements[0].strip())
				internal_names.append(elements[1].strip())

# # Create the system call table, which is used to call into the kernel jump table

# with open("syscalls.s", "w") as f:
# 	for call_name in sys_call_names:
# 		f.write("\t.public sys_{}\n".format(call_name))

# 	f.write("\n")

# 	for call_name in sys_call_names:
# 		f.write("\t.extern sjt_{}\n".format(call_name))
		
# 	f.write("\n\t.section farcode\n\n");

# 	for call_name in sys_call_names:
# 		f.write("sys_{:26}\t.equlab sjt_{}\n".format(call_name + ": ", call_name))

# Create the kernel jump table

with open("jumptable.s", "w") as f:
	for call_name in sys_call_names:
		f.write("\t.public sys_{}\n".format(call_name))

	f.write("\n")

	for internal_name in internal_names:
		f.write("\t.extern {}\n".format(internal_name))
		
	f.write("\n\t.section jumptable\n\n");

	for i in range(len(sys_call_names)):
		external_name = sys_call_names[i]
		internal_name = internal_names[i]
		f.write("sys_{:26}\tjmp long:{}\n".format(external_name + ": ", internal_name))