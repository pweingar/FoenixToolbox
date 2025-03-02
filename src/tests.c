/**
 * Simple test routines... can be removed when everything is up and running
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "tests.h"
#include "dev/block.h"

#define buff ((unsigned char *)0x70000)

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
    uint8_t c;

    for (i = 0; i < size; i++) {
        if ((i & 0x000f) == 0) {
			if (i > 0) {
				printf(" | %s\n", ascii_buffer);
			}

			memset(ascii_buffer, 0, 17);
            ascii_idx = 0;

            if (labels == 1) {
				printf("%04X:", i);
            } else if (labels == 2) {
				printf("%08lX:", (uint32_t)(buffer + i));
            }
        }

        c = buffer[i];
        if (isgraph(c)) {
            ascii_buffer[ascii_idx++] = c;
        } else {
            ascii_buffer[ascii_idx++] = '.';
        }

        if ((i & 0x0007) == 0) {
            printf(" ");
        }

		printf("%02X", c);
    }

	printf(" | %s\n", ascii_buffer);
}

/**
 * Make sure we can read from the PATA hard drive
 */
void test_hd() {
    printf("\nReading the hard drive's boot sector\n");
    short n = 0; // bdev_read(BDEV_HDC, 0L, buff, 512);
    if (n >= 0) {
        printf("Read %d bytes:\n", n);
        dump_buffer(buff, 512, 1);
    } else {
        printf("Unable to read the boot sector: %d\n", n);
    }
}

#define VKY_TXT_CHAR_A	((volatile char *)0xfeca0000)
#define VKY_TXT_COLOR_A	((volatile uint8_t *)0xfeca8000)

uint16_t vky_txt_pos = 0;

void vky_txt_emit(char c) {
	VKY_TXT_CHAR_A[vky_txt_pos] = c;
	VKY_TXT_COLOR_A[vky_txt_pos++] = 0xf0;
}