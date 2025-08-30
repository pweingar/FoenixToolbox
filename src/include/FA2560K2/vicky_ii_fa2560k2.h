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
 * @brief Vicky II Cursor Control Registers
 * 
 */
typedef struct tvky_crsr_ctrl_s {
	union {
		uint32_t control;
		struct {
			uint32_t enable : 1;
			uint32_t flash_rate : 2;
			uint32_t font_bank : 2;
			uint32_t font_size : 1;
			uint32_t rsrv_1 : 3;
			uint32_t character : 8;
			uint32_t rsrv_2 : 1;
		};
	};

	union {
		uint32_t cursor_xy;
		struct {
			uint32_t column : 16;
			uint32_t row : 16;
		};
	};

	t_color4 color;
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

#define tvky_mstr_ctrl ((volatile __attribute__((far)) uint32_t *)0xffd00000)
#define VKY_MCR_TEXT            0x0001
#define VKY_MCR_TEXT_OVERLAY    0x0002
#define VKY_MCR_GRAPHICS        0x0004
#define VKY_MCR_GAMMA           0x0040
#define VKY_MCR_SLEEP			0x0800
#define VKY_MCR_MEMTEXT			0x4000
#define VKY_MCR_FONT_OVERLAY	0x8000

#define tvky_crsr_ctrl ((volatile __attribute__((far)) tvky_crsr_ctrl_p)0xffd00010)

//
// Text Color Lookup Tables
//
#define tvky_text_fg_color ((volatile __attribute__((far)) t_color4 *)0xffd2c400)
#define tvky_text_bg_color ((volatile __attribute__((far)) t_color4 *)0xffd2c440)

#define VKY3_B_TEXT_LUT_FG ((volatile __attribute__((far)) uint32_t *)0xffd2c400)
#define VKY3_B_TEXT_LUT_BG ((volatile __attribute__((far)) uint32_t *)0xffd2c440)

//
// Text Fonts Sets 0 and 1
//
#define tvky_font_set_0 	((volatile __attribute__((far)) uint8_t *)0xffd08000)

//
// Text mode text and color matrixes
//
#define tvky_text_matrix    ((volatile __attribute__((far)) char *)0xffd20000)
#define tvky_color_matrix   ((volatile __attribute__((far)) char *)0xffd28000)

#endif
