/*
 * Definitions for access to the SDC controller using the SPI interface
 */

#ifndef __SDC_A2560ME_H
#define __SDC_A2560ME_H

#include <stdint.h>

typedef struct s_sdc_spi {
	uint8_t ctrl;
	uint8_t data;
} t_sdc_spi, *p_sdc_spi;

#define SDx_CS				0x01		// 1 = Enable
#define SDx_SLOW 			0x02		// 1 = Slow 400Khz, 0 = 25Mhz
#define SDx_BUSY			0x80		// 1 = Busy

#define SD0_REG				((volatile __attribute__((far)) p_sdc_spi)0xFEC00300)
#define SD1_REG				((volatile __attribute__((far)) p_sdc_spi)0xFEC00340)
#define SD2_REG				((volatile __attribute__((far)) p_sdc_spi)0xFEC00380)

#define SD0_STAT            (*(volatile __far uint16_t *)0xfec00000)
#define SD0_STAT_CD         0x1000  // When 1 = No Card, 0 = Card is Present
#define SD0_STAT_WP         0x2000  // When 0 = Writeable, 1 = Card is Protected
#define SD1_STAT_CD         0x4000  // When 1 = No Card, 0 = Card is Present
#define SD1_STAT_WP         0x8000  // When 0 = Writeable, 1 = Card is Protected

#endif
