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

// /**
//  * Union representing a memtext character
//  */
// typedef union memtext_char_u {
//     uint16_t raw;
//     struct {
//         uint16_t character:8;           // 8-bit codepoint for the character
//         uint16_t font:2;                // Font to use for the character (4 8x8 fonts, 2 8x16 fonts)
//         uint16_t foreground_lut:1;      // Foreground LUT to use
//         uint16_t background_lut:1;      // Background LUT to use
//         uint16_t inverse:1;             // Render character inverted
//         uint16_t flash:1;               // Render character as flashing
//         uint16_t reserved:2;
//     };
// } memtext_char_t, *memtext_char_p;

// /**
//  * Structure to represent the memtext cursor
//  */
// struct memtext_crsr_s {
//     union {
//         uint8_t raw;
//         struct memtext_crsr_ctrl_s {
//             uint8_t enable:1;           // Enable display of the cursor
//             uint8_t rate:2;             // Rate of flashing (rates???)
//             uint8_t reserved:4;
//             uint8_t flash_disable:1;    // Disable flashing
//         } control;
//     };

//     uint8_t cursor_x;                   // Cursor column position
//     uint8_t cursor_y;                   // Cursor row position
// };

// /**
//  * Structure to represent the memtext hardware registers
//  */
// typedef struct memtext_reg_s {
//     union {
//         uint8_t raw;
//         struct memtext_ctrl_s {
//             uint8_t enable:1;           // Enable memtext display
//             uint8_t size_8x6:1;         // Size of characters (1 = 8x16, 0 = 8x8)
//             uint8_t reserved:6;
//         } control;
//     };

//     struct memtext_crsr_s cursor;       // Cursor registers

//     memtext_p mem_char_ptr;             // Pointer to the character matrix

//     uint8_t reserved_1;

//     uint16_t * mem_color_ptr;            // Pointer to the color matrix

//     uint8_t reserved_2;                 // Skip a bit

//     t_color3 cursor_color;              // Color registers for the cursor

//     uint8_t cursor_graph[16];           // Cursor shape data (first 8 bytes for 8x8, all 16 for 8x16)
// } memtext_reg_t, *memtext_reg_p;


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