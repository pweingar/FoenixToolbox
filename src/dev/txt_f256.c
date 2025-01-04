/** @file txt_f256.c
 *
 * Text screen driver for f256
 */

#include <string.h>
#include <stdio.h>

#include "log.h"
#include "log_level.h"
#include "F256/vicky_ii.h"
#include "dev/txt_f256.h"
#include "dev/txt_screen.h"
#include "dev/uart.h"

#ifndef DEFAULT_LOG_LEVEL
    #define DEFAULT_LOG_LEVEL LOG_TRACE
#endif

extern const unsigned char MSX_CP437_8x8_bin[];

const t_color4 f256_clut[] = {
    {0, 0, 0},          // 0: Black
    {0, 0, 128},        // 1: Red
    {0, 128, 0},        // 2: Green
    {0, 128, 128},      // 3: Yellow
    {128, 0, 0},        // 4: Blue
    {128, 0, 128},      // 5: Magenta
    {128, 128, 0},      // 6: Cyan
    {192, 192, 192},    // 7: White
    {128, 128, 128},    // 8: Bright Gray
    {0, 0, 255},        // 9: Bright Red
    {0, 255, 0},        // A: Bright Green
    {0, 255, 255},      // B: Bright Yellow
    {255, 0, 0},        // C: Bright Blue
    {255, 0, 255},      // D: Bright Magenta
    {255, 255, 0},      // E: Bright Cyan
    {255, 255, 255}     // F: Bright White
};

/*
 * Driver level variables for the screen
 */

unsigned char f256_enable_set_sizes;    /* Flag to enable set_sizes to actually do its computation */

t_txt_capabilities f256_caps;           /* The capabilities of Channel A */

t_extent f256_resolutions[] = {   		/* The list of display resolutions */
	{ 640, 480 },
	{ 640, 400 },
    { 320, 240 },
	{ 320, 200 }
};

t_extent f256_fonts[] = {         		/* The list of supported font resolutions */
    { 8, 8 }
};

t_rect f256_region;                     /* The current region */
t_point f256_cursor;                    /* The current cursor position */
t_extent f256_resolution;               /* The current display resolution */
t_extent f256_font_size;                /* The current font size */
t_extent f256_max_size;                 /* The size of the screen in characters (without border removed) */
t_extent f256_visible_size;             /* The size of the visible screen in characters (with border removed) */
uint8_t f256_border_width = 0;          /* Width of the border on one side */
uint8_t f256_border_height = 0;         /* Height of the border on one side */
uint8_t f256_color = 0;            		/* The current color */
uint16_t mcr_shadow = 0;           		/* A shadow register for the Master Control Register */

/**
 * Gets the description of a screen's capabilities
 *
 * @return a pointer to the read-only description (0 on error)
 */
const p_txt_capabilities txt_f256_get_capabilities() {
    return &f256_caps;
}

/**
 * Calculate the size of the text screen in rows and columns so that
 * the kernel printing routines can work correctly.
 *
 * NOTE: this should be called whenever the VKY3 registers are changed
 */
static void txt_f256_set_sizes() {
	TRACE("txt_f256_set_sizes");
	
    if (f256_enable_set_sizes) {
        /* Only recalculate after initialization is mostly completed */

        /*
         * Calculate the maximum number of characters visible on the screen
         * This controls text layout in memory
         */
        f256_max_size.width = (int)((long)f256_resolution.width / (long)f256_font_size.width);
        f256_max_size.height = (int)((long)f256_resolution.height / (long)f256_font_size.height);

        /*
         * Calculate the characters that are visible in whole or in part
         */
        if ((f256_border_width != 0) && (f256_border_height != 0)) {
            short border_width = (int)((long)(2 * f256_border_width) / (long)f256_font_size.width);
            short border_height = (int)((long)(2 * f256_border_height) / (long)f256_font_size.height);

            f256_visible_size.width = f256_max_size.width - border_width;
            f256_visible_size.height = f256_max_size.height - border_height;
        } else {
            f256_visible_size.width = f256_max_size.width;            
            f256_visible_size.height = f256_max_size.height;
        }

        // DEBUG4("txt_f256_set_sizes max:%d,%d, visible:%d,%d", f256_max_size.width, f256_max_size.height, f256_visible_size.width, f256_visible_size.height);
    }
}

/**
 * Get the display resolutions
 *
 * @param text_size the size of the screen in visible characters (may be null)
 * @param pixel_size the size of the screen in pixels (may be null)
 */
static void txt_f256_get_sizes(p_extent text_size, p_extent pixel_size) {
    if (text_size) {
        text_size->width = f256_visible_size.width;
        text_size->height = f256_visible_size.height;
    }

    if (pixel_size) {
        pixel_size->width = f256_resolution.width;
        pixel_size->height = f256_resolution.height;
    }
}

/**
 * Set the display mode for the screen
 *
 * @param mode a bitfield of desired display mode options
 *
 * @return 0 on success, any other number means the mode is invalid for the screen
 */
static short txt_f256_set_mode(short mode) {
    /* Turn off anything not set */
    mcr_shadow &= ~(VKY_MCR_SLEEP | VKY_MCR_TEXT | VKY_MCR_TEXT_OVERLAY | VKY_MCR_GRAPHICS
		| VKY_MCR_BITMAP | VKY_MCR_TILE | VKY_MCR_SPRITE);

    if (mode & TXT_MODE_SLEEP) {
        /* Put the monitor to sleep: overrides all other option bits */
        mcr_shadow |= VKY_MCR_SLEEP;
        *tvky_mstr_ctrl = mcr_shadow;
        return 0;

    } else {
        if (mode & ~(TXT_MODE_TEXT | TXT_MODE_BITMAP | TXT_MODE_SPRITE | TXT_MODE_TILE)) {
            /* A mode bit was set beside one of the supported ones... */
            return -1;

        } else {
            if (mode & TXT_MODE_TEXT) {
                mcr_shadow |= VKY_MCR_TEXT;
            }

            if (mode & TXT_MODE_BITMAP) {
                mcr_shadow |= VKY_MCR_GRAPHICS | VKY_MCR_BITMAP;
            }

            if (mode & TXT_MODE_SPRITE) {
                mcr_shadow |= VKY_MCR_GRAPHICS | VKY_MCR_SPRITE;
            }

            if (mode & TXT_MODE_TILE) {
                mcr_shadow |= VKY_MCR_GRAPHICS | VKY_MCR_TILE;
            }

            if ((mcr_shadow & (VKY_MCR_GRAPHICS | VKY_MCR_TEXT)) == (VKY_MCR_GRAPHICS | VKY_MCR_TEXT)) {
                mcr_shadow |= VKY_MCR_TEXT_OVERLAY;
            }

            *tvky_mstr_ctrl = mcr_shadow;
			INFO1("Setting Vicky MCR: 0x%04x", mcr_shadow);
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
static short txt_f256_set_resolution(short width, short height) {
    // TODO: If no size specified, set it based on the DIP switch

    /* Turn off resolution bits */
    /* TODO: there gotta be a better way to do that */
    mcr_shadow &= ~(VKY_MCR_RES_MASK);

    if ((width == 640) && (height == 480)) {
        mcr_shadow |= VKY_MCR_RES_640x480;
        f256_resolution.width = width;
        f256_resolution.height = height;

        // Recalculate the size of the screen
        txt_f256_set_sizes();

        *tvky_mstr_ctrl = mcr_shadow;
        return 0;
    }
    else if ((width == 640) && (height == 400)) {
        mcr_shadow |= VKY_MCR_RES_640x400;
        f256_resolution.width = width;
        f256_resolution.height = height;

        // Recalculate the size of the screen
        txt_f256_set_sizes();

        *tvky_mstr_ctrl = mcr_shadow;
        return 0;
    }
    else if ((width == 320) && (height == 240)) {
        mcr_shadow |= VKY_MCR_RES_320x240;
        f256_resolution.width = width;
        f256_resolution.height = height;

        // Recalculate the size of the screen
        txt_f256_set_sizes();

        *tvky_mstr_ctrl = mcr_shadow;
        return 0;
    }
    else if ((width == 320) && (height == 200)) {
        mcr_shadow |= VKY_MCR_RES_320x200;
        f256_resolution.width = width;
        f256_resolution.height = height;

        // Recalculate the size of the screen
        txt_f256_set_sizes();

        *tvky_mstr_ctrl = mcr_shadow;
        return 0;
    }

    else {
        /* Unsupported resolution */
        return -1;
    }
}

/**
 * Set the size of the border of the screen (if supported)
 *
 * @param width the horizontal size of one side of the border (0 - 32 pixels)
 * @param height the vertical size of one side of the border (0 - 32 pixels)
 */
static void txt_f256_set_border(short width, short height) {
    if ((width > 0) || (height > 0)) {
        f256_border_width = width;
        f256_border_height = height;
        tvky_brdr_ctrl->control = 0x01;
        tvky_brdr_ctrl->size_x = width;
        tvky_brdr_ctrl->sizy_y = height;
        
    } else {
        tvky_brdr_ctrl->control = 0;
        tvky_brdr_ctrl->size_x = 0;
        tvky_brdr_ctrl->sizy_y = 0;
    }

    // Recalculate the size of the screen
    txt_f256_set_sizes();    
}

/**
 * Set the size of the border of the screen (if supported)
 *
 * @param red the red component of the color (0 - 255)
 * @param green the green component of the color (0 - 255)
 * @param blue the blue component of the color (0 - 255)
 */
static void txt_f256_set_border_color(unsigned char red, unsigned char green, unsigned char blue) {
    tvky_brdr_ctrl->color.red = red;
    tvky_brdr_ctrl->color.green = green;
    tvky_brdr_ctrl->color.blue = blue;
}

/**
 * Load a font as the current font for the screen
 *
 * @param width width of a character in pixels
 * @param height of a character in pixels
 * @param data pointer to the raw font data to be loaded
 */
static short txt_f256_set_font(short width, short height, const unsigned char * data) {
    if (width == 8 && height == 8) {
        int i;

        /* The size is valid... set the font */
        f256_font_size.width = width;
        f256_font_size.height = height;

        /* Copy the font data... this assumes a width of one byte! */
        /* TODO: generalize this for all possible font sizes */
        for (i = 0; i < 256 * height; i++) {
            tvky_font_set_0[i] = data[i];
        }

        // Recalculate the size of the screen
        txt_f256_set_sizes();

        return 0;

    } else {
        return -1;
    }
}

/**
 * Set the appearance of the cursor
 *
 * @param enable 0 to hide, any other number to make visible
 * @param rate the blink rate for the cursor (0=1s, 1=0.5s, 2=0.25s, 3=1/5s)
 * @param c the character in the current font to use as a cursor
 */
static void txt_f256_set_cursor(short enable, short rate, char c) {
    tvky_crsr_ctrl->control = ((rate & 0x03) << 1) | ((enable) ? 1 : 0);
    tvky_crsr_ctrl->character = c;
}

/**
 * Set if the cursor is visible or not
 *
 * @param enable 0 to hide, any other number to make visible
 */
static void txt_f256_set_cursor_visible(short enable) {
    if (enable) {
        tvky_crsr_ctrl->control |= 0x01;
    } else {
        tvky_crsr_ctrl->control &= ~0x01;
    }
}

/**
 * get the current region
 *
 * @param region pointer to a t_rect describing the rectangular region (using character cells for size and size)
 *
 * @return 0 on success, any other number means the region was invalid
 */
static short txt_f256_get_region(p_rect region) {
    region->origin.x = f256_region.origin.x;
    region->origin.y = f256_region.origin.y;
    region->size.width = f256_region.size.width;
    region->size.height = f256_region.size.height;

    return 0;
}

/**
 * Set a region to restrict further character display, scrolling, etc.
 * Note that a region of zero size will reset the region to the full size of the screen.
 *
 * @param region pointer to a t_rect describing the rectangular region (using character cells for size and size)
 *
 * @return 0 on success, any other number means the region was invalid
 */
static short txt_f256_set_region(const p_rect region) {    
    if ((region->size.width == 0) || (region->size.height == 0)) {
        /* Set the region to the default (full screen) */
        f256_region.origin.x = 0;
        f256_region.origin.y = 0;
        f256_region.size.width = f256_visible_size.width;
        f256_region.size.height = f256_visible_size.height;        
    } else {
        f256_region.origin.x = region->origin.x;
        f256_region.origin.y = region->origin.y;
        f256_region.size.width = region->size.width;
        f256_region.size.height = region->size.height;
    }

    return 0;
}

/**
 * Get the default foreground and background colors for printing
 *
 * @param pointer to the foreground the Text LUT index of the new current foreground color (0 - 15)
 * @param pointer to the background the Text LUT index of the new current background color (0 - 15)
 */
static short txt_f256_get_color(unsigned char * foreground, unsigned char * background) {
    *foreground = (f256_color & 0xf0) >> 4;
    *background = f256_color & 0x0f;
    return 0;
}

/**
 * Set the default foreground and background colors for printing
 *
 * @param foreground the Text LUT index of the new current foreground color (0 - 15)
 * @param background the Text LUT index of the new current background color (0 - 15)
 */
static short txt_f256_set_color(unsigned char foreground, unsigned char background) {
    f256_color = ((foreground & 0x0f) << 4) + (background & 0x0f);
    return 0;
}

extern void io_copy_down(uint16_t count, uint16_t dest, uint16_t src);
extern void io_copy_up(uint16_t count, uint16_t dest, uint16_t src);

/**
 * Copy data in the I/O space using the MVN/MVP instructions
 * 
 * @param count the number of bytes to copy
 * @param dest the address within the I/O bank to copy to
 * @param src the address within the I/O bank to copy from
 */
static void io_copy(uint16_t count, uint16_t dest, uint16_t src) {
	if (dest < src) {
		io_copy_down(count, dest, src);
	} else {
		io_copy_up(count, dest, src);
	}
}


/**
 * Scroll the screen for the most common case: full screen scrolls up by one full row.
 */
static void txt_f256_scroll_simple() {
	uint16_t rows = f256_max_size.height;
	uint16_t columns = f256_max_size.width;
	uint16_t count = (rows - 1) * columns;

	// Move the rows up
	uint16_t text_dest = (uint16_t)((uint32_t)tvky_text_matrix & 0xffff);
	uint16_t text_src = text_dest + columns;
	io_copy_down(count, text_dest, text_src); 

	uint16_t color_dest = (uint16_t)((uint32_t)tvky_color_matrix & 0xffff);
	uint16_t color_src = color_dest + columns;
	io_copy_down(count, color_dest, color_src); 

	// Clear the bottom line
	for (uint16_t i = count; i < rows * columns; i++) {
		tvky_text_matrix[i] = ' ';
		tvky_color_matrix[i] = f256_color;
	}
}

/**
 * Scroll the text in the current region
 * 
 * Supports the general case
 *
 * @param screen the number of the text device
 * @param horizontal the number of columns to scroll (negative is left, positive is right)
 * @param vertical the number of rows to scroll (negative is down, positive is up)
 */
static void txt_f256_scroll_complex(short horizontal, short vertical) {
	short x, x0, x1, x2, x3, dx;
    short y, y0, y1, y2, y3, dy;

    /*
     * Determine limits of rectangles to move and fill and directions of loops
     * x0 and y0 are the positions of the first cell to be over-written
     * x1 and y1 are the positions of the first cell to be copied... TEXT[x0,y0] := TEXT[x1,y1]
     * x2 and y2 are the position of the last cell to be over-written
     * x3 and y3 are the position of the last cell to be copied... TEXT[x2,y2] := TEXT[x3,y3]
     *
     * When blanking, the rectangles (x2,y0) - (x3,y3) and (x0,y2) - (x2,y3) are cleared
     */

    // Determine the row limits

    if (vertical >= 0) {
        y0 = f256_region.origin.y;
        y1 = y0 + vertical;
        y3 = f256_region.origin.y + f256_region.size.height;
        y2 = y3 - vertical;
        dy = 1;
    } else {
        y0 = f256_region.origin.y + f256_region.size.height - 1;
        y1 = y0 + vertical;
        y3 = f256_region.origin.y - 1;
        y2 = y3 - vertical;
        dy = -1;
    }

    // Determine the column limits

    if (horizontal >= 0) {
        x0 = f256_region.origin.x;
        x1 = x0 + horizontal;
        x3 = f256_region.origin.x + f256_region.size.width;
        x2 = x3 - horizontal;
        dx = 1;
    } else {
        x0 = f256_region.origin.x + f256_region.size.width - 1;
        x1 = x0 + horizontal;
        x3 = f256_region.origin.x - 1;
        x2 = x3 - horizontal;
        dx = -1;
    }
   
    /* Copy the rectangle. */
    int delta_y = dy * f256_max_size.width;
    int row_dst = y0 * f256_max_size.width - delta_y;
    int row_src = y0 * f256_max_size.width + vertical * f256_max_size.width - delta_y;
    for (y = y0; y != y2; y += dy) {
        row_dst += delta_y;
        row_src += delta_y;
        int offset_dst = row_dst + x0 - dx;
        int offset_src = row_src + horizontal + x0 - dx;

		// Move the rows up
		uint16_t count = x2 - x0;

		uint16_t text_dest = (uint16_t)((uint32_t)tvky_text_matrix & 0xffff) + offset_dst;
		uint16_t text_src =  (uint16_t)((uint32_t)tvky_text_matrix & 0xffff) + offset_src;
		io_copy(count, text_dest, text_src); 

		uint16_t color_dest = (uint16_t)((uint32_t)tvky_color_matrix & 0xffff) + offset_dst;
		uint16_t color_src = (uint16_t)((uint32_t)tvky_color_matrix & 0xffff) + offset_src;
		io_copy(count, color_dest, color_src);

        // for (x = x0; x != x2; x += dx) {
        //     offset_dst += dx;
        //     offset_src += dx;
        //     tvky_text_matrix[offset_dst] = tvky_text_matrix[offset_src];
        //     tvky_color_matrix[offset_dst] = tvky_color_matrix[offset_src];
        // }
    }

    /* Clear the rectangles */
    if (horizontal != 0) {
        row_dst = y0 * f256_max_size.width - delta_y;
        for (y = y0; y != y3; y += dy) {
            row_dst += delta_y;
            for (x = x2; x != x3; x += dx) {
                tvky_text_matrix[row_dst + x] = ' ';
                tvky_color_matrix[row_dst + x] = f256_color;
            }
        }
    }

    if (vertical != 0) {
        row_dst = y2 * f256_max_size.width - delta_y;
        for (y = y2; y != y3; y += dy) {
            row_dst += delta_y;
            for (x = x0; x != x3; x += dx) {
                tvky_text_matrix[row_dst + x] = ' ';
                tvky_color_matrix[row_dst + x] = f256_color;
            }
        }
    } 
}
/**
 * Scroll the text in the current region
 *
 * @param horizontal the number of columns to scroll (negative is left, positive is right)
 * @param vertical the number of rows to scroll (negative is down, positive is up)
 */
static void txt_f256_scroll(short horizontal, short vertical) {
	// If we're scrolling up one, and the region is the full screen, use a faster scrolling routine
	if ((horizontal == 0) && (vertical == 1) &&
		(f256_region.origin.x == 0) && (f256_region.origin.y == 0) &&
		(f256_region.size.width == f256_max_size.width) && (f256_region.size.height == f256_max_size.height)) {
		txt_f256_scroll_simple();
	} else {
		txt_f256_scroll_complex(horizontal, vertical);
	}
}

/**
 * Fill the current region with a character in the current color
 *
 * @param screen the number of the text device
 * @param c the character to fill the region with
 */
static void txt_f256_fill(char c) {
    int x;
    int y;

    for (y = 0; y < f256_region.size.height; y++) {
        int offset_row = ((f256_region.origin.y + y) * (int)f256_max_size.width);
        for (x = 0; x < f256_region.size.width; x++) {
            int offset = offset_row + f256_region.origin.x + x;
            tvky_text_matrix[offset] = c;
            tvky_color_matrix[offset] = f256_color;
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
static void txt_f256_set_xy(short x, short y) {
    /* Make sure X is within range for the current region... "print" a newline if not */
    if (x < 0) {
        x = 0;
    } else if (x >= f256_region.size.width) {
        x = 0;
        y++;
    }

    /* Make sure Y is within range for the current region... scroll if not */
    if (y < 0) {
        y = 0;
    } else if (y >= f256_region.size.height) {
        txt_f256_scroll(0, y - f256_region.size.height + 1);
        y = f256_region.size.height - 1;
    }

    f256_cursor.x = x;
    f256_cursor.y = y;

    /* Set register */
    tvky_crsr_ctrl->column = f256_region.origin.x + x;
    tvky_crsr_ctrl->row = f256_region.origin.y + y;
}

/**
 * Get the position of the cursor (x, y) relative to the current region
 *
 * @param screen the number of the text device
 * @param position pointer to a t_point record to fill out
 */
static void txt_f256_get_xy(p_point position) {
    position->x = f256_cursor.x;
    position->y = f256_cursor.y;
}

/**
 * Print a character to the current cursor position in the current color
 *
 * @param c the character to print
 */
static void txt_f256_put(char c) {
    short x;
    short y;
    unsigned int offset;

    x = f256_region.origin.x + f256_cursor.x;
    y = f256_region.origin.y + f256_cursor.y;
    offset = y * f256_max_size.width + x;
    tvky_text_matrix[offset] = c;
    tvky_color_matrix[offset] = f256_color;

    txt_f256_set_xy(f256_cursor.x + 1, f256_cursor.y);
}

/**
 * Initialize the screen
 */
static void txt_f256_init() {
	TRACE("txt_f256_init");

    char buffer[255];
    t_rect region;
    int i;

    f256_resolution.width = 0;
    f256_resolution.height = 0;
    f256_visible_size.width = 0;
    f256_visible_size.height = 0;
    f256_font_size.width = 0;
    f256_font_size.height = 0;

    /* Disable the set_sizes call for now, to avoid computing transcient unnecessary values */
    f256_enable_set_sizes = 0;

    /* Start with nothing on */
    mcr_shadow = 0;

    /* Define the capabilities */

    /* Specify the screen number. We have only one so... */
    f256_caps.number = TXT_SCREEN_F256;

    /* This screen can be text, bitmap or can be put to sleep */
    f256_caps.supported_modes = TXT_MODE_TEXT | TXT_MODE_SPRITE | TXT_MODE_BITMAP | TXT_MODE_TILE | TXT_MODE_SLEEP;

    /* Supported resolutions */
    f256_caps.resolution_count = sizeof(f256_resolutions) / sizeof(t_extent);
    f256_caps.resolutions = f256_resolutions;

    /* Only 8x8 on the U */
    f256_caps.font_size_count = sizeof(f256_fonts) / sizeof(t_extent);
    f256_caps.font_sizes = f256_fonts;

    /* Initialize the color lookup tables */
    for (i = 0; i < sizeof(f256_clut)/sizeof(t_color4); i++) {
		tvky_text_fg_color[i] = f256_clut[i];
		tvky_text_bg_color[i] = f256_clut[i];        
    }

    /* Set the mode to text */
    txt_f256_set_mode(TXT_MODE_TEXT);

    /* Set the resolution */
    txt_f256_set_resolution(640, 480);      

    /* Set the default color: light grey on blue */
    txt_f256_set_color(0x07, 0x04);

    /* Set the font */
    txt_f256_set_font(8, 8, MSX_CP437_8x8_bin);             /* Use 8x8 font */

    /* Set the cursor */
    txt_f256_set_cursor(1, 0, 0xB1);

    /* Set the border */
    txt_f256_set_border(0, 0);                              /* Set up the border */
    txt_f256_set_border_color(0xf0, 0, 0);

    /*
     * Enable set_sizes, now that everything is set up initially
     * And calculate the size of the screen
     */
    f256_enable_set_sizes = 1;
    txt_f256_set_sizes();

    /* Set region to default */
    region.origin.x = 0;
    region.origin.y = 0;
    region.size.width = 0;
    region.size.height = 0;
    txt_f256_set_region(&region);

    /* Clear the screen */
    txt_f256_fill(' ');

    /* Home the cursor */
    txt_f256_set_xy(0, 0);
}

/**
 * Initialize and install the driver
 *
 * @return 0 on success, any other number is an error
 */
short txt_f256_install() {
    t_txt_device device;

    device.number = TXT_SCREEN_F256;
    device.name = "SCREEN";

    device.init = txt_f256_init;
    device.get_capabilities = txt_f256_get_capabilities;
    device.set_mode = txt_f256_set_mode;
    device.set_sizes = txt_f256_set_sizes;
    device.set_resolution = txt_f256_set_resolution;
    device.set_border = txt_f256_set_border;
    device.set_border_color = txt_f256_set_border_color;
    device.set_font = txt_f256_set_font;
    device.set_cursor = txt_f256_set_cursor;
    device.set_cursor_visible = txt_f256_set_cursor_visible;
    device.get_region = txt_f256_get_region;
    device.set_region = txt_f256_set_region;
    device.get_color = txt_f256_get_color;
    device.set_color = txt_f256_set_color;
    device.set_xy = txt_f256_set_xy;
    device.get_xy = txt_f256_get_xy;
    device.put = txt_f256_put;
    device.scroll = txt_f256_scroll;
    device.fill = txt_f256_fill;
    device.get_sizes = txt_f256_get_sizes;

    return txt_register(&device);
}
