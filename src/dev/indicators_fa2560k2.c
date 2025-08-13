/*
 * System independent routines for controlling the built in indicator LEDs
 *
 * Indicators on the FA2560K2 are present on the keyboard and controlled through GABE
 */

#include <stdint.h>

#include "../include/FA2560K2/gabe_fa2560k.h"
#include "indicators.h"
#include "sys_general.h"
#include "../include/sys_types.h"

/**
 * Return the RGBA color code for the given state of an indicator
 * 
 * @param state the state code for the indicator
 * @return the RGB color code to display
 */
uint32_t ind_state_color(short state) {
    switch (state) {
        case IND_ON:
            /* Green for on */
            return 0x0000ff00;

        case IND_ERROR:
            /* Red for error */
            return 0x00ff0000;

        case IND_FOENIX:
            /* Return purple (well, magenta) */
            return 0x00ff00ff;

        default:
            /* Anything else is off */
            return 0x00000000;
    }
}

void ind_set_on_off(uint32_t bit, short state) {
    if (state == IND_OFF) {
        *GABE_CTRL_REG = *GABE_CTRL_REG & ~bit;
    } else {
        *GABE_CTRL_REG = *GABE_CTRL_REG | bit;
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
    switch (ind_number) {
        case IND_POWER:
            *GABE_FA2560K2_RGB0 = ind_state_color(state);
            ind_set_on_off(POWER_ON_LED, state);
            break;

        case IND_FDC:
            break;

        case IND_SDC:
            *GABE_CTRL_REG = *GABE_CTRL_REG & ~SDCARD_CTRL;
            *GABE_FA2560K2_RGB1 = ind_state_color(state);
            ind_set_on_off(SDCARD_LED, state);
            break;

        case IND_HDC:
            *GABE_CTRL_REG = *GABE_CTRL_REG | SDCARD_CTRL;
            *GABE_FA2560K2_RGB2 = ind_state_color(state);
            ind_set_on_off(SDCARD_LED, state);
            break;

        case IND_NET:
            *GABE_CTRL_REG = *GABE_CTRL_REG & ~NETWORK_CTRL;
            *GABE_FA2560K2_RGB3 = ind_state_color(state);
            *GABE_FA2560K2_RGB4 = ind_state_color(state);
            ind_set_on_off(NETWORK_LED, state);
            break;

        case IND_CAPS:
            if (state == IND_ON) {
                *GABE_FA2560K2_RGB5 = ind_state_color(IND_FOENIX);
                ind_set_on_off(CAPSLOCK_LED, state);
            } else {
                *GABE_FA2560K2_RGB5 = ind_state_color(state);
                ind_set_on_off(CAPSLOCK_LED, state);
            }
            break;

        default:
            break;
    }
}

/*
 * Initialize the indicators
 *
 * Generally, this means the indicators will all be turned off
 */
void ind_init() {
    *GABE_CTRL_REG = *GABE_CTRL_REG & 0xffff0000;

    ind_set(IND_POWER, IND_ON);
    ind_set(IND_FDC, IND_OFF);
    ind_set(IND_SDC, IND_OFF);
    ind_set(IND_HDC, IND_OFF);
    ind_set(IND_NET, IND_OFF);
    ind_set(IND_CAPS, IND_OFF);
}
