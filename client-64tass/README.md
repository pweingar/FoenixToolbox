# Foenix Toolbox Function Bindings for the 64TASS Assembler

This folder contains the assembly (*.s) files needed to call into the Foenix Toolbox using 64TASS assembly.

## Foenix Toolbox Assembly ABI

Foenix Toolbox functions are called using the Calypsi "simplecall" calling convention.

1. The first parameter is passed using the accumulator for 8 and 16 bit quantities, and the X register and accumulator for 24 and 32 bit quantities (with the X register holding the most significant bits).
2. Additional parameters are passed on the stack, pushed so that the left-most parameter is at the top of the stack before the call.
3. The return value is passed in the accumulator for 8 and 16 bit values or the X register and accumulator for 24 and 32 bit values.
4. The caller is responsible for removing the parameters from the stack (if any) after the call returns.
5. The Foenix Toolbox will preserve the caller's direct page and data bank registers at the start of the call and restore them just before returning to the caller.
6. All functions are accessed using long calls (`JSL`).

## Files

The files that are included as part of the client bindings are:

* `toolbox.s`: the main include file. This file includes `bindings.s` automatically and also defines the `alloc` and `free` macros that can be used to help manage the stack. NOTE: the `free` macro uses the first two bytes in the caller's direct page as scratch storage to preserve the return value from the function while cleaning up the stack.

* `types.s`: this include file defines some structures to match the data structures used by some of the Toolbox functions.

* `bindings.s`: this include file defines all the function labels as well as labels that may be used to treat the parameters as locations in memory, rather than as values on the stack. This may simplify coding in some cases.

* `genbindings.py`: this Python script is used to generate the `bindings.s` file in the case that Toolbox functions are removed, altered, or added. It uses the files in the Calypsi bindings folder as the source for what functions are present. The general user of the 64TASS client files will probably never use this script.

* `hello.s`: A simple "Hello, world" example. It also demonstrates using the program header for programs to run from RAM, flash, or the cartridge.


