/**
 * @file sprites.c
 * @author your name (you@domain.com)
 * @brief Simple support for sprites
 * @version 0.1
 * @date 2024-08-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include <string.h>

#include "sprites.h"
#include "F256/vicky_ii.h"

#define SPRITE_MAX 16

static t_sprite sprite_shadow[SPRITE_MAX];

uint32_t sprite_ram_base = 0x004000;

/**
 * @brief Update a sprite's hardware registers from the shadow registers
 * 
 * @param sprite the number of the sprite
 */
static void sprite_update(uint8_t sprite) {
	memcpy(&vky_sprite[sprite], &sprite_shadow[sprite], sizeof(t_sprite));
}

/**
 * @brief Set the basic information about the sprite
 * 
 * @param sprite the number of the sprite
 * @param address the address of the bitmap for the sprite
 * @param size the size of the sprite (0 = 32x32, 1 = 24x24, 2 = 16x16, 3 = 8x8)
 * @param layer the number of the layer for the sprite
 */
void sprite_assign(uint8_t sprite, const uint8_t * address, uint8_t size, uint8_t layer) {
	if (sprite < SPRITE_MAX) {

		uint8_t * sprite_mem = (uint8_t *)(sprite_ram_base + 32 * 32 * sprite);
		for (int i = 0; i < 32 * 32; i++) {
			sprite_mem[i] = address[i];
		}

		sprite_shadow[sprite].address = (p_far24)sprite_mem;
		uint8_t tmp = sprite_shadow[sprite].control & 0x87;	// Mask off the size and layer bits
		tmp |= (size & 0x03) << 5;							// Add in the new size
		tmp |= (layer & 0x03) << 3;							// Add in the new layer
		sprite_shadow[sprite].control = tmp;

		sprite_update(sprite);
	}
}

/**
 * @brief Set the color lookup table for the sprite
 * 
 * @param sprite the number of the sprite
 * @param clut the number of the graphics color lookup table to use
 */
void sprite_clut(uint8_t sprite, uint8_t clut) {
	if (sprite < SPRITE_MAX) {
		uint8_t tmp = sprite_shadow[sprite].control & 0xf1;	// Mask off the clut bits
		tmp |= (clut & 0x03) << 1;							// Add in the new clut
		sprite_shadow[sprite].control = tmp;

		sprite_update(sprite);
	}
}

/**
 * @brief Set the position of the sprite
 * 
 * @param sprite the number of the sprite
 * @param x the column for the sprite
 * @param y the row for the sprite
 */
void sprite_position(uint8_t sprite, uint16_t x, uint16_t y) {
	if (sprite < SPRITE_MAX) {
		sprite_shadow[sprite].x = x;
		sprite_shadow[sprite].y = y;

		sprite_update(sprite);
	}
}

/**
 * @brief Enable or disable a sprite
 * 
 * @param sprite the number of the sprite
 * @param is_enabled if true display the sprite, if false hide the sprite
 */
void sprite_enable(uint8_t sprite, bool is_enabled) {
	if (sprite < SPRITE_MAX) {
		if (is_enabled) {
			sprite_shadow[sprite].control |= 0x01;
		} else {
			sprite_shadow[sprite].control &= 0xfe;
		}

		sprite_update(sprite);
	}
}

/**
 * @brief Initialize the sprite registers
 * 
 */
void sprite_init() {
	for (int i = 0; i < SPRITE_MAX; i++) {
		memset(&sprite_shadow[i], 0, sizeof(t_sprite));
		sprite_update(i);
	}
}