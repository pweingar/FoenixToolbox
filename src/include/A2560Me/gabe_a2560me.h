/**
 * @file gabe_a2560me.h
 *
 * Define miscellaneous GABE registers
 */

/*
		ControlRegisters[4] <= 32'h00008000;		// Power RGB Value
		ControlRegisters[5] <= 32'h00800080;		// SDC0 (Purple)
		ControlRegisters[6] <= 32'h00000080;		// SDC1 (Blue)
		ControlRegisters[7] <= 32'h00808000;		// Network (Yellow) I think
*/

#ifndef __GABE_A2560M_H
#define __GABE_A2560M_H

#include <stdint.h>

#define GABE_CTRL_REG		((volatile uint32_t *)0xFEC00000)
#define POWER_ON_LED		0x00000001
#define SDC0_LED			0x00000002
#define SDC1_LED            0x00000004
#define MSATA_LED           0x00000008
#define NETWORK_LED         0x00000010
#define BUZZER_CONTROL		0x00000020
#define MANUAL_RESET		0x00008000		// Make sure the word "DEAD" is written in GABE_RESET_ID

#define GABE_LFSR_REG0		((volatile uint32_t *)0xFEC00004)

#define RGB_LED			    ((volatile uint32_t *)0xFEC00008)    // Writing Only - A2560K Only - 0x__RRGGBB
#define GABE_LFSR_STATDATA	((volatile uint32_t *)0xFEC00008)    // Read Only

#define GABE_SUBVER_ID	    ((volatile uint32_t *)0xFEC0000C)    // Subversion[31:16], Machine ID[3:2] - Read Only
#define GABE_CPU_ID_MASK    0x0000FF00
#define GABE_CPU_SPD_MASK   0x000000E0
#define GABE_ID_MASK        0x0000000C
#define GABE_CHIP_SV_MASK   0xFFFF0000

#define GABE_CHIP_VERSION   ((volatile uint32_t *)0xFEC00010)    // Number[31:16], Version[15:0]
#define GABE_CHIP_V_MASK    0x0000FFFF
#define GABE_CHIP_N_MASK    0xFFFF0000

#define GABE_FIRMWARE_DATE  ((volatile uint32_t *)0xFEC00014)    // xxDDMMYY
#define GABE_FIRMWARE_DATE_YEAR_MASK  0x000000FF
#define GABE_FIRMWARE_DATE_MONTH_MASK 0x0000FF00
#define GABE_FIRMWARE_DATE_DAY_MASK   0x00FF0000

#define GABE_DIP_REG        ((volatile uint32_t *)0xFEC00518)
#define GABE_DIP_BOOT_MASK  0x00000003  /* Mask for the boot mode: */
#define GABE_DIP_USER_MASK  0x00000700  /* Mask for the user switches: */

#define GABE_A2560M_PWR_RGB ((volatile uint32_t *)0xFEC00010)    // POWER ON - Value
#define GABE_A2560M_SD0_RGB ((volatile uint32_t *)0xFEC00014)    // SDC0
#define GABE_A2560M_SD1_RGB ((volatile uint32_t *)0xFEC00018)    // SDC1
#define GABE_A2560M_HDC_RGB ((volatile uint32_t *)0xFEC0001C)    // MSATA
#define GABE_A2560M_NET_RGB ((volatile uint32_t *)0xFEC00020)    // Network

#endif
