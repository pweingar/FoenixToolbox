# Foenix Toolbox

## About This Project

The Foenix Toolbox is a light-weight firmware package intended for the [Foenix Retro Systems](https://c256foenix.com) retro-style computers. The design purpose of the toolbox is primarily to boot the computer and to provide a library of core routines for various functions that the programmer might find tedious to implement.

## The Toolbox API

The API provided by the toolbox is broken out into several sections:

* Character stream based I/O: keyboard, text screen (ANSI escape support), serial port
* Block based storage I/O: SD card
* File systems: SD card FAT32 file access, IEC storage device (Commodore DOS access)
* Time: Real-time clock access, jiffie-clock counter
* Interrupt handler support [TO BE REMOVED?]
* Low-level keyboard and text (scan codes, keyboard layout, text mode controls)
* System ID
* Process routines (start a program or stop it) [TO BE REMOVED?]
* Memory routines (reserve memory) [TO BE REMOVED?]
* Loading and running binary files on storage (PGX, PGZ, and ELF) [TO BE REMOVED?]

## Boot Process

One of the key functions of the toolbox is to initialize the computer at bootup time and to launch a more complete operating system or user program. The system will boot in the following manner:


### Proposed Boot Sequence

1. Initialize I/O devices (screen, sound devices, *etc.*) to a known, neutral state.
2. Display a booting/splash screen
3. If the DIP switches are set to allow boot from RAM:
	1. Scan each 8KB block of RAM for the [program header](progheader.md).
	2. If one is found, run the program linked.
4. If a flash cartridge is inserted:
	1. Check the first 8KB block of the cartridge for the [program header](progheader.md).
	2. If one is found, run the program linked.
5. If an SD card is inserted:
	1. Check for an executable boot sector.
	2. If one is found, execute it.
6. Check any internal SD card for an executable boot sector.
7. If one is found, execute it.
8. Scan each 8KB block of the firmware flash for the [program header](progheader.md).
9. If one is found, run the program linked. 
10. If nothing else is found, wait for an SD card to be inserted and repeat step 4.

### Machine-specific Calling Conventions

* [ABI for WDC65C816-based Machines](src/C256/ABI.md)

## Building the Toolbox

1. Change your directory to the `src` directory
2. Execute `make clean` to ensure any previous build files are removed
3. Execute `make UNIT=<...> MEMORY=ROM` to build a binary for a particular Foenix model that can be loaded into the flash memory. Or Execute `make UNIT=<...> MEMORY=RAM` to build a RAM loadable version for testing.

The following UNIT labels are supported: `C256U`, `C256_FMX`, `F256Ke` for the original F256K with a W65C816 processor and the flat memory model FPGA, or `F256K2e` for the original F256K2 with a W65C816 processor and the flat memory model FPGA.

NOTE: Foenix Toolbox does not work on any F256 with the W65C02 processor or with the original FPGA load that provides a 64KB address space for the processor. It needs full 24-bit addressing to work.

### Prerequisites

The toolbox project is intended to be compiled using the [Calypsi C compiler](https://www.calypsi.cc/). To build the project, you will need:

* The correct version of Calypsi for the target system
* An implementation of `make` for your build computer
* Python3
* Some utility for uploading the binary to the Foenix machine (for instance [FoenixMgr](https://github.com/pweingar/FoenixMgr))

## License

Generally, the code for the Foenix Toolbox is covered under the BSD-3 open source license. The toolbox includes code from other projects, however, and that code is covered under the licenses used by those projects. This code includes:

* [FatFS](http://elm-chan.org/fsw/ff/), which is used for the FAT32 implementation and is distributed under a BSD-style license. The license and copyright notice for FatFS are included in the source code.

