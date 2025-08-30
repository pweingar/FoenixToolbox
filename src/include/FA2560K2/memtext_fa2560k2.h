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
 * MEMTEXT character format
 */
typedef struct memtext_char_s {
    uint16_t raw;
    union {
        uint16_t character : 8;
        uint16_t font : 2;
        uint16_t foreground_lut : 1;
        uint16_t background_lut : 1;
        uint16_t invert : 1;
        uint16_t flash : 1;
        uint16_t rsrv : 2;
    };
} memtext_char_t, *memtext_char_p;

/**
 * MEMTEXT color format
 */
typedef struct memtext_color_s {
    uint16_t raw;
    union {
        uint16_t background : 8;
        uint16_t foreground : 8;
    };
} memtext_color_t, *memtext_color_p;

/**
 * Structure to represent the MEMTEXT registers
 */
typedef struct memtext_reg_s {
    union {
        uint32_t control;
        struct {
            uint32_t enable : 1;
            uint32_t size_8x16 : 1;
            uint32_t rsrv_1 : 6;
            uint32_t cursor_en : 1;
            uint32_t cursor_rate : 2;
            uint32_t cursor_flash : 1;
            uint32_t text_rate : 2;
            uint32_t font_8x16 : 1;
            uint32_t rsrv_2 : 17;
        };
    };
       
    union {
        uint32_t cursor_position;
        struct {
            uint32_t cursor_x : 8;
            uint32_t cursor_y : 8;
            uint32_t rsrv_3 : 16;
        };
    };

    memtext_p text_addr;                        // Pointer to the character matrix

    memcolor_p color_addr;                      // Pointer to the color matrix

    uint32_t cursor_color;                      // ARGB color for the cursor

    uint8_t rsrv_4[12];

    uint8_t cursor_graph[16];                   // 8x8 or 8x16 pixel matrix for cursor
} memtext_reg_t, *memtext_reg_p;

#define MEMTEXT             ((volatile memtext_reg_p)0xffb28000)    // Address of the MEMTEXT register

#define MEMTEXT_CTRL        ((volatile uint32_t *)0xffb28000)
#define MEMTEXT_MAIN_EN     0x00000001
#define MEMTEXT_MAIN_8x16   0x00000002
#define MEMTEXT_CRSR_EN     0x00000100
#define MEMTEXT_CRSR_RATE   0x00000600
#define MEMTEXT_CRSR_FLASH  0x00000800
#define MEMTEXT_TEXT_RATE   0x00003000
#define MEMTEXT_TEXT_SIZE   0x00004000

#define MEMTEXT_CRSR_XY     ((volatile uint32_t *)0xffb28004)
#define MEMTEXT_TEXT_ADDR   ((volatile uint32_t **)0xffb28008)
#define MEMTEXT_COLOR_ADDR  ((volatile uint32_t **)0xffb2800c)
#define MEMTEXT_CRSR_COLOR  ((volatile p_color4)0xffb28010)

#define MEMTEXT_FG_0        ((volatile uint32_t *)0xffb29000)       // 256 x ARGB colors for the foreground colors
#define MEMTEXT_BG_0        ((volatile uint32_t *)0xffb29800)       // 256 x ARGB colors for the background colors
#define MEMTEXT_FG_1        ((volatile uint32_t *)0xffb29400)       // 256 x ARGB colors for the foreground colors
#define MEMTEXT_BG_1        ((volatile uint32_t *)0xffb29c00)       // 256 x ARGB colors for the background colors

#define MEMTEXT_FONT        ((volatile uint8_t *)0xffb2a000)        // Bitmaps for 4 8x8 fonts or 2 8x16 fonts

#endif