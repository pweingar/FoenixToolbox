/**
 * @file cartridge.h
 * @author your name (you@domain.com)
 * @brief Support for the flash cartridge
 * @version 0.1
 * @date 2024-08-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __cartridge_h__
#define __cartridge_h__

#include <stdint.h>

#define CART_ID_UNDEF	-2
#define CART_ID_NONE	-1
#define CART_ID_RAM		0
#define CART_ID_FLASH	1

/**
 * @brief Return a code describing the cartridge
 * 
 * @return short the code describing the cartridge (-1 for none found)
 */
extern short cart_id();

/**
 * @brief Erase the entire flash memory 
 * 
 */
extern void cart_erase();

/**
 * @brief Write a byte to the flash memory
 * 
 * @param address the address to write to (in CPU address space)
 * @param value the byte to write to the address
 */
extern void cart_write(uint32_t address, uint8_t value);

#endif