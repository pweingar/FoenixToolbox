/**
 * @file boot.c
 * @author your name (you@domain.com)
 * @brief Boot sequence control code for managing loading up the user's control code
 * @version 0.1
 * @date 2024-06-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "boot.h"
#include "memory.h"
#include "proc.h"
#include "dev/fsys.h"
#include "timers.h"
#include "dev/txt_screen.h"
#include "dev/sprites.h"
#include "dev/tiles.h"
#include "vicky_general.h"
#include "rsrc/sprites/boot_sprites.h"
#include "rsrc/tiles/boot_tiles.h"

#include <stdio.h>
#include <string.h>

const uint32_t boot_record_alignment = 8192;		// Number of bytes for boot record alignement
const uint32_t boot_rom_location = 0xf00000;		// Location to check for a boot record in ROM
const uint32_t boot_cart_location = 0xf40000;		// Location to check for a boot record in ROM

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
	char signature1;
	char signature2;
	uint32_t start_address;
	uint8_t version;
	uint32_t icon_address;
	uint32_t clut_address;
	const char * name;
} boot_record_t, *boot_record_p;

/**
 * @brief List bootable areas to check, in priority order
 * 
 */
static enum boot_src_e boot_chain[] = {
	BOOT_SRC_RAM,
	BOOT_SRC_CARTRIDGE,
	BOOT_SRC_SD0,
	// BOOT_SRC_SD1,
	BOOT_SRC_ROM,
};

/**
 * @brief A holder for empty arguments list so we have something to point to when starting a binary file
 * 
 */
static char * boot_args[] = {
	0,
	0
};

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
			if ((fsys_stat("/sd0/fnxboot.pgx", &file_info) >= 0) || (fsys_stat("/sd0/fnxboot.pgz", &file_info) >= 0)) {
				return true;
			}
			break;

		case BOOT_SRC_SD1:
			// if (fsys_stat("/sd1/fnxboot.pgx", &file_info) || fsys_stat("/sd1/fnxboot.pgz", &file_info)) {
			// 	return true;
			// }
			break;

		default:
			break;
	}

	return 0;
}

/**
 * @brief Do the initial display of the boot device icon
 * 
 * @param position the position in the boot sequence (0 - 4)
 * @param device the ID of the boot device
 */
void boot_icon(short position, enum boot_src_e device) {
	short base_x = 14*8;
	short base_y = 21*8;
	uint8_t x = base_x + 32*position;

	switch(device) {
		case BOOT_SRC_RAM:
			sprite_assign(position, ram_pixels, 0, 0);
			break;

		case BOOT_SRC_ROM:
			sprite_assign(position, rom_pixels, 0, 0);
			break;

		case BOOT_SRC_CARTRIDGE:
			sprite_assign(position, cartridge_pixels, 0, 0);
			break;
		
		case BOOT_SRC_SD0:
			sprite_assign(position, sd_ext_pixels, 0, 0);
			break;

		case BOOT_SRC_SD1:
			sprite_assign(position, sd_int_pixels, 0, 0);
			break;
					
		default:
			break;
	}

	// By default, we'll set the icon to dim
	// We'll make it bright when we've confirmed it is bootable
	sprite_clut(position, 1);

	// Set the position of the icon
	sprite_position(position, x, base_y);

	// Enable the icon to display it
	sprite_enable(position, true);
}

/**
 * @brief Indicate that the device is bootable by making it brighter
 * 
 * @param position the position in the boot sequence (0 - 4)
 */
void boot_icon_highlight(short position) {
	sprite_clut(position, 0);
}

void boot_from(enum boot_src_e device) {
	short result = 0;
	t_file_info file_info;

	switch(device) {
		case BOOT_SRC_SD0:
			if (fsys_stat("/sd0/fnxboot.pgz", &file_info) >= 0) {
				txt_print(0, "Booting: /sd0/fnxboot.pgz\n");
				proc_run("/sd0/fnxboot.pgz", 0, boot_args);

			} else if (fsys_stat("/sd0/fnxboot.pgx", &file_info) >= 0) {
				txt_print(0, "Booting: /sd0/fnxboot.pgx\n");
				result = proc_run("/sd0/fnxboot.pgx", 0, boot_args);
				if (result != 0) {
					printf("proc_run error: %d\n", result);
				}
			}
			break;

		default:
			txt_print(0, "No bootable device is present.\n");
			break;
	}
}

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
			return "CARTRIDGE";

		default:
			return "None"; 
	}
}

/**
 * @brief Setup the boot screen
 * 
 */
void boot_screen() {
	enum boot_src_e boot_source = BOOT_SRC_NONE;
	short i = 0;
	long jiffies_target = 0;

	// txt_set_mode(0, TXT_MODE_TEXT | TXT_MODE_SPRITE);
	*tvky_mstr_ctrl = (uint16_t)(VKY_MCR_TILE | VKY_MCR_SPRITE | VKY_MCR_GRAPHICS | VKY_MCR_TEXT_OVERLAY | VKY_MCR_TEXT);

	tvky_bg_color->blue = 0;
	tvky_bg_color->green = 0;
	tvky_bg_color->red = 0;

	for (i = 0; i < 4 * 256; i++) {
		VKY_GR_CLUT_0[i] = boot_clut[i];
		VKY_GR_CLUT_1[i] = boot_clut[i] >> 2;
	}

	// Set up the tiles for the background

	tile_init();
	sprite_init();

	tile_set_assign(0, (uint8_t *)boot_tiles_pixels, false);
	tile_set_update(0);

	tile_map_assign(0, (uint16_t *)boot_tiles_map, 42, 32, 1);
	tile_map_position(0, 0, 0);
	tile_map_enable(0, true);
	tile_map_update(0);

	// Make tile map 0 the top layer

	*tvky_layers = 0x0444;

	// Set up the text window for the boot messaging
	t_rect boot_text_window;
	boot_text_window.origin.x = 20;
	boot_text_window.origin.y = 19;
	boot_text_window.size.width = 40;
	boot_text_window.size.height = 12;
	txt_set_region(0, &boot_text_window);

	txt_print(0, "Scanning for bootable devices...\n");

	for (short position = 0; position < sizeof(boot_chain) / sizeof(enum boot_src_e); position++) {
		boot_record_p boot_record;

		boot_icon(position, boot_chain[position]);
		if (is_bootable(boot_chain[position], &boot_record)) {
			boot_icon_highlight(position);

			// Assign the boot source to this, if it hasn't already been bound
			if (boot_source == BOOT_SRC_NONE) {
				txt_print(0, "Default boot source: ");
				txt_print(0, boot_source_name(boot_chain[position]));
				txt_put(0, '\n');
				boot_source = boot_chain[position];
			}

			// If there is a boot icon specified in the boot record, change to that icon
			if (boot_record != 0) {
				if (boot_record->icon_address != 0) {
					sprite_assign(position, (uint8_t *)(boot_record->icon_address), 0, 0);

					// If there is a CLUT defined for the boot record, switch to use that clut
					if (boot_record->clut_address != 0) {
						for (i = 0; i < 4 * 256; i++) {
							uint8_t * source_clut = (uint8_t *)boot_record->clut_address;
							VKY_GR_CLUT_2[i] = source_clut[i];
						}
						sprite_clut(position, 2);
					}
				}
			}
		}
	}

	// Wait some time for user input
	jiffies_target = timers_jiffies() + 60 * 10;
	while (jiffies_target > timers_jiffies()) {
		;
	}

	// And launch the system
	boot_from(boot_source);
}