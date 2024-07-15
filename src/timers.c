#include "interrupt.h"
#include "timers.h"
#include "gabe_reg.h"
#include "timers_reg.h"
#include "sys_general.h"

/*
 * Initialize the timers and their interrupts
 */
void timers_init() {
#if MODEL == MODEL_FOENIX_A2560U || MODEL == MODEL_FOENIX_A2560U_PLUS

    *TIMER_TCR0 = 0;    // Reset timers 0, 1, and 2
    *TIMER_TCR1 = 0;    // Reset timers 3, and 4 (if 4 is available)

    // Clear timer 3

    *TIMER_VALUE_3 = 0;
    *TIMER_TCR1 = TCR_ENABLE_3 | TCR_CNTUP_3 | TCR_CLEAR_3;

    // Set timer 3 to count up and auto clear

    *TIMER_TCR1 = TCR_ENABLE_3 | TCR_CNTUP_3;

#elif MODEL == MODEL_FOENIX_F256 || MODEL == MODEL_FOENIX_F256K || MODEL == MODEL_FOENIX_F256K2

	// Timers off
	*TIMER_CTRL_0 = 0;
	*TIMER_CTRL_1 = 0;

	// Enable counting up on timer #1 and preclear

	*TIMER_CHG_L_1 = 0;
	*TIMER_CHG_M_1 = 0;
	*TIMER_CHG_H_1 = 0;

	*TIMER_CMP_L_1 = 0xff;
	*TIMER_CMP_M_1 = 0xff;
	*TIMER_CMP_H_1 = 0xff;

	*TIMER_CTRL_1 = TIMER_CTRL_EN | TIMER_CTRL_CNT_UP | TIMER_CTRL_SCLR;

	// Set timer 3 to count up and auto clear
	*TIMER_CMPC_1 = TIMER_CMP_RECLR;
    *TIMER_CTRL_1 = TIMER_CTRL_EN | TIMER_CTRL_CNT_UP;

#endif
}

/*
 * Return the number of jiffies (1/60 of a second) since last reset time
 */
SYSTEMCALL long timers_jiffies() {
#if MODEL == MODEL_FOENIX_A2560U || MODEL == MODEL_FOENIX_A2560U_PLUS
    return *TIMER_VALUE_3;

#elif MODEL == MODEL_FOENIX_F256 || MODEL == MODEL_FOENIX_F256K || MODEL == MODEL_FOENIX_F256K2
	uint32_t result = (uint32_t)(*TIMER_CHG_L_1) | ((uint32_t)(*TIMER_CHG_M_1) << 8) | ((uint32_t)(*TIMER_CHG_H_1) << 16);
    return result;

#endif
}
