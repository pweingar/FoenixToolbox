/**
 * Register declarations for optical keyboards
 */

#ifndef __kbd_optical__
#define __kbd_optical__

#include "sys_general.h"

#if MODEL == MODEL_FOENIX_F256K2 || MODEL == MODEL_FOENIX_F256K2X
#include "F256/kbd_opt_f256.h"

#elif MODEL == MODEL_FOENIX_FA2560K2
#include "FA2560K2/kbd_opt_fa2560k2.h"

#endif

#endif