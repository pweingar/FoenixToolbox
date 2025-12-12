# Foenix Toolbox: Bootstrap

This directory contains the source code to build a bootable flash image using the Toolbox as a base library.

# Structure

* <code>arch/</code> -- Contains the machine specific low-level code for the different builds (_e.g._ fa2560k2, f256).
* <code>arch/fa2560k2</code> -- Contains the machine specific code for the FA2560K2 FPGA load.

# Building

Build the project using the make utility. The makefile accepts four optional parameters:

* UNIT: selects which model of Foenix or Wildbits machine is the target
* MEMORY: describes where in memory the binary will live. RAM is appropriate for testing, while FLASH is appropriate for loading into the system's flash memory
* DATA: specifies the Calypsi data model to use (large or small). Default is "large".
* CODE: specifies the Calypsi code model to use (large or small). Default is "large".

<pre>
    make [UNIT={FA2560K2|F256K2|F256JR2}] [MEMORY={RAM|FLASH}] [DATA={large|small}] [CODE={large|small}]
</pre>

NOTE: the Toolbox library file must be built for the data and code models selected, as well as for the target unit. The library file will be expected to be in the repository's <code>binary</code> directory.