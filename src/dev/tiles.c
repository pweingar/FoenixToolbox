
#include <string.h>

#include "tiles.h"
#include "F256/vicky_ii.h"

static t_tile_set tile_set_shadow[VKY_TILESET_MAX];
static t_tile_map tile_map_shadow[VKY_TILEMAP_MAX];

const uint32_t tile_set_memory_base = 0x002000;
const uint32_t tile_map_memory_base = 0x003000;

/**
 * @brief Setup a tile set
 * 
 * @param set the number of the tile set (0 - 7)
 * @param address pointer to the bitmap for the tile set
 * @param size the number of bytes in the tile set
 * @param is_square if true, layout of image is square, if false it is 8 or 16 pixels wide
 */
void tile_set_assign(uint8_t set, uint8_t * address, int size, bool is_square) {
	if (set <= VKY_TILESET_MAX) {

		uint8_t * tile_set_ram = (uint8_t *)tile_set_memory_base;
		for (int i = 0; i < size; i++) {
			tile_set_ram[i] = address[i];
		}

		tile_set_shadow[set].address = (p_far24)tile_set_ram;
		if (is_square) {
			tile_set_shadow[set].control = VKY_TILESET_SQUARE;
		} else {
			tile_set_shadow[set].control = 0;
		}
	}
}

/**
 * @brief Update the hardware tile set from its shadow
 * 
 * @param set the number of the tile set (0 - 7)
 */
void tile_set_update(uint8_t set) {
	vky_tile_sets[set].address = tile_set_shadow[set].address;
	vky_tile_sets[set].control = tile_set_shadow[set].control;
}

/**
 * @brief Initialize the tiles
 * 
 */
void tile_init() {
	memset(tile_set_shadow, 0, VKY_TILESET_MAX * sizeof(t_tile_set));
	memset(tile_map_shadow, 0, VKY_TILEMAP_MAX * sizeof(t_tile_map));
}

/**
 * @brief Setup a tile map
 * 
 * @param map the number of the tile map (0 - 2)
 * @param address pointer to the actual tile map data
 * @param width number of columns in the  map
 * @param height number of rows in the map
 * @param size if 0, tiles are 16x16... if 1, tiles are 8x8
 */
void tile_map_assign(uint8_t map, uint16_t * address, uint8_t width, uint8_t height, uint8_t size) {
	if (map <= VKY_TILEMAP_MAX) {

		uint16_t * tile_map_ram = (uint8_t *)tile_map_memory_base;
		for (int i = 0; i < width*height; i++) {
			tile_map_ram[i] = address[i];
		}

		tile_map_shadow[map].address = (p_far24)tile_map_ram;
		tile_map_shadow[map].width = width;
		tile_map_shadow[map].height = height;
		if (size == 0) {
			tile_map_shadow[map].control &= ~VKY_TILEMAP_SIZE;
		} else {
			tile_map_shadow[map].control |= VKY_TILEMAP_SIZE;
		}
	}
}

/**
 * @brief Set the scoll position of the tile map
 * 
 * @param map the number of the tile map (0 - 2)
 * @param x the horizontal scroll position
 * @param y the vertical scroll position
 */
void tile_map_position(uint8_t map, uint16_t x, uint16_t y) {
	if (map <= VKY_TILEMAP_MAX) {
		tile_map_shadow[map].x = x;
		tile_map_shadow[map].y = y;
	}
}

/**
 * @brief Enable or disable the tile map
 * 
 * @param map the number of the tile map (0 - 2)
 * @param is_enabled if true, the map should be displayed, if false it should not
 */
void tile_map_enable(uint8_t map, bool is_enabled) {
	if (map <= VKY_TILEMAP_MAX) {
		if (is_enabled) {
			tile_map_shadow[map].control |= VKY_TILEMAP_ENABLE;
		} else {
			tile_map_shadow[map].control &= ~VKY_TILEMAP_ENABLE;
		}
	}
}

/**
 * @brief Update the hardware tile map from its shadow
 * 
 * @param map the number of the tile map (0 - 2)
 */
void tile_map_update(uint8_t map) {
	vky_tile_maps[map].address = tile_map_shadow[map].address;
	vky_tile_maps[map].width = tile_map_shadow[map].width;
	vky_tile_maps[map].height = tile_map_shadow[map].height;
	vky_tile_maps[map].x = tile_map_shadow[map].x;
	vky_tile_maps[map].y = tile_map_shadow[map].y;
	vky_tile_maps[map].control = tile_map_shadow[map].control;
}