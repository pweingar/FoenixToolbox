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

/**
 * Print out the current character set in a matrix
 */
void test_charset() {
    short screen = 0;

    txt_clear(screen, 2);
    txt_set_xy(screen, 0, 0);
    txt_print(screen, " 0123456789ABCDEF\n");
    txt_print(screen, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\nA\nB\nC\nD\nE\nF");

    for (short row = 0; row < 16; row++) {
        txt_set_xy(screen, 1, row + 1);
        for (short column = 0; column < 16; column++) {
            txt_put(screen, (row << 4) + column);
        }
    }

    while (1) ;
}

/**
 * Represent the display information for a key
 */
typedef struct visual_key_s {
    short x;
    short y;
    short scancode;
    const char * label;
} visual_key_t, *visual_key_p;

const visual_key_t keys_fnx79[] = {
    {0, 0, 0x01, "ESC"},
    {8, 0, 0x3b, "F1"},
    {14, 0, 0x3c, "F2"},
    {20, 0, 0x3d, "F3"},
    {26, 0, 0x3e, "F4"},
    {32, 0, 0x3f, "F5"},
    {38, 0, 0x40, "F6"},
    {44, 0, 0x41, "F7"},
    {50, 0, 0x42, "F8"},
    {57, 0, 0x43, "MON"},
    {64, 0, 0x44, "APP"},
    {72, 0, 0x78, "MENU"},
    // {57, 0, 0x54, "MON"},
    // {64, 0, 0x55, "APP"},
    // {72, 0, 0x56, "MENU"},

    {1, 3, 0x29, "`"},
    {6, 3, 0x02, "1"},
    {11, 3, 0x03, "2"},
    {16, 3, 0x04, "3"},
    {21, 3, 0x05, "4"},
    {26, 3, 0x06, "5"},
    {31, 3, 0x07, "6"},
    {36, 3, 0x08, "7"},
    {41, 3, 0x09, "8"},
    {46, 3, 0x0a, "9"},
    {51, 3, 0x0b, "0"},
    {56, 3, 0x0c, "-"},
    {61, 3, 0x0d, "="},
    {66, 3, 0x2b, "\\"},
    {71, 3, 0x0e, "BS"},
    {77, 3, 0x65, "DEL"},

    {1, 6, 0x0f, "TAB"},
    {8, 6, 0x10, "Q"},
    {13, 6, 0x11, "W"},
    {18, 6, 0x12, "E"},
    {23, 6, 0x13, "R"},
    {28, 6, 0x14, "T"},
    {33, 6, 0x15, "Y"},
    {38, 6, 0x16, "U"},
    {43, 6, 0x17, "I"},
    {48, 6, 0x18, "O"},
    {53, 6, 0x19, "P"},
    {58, 6, 0x1a, "["},
    {63, 6, 0x1b, "]"},
    {68, 6, 0x1c, "ENTER"},

    {1, 9, 0x3a, "CAPS"},
    {9, 9, 0x1e, "A"},
    {14, 9, 0x1f, "S"},
    {19, 9, 0x20, "D"},
    {24, 9, 0x21, "F"},
    {29, 9, 0x22, "G"},
    {34, 9, 0x23, "H"},
    {39, 9, 0x24, "J"},
    {44, 9, 0x25, "K"},
    {49, 9, 0x26, "L"},
    {54, 9, 0x27, ":"},
    {59, 9, 0x28, "'"},
    {64, 9, 0x59, " "},

    {1, 12, 0x2a, "SHIFT"},
    {10, 12, 0x5a, " "},
    {15, 12, 0x2c, "Z"},
    {20, 12, 0x2d, "X"},
    {25, 12, 0x2e, "C"},
    {30, 12, 0x2f, "V"},
    {35, 12, 0x30, "B"},
    {40, 12, 0x31, "N"},
    {45, 12, 0x32, "M"},
    {50, 12, 0x33, "<"},
    {55, 12, 0x34, ">"},
    {60, 12, 0x35, "/"},
    {65, 12, 0x36, "SHIFT"},
    {74, 12, 0x68, "\x1e"},

    {0, 15, 0x1d, "CTRL"},
    {8, 15, 0x38, "ALT"},
    {15, 15, 0x5b, "FNX"},
    {22, 15, 0x39, "                     "},
    {47, 15, 0x5d, "FNX"},
    {54, 15, 0x5c, "ALT"},
    {61, 15, 0x5e, "CTRL"},
    {69, 15, 0x69, "\x11"},
    {74, 15, 0x6a, "\x1f"},
    {79, 15, 0x6b, "\x10"},

    {-1, -1, -1, ""}
};

/**
 * Draw a key on the screen
 */
static void txt_draw_key(short screen, short x, short y, const char * label, bool is_pressed) {
    const short padding = 1;
    const uint8_t pressed_fg = 4;
    const uint8_t pressed_bg = 7;

    short label_length = strlen(label);
    short interior_width = label_length + padding * 2;
    uint8_t foreground = 0;
    uint8_t background = 0;

    // Get the current color
    txt_get_color(screen, &foreground, &background);
    if (is_pressed) {
        // If the key is pressed, set the text colors to the pressed color values
        txt_set_color(screen, pressed_fg, pressed_bg);
    }

    // Draw the top row
    txt_set_xy(screen, x, y);
    txt_put(screen, 0xda);
    for (short i = 0; i < interior_width; i++) {
        txt_put(screen, 0xc4);
    }
    txt_put(screen, 0xbf);

    // Draw the key label row
    txt_set_xy(screen, x, y + 1);
    printf("%c %s %c", 0xb3, label, 0xb3);

    // Draw the bottom row
    txt_set_xy(screen, x, y + 2);
    txt_put(screen, 0xc0);
    for (short i = 0; i < interior_width; i++) {
        txt_put(screen, 0xc4);
    }
    txt_put(screen, 0xd9);

    // Restore the original text screen colors
    if (is_pressed) {
        txt_set_color(screen, foreground, background);
    }
}

/**
 * Display a FNX79 keyboard
 */
void test_keys_fnx79() {
    const short screen = 0;
    short k = 0;

    bool is_down[128];
    bool is_changed[128];

    bool caps_active = false;

    for (k = 0; k < 128; k++) {
        is_down[k] = false;
        is_changed[k] = true;
    }

    txt_clear(screen, 2);
    txt_set_cursor_visible(screen, false);

    do {
        short key_code = kbd_get_scancode();

        if (key_code != 0) {
            short bare_code = key_code & 0x7f;
            bool is_break = (key_code & 0x80) == 0x80;

            if (is_break) {
                // Break
                if (is_down[bare_code]) {
                    is_down[bare_code] = false;
                    is_changed[bare_code] = true;
                }
            
            } else {
                // Make
                if (!is_down[bare_code]) {
                    is_down[bare_code] = true;
                    is_changed[bare_code] = true;
                }

                // if ((bare_code == 0x3a) && is_changed[bare_code]) {
                //     if (caps_active) {
                //         // We are pressing the CAPS LOCK for the first time since releasing it, and the LED is ON... turn it OFF
                //         caps_active = false;
                //         kbd_enable(false);
                //         kbd_set_status(0x00);
                //         kbd_enable(true);

                //     } else {
                //         // We are pressing the CAPS LOCK for the first time since releasing it, and the LED is OFF... turn it ON
                //         caps_active = true;
                //         kbd_enable(false);
                //         kbd_set_status(0x04);
                //         kbd_enable(true);
                //     }
                // }
            }
        }

        for (short k = 0; k < 128; k++) {
            short x = keys_fnx79[k].x;
            short y = keys_fnx79[k].y;
            short code = keys_fnx79[k].scancode;

            if (x != -1) {
                if (is_changed[code]) {
                    is_changed[code] = false;
                    txt_draw_key(screen, x, y, keys_fnx79[k].label, is_down[code]);
                }
            }
        }

    } while (1);
}