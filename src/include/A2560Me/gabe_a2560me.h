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
#define GABE_DIP_REG        ((volatile uint32_t *)0xFEC00000)

// Bit assignments on write:
#define POWER_ON_LED		0x00000001
#define SDC0_LED			0x00000002
#define SDC1_LED			0x00000004
#define MSATA_LED			0x00000008
#define NETWORK_LED			0x00000010
#define BUZZER_CONTROL		0x00000020
#define DDR_ENDIAN_CTRL     0x00000040      // CPU -> DDR3 Endianness Inversion (When dumping Data for Graphics or else) -> 0: Big Endian, 1: Little Endian (Default is set to 1)
#define SPEAKER_CONTROL     0x00000080      // Speaker Control (ATX Header Speaker Control) Has its own oscillator Now.
#define MANUAL_RESET		0x00008000		// Make sure the word "DEAD" is written in bits[31..24]

// Bit assignments on read
#define GABE_PCI_1X_DETECT	0x00020000		// PCI Express Card 1x Present
#define GABE_PCI_4X_DETECT	0x00040000		// PCI Express Card 4x Present
#define GABE_PCI_8X_DETECT	0x00080000		// PCI Express Card 8x Present
#define GABE_DIP_BOOT_MASK  0x00f00000      // DIP Boot Switch Positions (4 bits - read only)
#define GABE_DIP_USER_MASK  0x07000000      // DIP User Switch Positions (3 bits - read only)
#define SD0_CD              0x10000000      // SD0 (front) Card Detect (read only)
#define SD0_WP              0x20000000      // SD0 (front) Write-protect (read only)
#define SD1_CD              0x40000000      // SD1 (back) Card Detect (read only)
#define SD1_WP              0x80000000      // SD1 (back) Write-protect (read only)

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

// #define GABE_DIP_REG        ((volatile uint32_t *)0xFEC00518)
// #define GABE_DIP_BOOT_MASK  0x00000003  /* Mask for the boot mode: */
// #define GABE_DIP_USER_MASK  0x00000700  /* Mask for the user switches: */

#define GABE_A2560M_PWR_RGB ((volatile uint32_t *)0xFEC00020)    // POWER ON - Value
#define GABE_A2560M_SD0_RGB ((volatile uint32_t *)0xFEC00024)    // SDC0
#define GABE_A2560M_SD1_RGB ((volatile uint32_t *)0xFEC00028)    // SDC1
#define GABE_A2560M_HDC_RGB ((volatile uint32_t *)0xFEC0002C)    // MSATA
#define GABE_A2560M_NET_RGB ((volatile uint32_t *)0xFEC00030)    // Network

// Counter values for PCB buzzer and PC case speaker
// Counter Value x 2 (Down counter with a CPU Local Bus Frequency of 50.250Mhz)
// Concert A (440 Hz) should be approximately 56818
#define GABE_SOUND_COUNT	((volatile uint32_t *)0xFEC00034)
#define GABE_BUZZ_CNT_MASK	0x0000ffff
#define GABE_SPKR_CNT_MASK	0xffff0000

#endif
