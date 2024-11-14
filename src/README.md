# Toolbox Source Directory

This directory contains the main (mostly device-independent) source code for the Toolbox as well as the various sub-directories:

* C256 -- Directory for C256 and F256 low-level assembly code. (TODO: split these out into separate folders!)
* dev -- Directory for machine-specific device drivers.
* include -- Header files included by various files in the Toolbox. All register address definitions should be confined to these header files, mostly in directories and files for each specific system.
* boot.* -- Files for the boot screen
* cartridge.* -- Code to support accessing the F256 cartridge. This might need to be pushed down to the dev folder.
* indicators.* -- Files for the LEDs... indicators.c needs to be pushed to dev. indicators.h to include
* interrupt.* -- Files to support interrupts... TODO: remove or redistribute to dev and include?
* log_level.h -- Definitions of the various logging levels used by the Toolbox's internal logging code
* log.* -- Toolbox's internal logging code for TRACE/ERROR/INFO/DEBUG logging
* memory.* -- Public calls for the rudimentary memory management support
* newbuild.py -- Utility script to incriment the build number
* proc.* -- Code to support launching and exiting a user program
* ring_buffer.* -- Data type code for a ring buffer, used by a couple of drivers
* simpleio.* -- Rudimentary I/O code for printing messages before drivers are fully active
* sys_general.* -- System information, CPU and model identification, and some GABE functionality
* syscalls.c -- System call support but only relevant for m68k and m68040 systems (TODO: move elsewhere)
* timers.c -- A2560K timer code (TODO: push down to dev)
* timers.h -- Stubs for timers device code
* toolbox.c -- The main startup code for the Toolbox
* utilities.* -- Some useful functions that are needed in multiple places
* version.h -- Definitions of the version number for the current build
