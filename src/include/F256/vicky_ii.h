/**
 * @file f256_tinyvicky.h
 * @author Peter Weingartner (pjw@tailrecursive.org)
 * @brief Define the registers for Tiny Vicky on the F256 computers.
 * @version 0.1
 * @date 2023-08-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __f256_tinyvicky__
#define __f256_tinyvicky__

#include "sys_types.h"
#include <stdint.h>

/**
 * @brief Vicky II Border Control Registers
 * 
 */
typedef struct tvky_border_ctrl_s {
    uint8_t control;
    t_color3 color;
    uint8_t size_x;
    uint8_t sizy_y;
} *tvky_border_ctrl_p;

/**
 * @brief Vicky II Cursor Control Registers
 * 
 */
typedef struct tvky_crsr_ctrl_s {
    uint8_t control;
	uint8_t start_address;
    char character;
	uint8_t reserved;
    uint16_t column;
    uint16_t row;
} *tvky_crsr_ctrl_p;

#define VKY_CRSR_ENABLE			0x01
#define VKY_CRSR_FLASH_RATE_1S	0x00
#define VKY_CRSR_FLASH_RATE_12S	0x02
#define VKY_CRSR_FLASH_RATE_14S	0x04
#define VKY_CRSR_FLASH_RATE_15S	0x06
#define VKY_CRSR_FLASH_DIS		0x08

//
// Define the locations of the registers
//

#define tvky_mstr_ctrl ((volatile __attribute__((far)) uint16_t *)0xf01000)
#define VKY_MCR_TEXT            0x0001
#define VKY_MCR_TEXT_OVERLAY    0x0002
#define VKY_MCR_GRAPHICS        0x0004
#define VKY_MCR_BITMAP          0x0008
#define VKY_MCR_TILE            0x0010
#define VKY_MCR_SPRITE          0x0020
#define VKY_MCR_GAMMA           0x0040
#define VKY_MCR_CLK_70			0x0100
#define VKY_MCR_DBL_X			0x0200
#define VKY_MCR_DBL_Y			0x0400
#define VKY_MCR_SLEEP			0x0800
#define VKY_MCR_FONT_OVERLAY	0x1000
#define VKY_MCR_FONT_SET		0x2000

#define VKY_MCR_RES_MASK		0x0700
#define VKY_MCR_RES_640x480   	0x0000
#define VKY_MCR_RES_640x400   	0x0100
#define VKY_MCR_RES_320x240   	0x0600
#define VKY_MCR_RES_320x200   	0x0700

#define tvky_brdr_ctrl ((volatile  tvky_border_ctrl_p)0xf01004)

#define vky_brdr_ctrl ((volatile __attribute__((far)) uint8_t *)0xf01004)
#define vky_brdr_col_blue ((volatile __attribute__((far)) uint8_t *)0xf01005)
#define vky_brdr_col_green ((volatile __attribute__((far)) uint8_t *)0xf01006)
#define vky_brdr_col_red ((volatile __attribute__((far)) uint8_t *)0xf01007)
#define vky_brdr_size_x ((volatile __attribute__((far)) uint8_t *)0xf01008)
#define vky_brdr_size_y ((volatile __attribute__((far)) uint8_t *)0xf01009)


#define tvky_bg_color ((volatile __attribute__((far)) t_color3 *)0xf0100d)
#define tvky_crsr_ctrl ((volatile __attribute__((far)) tvky_crsr_ctrl_p)0xf01010)

//
// Text Color Lookup Tables
//
#define tvky_text_fg_color ((volatile __attribute__((far)) t_color4 *)0xf01800)
#define tvky_text_bg_color ((volatile __attribute__((far)) t_color4 *)0xf01840)

//
// Text Fonts Sets 0 and 1
//
#define tvky_font_set_0 	((volatile __attribute__((far)) uint8_t *)0xf02000)

//
// Graphics Color Lookup Tables
//
#define VKY_GR_CLUT_0		((volatile __attribute__((far)) uint8_t *)0xf03000)
#define VKY_GR_CLUT_1		((volatile __attribute__((far)) uint8_t *)0xf03400)
#define VKY_GR_CLUT_2		((volatile __attribute__((far)) uint8_t *)0xf03800)
#define VKY_GR_CLUT_3		((volatile __attribute__((far)) uint8_t *)0xf03c00)

//
// Text mode text and color matrixes
//
#define tvky_text_matrix    ((volatile __attribute__((far)) char *)0xf04000)
#define tvky_color_matrix   ((volatile __attribute__((far)) char *)0xf06000)

//
// Bitmap graphics registers
//

typedef volatile __attribute__((far24)) uint8_t *p_far24;

#define bm0_control         ((volatile __attribute__((far)) uint8_t *)0xf01100)
#define bm0_address         ((volatile __attribute__((far)) uint8_t *)0xf01101)

#define MousePointer_Mem_A	((volatile __attribute__((far)) uint8_t *)0xf00c00)
#define MousePtr_A_CTRL_Reg	((volatile __attribute__((far)) uint8_t *)0xf016e0)
#define MousePtr_En         0x0001

#define MousePtr_A_X_Pos	((volatile __attribute__((far)) uint16_t *)0xf016e2)
#define MousePtr_A_Y_Pos	((volatile __attribute__((far)) uint16_t *)0xf016e4)
#define MousePtr_A_Mouse0	((volatile __attribute__((far)) uint8_t *)0xf016e6)
#define MousePtr_A_Mouse1	((volatile __attribute__((far)) uint8_t *)0xf016e7)
#define MousePtr_A_Mouse2	((volatile __attribute__((far)) uint8_t *)0xf016e8)

//
// Video RAM
//

#define vram_base           ((volatile p_far24)0)

#endif
