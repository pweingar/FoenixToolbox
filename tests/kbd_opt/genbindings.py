
#
# Create the 64tass parameters structures for the Toolbox functions
#

import re
import sys

class FunctionParameter:
	def __init__(self):
		self._name = "UNKNOWN"
		self._position = 0
		self._type = ""
		self._description = ""

	def set_name(self, name):
		if name.endswith("[]"):
			name = name[0:-2]
		self._name = name

	def name(self):
		return self._name

	def set_position(self, position):
		self._position = position

	def position(self):
		return self._position

	def set_type(self, type):
		self._type = type

	def type(self):
		return self._type

	def set_description(self, description):
		self._description = description

	def description(self):
		return self._description

	def print(self):
		"""Print a simple version of a parameter"""

		print("\tName: {0}, Type: {1}, Comment: {2}".format(self._name, self._type, self._description))

	def size(self):
		"""Return the size of the parameter in bytes."""
		if self.type() == "char" or self.type() == "unsigned char" or self.type() == "uint8_t":
			return 1
		elif self.type() == "short" or self.type() == "unsigned short" or self.type() == "uint16_t":
			return 2
		else:
			return 4

	def emit_asm(self, output):
		"""Emit the assembly reference for the parameter."""

		size = self.size()

		if size == 1:
			output.write("{0:<16}.byte ?       ; {1}\n".format(self.name(), self.description()))
		elif size == 2:
			output.write("{0:<16}.word ?       ; {1}\n".format(self.name(), self.description()))
		else:
			output.write("{0:<16}.dword ?      ; {1}\n".format(self.name(), self.description()))

	def emit_accumulator(self, output):
		size = self.size()
		if size == 1:
			output.write("; {0} goes in A[7..0]\n".format(self.name()))
		elif size == 2:
			output.write("; {0} goes in A[15..0]\n".format(self.name()))
		else:
			output.write("; {0} goes in X[15..0]:A[15..0]\n".format(self.name()))

class Function:
	def __init__(self):
		self._name = "UNKNOWN"
		self._brief = ""
		self._description = ""
		self._type = "void"
		self._parameters = []
		self._address = 0
		self._prototype = ""
	
	def set_name(self, name):
		self._name = name

	def name(self):
		return self._name

	def set_prototype(self, prototype):
		self._prototype = prototype

	def prototype(self):
		return self._prototype

	def set_brief(self, brief):
		self._brief = brief
	
	def brief(self):
		return self._brief

	def set_description(self, description):
		self._description = description

	def description(self):
		return self._description

	def set_type(self, type):
		self._type = type

	def type(self):
		return self._type
	
	def set_address(self, address):
		self._address = address

	def address(self):
		return self._address

	def add_parameter(self, param_name, param_type):
		"""Add a parameter to the function."""

		param = FunctionParameter()
		param.set_name(param_name)
		param.set_type(param_type)
		param.set_position(len(self._parameters))

		self._parameters.append(param)

	def parameters(self):
		return self._parameters
	
	def stack_size(self):
		"""Return the number of bytes needed on the stack for the parameters"""

		size = 0
		if len(self.parameters()) > 1:
			for parameter in self.parameters()[1:]:
				size = size + parameter.size()
		return size

	def add_param_comment(self, param_name, param_comment):
		"""Add a comment to a parameter"""

		for param in self._parameters:
			if param._name == param_name:
				param.set_description(param_comment)
				break
				
	def print(self):
		"""Print out a simple description of the function"""

		print("Name: {0}, Type: {1}".format(self._name, self._type))
		print(self._description)
		for parameter in self._parameters:
			parameter.print()

		print()

	def emit_asm(self, output):
		"""Emit the assembly reference for the function."""

		output.write("\n;\n; {0}\n".format(self.prototype()))
		if len(self.parameters()) > 0:
			first = self.parameters()[0]
			output.write("; \n")
			first.emit_accumulator(output)
		output.write(";\n")
		output.write("; {0} bytes needed for the stack parameters\n;\n".format(self.stack_size()))

		output.write("{0} = ${1:06x}\n\n".format(self.name(), self.address()))

		if len(self.parameters()) > 1:
			m = re.match("sys_(\w+)", self.name())
			if m:
				short_name = m.group(1)
			else:
				short_name = self.name()
			stack_parameters = self.parameters()[1:]

			output.write("{0:<16}.namespace\n".format(short_name))
			output.write("                .virtual 1,s\n")

			for parameter in stack_parameters:
				parameter.emit_asm(output)

			output.write("                .endv\n")
			output.write("                .endn\n")


comments = []
functions = []

def process_comment(line):
	"""Deal with a comment line while we're in the middle of a block comment."""
	index = line.index("*")
	if index > -1:
		comment_line = line[index+1:].strip()
	else:
		comment_line = line.strip()

	if comment_line != "":
		comments.append(comment_line)

def process_definition(type, name, parameters, comments, prototype):
	"""Deal with a function prototype."""

	func = Function()
	func.set_name(name)
	func.set_type(type)
	func.set_prototype(prototype)

	is_in_func_comments = True

	for param in func_parameters:
		m1 = re.match("^\s*(.*)\s(\S+)\s*$", param)
		if m1:
			param_type = m1.group(1).strip()
			param_name = m1.group(2).strip()
			func.add_parameter(param_name, param_type)
			
	for comment in comments:
		m2 = re.match("@param\s(\w+)\s+(.*)$", comment)
		if m2:
			param_name = m2.group(1).strip()
			param_comment = m2.group(2).strip()
			func.add_param_comment(param_name, param_comment)
		else:
			func._description = func._description + comment

	functions.append(func)

#
# Read in the C header file with the extern prototypes and parse all the function names,
# return values, and parameters
#
with open(sys.argv[1], "r") as input:
	is_in_comment = False

	for line in input.readlines():
		line = line.strip()

		if line.startswith("/**"):
			is_in_comments = True
			comments = []
		elif line.endswith("*/"):
			is_in_comments = False

		if is_in_comments and line.startswith("*"):
			process_comment(line)

		else:
			m = re.match("extern\s+SYSTEMCALL\s+(\w+)\s+(\w+)\((.*)\)", line)
			if m:
				func_type = m.group(1)
				func_name = m.group(2)
				func_parameters = str.split(m.group(3), ",")
				process_definition(func_type, func_name, func_parameters, comments, line)

#
# Read in the Caylpsi Toolbox jumptable assembly file and extract the addresses
#
with open(sys.argv[2], "r") as addresses:
	for line in addresses.readlines():
		m = re.match("^(\w+):\s+.equlab\s+0x([0-9a-fA-F]+)", line)
		if m:
			func_name = m.group(1).strip()
			func_address = int(m.group(2), 16)

			for func in functions:
				if func.name() == func_name:
					func.set_address(func_address)
					break

#
# Create the bindings file with the relevant information
#
with open("bindings.s", "w") as bindings:
	for func in functions:
		func.emit_asm(bindings)
