/*
 * Sound device register definitions for the F256K
 */

#ifndef __SOUND_F256_H
#define __SOUND_F256_H

#include <stdint.h>

#define PSG_PORT        ((volatile __attribute__((far)) uint8_t *)0xffc02000) 	/* Control register for the SN76489 */
#define PSG_PORT_L      ((volatile __attribute__((far)) uint8_t *)0xffc02010) 	/* Control register for the SN76489 */
#define PSG_PORT_R      ((volatile __attribute__((far)) uint8_t *)0xffc02020) 	/* Control register for the SN76489 */

#define OPL3_PORT       ((volatile uint8_t *)0xffc06000)  						/* Access port for the OPL3 */

#define CODEC           ((volatile __attribute__((far)) uint16_t *)0xffc00200)	/* Data register for the CODEC */
#define CODEC_WR_CTRL	((volatile __attribute__((far)) uint8_t *)0xffc00202)	/* Data register for the CODEC */

/*
 * Internal SID
 */

struct s_sid_voice {
	uint16_t frequency;
	uint16_t pulse_width;
	uint8_t control;
	uint8_t attack_decay;
	uint8_t sustain_release;
};

struct s_sid {
	struct s_sid_voice v1;
	struct s_sid_voice v2;
	struct s_sid_voice v3;

	uint16_t filter_frequency;
	uint8_t resonance_filter;
	uint8_t mode_volume;
	
	uint8_t pot_x;
	uint8_t pot_y;
	uint8_t osc3;
	uint8_t env3;
};

/*
 * Internal SID Left Channel
 */

#define SID_INT_L_V1_FREQ_LO  	((uint8_t *)0xFFC04000)
#define SID_INT_L_V1_FREQ_HI  	((uint8_t *)0xFFC04001)
#define SID_INT_L_V1_PW_LO		((uint8_t *)0xFFC04002)
#define SID_INT_L_V1_PW_HI		((uint8_t *)0xFFC04003)
#define SID_INT_L_V1_CTRL		((uint8_t *)0xFFC04004)
#define SID_INT_L_V1_ATCK_DECY  ((uint8_t *)0xFFC04005)
#define SID_INT_L_V1_SSTN_RLSE  ((uint8_t *)0xFFC04006)
#define SID_INT_L_V2_FREQ_LO    ((uint8_t *)0xFFC04007)
#define SID_INT_L_V2_FREQ_HI    ((uint8_t *)0xFFC04008)
#define SID_INT_L_V2_PW_LO      ((uint8_t *)0xFFC04009)
#define SID_INT_L_V2_PW_HI		((uint8_t *)0xFFC0400A)
#define SID_INT_L_V2_CTRL       ((uint8_t *)0xFFC0400B)
#define SID_INT_L_V2_ATCK_DECY  ((uint8_t *)0xFFC0400C)
#define SID_INT_L_V2_SSTN_RLSE  ((uint8_t *)0xFFC0400D)
#define SID_INT_L_V3_FREQ_LO    ((uint8_t *)0xFFC0400E)
#define SID_INT_L_V3_FREQ_HI    ((uint8_t *)0xFFC0400F)
#define SID_INT_L_V3_PW_LO      ((uint8_t *)0xFFC04010)
#define SID_INT_L_V3_PW_HI		((uint8_t *)0xFFC04011)
#define SID_INT_L_V3_CTRL       ((uint8_t *)0xFFC04012)
#define SID_INT_L_V3_ATCK_DECY  ((uint8_t *)0xFFC04013)
#define SID_INT_L_V3_SSTN_RLSE  ((uint8_t *)0xFFC04014)
#define SID_INT_L_FC_LO			((uint8_t *)0xFFC04015)
#define SID_INT_L_FC_HI         ((uint8_t *)0xFFC04016)
#define SID_INT_L_RES_FILT      ((uint8_t *)0xFFC04017)
#define SID_INT_L_MODE_VOL      ((uint8_t *)0xFFC04018)
#define SID_INT_L_POT_X         ((uint8_t *)0xFFC04019)
#define SID_INT_L_POT_Y         ((uint8_t *)0xFFC0401A)
#define SID_INT_L_OSC3_RND      ((uint8_t *)0xFFC0401B)
#define SID_INT_L_ENV3          ((uint8_t *)0xFFC0401C)
#define SID_INT_L_NOT_USED0     ((uint8_t *)0xFFC0401D)
#define SID_INT_L_NOT_USED1     ((uint8_t *)0xFFC0401E)
#define SID_INT_L_NOT_USED2     ((uint8_t *)0xFFC0401F)

/*
 * Internal SID Right Channel
 */

#define SID_INT_R_V1_FREQ_LO  	((uint8_t *)0xFFC04100)
#define SID_INT_R_V1_FREQ_HI  	((uint8_t *)0xFFC04101)
#define SID_INT_R_V1_PW_LO		((uint8_t *)0xFFC04102)
#define SID_INT_R_V1_PW_HI		((uint8_t *)0xFFC04103)
#define SID_INT_R_V1_CTRL		((uint8_t *)0xFFC04104)
#define SID_INT_R_V1_ATCK_DECY  ((uint8_t *)0xFFC04105)
#define SID_INT_R_V1_SSTN_RLSE  ((uint8_t *)0xFFC04106)
#define SID_INT_R_V2_FREQ_LO    ((uint8_t *)0xFFC04107)
#define SID_INT_R_V2_FREQ_HI    ((uint8_t *)0xFFC04108)
#define SID_INT_R_V2_PW_LO      ((uint8_t *)0xFFC04109)
#define SID_INT_R_V2_PW_HI		((uint8_t *)0xFFC0410A)
#define SID_INT_R_V2_CTRL       ((uint8_t *)0xFFC0410B)
#define SID_INT_R_V2_ATCK_DECY  ((uint8_t *)0xFFC0410C)
#define SID_INT_R_V2_SSTN_RLSE  ((uint8_t *)0xFFC0410D)
#define SID_INT_R_V3_FREQ_LO    ((uint8_t *)0xFFC0410E)
#define SID_INT_R_V3_FREQ_HI    ((uint8_t *)0xFFC0410F)
#define SID_INT_R_V3_PW_LO      ((uint8_t *)0xFFC04110)
#define SID_INT_R_V3_PW_HI		((uint8_t *)0xFFC04111)
#define SID_INT_R_V3_CTRL       ((uint8_t *)0xFFC04112)
#define SID_INT_R_V3_ATCK_DECY  ((uint8_t *)0xFFC04113)
#define SID_INT_R_V3_SSTN_RLSE  ((uint8_t *)0xFFC04114)
#define SID_INT_R_FC_LO			((uint8_t *)0xFFC04115)
#define SID_INT_R_FC_HI         ((uint8_t *)0xFFC04116)
#define SID_INT_R_RES_FILT      ((uint8_t *)0xFFC04117)
#define SID_INT_R_MODE_VOL      ((uint8_t *)0xFFC04118)
#define SID_INT_R_POT_X         ((uint8_t *)0xFFC04119)
#define SID_INT_R_POT_Y         ((uint8_t *)0xFFC0411A)
#define SID_INT_R_OSC3_RND      ((uint8_t *)0xFFC0411B)
#define SID_INT_R_ENV3          ((uint8_t *)0xFFC0411C)
#define SID_INT_R_NOT_USED0     ((uint8_t *)0xFFC0411D)
#define SID_INT_R_NOT_USED1     ((uint8_t *)0xFFC0411E)
#define SID_INT_R_NOT_USED2     ((uint8_t *)0xFFC0411F)

/*
 * Internal SID Neutral Channel - When writting here, the value is written in R and L Channel at the same time
 */

#define SID_INT_N_V1_FREQ_LO  	((uint8_t *)0xFFC04200)
#define SID_INT_N_V1_FREQ_HI  	((uint8_t *)0xFFC04201)
#define SID_INT_N_V1_PW_LO		((uint8_t *)0xFFC04202)
#define SID_INT_N_V1_PW_HI		((uint8_t *)0xFFC04203)
#define SID_INT_N_V1_CTRL		((uint8_t *)0xFFC04204)
#define SID_INT_N_V1_ATCK_DECY  ((uint8_t *)0xFFC04205)
#define SID_INT_N_V1_SSTN_RLSE  ((uint8_t *)0xFFC04206)
#define SID_INT_N_V2_FREQ_LO    ((uint8_t *)0xFFC04207)
#define SID_INT_N_V2_FREQ_HI    ((uint8_t *)0xFFC04208)
#define SID_INT_N_V2_PW_LO      ((uint8_t *)0xFFC04209)
#define SID_INT_N_V2_PW_HI		((uint8_t *)0xFFC0420A)
#define SID_INT_N_V2_CTRL       ((uint8_t *)0xFFC0420B)
#define SID_INT_N_V2_ATCK_DECY  ((uint8_t *)0xFFC0420C)
#define SID_INT_N_V2_SSTN_RLSE  ((uint8_t *)0xFFC0420D)
#define SID_INT_N_V3_FREQ_LO    ((uint8_t *)0xFFC0420E)
#define SID_INT_N_V3_FREQ_HI    ((uint8_t *)0xFFC0420F)
#define SID_INT_N_V3_PW_LO      ((uint8_t *)0xFFC04210)
#define SID_INT_N_V3_PW_HI		((uint8_t *)0xFFC04211)
#define SID_INT_N_V3_CTRL       ((uint8_t *)0xFFC04212)
#define SID_INT_N_V3_ATCK_DECY  ((uint8_t *)0xFFC04213)
#define SID_INT_N_V3_SSTN_RLSE  ((uint8_t *)0xFFC04214)
#define SID_INT_N_FC_LO			((uint8_t *)0xFFC04215)
#define SID_INT_N_FC_HI         ((uint8_t *)0xFFC04216)
#define SID_INT_N_RES_FILT      ((uint8_t *)0xFFC04217)
#define SID_INT_N_MODE_VOL      ((uint8_t *)0xFFC04218)
#define SID_INT_N_POT_X         ((uint8_t *)0xFFC04219)
#define SID_INT_N_POT_Y         ((uint8_t *)0xFFC0421A)
#define SID_INT_N_OSC3_RND      ((uint8_t *)0xFFC0421B)
#define SID_INT_N_ENV3          ((uint8_t *)0xFFC0421C)
#define SID_INT_N_NOT_USED0     ((uint8_t *)0xFFC0421D)
#define SID_INT_N_NOT_USED1     ((uint8_t *)0xFFC0421E)
#define SID_INT_N_NOT_USED2     ((uint8_t *)0xFFC0421F)

#define SID_INT_N  				((volatile __attribute__((far)) struct s_sid *)0xffc04200)

#endif
