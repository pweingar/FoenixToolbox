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

#include "sys_macros.h"

#define CART_ID_UNDEF	-2
#define CART_ID_NONE	-1
#define CART_ID_RAM		0
#define CART_ID_FLASH	1

/**
 * @brief Return a code describing the cartridge
 * 
 * @return short the code describing the cartridge (-1 for none found)
 */
extern SYSTEMCALL short cart_id();

/**
 * @brief Erase the entire flash memory 
 * 
 */
extern SYSTEMCALL void cart_erase();

/**
 * @brief Write a byte to the flash memory
 * 
 * @param address the address to write to (in CPU address space)
 * @param value the byte to write to the address
 */
extern SYSTEMCALL void cart_write_b(uint32_t address, uint8_t value);

/**
 * @brief Write a block of bytes to the flash cartridge (if present)
 * 
 * @param dest the address within the flash cartridge to start writing to
 * @param src the address in regular memory to start reading from
 * @param count the number of bytes to write
 */
extern SYSTEMCALL void cart_write(uint32_t dest, uint32_t src, int count);

#endif