/**
 * @file cartdridge.c
 * @author your name (you@domain.com)
 * @brief Support for the flash cartridge
 * @version 0.1
 * @date 2024-08-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdbool.h>

#include "cartridge.h"
#include "timers.h"

volatile uint8_t * cartridge = ((volatile uint8_t *)0xf40000);

static short cart_id_memo = CART_ID_UNDEF;

static bool cart_rw_match(uint8_t value) {
	*cartridge = value;
	uint8_t new_value = *cartridge;
	return (new_value == value);
}

/**
 * @brief Send a command to the flash chip
 * 
 * @param command the number of the command
 */
static void cart_flash_command(uint8_t command) {
	cartridge[0x5555] = 0xaa;
	cartridge[0x2aaa] = 0x55;
	cartridge[0x5555] = command;
}

/**
 * @brief Send the command to enter system ID mode on the flash chip
 * 
 */
static void cart_flash_system_id_enter() {
	// Attempt to enter software ID mode on the flash cartridge
	cart_flash_command(0x90);
}

/**
 * @brief Send the command to exit the system ID mode on the flash chip
 * 
 */
static void cart_flash_system_id_exit() {
	// Attempt to exit software ID mode on the flash cartridge
	cart_flash_command(0xf0);
}

/**
 * @brief Return a code describing the cartridge
 * 
 * @return short the code describing the cartridge (-1 for none found)
 */
short cart_id() {
	if (cart_id_memo == CART_ID_UNDEF) {
		// Start off assuming we don't have anything in the slot
		cart_id_memo = -1;

		// Check to see if there is RAM at the first byte of the cartridge
		if (cart_rw_match(0x12) && cart_rw_match(0x23) && cart_rw_match(0x34)) {
			cart_id_memo = CART_ID_RAM;
		} else {
			// Check to see if we have a flash chip
			cart_flash_system_id_enter();

			// Check for the manufacturer's ID we expect (BF for Microchip)
			if (cartridge[0x0000] == 0xbf) {
				// Yes... get the chip ID
				uint8_t chip_id = cartridge[0x0001];
				if ((chip_id == 0xd5) || (chip_id == 0xd6) || (chip_id == 0xd7)) {
					cart_id_memo = CART_ID_FLASH;
				}
			}

			// Leave system ID mode, regardless
			cart_flash_system_id_exit();
		}
	}

	return cart_id_memo;
}

/**
 * @brief Erase the entire flash memory 
 * 
 */
void cart_erase() {
	if (cart_id() == CART_ID_FLASH) {
		cart_flash_command(0x80);
		cart_flash_command(0x10);

		// Wait half a second
		long target_jiffies = timers_jiffies() + 30;
		while (timers_jiffies() < target_jiffies) ;
	}
}

/**
 * @brief Write a byte to the flash memory
 * 
 * @param address the address to write to (in CPU address space)
 * @param value the byte to write to the address
 */
void cart_write(uint32_t address, uint8_t value) {
	uint32_t cart_base_address = (uint32_t)cartridge;
	uint32_t cart_end_address = 0xf7ffff;
	uint8_t current_value = 0;

	if (cart_id() == CART_ID_FLASH) {
		if ((cart_base_address <= address) && (cart_end_address >= address)) {
			cart_flash_command(0xa0);
			
			// Attempt to write the data
			volatile uint8_t * dest = ((volatile uint8_t *)address);
			*dest = value;

			// Wait for the value to show up at the destination
			long target_jiffies = timers_jiffies() + 5;
			while (timers_jiffies() < target_jiffies) ;
		}
	}
}
