/**
 * @file tiles.h
 * @author your name (you@domain.com)
 * @brief Provide some basic tile functions
 * @version 0.1
 * @date 2024-08-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __tiles_h__
#define __tiles_h__

#include <stdbool.h>
#include <stdint.h>

extern uint32_t tile_set_memory_base;
extern uint32_t tile_map_memory_base;

/**
 * @brief Setup a tile set
 * 
 * @param set the number of the tile set (0 - 7)
 * @param address pointer to the bitmap for the tile set
 * @param size the number of bytes in the tile set
 * @param is_square if true, layout of image is square, if false it is 8 or 16 pixels wide
 */
extern void tile_set_assign(uint8_t set, uint8_t * address, int size, bool is_square);

/**
 * @brief Update the hardware tile set from its shadow
 * 
 * @param set the number of the tile set (0 - 7)
 */
extern void tile_set_update(uint8_t set);

/**
 * @brief Initialize the tiles
 * 
 */
extern void tile_init();

/**
 * @brief Setup a tile map
 * 
 * @param map the number of the tile map (0 - 2)
 * @param address pointer to the actual tile map data
 * @param width number of columns in the  map
 * @param height number of rows in the map
 * @param size if 0, tiles are 16x16... if 1, tiles are 8x8
 */
extern void tile_map_assign(uint8_t map, uint16_t * address, uint8_t width, uint8_t height, uint8_t size);

/**
 * @brief Set the scoll position of the tile map
 * 
 * @param map the number of the tile map (0 - 2)
 * @param x the horizontal scroll position
 * @param y the vertical scroll position
 */
extern void tile_map_position(uint8_t map, uint16_t x, uint16_t y);

/**
 * @brief Enable or disable the tile map
 * 
 * @param map the number of the tile map (0 - 2)
 * @param is_enabled if true, the map should be displayed, if false it should not
 */
extern void tile_map_enable(uint8_t map, bool is_enabled);

/**
 * @brief Update the hardware tile map from its shadow
 * 
 * @param map the number of the tile map (0 - 2)
 */
extern void tile_map_update(uint8_t set);

#endif