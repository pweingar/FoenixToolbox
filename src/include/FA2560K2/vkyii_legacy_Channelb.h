/**
 * @file vky_chan_b.h
 *
 * Define register addresses needed for A2560K channel B text driver
 */

#ifndef __VKY_CHAN_B_H
#define __VKY_CHAN_B_H

//FFD00000
/** Master Control Register for Channel B, and its supported bits */
#define VKY3_B_MCR          ((volatile unsigned long *)0xFFD00000)
#define VKY3_B_MCR_TEXT     0x0001  /**< Text mode enable bit */
#define VKY3_B_MCR_TXT_OVR  0x0002  /**< Text overlay enable bit */
#define VKY3_B_MCR_GRAPHICS 0x0004  /**< Graphics mode enable bit */
#define VKY3_B_MCR_BITMAP   0x0008  /**< Bitmap engineg enable bit */
#define VKY3_B_MCR_TILE     0x0010  /**< Tile engine enable bit */
#define VKY3_B_MCR_SPRITE   0x0020  /**< Sprite engine enable bit */
#define VKY3_B_MCR_BLANK    0x0080  /**< Disable display engine enable bit */

#define VKY3_B_MODE0        0x0100  /**< Video Mode Bit 0 */
#define VKY3_B_MODE1        0x0200  /**< Video Mode Bit 1 */
#define VKY3_B_DOUBLE       0x0400  /**< Pixel Double Enable bit */
#define VKY3_B_HIRES        0x0000  /**< DIP switch for hires mode */
#define VKY3_B_PLL          0x0800  /**< Controls dot clock */
#define VKY3_B_MCR_SLEEP    0x0000  /**< Monitor sleep (synch disable) bit */
#define VKY3_B_CLK40        0x0000  /**< Indicate if PLL is 25MHz (0) or 40MHz (1) */

/** Border control register for Channel B */
#define VKY3_B_BCR          ((volatile unsigned long *)0xFFD00004)
#define VKY3_B_BCR_ENABLE   0x00000001  /**< Bit to enable the display of the border */

/** Border color register for Channel B */
#define VKY3_B_BRDCOLOR     ((volatile unsigned long *)0xFFD00008)

/** Cursor Control Register for Channel B */
#define VKY3_B_CCR          ((volatile unsigned long *)0xFFD00010)
#define VKY3_B_CCR_ENABLE   0x00000001  /**< Bit to enable the display of the cursor */
#define VKY3_B_CCR_RATE0    0x00000002  /**< Bit0 to specify the blink rate */
#define VKY3_B_CCR_RATE1    0x00000004  /**< Bit1 to specify the blink rate */

/** Cursor Position Register for Channel B */
#define VKY3_B_CPR          ((volatile unsigned long *)0xFFD00014)

/** Font memory block for Channel B */
/* $FFD08000 */
#define VKY3_B_FONT_MEMORY  ((volatile unsigned char *)0xFFD08000)

/** Text Matrix for Channel B */
//$FFD20000
#define VKY3_B_TEXT_MATRIX  ((volatile unsigned char *)0xFFD20000)

/** Color Matrix for Channel B */
#define VKY3_B_COLOR_MATRIX ((volatile unsigned char *)0xFFD28000)

/* Text Color LUTs for Channel B */
#define VKY3_B_LUT_SIZE     16
#define VKY3_B_TEXT_LUT_FG  ((volatile unsigned int *)0xFFD2C400)  /**< Text foreground color look up table for channel B */
#define VKY3_B_TEXT_LUT_BG  ((volatile unsigned int *)0xFFD2C440)  /**< Text background color look up table for channel B */

#endif
