/** @file txt_fa2560k2.c
 *
 * Text screen driver for fa2560k2
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "errors.h"
#include "log.h"
#include "sys_general.h"
#include "utilities.h"
#include "FA2560K2/vicky_ii_fa2560k2.h"
#include "FA2560K2/memtext_fa2560k2.h"
#include "dev/txt_screen.h"
#include "dev/txt_fa2560k2.h"

//
// Types
//

typedef struct text_state_e {
    bool use_memtext;                   // True if the screen should use the MEMTEXT system
    bool enable_set_sizes;              // True if setsizes should actually update the variables
    t_txt_capabilities caps;            // Capabilities of this screen
    t_rect region;                      // The current region
    t_point cursor;                     // The current cursor position
    t_extent resolution;                // The current display resolution
    t_extent font_size;                 // The current font size
    t_extent max_size;                  // The size of the screen in characters (without border removed)
    t_extent visible_size;              // The size of the visible screen in characters (with border removed)
    short border_width;                 // Width of the border on one side
    short border_height;                // Height of the border on one side
    uint16_t color;                     // The current color
    uint16_t attribute;                 // The current attribute for MEMTEXT
    uint32_t msr_shadow_b;              // A shadow register for the Master Control Register
} text_state_t, *text_state_p;

//
// Constants
//

extern const unsigned char MSX_CP437_8x8_bin[];

/* Default text color lookup table values (AARRGGBB) */
const unsigned long fa2560k2_lut[] = {
    0xFF000000,	// Black (transparent) - 0x00
	0xFF800000, // Mid-Tone Red - 0x01
	0xFF008000, // Mid-Tone Green - 0x02
	0xFF808000, // Mid-Tone Yellow - 0x03
	0xFF000080, // Mid-Tone Blue - 0x04
	0xFFAA5500, // Mid-Tone Orange - 0x05
	0xFF008080, // Mid-Tone Cian - 0x06
	0xFF808080, // 50% Grey - 0x07
    0xFF202020, // Dark Grey
    0xFFFF0000, // Bright Red - 0x09
	0xFF55FF55, // Bright Green - 0x0A
	0xFFFFFF55, // Bright Yellow - 0x0B
	0xFF5555FF, // Bright Blue - 0x0C
	0xFFFF7FFF, // Bright Orange - 0x0D
	0xFF55FFFF, // Bright Cyan - 0x0E
	0xFFFFFFFF 	// White - 0x0F
};

/*
 * Driver level variables for the screen
 */

static text_state_t self;
static t_extent textmode_resolutions[1];            // Resolutions for the legacy textmode
static t_extent memtext_resolutions[1];             // Resolutions for the MEMTEXT mode
static t_extent font_sizes[1];                      // The list of font resolutions in block text mode
static t_extent memtext_font_sizes[2];              // List of font sizes in MEMTEXT mode

static __attribute__((aligned(16))) uint16_t memtext_text[128 * 96];
static __attribute__((aligned(16))) uint16_t memtext_color[128 * 96];

//
// Functions
//

void txt_fa2560k2_set_sizes();
short txt_fa2560k2_set_resolution(short width, short height);
short txt_fa2560k2_set_region(p_rect region);
short txt_fa2560k2_get_color(unsigned char * foreground, unsigned char * background);

static void txt_fa2560k2_set_attribute(uint16_t attr) {
    self.attribute = attr;
}

/**
 * Change between legacy text mode and memtext mode
 */
static void switch_text_mode(bool use_memtext) {
    t_rect region;

    region.origin.x = 0;
    region.origin.y = 0;
    region.size.width = 0;
    region.size.height = 0;

    if (use_memtext) {
        // Switch to MEMTEXT mode
        self.use_memtext = true;
        self.caps.resolutions = &memtext_resolutions;
        self.caps.resolution_count = sizeof(memtext_resolutions) / sizeof(t_extent);
        self.caps.font_sizes = &memtext_font_sizes;
        self.caps.font_size_count = sizeof(memtext_font_sizes) / sizeof(t_extent);
        
        *MEMTEXT_CTRL = MEMTEXT_MAIN_EN;
        *MEMTEXT_TEXT_ADDR = memtext_text;
        *MEMTEXT_COLOR_ADDR = memtext_color;

        self.msr_shadow_b |= VKY_MCR_MEMTEXT;
        *tvky_mstr_ctrl = self.msr_shadow_b;

        txt_fa2560k2_set_resolution(1024, 768);
        txt_fa2560k2_set_sizes();
        txt_fa2560k2_set_region(&region);

    } else {
        // Switch to block text mode
        self.use_memtext = false;
        self.caps.resolutions = &textmode_resolutions;
        self.caps.resolution_count = sizeof(textmode_resolutions) / sizeof(t_extent);
        self.caps.font_sizes = &font_sizes;
        self.caps.font_size_count = sizeof(font_sizes) / sizeof(t_extent);

        *MEMTEXT_CTRL = 0;

        self.msr_shadow_b &= ~VKY_MCR_MEMTEXT;
        *tvky_mstr_ctrl = self.msr_shadow_b;

        txt_fa2560k2_set_resolution(640, 480);
        txt_fa2560k2_set_sizes();
        txt_fa2560k2_set_region(&region);
    }
}

/**
 * Gets the description of a screen's capabilities
 *
 * @return a pointer to the read-only description (0 on error)
 */
const p_txt_capabilities txt_fa2560k2_get_capabilities() {
    return &self.caps;
}

/**
 * Calculate the size of the text screen in rows and columns so that
 * the kernel printing routines can work correctly.
 *
 * NOTE: this should be called whenever the VKY3 Channel B registers are changed
 */
void txt_fa2560k2_set_sizes() {
    if (self.enable_set_sizes) {
        /* Only recalculate after initialization is mostly completed */

        /*
         * Calculate the maximum number of characters visible on the screen
         * This controls text layout in memory
         */

        self.max_size.width = self.resolution.width / self.font_size.width;
        self.max_size.height = self.resolution.height / self.font_size.height;

        /*
         * Calculate the characters that are visible in whole or in part
         *
         * NOTE: There is no Border in the Vicky II Legacy Block of the fa2560k2
         */ 
        self.visible_size.width = self.max_size.width;
        self.visible_size.height = self.max_size.height;
    }
}

/**
 * Get the display resolutions
 *
 * @param text_size the size of the screen in visible characters (may be null)
 * @param pixel_size the size of the screen in pixels (may be null)
 */
void txt_fa2560k2_get_sizes(p_extent text_size, p_extent pixel_size) {
    if (text_size) {
        text_size->width = self.visible_size.width;
        text_size->height = self.visible_size.height;
    }

    if (pixel_size) {
        pixel_size->width = self.resolution.width;
        pixel_size->height = self.resolution.height;
    }
}

/**
 * Set the display mode for the screen
 *
 * @param mode a bitfield of desired display mode options
 *
 * @return 0 on success, any other number means the mode is invalid for the screen
 */
short txt_fa2560k2_set_mode(short mode) {
    /* Turn off anything not set */
    // FYI, there is no more Graphics Mode in the fa2560k2 VICKY II Legacy Block (only the text mode)
    self.msr_shadow_b &= ~(VKY_MCR_TEXT | VKY_MCR_MEMTEXT);

    if (mode & TXT_MODE_SLEEP) {
        /* Put the monitor to sleep: overrides all other option bits */
        self.msr_shadow_b |= VKY_MCR_SLEEP;
        *tvky_mstr_ctrl = self.msr_shadow_b;
        return 0;

    } else {
        if (mode & ~(TXT_MODE_TEXT | TXT_MODE_MEMORY)) {
            /* A mode bit was set beside one of the supported ones... */
            return -1;

        } else {
            if (mode & (TXT_MODE_TEXT | TXT_MODE_MEMORY)) {
                self.msr_shadow_b |= VKY_MCR_TEXT;
            }

            if (mode & TXT_MODE_MEMORY) {
                switch_text_mode(true);
            } else {
                switch_text_mode(false);
            }

            return 0;
        }
    }
}

/**
 * Set the display resolution of the screen
 *
 * @param width the desired horizontal resolution in pixels
 * @param height the desired veritical resolution in pixels
 *
 * @return 0 on success, any other number means the resolution is unsupported
 */
short txt_fa2560k2_set_resolution(short width, short height) {
    int i;

    for (i = 0; i < self.caps.resolution_count; i++) {
        if ((width == self.caps.resolutions[i].width) && (height == self.caps.resolutions[i].height)) {
            // We support this resolution, update the kernel variables
            self.resolution.width = width;
            self.resolution.height = height;

            // Recalculate the size of the screen
            txt_fa2560k2_set_sizes();

            return 0;
        }
    }

    // We don't support this resolution
    return ERR_NOT_SUPPORTED;
}

/**
 * Load a font as the current font for the screen
 *
 * @param width width of a character in pixels
 * @param height of a character in pixels
 * @param data pointer to the raw font data to be loaded
 */
short txt_fa2560k2_set_font(short width, short height, const unsigned char * data) {
    for (short i = 0; i < self.caps.font_size_count; i++) {
        if ((width == self.caps.font_sizes[i].width) && (height == self.caps.font_sizes[i].height)) {
            // We support this font size, update the kernel variables

            /* The size is valid... set the font */
            self.font_size.width = width;
            self.font_size.height = height;

            if (self.use_memtext) {
                /* Copy the font data to MEMTEXT font #0... this assumes a width of one byte! */
                for (i = 0; i < 256 * height; i++) {
                    MEMTEXT_FONT[i] = data[i];
                }

            } else {
                /* Copy the font data to the block text font... this assumes a width of one byte! */
                for (i = 0; i < 256 * height; i++) {
                    tvky_font_set_0[i] = data[i];
                }
            }

            return 0;
        }
    }

    // We don't support this font size
    return ERR_NOT_SUPPORTED;
}

/**
 * Set the appearance of the cursor
 *
 * @param enable 0 to hide, any other number to make visible
 * @param rate the blink rate for the cursor (0=1s, 1=0.5s, 2=0.25s, 3=1/5s)
 * @param c the character in the current font to use as a cursor
 */
void txt_fa2560k2_set_cursor(short enable, short rate, char c) {
    if (self.use_memtext) {
        uint32_t raw = *MEMTEXT_CTRL & ~(MEMTEXT_CRSR_EN | MEMTEXT_CRSR_RATE | MEMTEXT_CRSR_FLASH);

        if (enable) {
            *MEMTEXT_CTRL = raw | MEMTEXT_CRSR_EN | ((uint32_t)(rate & 0x03) << 9);
        } else {
            *MEMTEXT_CTRL = raw; 
        }

        switch(c) {
            case 0:
                MEMTEXT_CRSR_BITS[0] = 0x00000000;
                MEMTEXT_CRSR_BITS[1] = 0xffff0000;
                break;

            default:
                MEMTEXT_CRSR_BITS[0] = 0xffffffff;
                MEMTEXT_CRSR_BITS[1] = 0xffffffff;
                break;
        }

        uint8_t background = 0;
        uint8_t foreground = 0;
        txt_fa2560k2_get_color(&foreground, &background);
        *MEMTEXT_CRSR_COLOR = MEMTEXT_FG_0[foreground];
                
    } else {
        tvky_crsr_ctrl->control = ((self.color & 0xff) << 24) | ((c & 0xff) << 16) | ((rate & 0x03) << 1) | (enable & 0x01);
    }
}

/**
 * Set if the cursor is visible or not
 *
 * @param enable 0 to hide, any other number to make visible
 */
void txt_fa2560k2_set_cursor_visible(short enable) {
    if (self.use_memtext) {
        uint32_t raw = *MEMTEXT_CTRL & ~(MEMTEXT_CRSR_EN);

        if (enable) {
            *MEMTEXT_CTRL = raw | MEMTEXT_CRSR_EN;
        } else {
            *MEMTEXT_CTRL = raw;
        }

    } else {
        if (enable) {
            tvky_crsr_ctrl->enable = 1;
        } else {
            tvky_crsr_ctrl->enable = 0;
        }
    }
}

/**
 * Set a region to restrict further character display, scrolling, etc.
 * Note that a null region pointer and a region of zero size will reset the region to the full size of the screen.
 *
 * @param region pointer to a t_rect describing the rectangular region (using character cells for size and size)
 *
 * @return 0 on success, any other number means the region was invalid
 */
 short txt_fa2560k2_set_region(p_rect region) {
     if ((region->size.width == 0) || (region->size.height == 0)) {
         /* Set the region to the default (full screen) */
         self.region.origin.x = 0;
         self.region.origin.y = 0;
         self.region.size.width = self.visible_size.width;
         self.region.size.height = self.visible_size.height;

     } else {
         self.region.origin.x = region->origin.x;
         self.region.origin.y = region->origin.y;
         self.region.size.width = region->size.width;
         self.region.size.height = region->size.height;
     }

     return 0;
 }

/**
 * get the current region
 *
 * @param region pointer to a t_rect describing the rectangular region (using character cells for size and size)
 *
 * @return 0 on success, any other number means the region was invalid
 */
short txt_fa2560k2_get_region(p_rect region) {
    region->origin.x = self.region.origin.x;
    region->origin.y = self.region.origin.y;
    region->size.width = self.region.size.width;
    region->size.height = self.region.size.height;

    return 0;
}

/**
 * Set the default foreground and background colors for printing
 *
 * @param foreground the Text LUT index of the new current foreground color (0 - 15)
 * @param background the Text LUT index of the new current background color (0 - 15)
 */
short txt_fa2560k2_set_color(unsigned char foreground, unsigned char background) {
    if (self.use_memtext) {
        self.color = ((uint16_t)(foreground & 0xff) << 8) | (uint16_t)(background & 0xff);
        *MEMTEXT_CRSR_COLOR = MEMTEXT_FG_0[foreground];

    } else {
        self.color = ((foreground & 0x0f) << 4) | (background & 0x0f);
    }

    return 0;
}

/**
 * Get the default foreground and background colors for printing
 *
 * @param pointer to the foreground the Text LUT index of the new current foreground color (0 - 15)
 * @param pointer to the background the Text LUT index of the new current background color (0 - 15)
 */
short txt_fa2560k2_get_color(unsigned char * foreground, unsigned char * background) {
    if (self.use_memtext) {
        *foreground = (self.color & 0xff00) >> 8;
        *background = self.color & 0xff;

    } else {
        *foreground = (self.color & 0xf0) >> 4;
        *background = self.color & 0x0f;
    }

    return 0;
}


/**
 * Scroll the text in the current region
 *
 * @param screen the number of the text device
 * @param horizontal the number of columns to scroll (negative is left, positive is right)
 * @param vertical the number of rows to scroll (negative is down, positive is up)
 */
void txt_fa2560k2_scroll(short horizontal, short vertical) {
    short x, x0, x1, x2, x3, dx;
    short y, y0, y1, y2, y3, dy;

    /*
     * Determine limits of rectangles to move and fill and directions of loops
     * x0 and y0 are the positions of the first cell to be over-written
     * x1 and y1 are the positions of the first cell to be copyed... TEXT[x0,y0] := TEXT[x1,y1]
     * x2 and y2 are the position of the last cell to be over-written
     * x3 and y3 are the position of the last cell to be copied... TEXT[x2,y2] := TEXT[x3,y3]
     *
     * When blanking, the rectangles (x2,y0) - (x3,y3) and (x0,y2) - (x2,y3) are cleared
     */

    // Determine the row limits

    if (vertical >= 0) {
        y0 = self.region.origin.y;
        y1 = y0 + vertical;
        y3 = self.region.origin.y + self.region.size.height;
        y2 = y3 - vertical;
        dy = 1;
    } else {
        y0 = self.region.origin.y + self.region.size.height - 1;
        y1 = y0 + vertical;
        y3 = self.region.origin.y - 1;
        y2 = y3 - vertical;
        dy = -1;
    }

    // Determine the column limits

    if (horizontal >= 0) {
        x0 = self.region.origin.x;
        x1 = x0 + horizontal;
        x3 = self.region.origin.x + self.region.size.width;
        x2 = x3 - horizontal;
        dx = 1;
    } else {
        x0 = self.region.origin.x + self.region.size.width - 1;
        x1 = x0 + horizontal;
        x3 = self.region.origin.x - 1;
        x2 = x3 - horizontal;
        dx = -1;
    }

    /* Copy the rectangle */

    if (self.use_memtext) {
        // Copy the rectangle in the memtext mode
        
        for (y = y0; y != y2; y += dy) {
            int row_dst = y * self.max_size.width;
            int row_src = (y + vertical) * self.max_size.width;

            for (x = x0; x != x2; x += dx) {
                int offset_dst = row_dst + x;
                int offset_src = row_src + x + horizontal;

                memtext_text[offset_dst] = memtext_text[offset_src];
                memtext_color[offset_dst] = memtext_color[offset_src];
            }
        }

    } else {
        // Copy the rectangle in the block text mode

        for (y = y0; y != y2; y += dy) {
            int row_dst = y * self.max_size.width;
            int row_src = (y + vertical) * self.max_size.width;

            for (x = x0; x != x2; x += dx) {
                int offset_dst = row_dst + x;
                int offset_src = row_src + x + horizontal;

                tvky_text_matrix[offset_dst] = tvky_text_matrix[offset_src];
                tvky_color_matrix[offset_dst] = tvky_color_matrix[offset_src];
            }
        }
    }

    /* Clear the rectangles */

    if (horizontal != 0) {
        if (self.use_memtext) {
            // Clear the rectangle in memtext mode
            for (y = y0; y != y3; y += dy) {
                int row_dst = y * self.max_size.width;
                for (x = x2; x != x3; x += dx) {
                    memtext_text[row_dst + x] = ' ';
                    memtext_color[row_dst + x] = self.color;
                }
            }

        } else {
            // Clear the rectangle in block text mode
            for (y = y0; y != y3; y += dy) {
                int row_dst = y * self.max_size.width;
                for (x = x2; x != x3; x += dx) {
                    tvky_text_matrix[row_dst + x] = ' ';
                    tvky_color_matrix[row_dst + x] = self.color;
                }
            }
        }
    }

    if (vertical != 0) {
        if (self.use_memtext) {
            // Clear the rectangle in memtext mode
            for (y = y2; y != y3; y += dy) {
                int row_dst = y * self.max_size.width;
                for (x = x0; x != x3; x += dx) {
                    memtext_text[row_dst + x] = ' ';
                    memtext_color[row_dst + x] = self.color;
                }
            }

        } else {
            // Clear the rectangle in block text mode
            for (y = y2; y != y3; y += dy) {
                int row_dst = y * self.max_size.width;
                for (x = x0; x != x3; x += dx) {
                    tvky_text_matrix[row_dst + x] = ' ';
                    tvky_color_matrix[row_dst + x] = self.color;
                }
            }
        }
    }
}

/**
 * Fill the current region with a character in the current color
 *
 * @param screen the number of the text device
 * @param c the character to fill the region with
 */
void txt_fa2560k2_fill(char c) {
    int x;
    int y;

    if (self.use_memtext) {
        // Fill the region in memtext mode
        
        for (y = 0; y < self.region.size.height; y++) {
            int offset_row = (self.region.origin.y + y) * self.max_size.width;
            for (x = 0; x < self.region.size.width; x++) {
                int offset = offset_row + self.region.origin.x + x;
                memtext_text[offset] = c;
                memtext_color[offset] = self.color;
            }
        }

    } else {
        // Fill the region in block text mode

        for (y = 0; y < self.region.size.height; y++) {
            int offset_row = (self.region.origin.y + y) * self.max_size.width;
            for (x = 0; x < self.region.size.width; x++) {
                int offset = offset_row + self.region.origin.x + x;
                tvky_text_matrix[offset] = c;
                tvky_color_matrix[offset] = self.color;
            }
        }
    }
}

/**
 * Set the position of the cursor to (x, y) relative to the current region
 * If the (x, y) coordinate is outside the region, it will be clipped to the region.
 * If y is greater than the height of the region, the region will scroll until that relative
 * position would be within view.
 *
 * @param x the column for the cursor
 * @param y the row for the cursor
 */
void txt_fa2560k2_set_xy(short x, short y) {
    /* Make sure X is within range for the current region... "print" a newline if not */
    if (x < 0) {
        x = 0;
    } else if (x >= self.region.size.width) {
        x = 0;
        y++;
    }

    /* Make sure Y is within range for the current region... scroll if not */
    if (y < 0) {
        y = 0;
    } else if (y >= self.region.size.height) {
        txt_fa2560k2_scroll(0, y - self.region.size.height + 1);
        y = self.region.size.height - 1;
    }

    self.cursor.x = x;
    self.cursor.y = y;

    /* Set register */
    if (self.use_memtext) {
        // Set the cursor position in memtext mode

        *MEMTEXT_CRSR_XY = (((uint32_t)(self.region.origin.y + y) & 0x000000ff) << 8) |
                            ((uint32_t)(self.region.origin.x + x) & 0x00ff);

    } else {
        // Set the cursor position in block text mode
        tvky_crsr_ctrl->cursor_xy = (((self.region.origin.y + y) & 0xffff) << 16) | ((self.region.origin.x + x) & 0xffff);
    }
}

/**
 * Get the position of the cursor (x, y) relative to the current region
 *
 * @param screen the number of the text device
 * @param position pointer to a t_point record to fill out
 */
void txt_fa2560k2_get_xy(p_point position) {
    position->x = self.cursor.x;
    position->y = self.cursor.y;
}

/**
 * Print a character to the current cursor position in the current color
 *
 * @param screen the number of the text device
 * @param c the character to print
 */
void txt_fa2560k2_put(char c) {
    short x;
    short y;
    unsigned int offset;

    x = self.region.origin.x + self.cursor.x;
    y = self.region.origin.y + self.cursor.y;
    offset = y * self.max_size.width + x;

    if (self.use_memtext) {
        memtext_text[offset] = c;
        memtext_color[offset] = self.color;

    } else {
        tvky_text_matrix[offset] = c;
        tvky_color_matrix[offset] = self.color;
    }
    
    txt_fa2560k2_set_xy(self.cursor.x + 1, self.cursor.y);
}

/**
 * Initialize the screen
 */
void txt_fa2560k2_init() {
    char buffer[255];
    t_rect region;
    int i;

    self.use_memtext = false;

    self.resolution.width = 0;
    self.resolution.height = 0;
    self.font_size.width = 0;
    self.font_size.height = 0;

    /* Disable the set_sizes call for now */
    self.enable_set_sizes = 0;

    /* Start with nothing on */
    self.msr_shadow_b = 0;
    *tvky_mstr_ctrl = self.msr_shadow_b;
    *MEMTEXT_CTRL = 0;

    // Default attribute is 0
    self.attribute = 0;

    /* Define the capabilities */

    /* Specify the screen number */
    self.caps.number = TXT_SCREEN_FA2560K2;

    // Set the capabilities of this display.
    // For the moment, that's just BLOCK TEXT and MEMTEXT
    self.caps.supported_modes = TXT_MODE_TEXT | TXT_MODE_MEMORY;

    /* Resolutions supported: 640x480 */
    textmode_resolutions[0].width = 640;
    textmode_resolutions[0].height = 480;
    self.caps.resolution_count = 1;
    self.caps.resolutions = textmode_resolutions;

    memtext_resolutions[0].width = 1024;
    memtext_resolutions[0].height = 768;

    /* Block text supports 8x8 fonts ONLY */
    font_sizes[0].width = 8;
    font_sizes[0].height = 8;
    self.caps.font_size_count = 1;
    self.caps.font_sizes = font_sizes;

    // MEMTEXT supports 8x8 and 8x16 fonts */
    memtext_font_sizes[0].width = 8;
    memtext_font_sizes[0].height = 8;
    memtext_font_sizes[1].width = 8;
    memtext_font_sizes[1].height = 16;

    for (i = 0; i < 16; i++) {
        VKY3_B_TEXT_LUT_FG[i] = fa2560k2_lut[i];
        VKY3_B_TEXT_LUT_BG[i] = fa2560k2_lut[i];

        MEMTEXT_FG_0[i] = fa2560k2_lut[i];
        MEMTEXT_BG_0[i] = fa2560k2_lut[i];

        MEMTEXT_FG_1[i] = fa2560k2_lut[i];
        MEMTEXT_BG_1[i] = fa2560k2_lut[i];
    }

    /* Set the mode to memtext */
    txt_fa2560k2_set_mode(TXT_MODE_TEXT | TXT_MODE_MEMORY);

    if (self.use_memtext) {
        txt_fa2560k2_set_cursor(1, 1, 0);
        txt_fa2560k2_set_color(0xff, 0x04);

        /* Set the resolution */
        txt_fa2560k2_set_resolution(1024, 768);                 /* Default resolution is 1024x768 */

    } else {
        txt_fa2560k2_set_cursor(1, 1, 0);
        txt_fa2560k2_set_color(0x0f, 0x04);

        /* Set the resolution */
        txt_fa2560k2_set_resolution(640, 480);                  /* Default resolution is 640x480 */
    }

    /* Set the font */
    txt_fa2560k2_set_font(8, 8, MSX_CP437_8x8_bin);             /* Use 8x8 font */

    /*
     * Enable set_sizes, now that everything is set up initially
     * And calculate the size of the screen
     */
    self.enable_set_sizes = 1;
    txt_fa2560k2_set_sizes();

    /* Set region to default */
    region.origin.x = 0;
    region.origin.y = 0;
    region.size.width = 0;
    region.size.height = 0;
    txt_fa2560k2_set_region(&region);

    /* Home the cursor */
    txt_fa2560k2_set_xy(0, 0);

    /* Clear the screen */
    txt_fa2560k2_fill(' ');
}

/**
 * Initialize and install the driver
 *
 * @return 0 on success, any other number is an error
 */
short txt_fa2560k2_install() {
    t_txt_device device;

    INFO("txt_fa2560k2_install");
    device.number = TXT_SCREEN_FA2560K2;
    device.name = "FA2560K2 SCREEN";

    device.init = txt_fa2560k2_init;
    device.get_capabilities = txt_fa2560k2_get_capabilities;
    device.set_mode = txt_fa2560k2_set_mode;
    device.set_sizes = txt_fa2560k2_set_sizes;
    device.set_resolution = txt_fa2560k2_set_resolution;
    device.set_border = 0;
    device.set_border_color = 0;
    device.set_font = txt_fa2560k2_set_font;
    device.set_cursor = txt_fa2560k2_set_cursor;
    device.set_cursor_visible = txt_fa2560k2_set_cursor_visible;
    device.set_region = txt_fa2560k2_set_region;
    device.get_region = txt_fa2560k2_get_region;
    device.set_color = txt_fa2560k2_set_color;
    device.get_color = txt_fa2560k2_get_color;
    device.set_xy = txt_fa2560k2_set_xy;
    device.get_xy = txt_fa2560k2_get_xy;
    device.put = txt_fa2560k2_put;
    device.scroll = txt_fa2560k2_scroll;
    device.fill = txt_fa2560k2_fill;
    device.get_sizes = txt_fa2560k2_get_sizes;
    
    INFO("txt_fa2560k2_install_Done");
    return txt_register(&device);
}
