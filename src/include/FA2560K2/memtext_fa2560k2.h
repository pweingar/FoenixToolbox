/**
 * Memory based text module
 */

#ifndef __memtext__
#define __memtext__

#include <stdint.h>

#include "sys_types.h"

typedef volatile uint16_t *memtext_p;
typedef volatile uint16_t *memcolor_p;

/**
 * Structure to represent the MEMTEXT registers
 */
typedef struct memtext_reg_s {
    union {
        uint32_t control;
        struct {
            uint32_t enable : 1;                // Enable the memtext system
            uint32_t size_8x16 : 1;             // Use 8x8 (0) or 16x16 (1) font
            uint32_t reserved_1 : 5;
            uint32_t cursor_en : 1;             // Enable display of the cursor
            uint32_t cursor_flash : 2;          // Flash rate of cursor
            uint32_t font_8x16 : 1;             // ???
            uint32_t reserved_2 : 17;
        };
    };
        
    union {
        uint32_t cursor_xy;
        struct {
            uint32_t cursor_x : 8;              // Cursor column
            uint32_t cursor_y : 8;              // Cursor row
            uint32_t reserved_3 : 16;
        };
    };

    memtext_p text_addr;                        // Pointer to the character matrix

    memcolor_p color_addr;                      // Pointer to the color matrix

    uint32_t cursor_color;                      // ARGB color for the cursor

    uint8_t reserved_4[12];

    uint8_t cursor_graph[16];                   // 8x8 or 8x16 pixel matrix for cursor
} memtext_reg_t, *memtext_reg_p;

#define MEMTEXT         ((volatile memtext_reg_p)0xffb8000)     // Address of the MEMTEXT registers
#define MEMTEXT_FG      ((volatile t_color4 *)0xffb29000)       // 256 x ARGB colors for the foreground colors
#define MEMTEXT_BG      ((volatile t_color4 *)0xffb29800)       // 256 x ARGB colors for the background colors
#define MEMTEXT_FONT    ((volatile uint8_t *)0xffb2a000)        // Bitmaps for 4 8x8 fonts or 2 8x16 fonts

#endif