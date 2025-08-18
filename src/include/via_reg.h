/**
 * Register definitions for the VIA chips present on some systems
 */

#ifndef __via__
#define __via__

#include "sys_general.h"

#if MODEL == MODEL_FOENIX_F256_GEN
#include "F256/via_f256.h"

#elif MODEL == MODEL_FOENIX_FA2560K2
#include "FA2560K2/via_fa2560k.h"

#endif

#endif
