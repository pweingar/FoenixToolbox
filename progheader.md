# Foenix Toolbox Program Header

To recognize a program or bootable module in RAM or flash, the toolbox needs a small header to identify the program. This header is based on the Kernel User Program headers used by the default kernel for the 8-bit versions of the F256, but it has been modified to conform to the 24-bit memory model of the F256K2e:

| Byte     | Value                                                    |
| ----     | -----                                                    |
| 0        | Signature: $F8                                           |
| 1        | Signature: $16                                           |
| 2 -- 5   | Starting address of the program, little-endian format    |
| 6        | Header structure version number                          |
| 7 -- 9   | Reserved                                                 |
| 10 -- m  | Zero-terminated string: name of the program              |
| m+1 -- n | Zero-terminated string describing the arguments          |
| n+1 -- p | Zero-terminated string describing the program's function |

The header must be present at the beginning of an 8KB block of memory to be recognized. If the option to boot from RAM is present, then this header may be present at the beginning of any 8KB block of memory. If the option is off, headers in RAM will be ignored.

To boot from the flash cartridge, this header must be the first thing in the cartridge memory. That is, only the first 8KB block of the expansion cartridge will be checked at boot time.
