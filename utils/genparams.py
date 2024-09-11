
#
# Create the 64tass parameters structures for the Toolbox functions
#

import re

class FunctionParameter:
	def __init__(self):
		self._name = "UNKNOWN"
		self._position = 0
		self._type = ""
		self._description = ""

	def set_name(self, name):
		self._name = name

	def set_position(self, position):
		self._position = position

	def set_type(self, type):
		self._type = type

	def set_description(self, description):
		self._description = description

class Function:
	def __init__(self):
		self._name = "UNKNOWN"
		self._brief = ""
		self._description = ""
		self._type = "void"
		self._parameters = []
		self._address = 0
	
	def set_name(self, name):
		self._name = name

	def set_brief(self, brief):
		self._brief = brief

	def set_description(self, description):
		self._description = description

	def set_type(self, type):
		self._type = type

with open("toolbox.h", "r") as input:
	for line in input.readlines():
		m = re.match("extern\s+SYSTEMCALL\s+(\w+)\s+(\w+)\((.*)\)", line)
		if m:
			func_type = m.group(1)
			func_name = m.group(2)
			func_parameters = str.split(m.group(3), ",")
			print("Type: {0}, Name: {1}".format(func_type, func_name))
			for param in func_parameters:
				m2 = re.match("^\s*(.*)\s(\S+)\s*$", param)
				if m2:
					param_type = m2.group(1).strip()
					param_name = m2.group(2).strip()
					print("\tName: {0}, Type: {1}".format(param_name, param_type))

