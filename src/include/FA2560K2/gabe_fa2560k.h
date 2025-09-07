/**
 * @file gabe_fa2560k2.h
 *
 * Define miscellaneous GABE registers
 */
#ifndef __GABE_FA2560K2_H
#define __GABE_FA2560K2_H

#include <stdint.h>

// ControlRegisters[0] $FFB0_0000
#define GABE_CTRL_REG		((volatile uint32_t *)0xFFB00000)
#define POWER_ON_LED		0x00000001
#define STATUS_LED0			0x00000002
#define STATUS_LED1			0x00000004
#define CAPSLOCK_LED		0x00000008
#define SDCARD_LED			0x00000010	// Turn the SDCARD LED ON or OFF
#define SDCARD_CTRL         0x00000020	// Switch the Color From SDCard0 to SDCARD1 Colors
#define NETWORK_LED         0x00000040  // Turn the NETWORK LED ON or OFF
#define NETWORK_CTRL        0x00000080  // Switch the Color From Wizfi360 to WS6100 Colors
#define BUZZER_CONTROL		0x00000100
#define MANUAL_RESET		0x00008000		// Make sure the word "DEAD" is written in GABE_RESET_ID

/*
		5'b0_000x: DataOut_GAVIN_CONFIG = { 4'b0000, USER_i[2:0], BOOT_MODE_i[3:0], MACHINE_ID};					// LED Control /S
		5'b0_001x: DataOut_GAVIN_CONFIG = { F_SD_WP_i, F_SD_CD_i, ControlRegisters[0][13:0]};			// DipSw/MID

        USER_i[2:0], BOOT_MODE_i[3:0], MACHINE_ID, F_SD_WP_i, F_SD_CD_i, ControlRegisters[0][13:0]
*/

#define GABE_DIP_REG        ((volatile uint32_t *)0xFFB00000)
#define GABE_ID_MASK        0x001F0000	/* Machine ID*/
#define GABE_DIP_BOOT_MASK  0x01E00000  /* Mask for the boot mode: */
#define GABE_DIP_USER_MASK  0x0E000000  /* Mask for the user switches: */

#define DIP_BOOT_RAM		0x01

// ControlRegisters[1] $FFB0_0004
#define GABE_LFSR_REG0		((volatile uint32_t *)0xFFB00004)
#define GABE_LFSR_STATDATA	((volatile uint32_t *)0xFFB00004)    // Read Only

// ControlRegisters[2] $FFB0_0008
#define GABE_SUBVER_ID	    ((volatile uint32_t *)0xFFB00008)    // Subversion[31:16], Machine ID[3:2] - Read Only
#define GABE_CPU_ID_MASK    0x0000F000
#define GABE_CPU_SPD_MASK   0x000000F0

// ControlRegisters[3] $FFB0_000C
#define GABE_CHIP_ID	    ((volatile uint32_t *)0xFFB0000C) 
#define GABE_CHIP_N_MASK    0xFFFFFFFF

// ControlRegisters[4] $FFB0_0010
#define GABE_CHIP_VERSION   ((volatile uint32_t *)0xFFB00010)    // Number[31:16], Version[15:0]
#define GABE_CHIP_SV_MASK   0xFFFF0000
#define GABE_CHIP_V_MASK    0x0000FFFF

// ControlRegisters[5] $FFB0_0014
#define GABE_FIRMWARE_DATE  ((volatile unsigned int *)0xFFB00014)    // xxDDMMYY
#define GABE_FIRMWARE_DATE_YEAR_MASK  0x000000FF
#define GABE_FIRMWARE_DATE_MONTH_MASK 0x0000FF00
#define GABE_FIRMWARE_DATE_DAY_MASK   0x00FF0000

// ControlRegisters[6] $FFB0_0018
#define GABE_HARDWARE_REV  	((volatile uint32_t *)0xFFB00018)    // XX,YY,ZZ,00

// ControlRegisters[7] $FFB0_001C
#define GABE_HARDWARE_DATE  ((volatile uint32_t *)0xFFB0001C)    // 00, DY, MT, YR

// Write ONLY - RGB Values
#define GABE_FA2560K2_RGB0  ((volatile uint32_t *)0xFFB00008)    // POWER ON - Value (LED0)
#define GABE_FA2560K2_RGB1  ((volatile uint32_t *)0xFFB0000C)    // SDC0 (SAME LED1)
#define GABE_FA2560K2_RGB2  ((volatile uint32_t *)0xFFB00010)    // SDC1 (SAME LED1)
#define GABE_FA2560K2_RGB3  ((volatile uint32_t *)0xFFB00014)    // NETWORK0 (SAME LED2) Wizfi360
#define GABE_FA2560K2_RGB4  ((volatile uint32_t *)0xFFB00018)    // NETWORK1 (SAME LED2) WS6100
#define GABE_FA2560K2_RGB5	((volatile uint32_t *)0xFFB0001C)    // CAPSLOCK (LED3)


#endif
