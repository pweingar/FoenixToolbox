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
#include "dev/fsys.h"
#include "dev/kbd_mo.h"
#include "dev/txt_screen.h"

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
				sprintf(line, "%08lX:", (uint32_t)(buffer + i));
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
 * Make sure we can read from the PATA hard drive
 */
void test_hd() {
    char line[80];
    uint8_t my_buf[512];

    printf("\nReading the hard drive's boot sector\n\n");
    short result = bdev_init(BDEV_HD0);
    if (result < 0) {
        sprintf(line, "Unable to initialize the hard drive: %s [%d]\n", err_message(result), result);
        txt_print(0, line);
    } else {
        short n = bdev_read(BDEV_HD0, 0L, my_buf, 512);
        if (n >= 0) {
            // printf("Read %d bytes:\n", n);
            dump_buffer(my_buf, 512, 1);
        } else {
            sprintf(line, "Unable to read the boot sector: %s [%d]\n", err_message(n), n);
            txt_print(0, line);
        }
    }
}

/**
 * Make sure we can read from the external SD card
 */
void test_sd0() {
    char line[80];
    uint8_t my_buf[512];

    printf("\nReading the SD card's boot sector\n\n");
    short result = bdev_init(BDEV_SD0);
    if (result < 0) {
        sprintf(line, "Unable to initialize the SD card: %d\n", result);
        txt_print(0, line);
    } else {
        short n = bdev_read(BDEV_SD0, 0L, my_buf, 512);
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
                    printf("%s\t%4ld\n", entry.name, entry.size);
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