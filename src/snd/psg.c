/*
 * Definitions for the SN76489 PSG
 */

#include <stdint.h>

#include "psg.h"
#include "sound_reg.h"
#include "log.h"

/**
 * @brief Return the I/O base address of the PSG chip
 * 
 * @param chip the number of the chip
 * @return uint8_t* pointer to the first register
 */
static volatile uint8_t * psg_port(short chip) {
	volatile uint8_t * port = 0;

#if MODEL == MODEL_FOENIX_A2560K || MODEL == MODEL_FOENIX_GENX || MODEL == MODEL_FOENIX_A2560X
	switch (chip) {
		case 1:
			port = (uint8_t *)PSG_INT_L_PORT;
			break;

		case 2:
			port = (uint8_t *)PSG_INT_R_PORT;
			break;

		case 3:
			port = (uint8_t *)PSG_INT_S_PORT;
			break;

		default:
			port = (uint8_t *)PSG_PORT;
			break;
	}
#elif MODEL == MODEL_FOENIX_F256 || MODEL == MODEL_FOENIX_F256K || MODEL == MODEL_FOENIX_F256K2
	switch (chip) {
		case 1:
			port = PSG_PORT_L;
			break;

		case 2:
			port = PSG_PORT_R;
			break;

		case 3:
			port = PSG_PORT;
			break;

		default:
			break;
	}
#endif

	return port;
}

/*
 * Mute all voices on the PSG
 */
void psg_mute_all() {
    short chip;
    short voice;

    for (chip = 0; chip < 4; chip++) {
        /* Set attenuation on all voices to full */
        for (voice = 0; voice < 4; voice++) {
            psg_attenuation(chip, voice, 15);
        }
    }
}

/*
 * Set the frequency of one of the three tone voices
 *
 * Inputs:
 * chip = the chip to use (0 = external, 1 = internal left, 2 = internal right, 3 = internal stereo)
 * voice = the number of the voice (0 - 3)
 * frequency = the frequency
 */
void psg_tone(short chip, unsigned short voice, int frequency) {
    volatile uint8_t * port = psg_port(chip);
	if (port) {
        int n = 0;
        if (frequency != 0) {
			n = 11320 / frequency;
            // n = 3579545 / (32 * frequency);
        }
        *port = (uint8_t)(0x80 | ((voice & 0x03) << 5) | (n & 0x0f));
        *port = (uint8_t)((n & 0x3f0) >> 4);
	}
}

/*
 * Set the attenuation of a voice
 *
 * Inputs:
 * chip = the chip to use (0 = external, 1 = internal left, 2 = internal right, 3 = internal stereo)
 * voice = the number of the voice (0 - 3)
 * attenuation = volume level 0 = loudest, 15 = silent
 */
void psg_attenuation(short chip, unsigned short voice, short attenuation) {
    volatile uint8_t * port = psg_port(chip);
	if (port) {
	    *port = (uint8_t)(0x90 | ((voice & 0x03) << 5) | (attenuation & 0x0f));
	}
}
