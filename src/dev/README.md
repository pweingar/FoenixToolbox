# Device Drivers
This folder contains the machine-specific device driver code for the various I/O the Toolbox has to support:

* block.c -- Public functions to access block devices (currently just SDC, but could also include floppy drive and hard drive). Used by FatFS to access the drives.
* channel.c -- Public functions to access channel devices. Channel devices include the console, serial port, and open files.
* console.c -- Functions to provide a console like device for the keyboard and main screen. This is where the ANSI terminal code support is provided.
* fsys.c -- Provides the file system public functions as well as the channel device driver for open files.
* iec.c -- Provides some wrapper functions around the very lowest level Commodore serial port assembly code. Eventually, this will be used to provide Toolbox public functions for IEC (probably through the channel interface).
* indicators_*.c -- Low level access to the various indicators (LEDs) on the boards: power, media, caps-lock, etc.
* interrupts_*.c -- Machine-specific code for managing interrupts (currently, only the keyboard interrupt is used)
* kbd_f256.c -- Common code for all F256 keyboard (F256K2e mechanical and optical keyboard, F256jr PS/2)
* kbd_f256k.c -- Device specific code for F256K mechanical and optical keyboards
* kbd_f256jr.c -- Device specific code for the F256jr PS/2 keyboard
* ps2.c -- Low-level support for the PS/2 interface
* rtc.c -- Access to the realtime clock
* sdc_spi.c -- Device driver code for the F256's SD card via the SPI interface. This provides a block device driver that is used by FatFS.
* sprites.c -- Code to allow the boot screen to use sprites
* tiles.c -- Code to allow the boot screen to use tiles
* timers_c256.c -- Low-level code to set up the timers used by the C256 code
* txt_c256.c -- Channel device driver code for the C256's main screen
* txt_evid.c -- Channel device driver code for the C256's EVID expansion card
* txt_f256.c -- Channel device driver code for the F256's screen
* txt_screen.c -- Common code for all text screen drivers
* uart.c -- Channel device driver code for the serial port

NOTE: the directory `unsupported_systems` contains the original device driver files from FoenixMCP that are relevant to systems not currently supported by the Toolbox. They are left here so they could be brought back, if desired.
