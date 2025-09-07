/**
 * @file boot.c
 * @author your name (you@domain.com)
 * @brief Boot sequence control code for managing loading up the user's control code on the FA2560K2
 * @version 0.1
 * @date 2025-09-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "boot.h"
#include "memory.h"
#include "proc.h"
#include "cartridge.h"
#include "dev/fsys.h"
#include "dev/channel.h"
#include "dev/console.h"
#include "dev/kbd_f256.h"
#include "dev/txt_screen.h"
#include "dev/sprites.h"
#include "dev/tiles.h"
#include "gabe_reg.h"
#include "timers.h"
#include "vicky_general.h"
#include "rsrc/sprites/boot_sprites.h"
#include "rsrc/tiles/boot_tiles.h"

#include <stdio.h>
#include <string.h>

#define MAX_BOOT_SRC 5

const short display_margin = 10;
const short display_width = 108;
const short display_height = 76;

const uint32_t boot_record_alignment = 8192;		// Number of bytes for boot record alignement
const uint32_t boot_rom_location = 0xf80000;		// Location to check for a boot record in ROM
const uint32_t boot_cart_location = 0xf40000;		// Location to check for a boot record in ROM

const unsigned short kbd_mod_shift = 0x0800;
const unsigned short kbd_sc_space = 0x0039;
const unsigned short kbd_sc_f1 = 0x003b;
const unsigned short kbd_sc_f2 = 0x003c;
const unsigned short kbd_sc_f3 = 0x003d;
const unsigned short kbd_sc_f4 = 0x003e;
const unsigned short kbd_sc_f5 = 0x003f;
const unsigned short kbd_sc_f6 = 0x0040;
const unsigned short kbd_sc_f7 = 0x0041;
const unsigned short kbd_sc_f8 = 0x0042;

enum boot_src_e {
	BOOT_SRC_NONE = 0,		// Nothing more to check
	BOOT_SRC_RAM,			// Check RAM for a bootable
	BOOT_SRC_ROM,			// Check the ROM drive
	BOOT_SRC_CARTRIDGE,		// Check the cartridge
	BOOT_SRC_SD0,			// Check the external SD card
	BOOT_SRC_SD1			// Check the internal SD card / IDE
};

/**
 * @brief Structure to hold a boot record
 * 
 */
typedef struct boot_record_s {
	char signature1;				// Needs to be $f8
	char signature2;				// Needs to be $16
	uint8_t version;				// Currently $00
	uint32_t start_address;			// Address to start executing (in little-endian format of the 65816)
	uint32_t icon_address;			// Address of an icon to show (32x32 sprite data, use 0 for no icon)
	uint32_t clut_address;			// Address of the palette for the icon in Vicky format (0 to use the default)
	const char * name;				// A display name/command word for the program (not currently used)
} boot_record_t, *boot_record_p;

/**
 * @brief List bootable areas to check, in priority order (without RAM booting)
 * 
 */
static enum boot_src_e boot_chain[MAX_BOOT_SRC];
static bool bootable[MAX_BOOT_SRC];
static short boot_src_cnt = 0;

extern t_sys_info info;

/**
 * @brief A holder for empty arguments list so we have something to point to when starting a binary file
 * 
 */
static char * boot_args[] = {
	0,
	0
};

/**
 * @brief Display the system information
 * 
 */
static void display_sysinfo() {
	printf("\e[H");
	printf("______ ___   _____  _____  ____ _____ _   __ _____ \n");
	printf("|  ___/ _ \\ / __  \\|  ___|/ ___|  _  | | / // __  \\\n");
	printf("| |_ / /_\\ \\`' / /'|___ \\/ /___| |/' | |/ / `' / /'\n");
	printf("|  _||  _  |  / /      \\ \\ ___ \\  /| |    \\   / /  \n");
	printf("| |  | | | |./ /___/\\__/ / \\_/ \\ |_/ / |\\  \\./ /___\n");
	printf("\\_|  \\_| |_/\\_____/\\____/\\_____/\\___/\\_| \\_/\\_____/\n\n\n");                                                
	
	// printf("Model   %s\n", info.model_name);
	printf("CPU     %s\n", info.cpu_name);
	printf("Clock   %lu MHz\n", info.cpu_clock_khz / (long)1000);
	printf("Memory  %d KB\n", (int)(info.system_ram_size / ((long)1024 * (long)1024)));
	printf("FPGA    %04X %04X.%04X\n", info.fpga_model, info.fpga_version, info.fpga_subver);
	printf("Toolbox v%d.%02d.%04d\n\n", info.mcp_version, info.mcp_rev, info.mcp_build);
}

/**
 * @brief Check the memory indicated to validate it is a boot record... if so, launch the code indicated
 * 
 * @param record pointer to the possible boot record
 * @return 1 if success, 0 if boot record not validated
 */
short boot_ram_launch(boot_record_p record) {
	if ((record->signature1 == 0xf8) && (record->signature2 == 0x16) && (record->version == 0)) {
		// Memory does indeed hold a boot record
		proc_exec(record->start_address, 0, 0, 0);
		return 1;
	} else {
		return 0;
	}
}

/**
 * @brief Check to see if the boot record is valid
 * 
 * @param record pointer to the potential boot record to validate
 * @return true the record is valid
 * @return false the record is not valid
 */
bool is_valid_boot_record(boot_record_p record) {
	return ((record->signature1 == 0xf8) && (record->signature2 == 0x16) && (record->version == 0));
}

/**
 * @brief Check to see if a device has bootable code
 * 
 * For RAM, ROM, and Flash boot devices, there will be a boot record on the device.
 * The SD cards will not have a boot record.
 * 
 * @param device the ID of the boot device to check
 * @param boot_record pointer to the pointer that should be set for the boot record on the device
 * @return true if the device is bootable
 * @return false if the device cannot be booted
 */
bool is_bootable(enum boot_src_e device, boot_record_p * boot_record) {
	uint32_t top_ram = 0;
	boot_record_p record = 0;
	t_file_info file_info;

	// By default, do not have a boot record
	*boot_record = 0;

	switch(device) {
		case BOOT_SRC_RAM:
			top_ram = (uint32_t)mem_get_ramtop();
			for (uint32_t address = 0; address < top_ram; address += boot_record_alignment) {
				record = (boot_record_p)address;

				if (is_valid_boot_record(record)) {
					*boot_record = record;
					return true;
				}
			}
			break;

		case BOOT_SRC_ROM:
			record = (boot_record_p)boot_rom_location;
			if (is_valid_boot_record(record)) {
				*boot_record = record;
				return true;
			}
			break;

		case BOOT_SRC_CARTRIDGE:
			record = (boot_record_p)boot_cart_location;
			if (is_valid_boot_record(record)) {
				*boot_record = record;
				return true;
			}
			break;

		case BOOT_SRC_SD0:
			if ((fsys_stat("/sd0/fnxboot68.pgx", &file_info) >= 0) || (fsys_stat("/sd0/fnxboot68.pgz", &file_info) >= 0)) {
				return true;
			}
			break;

		case BOOT_SRC_SD1:
			if ((fsys_stat("/sd1/fnxboot68.pgx", &file_info) >= 0) || (fsys_stat("/sd1/fnxboot68.pgz", &file_info) >= 0)) {
				return true;
			}
			break;

		default:
			break;
	}

	return 0;
}

/**
 * Get the display name of a given boot device
 * 
 * @param device the ID of the device
 * @return the display name for the device
 */
const char * boot_source_name(enum boot_src_e device) {
	switch(device) {
		case BOOT_SRC_SD0:
			return "External SDC";

		case BOOT_SRC_SD1:
			return "Internal SDC";

		case BOOT_SRC_RAM:
			return "RAM";

		case BOOT_SRC_ROM:
			return "ROM";

		case BOOT_SRC_CARTRIDGE:
			return "Cartridge";

		default:
			return "None"; 
	}
}

static void boot_reset_screen() {
	// Reset the region
	t_rect boot_text_window;
	boot_text_window.origin.x = 0;
	boot_text_window.origin.y = 0;
	boot_text_window.size.width = 0;
	boot_text_window.size.height = 0;
	txt_set_region(0, &boot_text_window);

	// Clear the text screen
	txt_clear(0, 2);
	txt_set_xy(0, 0, 0);
}

void boot_from(enum boot_src_e device, boot_record_p boot_record) {
	short result = 0;
	t_file_info file_info;

	switch(device) {
		case BOOT_SRC_SD0:
			if (fsys_stat("/sd0/fnxboot68.pgz", &file_info) >= 0) {
				printf("Booting: /sd0/fnxboot68.pgz\n");
				boot_reset_screen();
				result = proc_run("/sd0/fnxboot68.pgz", 0, boot_args);
				if (result != 0) {
					printf("Could not load /sd0/fnxboot68.pgz: %d\n", result);
				}

			} else if (fsys_stat("/sd0/fnxboot68.pgx", &file_info) >= 0) {
				printf("Booting: /sd0/fnxboot68.pgx\n");
				boot_reset_screen();
				result = proc_run("/sd0/fnxboot68.pgx", 0, boot_args);
				if (result != 0) {
					printf("Could not load /sd0/fnxboot68.pgx: %d\n", result);
				}
			}
			break;

		case BOOT_SRC_SD1:
			if (fsys_stat("/sd1/fnxboot68.pgz", &file_info) >= 0) {
				printf("Booting: /sd1/fnxboot68.pgz\n");
				boot_reset_screen();
				result = proc_run("/sd1/fnxboot68.pgz", 0, boot_args);
				if (result != 0) {
					printf("Could not load /sd1/fnxboot68.pgz: %d\n", result);
				}

			} else if (fsys_stat("/sd1/fnxboot68.pgx", &file_info) >= 0) {
				printf("Booting: /sd1/fnxboot68.pgx\n");
				boot_reset_screen();
				result = proc_run("/sd1/fnxboot68.pgx", 0, boot_args);
				if (result != 0) {
					printf("Could not load /sd1/fnxboot68.pgx: %d\n", result);
				}
			}
			break;

		case BOOT_SRC_CARTRIDGE:
		case BOOT_SRC_ROM:
		case BOOT_SRC_RAM:
			printf("Booting from %s\n", boot_source_name(device));

			// Double-check that the boot record is valid before we attempt to boot
			if (boot_record != 0) {
				if ((boot_record->signature1 == 0xf8) && (boot_record->signature2 == 0x16) && (boot_record->version == 0)) {
					// Memory does indeed hold a boot record
					boot_reset_screen();
					proc_exec(boot_record->start_address, 0, 0, boot_args);
				}
			} else {
				printf("A valid boot record was not found.\n");
			}

			break;

		default:
			printf("No bootable device is present.\n");
			break;
	}
}

/**
 * @brief Convert a scan code to a menu selection
 *
 * Convert: function keys F1 - F8 to 1 - 8
 *          SPACE to 0x20
 * 
 * @param scancode 
 * @return short 
 */
static short sc_to_function(unsigned short scancode) {
	switch(scancode) {
		case kbd_sc_space:
			return 0x20;

		case kbd_sc_f1:
			return 1;

		case kbd_sc_f1 | kbd_mod_shift:
		case kbd_sc_f2:
			return 2;

		case kbd_sc_f3:
			return 3;
			
		case kbd_sc_f3 | kbd_mod_shift:
		case kbd_sc_f4:
			return 4;

		case kbd_sc_f5:
			return 5;
			
		case kbd_sc_f5 | kbd_mod_shift:
		case kbd_sc_f6:
			return 6;

		default:
			return 0;
	}
}

/**
 * @brief Setup the boot screen
 * 
 */
void boot_screen() {
	enum boot_src_e boot_source = BOOT_SRC_NONE;
	boot_record_p boot_record[MAX_BOOT_SRC];
	short boot_position = 0;
	short bootable_count = 0;
	short i = 0;
	long jiffies_target = 0;
	char message[80];

	// Check the DIP switches to see if we should include RAM booting
	// Choose the correct boot chain accordingly

	i = 0;
	boot_src_cnt = 0;
	// if ((*GABE_DIP_REG & DIP_BOOT_RAM) == 0) {
		boot_chain[0] = BOOT_SRC_RAM;
		i = 1;
		boot_src_cnt++;
	// }
	boot_chain[i++] = BOOT_SRC_CARTRIDGE;
	boot_chain[i++] = BOOT_SRC_SD0;
	boot_chain[i++] = BOOT_SRC_SD1;
	boot_chain[i++] = BOOT_SRC_ROM;
	boot_src_cnt += 4;

	// Make sure that ANSI escape codes will be honored
	chan_ioctrl(0, CON_IOCTRL_ANSI_ON, 0, 0);

	// Set the screen for the FA2560K2
	txt_set_mode(0, TXT_MODE_TEXT | TXT_MODE_MEMORY);
	txt_set_resolution(0, 1024, 768);
	txt_set_color(0, 0x0f, 0x00);

	// Fill the background of the screen

	t_rect fullscreen;
	fullscreen.origin.x = 0;
	fullscreen.origin.y = 0;
	fullscreen.size.width = 0;
	fullscreen.size.height = 0;
	txt_set_region(0, &fullscreen);
	txt_fill(0, 0xb0);

	// Set up the text window for the boot messaging
	t_rect boot_text_window;
	boot_text_window.origin.x = display_margin;
	boot_text_window.origin.y = display_margin;
	boot_text_window.size.width = display_width;
	boot_text_window.size.height = display_height;
	txt_set_region(0, &boot_text_window);
	txt_set_color(0, 0x0f, 0x04);
	txt_clear(0, 2);

	display_sysinfo();

	printf("Scanning for bootable devices...\n");

	for (short position = 0; position < boot_src_cnt; position++) {
		bootable[position] = false;
		// boot_icon(position, boot_chain[position]);
		if (is_bootable(boot_chain[position], &boot_record[position])) {
			// boot_icon_highlight(position);
			bootable[position] = true;
			bootable_count++;
 
			// Assign the boot source to this, if it hasn't already been bound
			if (boot_source == BOOT_SRC_NONE) {
				printf("Default boot source: %s\n", boot_source_name(boot_chain[position]));
				boot_source = boot_chain[position];
				boot_position = position;
			}
		}
	}

	// List out all the selectable boot sources
	if (bootable_count > 1) {
		printf("\nSelect a boot source:\n\n");

		for (i = 0; i < boot_src_cnt; i++) {
			if (bootable[i]) {
				sprintf(message, "\e[93mF%d\e[37m-%s\n", i+1, boot_source_name(boot_chain[i]));
				chan_write(0, (uint8_t *)message, strlen(message));
			}
		}
	}

	sprintf(message, "\nPress \e[93mSPACE\e[37m for default.\n");
	chan_write(0, (uint8_t *)message, strlen(message));

	// Give the user time to press a key to select a boot source
	// If the time expires, boot the default source (earliest in the boot chain)

	jiffies_target = timers_jiffies() + 60 * 15;
	while (jiffies_target > timers_jiffies()) {
		unsigned short scancode = kbd_get_scancode();
		if (scancode > 0) {
			short selected = sc_to_function(scancode);

			if (selected == 0x20) {
				// SPACE was pressed... just boot the default
				break;

			} else if (selected > 0) {
				if (bootable[selected - 1]) {
					boot_position = selected - 1;
					boot_source = boot_chain[boot_position];
					break;
				}
			}
		}
	}

	// And launch the system

	boot_from(boot_source, boot_record[boot_position]);
}
