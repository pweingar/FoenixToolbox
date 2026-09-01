#include <stdint.h>

#include "A2560Me/gabe_a2560me.h"
#include "indicators.h"

/*
 * System independent routines for controlling the built in indicator LEDs
 *
 * Indicators on the C256 and most of the A2560 lines of computers will be built in to
 * the board and contolled by GABE. They will usually be simple off-on indicators.
 *
 */

/**
 * Return the color for the given indicator and state
 * 
 * @param ind_number the number of the indicator being set
 * @param state the state the indicator will have
 * @return the RGB color code to indicate that state on that RGB LED
 */
 uint32_t int_state_color(short ind_number, short state) {
    switch(state) {
        case IND_ON:
            // Green for on
            return 0x00008000;

        case IND_OFF:
            // Blank for off
            return 0x00000000;

        case IND_ERROR:
            // Red for error
            return 0x00800000;

        default:
            return 0x00000000;
    }
}

/*
 * Set an indicator to the given state
 *
 * Inputs:
 * ind_number = the number of the indicator to change
 * state = the state the indicator should take (on, off, error)
 */
void ind_set(short ind_number, short state) {
    uint32_t mask = 0;
    
    // Compute and set the color the indicator should assume
    switch(ind_number) {
        case IND_POWER:
            *GABE_A2560M_PWR_RGB = int_state_color(ind_number, state);
            break;

        case IND_SDC:
            *GABE_A2560M_SD0_RGB = int_state_color(ind_number, state);
            break;

        case IND_SDC1:
            *GABE_A2560M_SD1_RGB = int_state_color(ind_number, state);
            break;

        case IND_HDC:
            *GABE_A2560M_HDC_RGB = int_state_color(ind_number, state);
            break;

        case IND_NET:
            *GABE_A2560M_NET_RGB = int_state_color(ind_number, state);
            break;

        default:
            return;
    }

    // Turn the LED on or off depending on the state
    switch(ind_number) {
        case IND_POWER:
            mask = POWER_ON_LED;
            break;

        case IND_SDC:
            mask = SDC0_LED;
            break;

        case IND_SDC1:
            mask = SDC1_LED;
            break;

        case IND_HDC:
            mask = MSATA_LED;
            break;

        case IND_NET:
            mask = NETWORK_LED;
            break;

        default:
            return;
    }

    if (state == IND_OFF) {
        *GABE_CTRL_REG = *GABE_CTRL_REG & ~mask;
    } else {
        *GABE_CTRL_REG = *GABE_CTRL_REG | mask;
    }
}

/*
 * Initialize the indicators
 *
 * Generally, this means the indicators will all be turned off
 */
void ind_init() {
    ind_set(IND_POWER, IND_ON);
    ind_set(IND_FDC, IND_OFF);
    ind_set(IND_HDC, IND_OFF);
    ind_set(IND_SDC, IND_OFF);
    ind_set(IND_SDC1, IND_OFF);
    ind_set(IND_NET, IND_OFF);
}