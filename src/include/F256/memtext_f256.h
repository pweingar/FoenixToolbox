/**
 * Memory based text module
 */

#ifndef __memtext__
#define __memtext__

#include <stdint.h>

#include "sys_types.h"

typedef volatile __attribute__((far24)) uint16_t *memtext_p;
typedef volatile __attribute__((far24)) uint16_t *memcolor_p;

// It should be there Peter, @ $F0_8000. By the way, the FONT Memory (2x 8x8 with 1x 8x16) and LUT are already pre-loaded with grey scaled values
// $F0_1300 = $D300
// $F0_8000 = IOBANK4 - FG
// $F0_8800 = IOBANK4 - BG
// $F0_9000 = FONT



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
        uint16_t control;
        struct {
            uint16_t enable : 1;
            uint16_t size_8x16 : 1;
            uint16_t rsrv_1 : 6;
            uint16_t cursor_en : 1;
            uint16_t cursor_rate : 2;
            uint16_t cursor_flash : 1;
            uint16_t text_rate : 2;
            uint16_t font_8x16 : 1;
            uint16_t rsrv_2 : 1;
        };
    };
       
    union {
        uint16_t cursor_position;
        struct {
            uint16_t cursor_x : 8;
            uint16_t cursor_y : 8;
        };
    };

    memtext_p text_addr;                        // Pointer to the character matrix

    memcolor_p color_addr;                      // Pointer to the color matrix

    uint32_t cursor_color;                      // ARGB color for the cursor

    uint8_t cursor_graph[16];                   // 8x8 or 8x16 pixel matrix for cursor
} memtext_reg_t, *memtext_reg_p;


#define MEMTEXT         ((volatile memtext_reg_p)0xf01300)
#define MEMTEXT_FG      ((volatile t_color4 *)0xf08000)
#define MEMTEXT_BG      ((volatile t_color4 *)0xf08800)
#define MEMTEXT_FONT    ((volatile uint8_t *)0xf09000)

#define MEMTEXT_MAIN_CTRL   ((volatile uint8_t *)0xf01300)
#define MEMTEXT_MAIN_EN     0x01
#define MEMTEXT_MAIN_8x16   0x02

#define MEMTEXT_CRSR_CTRL   ((volatile uint8_t *)0xf01301)

#define MEMTEXT_CRSR_X      ((volatile uint8_t *)0xf01302)
#define MEMTEXT_CRSR_Y      ((volatile uint8_t *)0xf01303)

#define MEMTEXT_CHAR_PTR    ((volatile memtext_p *)0xf01304)
#define MEMTEXT_COLOR_PTR   ((volatile memcolor_p *)0xf01308)

#endif