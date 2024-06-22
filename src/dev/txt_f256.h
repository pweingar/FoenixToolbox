/** @file txt_f256.h
 *
 * Text screen driver for the F256 (JR and K)
 */

#ifndef __TXT_F256_H
#define __TXT_F256_H

/* We only have one screen */
#define TXT_SCREEN_F256 0

/**
 * Initialize and install the driver
 *
 * @return 0 on success, any other number is an error
 */
extern short txt_f256_install();

#endif
