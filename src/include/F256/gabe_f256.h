/**
 * @file gabe.h
 * @brief Definitions for the GABE chip
 * @version 0.1
 * @date 2023-08-30
 * 
 */

#ifndef __GABE_H_
#define __GABE_H_

#include <stdint.h>
#include "sys_types.h"

#define GABE_MSTR_CTRL			((volatile __attribute__((far)) uint8_t *)0xf016a0)
#define GABE_CTRL_PWR_LED		0x01     	// Controls the LED in the Front of the case (Next to the reset button)
#define GABE_CTRL_SDC_LED		0x02     	// Controls the LED in the Front of the Case (Next to SDCard)
#define GABE_CTRL_STS_LED0		0x04     	// Control Status LED0 (General Use) - C256 Foenix U Only
#define GABE_CTRL_STS_LED1		0x08     	// Control Status LED0 (General Use) - C256 Foenix U Only
#define GABE_CTRL_BUZZER		0x10     	// Controls the Buzzer
#define GABE_CAP_EN				0x20		// SD card detect
#define GABE_SD_CD				0x20		// SD card detect
#define GABE_SD_WP				0x40		// SD card write protect
#define GABE_CTRL_WRM_RST		0x80    	// Warm Reset (needs to Setup other registers)

#define GABE_LED_FLASH_CTRL		((volatile __attribute__((far)) uint8_t *)0xf016a1)
#define GABE_LED0_FLASH_CTRL	0x01
#define GABE_LED1_FLASH_CTRL	0x02
#define GABE_CTRL_LED0_RATE		0x30
#define GABE_CTRL_LED1_RATE		0xc0

#define GABE_LD0_FLASH_FRQ_MASK	0x30
#define GABE_LD0_FLASH_FRQ_1HZ	0x00		// LD0 flashes at 1Hz
#define GABE_LD0_FLASH_FRQ_2HZ	0x10		// LD0 flashes at 2Hz
#define GABE_LD0_FLASH_FRQ_4HZ	0x20		// LD0 flashes at 4Hz
#define GABE_LD0_FLASH_FRQ_5HZ	0x30		// LD0 flashes at 5Hz

#define GABE_LD1_FLASH_FRQ_MASK	0xc0
#define GABE_LD1_FLASH_FRQ_1HZ	0x00		// LD1 flashes at 2Hz
#define GABE_LD1_FLASH_FRQ_2HZ	0x40		// LD1 flashes at 4Hz
#define GABE_LD1_FLASH_FRQ_4HZ	0x80		// LD1 flashes at 5Hz
#define GABE_LD1_FLASH_FRQ_5HZ	0xc0		// LD1 flashes at 1Hz

#define GABE_RST_AUTH			((volatile __attribute__((far)) uint16_t *)0xf016a2)	// Set to 0xDEAD to enable reset

#define GABE_RNG_DATA			((volatile __attribute__((far)) uint16_t *)0xf016a4)	// Random Number Generator (read for data, write for seed)
#define GABE_RNG_SEED			((volatile __attribute__((far)) uint16_t *)0xf016a4)	// Random Number Generator (read for data, write for seed)
#define GABE_RNG_STAT			((volatile __attribute__((far)) uint16_t *)0xf016a6)	// Random Number Generator Status (read)
#define GABE_RNG_LFSR_DONE		0x80
#define GABE_RNG_CTRL			((volatile __attribute__((far)) uint16_t *)0xf016a6)	// Random Number Generator Control (write)
#define GABE_RNG_CTRL_EN		0x01													// Enable the LFSR block
#define GABE_RNG_CTRL_LD_SEED	0x02													// Toggle after setting seed to load seed

#define GABE_DIP_REG			((volatile __attribute__((far)) uint8_t *)0xf01670)		// User and boot mode DIP switches
#define DIP_GAMMA_EN			0x80
#define DIP_640_400				0x40
#define DIP_BOOT_RAM			0x01

/**
 * @brief Structure to represent the machine ID and expansion card info
 * 
 */
union gabe_sys_stat_u {
	struct {
		uint8_t machine_id:5;
	};
	uint8_t reg;
};

#define GABE_SYS_STAT			((volatile __attribute__((far)) union gabe_sys_stat_u *)0xf016a7)

/**
 * @brief Structure to respresent the version of the GABE chip
 * 
 */
struct gabe_version_s {
	uint16_t subversion;
	uint16_t version;
	uint16_t model;
};

#define GABE_VERSION			((volatile __attribute__((far)) struct gabe_version_s *)0xf016aa)

struct pcb_revision_s {
	char major;
	char minor;
	uint8_t day;
	uint8_t month;
	uint8_t year;
};

#define PCB_ID_LABEL			((volatile __attribute__((far)) char *)0xf016a8)
#define PCB_VERSION				((volatile __attribute__((far)) struct pcb_revision_s *)0xf016aa)

#endif

