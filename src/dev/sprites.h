/**
 * @file sprites.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __sprites_h__
#define __sprites_h__

#include <stdint.h>
#include <stdbool.h>

extern uint32_t sprite_ram_base;

/**
 * @brief Set the basic information about the sprite
 * 
 * @param sprite the number of the sprite
 * @param address the address of the bitmap for the sprite
 * @param size the size of the sprite (0 = 32x32, 1 = 24x24, 2 = 16x16, 3 = 8x8)
 * @param layer the number of the layer for the sprite
 */
extern void sprite_assign(uint8_t sprite, const uint8_t * address, uint8_t size, uint8_t layer);

/**
 * @brief Set the color lookup table for the sprite
 * 
 * @param sprite the number of the sprite
 * @param clut the number of the graphics color lookup table to use
 */
extern void sprite_clut(uint8_t sprite, uint8_t clut);

/**
 * @brief Set the position of the sprite
 * 
 * @param sprite the number of the sprite
 * @param x the column for the sprite
 * @param y the row for the sprite
 */
extern void sprite_position(uint8_t sprite, uint16_t x, uint16_t y);

/**
 * @brief Enable or disable a sprite
 * 
 * @param sprite the number of the sprite
 * @param is_enabled if true display the sprite, if false hide the sprite
 */
extern void sprite_enable(uint8_t sprite, bool is_enabled);

/**
 * @brief Initialize the sprite registers
 * 
 */
extern void sprite_init();

#endif