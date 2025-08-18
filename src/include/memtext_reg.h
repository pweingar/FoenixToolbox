/**
 * Registers for the MEMTEXT system
 */

#ifndef __memtext_reg__
#define __memtext_reg__

#if MODEL == MODEL_FOENIX_F256K2X
#include "F256/memtext_f256.h"

#elif MODEL == MODEL_FOENIX_FA2560K2
#include "FA2560K2/memtext_fa2560k2.h"

#endif

#endif