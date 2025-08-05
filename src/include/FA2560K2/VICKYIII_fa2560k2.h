/*
 * Registers and memory blocks for VICKY III for the A2560K
 */

#ifndef __VICKYIII_FA2560K2_General_H
#define __VICKYIII_FA2560K2_General_H


// NEW FA2560K2 - Graphic Engine
// 16bits Access
// VICKY III - MARK II
// THis is for the bitmap
#define VKYIII_MRKII_MSTR_CTRL 	    ((volatile unsigned short *)0xFFE00000)
#define VKYIIIMRKII_MCR_EN          0x00000001  // Enable the bitmap background 'DDR3 Memory'
#define VKYIIIMRKII_MCR_BM_MODE0    0x00000002  // Bitmap Mode 0
#define VKYIIIMRKII_MCR_BM_MODE1    0x00000004  // Bitmap Mode 1
#define VKYIIIMRKII_MCR_BM_MODE2    0x00000008  // Bitmap Mode 2
// Bitmap Mode [2:0]
// 0.0.0 :  512x768     - 1bpp
// 0.0.1 :  512x768     - 8bpp
// 0.1.0 :  512x768     - 16bpp
// 0.1.1 :  512x768     - 32bpp
// 1.0.0 :  1024x768    - 1bpp
// 1.0.1 :  1024x768    - 8bpp
// 1.1.0 :  1024x768    - 16bpp
// 1.1.1 :  1024x768    - 32bpp
#define VKYIII_MRKII_BM_ADDY 	   ((volatile unsigned long *)0xFFE00004)   // DDR3 Starting Addy
#define VKYIII_MRKII_MONO_CLR 	   ((volatile unsigned long *)0xFFE00008)   // 1bpp - Color for the Pixel

#define VKYIII_MRKII_LUT_0	       ((volatile unsigned int *)0xFFE02000)
// Bit[0]       Enable BM
// Bit[1]       Color Mode 0 
// Bit[2]       Color Mode 0 
// 0 0          - 1 bit per pixel
// 0 1          - 8bits per pixel
// 1 0          - 16bits per pixel
// 1 1          - 32bits per pixel
// Bit[3]       Double Pixel (1024/768) / (512x384)
// 

//		GAVIN CONTROL REGISTERS						$FFB0_0000		$FFB0_1FFF	(8K)
//		Interrupt Control Registers					$FFB0_2000		$FFB0_3FFF	(8K)
//		Timer Control Registers						$FFB0_4000		$FFB0_5FFF	(8K)
//		DMA Control Registers						$FFB0_6000		$FFB0_7FFF	(8K)
//		SDCard 0 Control Registers (SPI)			$FFB0_8000		$FFB0_9FFF	(8K)
//		SDCard 1 Control Registers (SPI)			$FFB0_A000		$FFB0_BFFF	(8K)
//		VIA 0 Control Registers						$FFB0_C000		$FFB0_DFFF	(8K)
//		VIA 1 Control Registers						$FFB0_E000		$FFB0_FFFF	(8K)
//		PS2 Port Control Registers					$FFB1_0000		$FFB1_1FFF	(8K)
//		4N4S Port Control Registers					$FFB1_2000		$FFB1_3FFF	(8K)
//		WS6100 Port Control Registers				$FFB1_4000		$FFB1_5FFF	(8K)
//		WIFI Control & FIFO Port (DMA)				$FFB1_6000		$FFB1_7FFF	(8K)
//		UART16550 Control Registers					$FFB1_8000		$FFB1_9FFF	(8K)
//		Fixed Point Math Block Registers			$FFB1_A000		$FFB1_BFFF	(8K)
//		Float Point Math Block Registers			$FFB1_C000		$FFB1_DFFF	(8K)
//		Optical Keyboard Control Registers			$FFB1_E000		$FFB1_FFFF	(8K)
//		LCD Port Control Registers					$FFB2_0000		$FFB2_1FFF	(8K)
//		Serial Flash Control Registers				$FFB2_2000		$FFB2_3FFF	(8K)
//		MAC Address Control Registers (I2C)			$FFB2_4000		$FFB2_5FFF	(8K)

/*
 * Screen Channel A
 */
// FFD4 is a fake address, there is no more Screen A
#define MasterControlReg_A		    ((volatile unsigned short *)0xFFD4000)
#define VKY3_MCR_TEXT_EN            0x00000001  /* Text Mode Enable */
#define VKY3_MCR_TEXT_OVRLY         0x00000002  /* Text Mode overlay */
#define VKY3_MCR_GRAPH_EN           0x00000004  /* Graphic Mode Enable */
#define VKY3_MCR_BITMAP_EN          0x00000008  /* Bitmap Engine Enable */
#define VKY3_MCR_VIDEO_DISABLE      0x00000080  /* Disable the video engine and VRAM access by Vicky */
#define VKY3_MCR_RESOLUTION_MASK    0x00000300  /* Resolution - 00: 640x480, 01:800x600, 10: 1024x768, 11: 640x400 */
#define VKY3_MCR_640x480            0x00000000
#define VKY3_MCR_800x600            0x00000100
#define VKY3_MCR_1024x768           0x00000200
#define VKY3_MCR_640x400            0x00000300
#define VKY3_MCRA_1024x768          0x00000800
#define VKY3_MCR_DOUBLE_EN          0x00000400  /* Doubling Pixel */
#define VKY3_MCR_GAMMA_EN           0x00010000  /* GAMMA Enable */
#define VKY3_MCR_MANUAL_GAMMA_EN    0x00020000  /* Enable Manual GAMMA Enable */
#define VKY3_MCR_DISABLE_SYNC       0x00040000  /* Turn OFF sync (to monitor in sleep mode) */


/* Access to DIP switch information (read only) */
#define VKY3_DIP_REG                ((volatile unsigned short *)0xFFB00002)
/* Bits 0 - 12: Master Control Register data */
#define VKY3_DIP_GAMMA              0x2000      /* DIP switch indication for Gamma correction */
#define VKY3_DIP_HIRES              0x4000      /* DIP switch for high resolution mode */
#define VKY3_PLL_ACTIVE_CLK         0x8000      /* Active Clock --- 0: 25.175Mhz, 1: 40Mhz */

#define BorderControlReg_L_A	    ((volatile unsigned short *)0xFFD40004)
#define VKY3_BRDR_EN                0x00000001  /* Border Enable */
#define VKY3_X_SCROLL_MASK          0x00000070  /* X Scroll */
#define VKY3_X_SIZE_MASK            0x00003f00  /* X Size */
#define VKY3_Y_SIZE_MASK            0x003f0000  /* Y Size */

// FFD4 is a fake address, there is no more Screen A
#define BorderControlReg_H_A 	    ((volatile unsigned short *)0xFFD40008)
#define BackGroundControlReg_A	    ((volatile unsigned short *)0xFFD4000C)
#define CursorControlReg_L_A	    ((volatile unsigned short *)0xFFD40010)
#define CursorControlReg_H_A	    ((volatile unsigned short *)0xFFD40014)

#define LineInterrupt0_A		    ((volatile unsigned short *)0xFFE00018)
#define LineInterrupt1_A		    ((volatile unsigned short *)0xFFE0001A)
#define LineInterrupt2_A		    ((volatile unsigned short *)0xFFE0001C)
#define LineInterrupt3_A		    ((volatile unsigned short *)0xFFE0001E)

#define FONT_Size_Ctrl_A            ((volatile unsigned short *)0xFFE00020)
#define FONT_Count_Ctrl_A           ((volatile unsigned short *)0xFFE00024)

#define MousePointer_Mem_A		    ((volatile unsigned short *)0xFFE00400)
#define MousePtr_A_CTRL_Reg		    ((volatile unsigned short *)0xFFE00C00)
#define MousePtr_En                 0x0001

#define MousePtr_A_X_Pos		    ((volatile unsigned short *)0xFEC40C02)
#define MousePtr_A_Y_Pos		    ((volatile unsigned short *)0xFEC40C04)
#define MousePtr_A_Mouse0		    ((volatile unsigned short *)0xFEC40C0A)
#define MousePtr_A_Mouse1		    ((volatile unsigned short *)0xFEC40C0C)
#define MousePtr_A_Mouse2		    ((volatile unsigned short *)0xFEC40C0E)

#define ScreenText_A			    ((volatile char *)0xFFD20000)             /* Text matrix */
#define ColorText_A				    ((volatile uint8_t *)0xFFD28000)	      /* Color matrix */
#define FG_CLUT_A 				    ((volatile unsigned short *)0xFFD2C400)    /* Foreground LUT */
#define BG_CLUT_A 				    ((volatile unsigned short *)0xFFD2C440)    /* Background LUT */

/*
 * Screen Channel B
 * LEGACY VICKY II
 */
//$FFD0_0000..$FFD0_007F	- Control Registers
//$FFD0_8000..$FFD0_8FFF	- FONT MEMORY
//$FFD2_0000 - $FFFA_7FFF - Text Memory
//$FFD2_8000 - $FFFA_BFFF - Color Memory 
//$FFD2_C400 - $FFFA_C43F 
//$FFF2_C440 - $FFFA_C47F



#define MasterControlReg_B		    ((volatile unsigned short *)0xFED00000)
#define BorderControlReg_L_B	    ((volatile unsigned short *)0xFED00004)
#define BorderControlReg_H_B 	    ((volatile unsigned short *)0xFEC80008)
#define BackGroundControlReg_B	    ((volatile unsigned short *)0xFED0000C)
#define CursorControlReg_L_B	    ((volatile unsigned short *)0xFED00010)
#define CursorControlReg_H_B	    ((volatile unsigned short *)0xFED00014)

#define LineInterrupt0_B		    ((volatile unsigned short *)0xFED00018)
#define LineInterrupt1_B		    ((volatile unsigned short *)0xFED0001A)
#define LineInterrupt2_B		    ((volatile unsigned short *)0xFED0001C)
#define LineInterrupt3_B		    ((volatile unsigned short *)0xFED0001E)

#define MousePointer_Mem_B		    ((volatile unsigned short *)0xFED00400)
#define MousePtr_B_CTRL_Reg		    ((volatile unsigned short *)0xFED00C00)

#define MousePtr_B_X_Pos		    ((volatile unsigned short *)0xFED00C02)
#define MousePtr_B_Y_Pos		    ((volatile unsigned short *)0xFED00C04)
#define MousePtr_B_Mouse0		    ((volatile unsigned short *)0xFED00C0A)
#define MousePtr_B_Mouse1		    ((volatile unsigned short *)0xFED00C0C)
#define MousePtr_B_Mouse2		    ((volatile unsigned short *)0xFED00C0E)

#define ScreenText_B			    ((volatile char *)0xFFD20000)		    /* Text matrix */
#define ColorText_B				    ((volatile uint8_t *)0xFFD28000)	    /* Color matrix */
#define FG_CLUT_B 				    ((volatile unsigned short *)0xFFD2C400)	/* Foreground LUT */
#define BG_CLUT_B 				    ((volatile unsigned short *)0xFFD2C440)	/* Background LUT */

#define BM0_Control_Reg			    ((volatile unsigned long *)0xFED00100)
#define BM0_Addy_Pointer_Reg	    ((volatile unsigned long *)0xFED00104)

#define Sprite_0_CTRL			    ((volatile unsigned long *)0xFED01000)
#define Sprite_0_ADDY_HI		    ((volatile unsigned long *)0xFED01002)
#define Sprite_0_POS_X	     	    ((volatile unsigned long *)0xFED01004)
#define Sprite_0_POS_Y	   		    ((volatile unsigned long *)0xFED01006)

/*
 * Color lookup tables
 */
// A2560M
// $FC00_0000		$FC00_1FFF	(8K) - Control Registers
// $FC00_2000		$FC00_3FFF	(8K) - 8BPP LUT

#define LUT_0					    ((volatile uint8_t *)0xFFE20000)
#define LUT_1					    ((volatile uint8_t *)0xFFE20400)
#define LUT_2					    ((volatile uint8_t *)0xFFE20800)
#define LUT_3					    ((volatile uint8_t *)0xFFE20C00)
#define LUT_4					    ((volatile uint8_t *)0xFFE21000)
#define LUT_5					    ((volatile uint8_t *)0xFFE21400)
#define LUT_6					    ((volatile uint8_t *)0xFFE21800)
#define LUT_7					    ((volatile uint8_t *)0xFFE21C00)

/*
 * Text mode font memory
 */

#define VICKY_TXT_FONT_A            ((volatile unsigned char *)0xFFD48000)  /* $00C48000..$00C48FFF    - FONT MEMORY Channel A */
#define VICKY_TXT_FONT_B            ((volatile unsigned char *)0xFFD08000)  /* $00C88000..$00C88FFF    - FONT MEMORY Channel B */

/*
 * Location of VRAM
 */

#define VRAM_Bank0				    ((volatile uint8_t *)0x00800000)
#define VRAM_Bank1				    ((volatile uint8_t *)0x00A00000)

#endif
