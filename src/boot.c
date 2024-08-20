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
#include "dev/txt_screen.h"
#include "dev/sprites.h"
#include "dev/tiles.h"
#include "vicky_general.h"
#include "rsrc/sprites/boot_sprites.h"
#include "rsrc/tiles/boot_tiles.h"

#include <string.h>

const uint32_t boot_record_alignment = 8192;		// Number of bytes for boot record alignement

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
	BOOT_SRC_SD1,
	BOOT_SRC_ROM,
	BOOT_SRC_NONE
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
 * @brief Look for a boot record in RAM
 * @return 1 if success, 0 if boot record not validated
 * 
 */
short boot_ram() {
	unsigned long top_ram = mem_get_ramtop();
	for (uint32_t address = 0; address < top_ram; address += boot_record_alignment) {
		if (boot_ram_launch((boot_record_p)address)) {
			return 1;
		}
	}

	return 0;
}

/**
 * @brief Look for a boot record in the flash cartridge
 * @return 1 if success, 0 if boot record not validated
 * 
 */
short boot_cartridge() {
	return 0;
}

/**
 * @brief Find and launch the user's code
 * 
 */
void boot_launch() {
	for (short i = 0; boot_chain[i] != BOOT_SRC_NONE; i++) {
		switch(boot_chain[i]) {
			case BOOT_SRC_RAM:
				if (boot_ram()) {
					return;
				}
				break;

			case BOOT_SRC_SD0:
				if (proc_run("/sd0/fnxboot.pgz", 0, boot_args) == 0) {
					return;
				} else if (proc_run("/sd0/fnxboot.pgx", 0, boot_args) == 0) {
					return;
				} else if (proc_run("/sd0/fnxboot.elf", 0, boot_args) == 0) {
					return;
				}
				break;

			default:
				break;
		}
	}
}

/**
 * @brief Setup the boot screen
 * 
 */
void boot_screen() {
	// txt_set_mode(0, TXT_MODE_TEXT | TXT_MODE_SPRITE);
	*tvky_mstr_ctrl = (uint16_t)(VKY_MCR_TILE | VKY_MCR_SPRITE | VKY_MCR_GRAPHICS | VKY_MCR_TEXT_OVERLAY | VKY_MCR_TEXT);

	tvky_bg_color->blue = 0;
	tvky_bg_color->green = 0;
	tvky_bg_color->red = 0;

	for (int i = 0; i < 4 * 256; i++) {
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

	short base_x = 14*8;
	short base_y = 21*8;

	// Set up the text window for the boot messaging
	t_rect boot_text_window;
	boot_text_window.origin.x = 20;
	boot_text_window.origin.y = 19;
	boot_text_window.size.width = 40;
	boot_text_window.size.height = 12;
	txt_set_region(0, &boot_text_window);

	txt_print(0, "Scanning for bootable devices...\n\n");

	sprite_assign(0, cartridge_pixels, 0, 0);
	sprite_assign(1, ram_pixels, 0, 0);
	sprite_assign(2, sd_ext_pixels, 0, 0);
	sprite_assign(3, sd_int_pixels, 0, 0);
	sprite_assign(4, rom_pixels, 0, 0);

	sprite_clut(0, 1);
	sprite_clut(1, 0);
	sprite_clut(2, 1);
	sprite_clut(3, 0);
	sprite_clut(4, 0);

	sprite_position(0, base_x, base_y);
	sprite_position(1, base_x + 32, base_y);
	sprite_position(2, base_x + 32*2, base_y);
	sprite_position(3, base_x + 32*3, base_y);
	sprite_position(4, base_x + 32*4, base_y);

	sprite_enable(0, true);
	sprite_enable(1, true);
	sprite_enable(2, true);
	sprite_enable(3, true);
	sprite_enable(4, true);

	// txt_print(0, "1: Start 'Frogger' from the cartridge.\n");
	txt_print(0, "1: Start 'Foobar' in RAM.\n");
	// txt_print(0, "3: Start from external SD card.\n");
	txt_print(0, "2: Start from internal SD card.\n");
	txt_print(0, "3: Start 'f/Manager'.\n");
}