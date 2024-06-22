/**
 * @file dma_f256.h
 * @brief Definitions of the video and system DMA registers
 * @version 0.1
 * @date 2023-10-02
 * 
 */

#ifndef __DMA_F256_H_
#define __DMA_F256_H_

//
// Video RAM DMA Registers
//

#define DMA_CTRL			((volatile __attribute__((far)) uint8_t *)0xf01f00)
#define DMA_CTRL_EN			0x01
#define DMA_CTRL_2D			0x02
#define DMA_CTRL_FILL		0x04
#define DMA_CTRL_IEN		0x08	// Interrupt Enable
#define DMA_CTRL_TRF		0x80

#define DMA_STAT			((volatile __attribute__((far)) uint8_t *)0xf01f01)
#define DMA_STAT_TFR_BUSY	0x80

#define DMA_FILL_VALUE		((volatile __attribute__((far)) uint8_t *)0xf01f01)
#define DMA_SRC_ADDR		((volatile __attribute__((far)) uint8_t *)0xf01f04)
#define DMA_DST_ADDR		((volatile __attribute__((far)) uint8_t *)0xf01f08)
#define DMA_SIZE			((volatile __attribute__((far)) uint8_t *)0xf01f0c)
#define DMA_SIZE_X			((volatile __attribute__((far)) uint16_t *)0xf01f0c)
#define DMA_SIZE_Y			((volatile __attribute__((far)) uint16_t *)0xf01f0e)
#define DMA_SRC_STRIDE		((volatile __attribute__((far)) uint16_t *)0xf01f10)
#define DMA_DST_STRIDE		((volatile __attribute__((far)) uint16_t *)0xf01f12)

#endif
