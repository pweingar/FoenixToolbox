/**
 * Basic controls for bitmap display
 */

#include <stdlib.h>

#include "txt_screen.h"
#include "FA2560K2/vkyii_legacy_Channelb.h"
#include "FA2560K2/VICKYIII_fa2560k2.h"

volatile uint8_t * vram = (uint8_t *) 0x00200000;   
volatile uint32_t * vicky_mcr = (uint32_t *)0xffd00000;

/**
 * Initialize a blank bitmap
 */
void bm_init() {
    t_color4 white;
    white.red = 255;
    white.green = 255;
    white.blue = 255;

    *vicky_mcr = 0x00000007;
    VICKY3->control = 0x0000000b;
    VICKY3->bitmap_addr = (uint32_t)vram >> 1;
    VICKY3->mono_color = white;

    for (short y = 0; y < 384; y++) {
        uint8_t value = ((y & 0x01) == 0x01) ? 0xaa : 0x55;
        for (short x = 0; x < 512; x++) {
            vram[y * 512 + x] = value;
        }
    }
}