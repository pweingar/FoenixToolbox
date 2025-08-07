
#
# Use interrupt mapping CSV file to generate interrupt group and mask tables
#

import csv
import re

group_mapping = [0xffff] * 48
mask_mapping = [0xffff] * 48

with open("int_map_fa2560k2.csv") as csvfile:
    reader = csv.reader(csvfile)
    for row in reader:
        group = row[0]
        bit = row[1]
        number_hex = row[3]

        m = re.match("0x(..)", number_hex)
        if m:
            number = int(m.group(1), 16)
            group_mapping[number] = int(group)
            mask_mapping[number] = 1 << int(bit)

print("const uint16_t g_int_group[] = {")
for i in range(0, 47):
    print("\t0x{:04x}, ".format(group_mapping[i]))
print("};")

print("const uint16_t g_int_mask[] = {")
for i in range(0, 47):
    print("\t0x{:04x},".format(mask_mapping[i]))
print("};")