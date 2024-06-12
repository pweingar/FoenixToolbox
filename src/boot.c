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

#include <string.h>

const uint32_t boot_record_alignment = 8192;		// Number of bytes for boot record alignement
const uint32_t boot_firmware_address = 0x380000;

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
	uint8_t reserved0;
	uint8_t reserved1;
	uint8_t reserved2;
	char * text_data;
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
