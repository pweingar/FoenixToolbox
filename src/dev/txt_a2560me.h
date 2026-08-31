/** @file txt_a2560me.h
 *
 * Text screen driver for A2560Me legacy text block
 */

#ifndef __TXT_A2560ME_H
#define __TXT_A2560ME_H

/* Channel B is assigned to screen #0, it will be considered the primary channel */
#define TXT_SCREEN_A2560ME 0

/**
 * Initialize and install the driver
 *
 * @return 0 on success, any other number is an error
 */
extern short txt_a2560me_install();

#endif
