/*
 * Sound device register definitions for the F256K
 */

#ifndef __SOUND_C256_H
#define __SOUND_C256_H

#include <stdint.h>

#define PSG_PORT        ((volatile __attribute__((far)) uint8_t *)0xf01608) 	/* Control register for the SN76489 */
#define PSG_PORT_L      ((volatile __attribute__((far)) uint8_t *)0xf01600) 	/* Control register for the SN76489 */
#define PSG_PORT_R      ((volatile __attribute__((far)) uint8_t *)0xf01610) 	/* Control register for the SN76489 */

#define OPL3_PORT       ((volatile unsigned char *)0xf01580)  /* Access port for the OPL3 */

#define CODEC           ((volatile __attribute__((far)) uint16_t *)0xf01620)	/* Data register for the CODEC */
#define CODEC_WR_CTRL	((volatile __attribute__((far)) uint8_t *)0xf01622)		/* Data register for the CODEC */

/*
 * Internal SID
 */

#define SID_INT_N_V1_FREQ_LO  	((volatile __attribute__((far)) uint8_t *)0xf01480)
#define SID_INT_L_V1_FREQ_LO  	((volatile __attribute__((far)) uint8_t *)0xf01400)
#define SID_INT_R_V1_FREQ_LO  	((volatile __attribute__((far)) uint8_t *)0xf01500)

#endif
