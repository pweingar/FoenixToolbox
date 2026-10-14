/**
 * Simple test routines... can be removed when everything is up and running
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "tests.h"
#include "pata_reg.h"
#include "dev/block.h"
#include "dev/channel.h"
#include "dev/fsys.h"
#include "dev/kbd_mo.h"
#include "dev/kbd_f256.h"
#include "dev/kbd_ps2.h"
#include "dev/txt_screen.h"
#include "serial_reg.h"
#include "serial_reg.h"
#include "timers.h"

#define VKY_TXT_CHAR_A	((volatile char *)0xfeca0000)
#define VKY_TXT_COLOR_A	((volatile uint8_t *)0xfeca8000)

uint16_t vky_txt_pos = 0;

void vky_txt_emit(char c) {
	VKY_TXT_CHAR_A[vky_txt_pos] = c;
	VKY_TXT_COLOR_A[vky_txt_pos++] = 0xf0;
}

/**
 * Print a nice dump of a byte buffer to the channel
 *
 * @param buffer the byte buffer to print
 * @param size the number of bytes to print
 * @param labels 0: none, 1: offset, 2: address
 */
void dump_buffer(const uint8_t * buffer, short size, short labels) {
    short i = 0;
	short j = 0;
	short ascii_idx = 0;
    char ascii_buffer[17];
    char line[80];
    uint8_t c;

    for (i = 0; i < size; i++) {
        if ((i & 0x000f) == 0) {
			if (i > 0) {
				sprintf(line, " | %s\n", ascii_buffer);
                txt_print(0, line);
			}

			memset(ascii_buffer, 0, 17);
            ascii_idx = 0;

            if (labels == 1) {
				sprintf(line, "%04X:", i);
                txt_print(0, line);
            } else if (labels == 2) {
				sprintf(line, "%08X:", (uint32_t)(buffer + i));
                txt_print(0, line);
            }
        }

        c = buffer[i];
        if (isgraph(c)) {
            ascii_buffer[ascii_idx++] = c;
        } else {
            ascii_buffer[ascii_idx++] = '.';
        }

        if ((i & 0x0007) == 0) {
            // printf(" ");
            txt_put(0, ' ');
        }

		sprintf(line, "%02X", c);
        txt_print(0, line);
    }

	sprintf(line, " | %s\n", ascii_buffer);
    txt_print(0, line);
}

/**
 * Make sure we can read from the external SD card
 */
void test_bdev(short dev) {
    char line[80];
    uint8_t my_buf[512];

    printf("\nReading block device %d boot sector\n\n", dev);
    short result = bdev_init(dev);
    if (result < 0) {
        sprintf(line, "Unable to initialize device %d: %d\n", dev, result);
        txt_print(0, line);
    } else {
        short n = bdev_read(dev, 0L, my_buf, 512);
        if (n >= 0) {
            // printf("Read %d bytes:\n", n);
            dump_buffer(my_buf, 512, 1);
        } else {
            sprintf(line, "Unable to read the boot sector: %d\n", n);
            txt_print(0, line);
        }
    }
}

/**
 * Try to read and print a directory
 */
void test_dir(const char * path) {
    t_file_info entry;

    printf("Attempting to read the directory for %s\n", path);
    short dir = fsys_opendir(path);
	if (dir < 0) {
		printf("Unable to open directory: %s\n", err_message(dir));

	} else {
        printf("Contents of %s\n", path);

		short result = fsys_readdir(dir, &entry);
		while (result >= 0) {
			if (entry.name[0] == 0) {
				// We're finished if the name is blank
				break;
			}

			if ((entry.attributes & FSYS_AM_HID) == 0) {
				if (entry.attributes & FSYS_AM_DIR) {
                    printf("%s/\n", entry.name);
				} else {
                    printf("%s %10ld\n", entry.name, entry.size);
				}
			}

			result = fsys_readdir(dir, &entry);
		}

		fsys_closedir(dir);
    }
}

/**
 * Test the A2560K built-in keyboard
 */
void test_kbd_mo() {
    // do {
    //     unsigned short scancode = kbdmo_get_scancode();
    //     if (scancode) {
    //         printf("%04X ", scancode);
    //     }
    // } while(true);
}

/**
 * Test the keyboard by echoing scancodes
 */
void test_kbd() {
    printf("Type something:\n");
    do {
        // kbd_handle_irq();
        unsigned short scancode = kbd_get_scancode();
        if (scancode) {
            printf("%04X ", scancode);
        }
    } while(true);
}

/**
 * Test the COM1 serial port
 */
void test_com1() {
    printf("Testing COM1...\n");
    short com1 = chan_open(CDEV_COM1, (uint8_t *)"9600,8,1,NONE", 0);
    if (com1 < 0) {
        printf("Unable to open COM1: %d\n", com1);
        return;
    }

    do {
        for (uint8_t c = 0x20; c < 0x7f; c++) {
            chan_write_b(com1, c);
        }
    } while (1);
}

/**
 * Test the COM3 serial port
 */
void test_com3() {
    printf("Attempting to send to COM3\n");
    short com3 = chan_open(CDEV_COM3, 0, 0);
    if (com3 > 0) {
        do {
            for (uint8_t c = 0x20; c < 0x7f; c++) {
                chan_write_b(com3, c);
            }
        } while (1);
    } else {
        printf("Could not open COM3: %d\n", com3);
    }
}

/**
 * Test the COM4 serial port
 */
void test_com4() {
    printf("Attempting to send to COM4\n");
    short com4 = chan_open(CDEV_COM4, 0, 0);
    if (com4 > 0) {
        do {
            for (uint8_t c = 0x20; c < 0x7f; c++) {
                chan_write_b(com4, c);
            }
        } while (1);
    } else {
        printf("Could not open COM4: %d\n", com4);
    }
}

/**
 * Test connecting to the WizFi interface
 */
void test_wizfi() {
    printf("Attempting to send a test command to the WizFi interface\n");
    short wizfi = chan_open(CDEV_WIZFI, 0, 0);
    if (wizfi) {
        char buffer[80];
        const char * command = "AT\r\n";
        short n = chan_write(wizfi, (uint8_t *)command, strlen(command));
        printf("Send %d bytes to the WizFi: %s", n, command);

        n = chan_read(wizfi, (uint8_t *)buffer, 80);
        if (n > 0) {
            printf("WizFi responded: '%s'\n", buffer);
        } else {
            printf("WizFi did not respond [%s].\n", buffer);
        }

        chan_close(wizfi);
    }
}

/**
 * Send a message to the printer on LPT
 */
void test_printer() {
    printf("Attempting to send to LPT\n");
    short lpt = chan_open(CDEV_LPT, 0, 0);
    short count = 0;
    if (lpt > 0) {
        short status = chan_status(lpt);
        printf("LPT Status: 0x%02X\n", status);
        do {
            for (uint8_t c = 0x20; c < 0x7f; c++) {
                chan_write_b(lpt, c);
                count++;
                if (count > 70) {
                    count = 0;
                    chan_write_b(lpt, 13);
                    chan_write_b(lpt, 10);
                }
            }
        } while (1);
    } else {
        printf("Could not open LPT: %d\n", lpt);
    }
}

/**
 * Test the microsecond resolution timer
 */
void test_usec() {
    const char animate[] = "\\|/-";
    short counter = 0;

    printf("TIMER2 multiplier: %d\n", timers_get_multiplier());

    long jiffies_start = timers_jiffies();

    do {
        timers_wait_usec(1000000);
        txt_set_xy(0, 0, 0);
        counter++;
        printf("%d ?= %ld", counter, (timers_jiffies() - jiffies_start) / 60);
    } while (1);
}