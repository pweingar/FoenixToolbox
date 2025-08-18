/**
 * 
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "log.h"
#include "log_level.h"
#include "txt_mem.h"
#include "txt_screen.h"
#include "vicky_general.h"
#include "memtext_reg.h"

// #include "F256/dma_f256.h"

//
// Types
//

/**
 * Structure to hold the information describing the 2D DMA transfer to perform
 */
typedef struct dma_2d_cmd_s {
    uint16_t * destination;
    uint16_t * source;
    uint16_t width;
    uint16_t height;
    uint16_t dest_stride;
    uint16_t src_stride;
} dma_2d_cmd_t, *dma_2d_cmd_p;

//
// Constants
//

/**
 * Pointers to our actual memory locations..
 * TODO: handle these more automatically
 */
#define text_matrix     ((uint16_t *)0x100000)
#define color_matrix    ((uint16_t *)0x108000)

#define VKY_MCR_MEMTEXT 0x4000

extern const unsigned char MSX_CP437_8x8_bin[];

const t_color4 mem_clut[] = {
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

const t_extent mem_resolutions[] = {   		/* The list of display resolutions */
#if MODEL == MODEL_FOENIX_F256K2 || MODEL == MODEL_FOENIX_F256K2X
    { 640, 480 },
	{ 640, 400 },
    { 320, 240 },
	{ 320, 200 }
#elif MODEL == MODEL_FOENIX_FA2560K2
    { 1024, 768 }
#endif
};


const t_extent mem_fonts[] = {         		/* The list of supported font resolutions */
    { 8, 8 },
    { 8, 16 }
};

//
// Driver level variables for the screen
//

unsigned char mem_enable_set_sizes;     /* Flag to enable set_sizes to actually do its computation */

t_txt_capabilities mem_caps;            /* The capabilities of Channel A */

static t_rect mem_region;               /* The current region */
static t_point mem_cursor;              /* The current cursor position */
static t_extent mem_resolution;         /* The current display resolution */
static t_extent mem_font_size;          /* The current font size */
static t_extent mem_max_size;           /* The size of the screen in characters (without border removed) */
static t_extent mem_visible_size;       /* The size of the visible screen in characters (with border removed) */
static uint8_t mem_border_width = 0;    /* Width of the border on one side */
static uint8_t mem_border_height = 0;   /* Height of the border on one side */
static uint16_t mem_color = 0;          /* The current color */
static uint8_t mem_attribute = 0;       /* The default attribute for a character */
static uint16_t mem_mcr_shadow = 0;         /* A shadow register for the Master Control Register */

#if MODEL == MODEL_FOENIX_F256K2 || MODEL == MODEL_FOENIX_F256K2X
static __attribute__((aligned(16))) uint16_t mem_text_matrix[80*60];
static __attribute__((aligned(16))) uint16_t mem_color_matrix[80*60];
#elif MODEL == MODEL_FOENIX_FA2560K2
static __attribute__((aligned(16))) uint16_t mem_text_matrix[128*96];
static __attribute__((aligned(16))) uint16_t mem_color_matrix[128*96];
#endif

//
// Code for the driver
//

/**
 * Use DMA to copy a rectangular block of memory from one location to another
 * 
 * @param dma_cmd pointer to a description of the block transfer to perform
 * @return 0 on success, any other number is an error
 */
extern short dma_copyw_2d(dma_2d_cmd_p dma_cmd);

/**
 * Gets the description of a screen's capabilities
 *
 * @return a pointer to the read-only description (0 on error)
 */
const p_txt_capabilities txt_mem_get_capabilities() {
    return &mem_caps;
}

/**
 * Calculate the size of the text screen in rows and columns so that
 * the kernel printing routines can work correctly.
 *
 * NOTE: this should be called whenever the VKY3 registers are changed
 */
static void txt_mem_set_sizes() {
	TRACE("txt_mem_set_sizes");
	
    if (mem_enable_set_sizes) {
        /* Only recalculate after initialization is mostly completed */

        /*
         * Calculate the maximum number of characters visible on the screen
         * This controls text layout in memory
         */
        mem_max_size.width = (int)((long)mem_resolution.width / (long)mem_font_size.width);
        mem_max_size.height = (int)((long)mem_resolution.height / (long)mem_font_size.height);

        /*
         * Calculate the characters that are visible in whole or in part
         */
        if ((mem_border_width != 0) && (mem_border_height != 0)) {
            short border_width = (int)((long)(2 * mem_border_width) / (long)mem_font_size.width);
            short border_height = (int)((long)(2 * mem_border_height) / (long)mem_font_size.height);

            mem_visible_size.width = mem_max_size.width - border_width;
            mem_visible_size.height = mem_max_size.height - border_height;
        } else {
            mem_visible_size.width = mem_max_size.width;            
            mem_visible_size.height = mem_max_size.height;
        }
    }
}

/**
 * Get the display resolutions
 *
 * @param text_size the size of the screen in visible characters (may be null)
 * @param pixel_size the size of the screen in pixels (may be null)
 */
static void txt_mem_get_sizes(p_extent text_size, p_extent pixel_size) {
    if (text_size) {
        text_size->width = mem_visible_size.width;
        text_size->height = mem_visible_size.height;
    }

    if (pixel_size) {
        pixel_size->width = mem_resolution.width;
        pixel_size->height = mem_resolution.height;
    }
}

/**
 * Set the display mode for the screen
 *
 * @param mode a bitfield of desired display mode options
 *
 * @return 0 on success, any other number means the mode is invalid for the screen
 */
static short txt_mem_set_mode(short mode) {
    /* Turn off anything not set */
    mem_mcr_shadow &= ~(VKY_MCR_SLEEP | VKY_MCR_TEXT | VKY_MCR_TEXT_OVERLAY | VKY_MCR_GRAPHICS
		| VKY_MCR_BITMAP | VKY_MCR_TILE | VKY_MCR_SPRITE | VKY_MCR_MEMTEXT);

    MEMTEXT->control = 0;

    if (mode & TXT_MODE_SLEEP) {
        /* Put the monitor to sleep: overrides all other option bits */
        mem_mcr_shadow |= VKY_MCR_SLEEP;
        *tvky_mstr_ctrl = mem_mcr_shadow;
        return 0;

    } else {
        if (mode & ~(TXT_MODE_TEXT | TXT_MODE_BITMAP | TXT_MODE_SPRITE | TXT_MODE_TILE)) {
            /* A mode bit was set beside one of the supported ones... */
            return -1;

        } else {
            if (mode & TXT_MODE_TEXT) {
                mem_mcr_shadow |= VKY_MCR_MEMTEXT | VKY_MCR_TEXT;

                if (mem_font_size.height == 16) {
                    MEMTEXT->enable = 1;
                    MEMTEXT->size8x16 = 1;
                } else {
                    MEMTEXT->enable = 1;
                }
            }

            if (mode & TXT_MODE_BITMAP) {
                mem_mcr_shadow |= VKY_MCR_GRAPHICS | VKY_MCR_BITMAP;
            }

            if (mode & TXT_MODE_SPRITE) {
                mem_mcr_shadow |= VKY_MCR_GRAPHICS | VKY_MCR_SPRITE;
            }

            if (mode & TXT_MODE_TILE) {
                mem_mcr_shadow |= VKY_MCR_GRAPHICS | VKY_MCR_TILE;
            }

            if ((mem_mcr_shadow & (VKY_MCR_GRAPHICS | VKY_MCR_TEXT)) == (VKY_MCR_GRAPHICS | VKY_MCR_MEMTEXT)) {
                mem_mcr_shadow |= VKY_MCR_TEXT_OVERLAY;
            }

            *tvky_mstr_ctrl = mem_mcr_shadow;
			INFO1("Setting Vicky MCR: 0x%04x", mem_mcr_shadow);
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
static short txt_mem_set_resolution(short width, short height) {
    // TODO: If no size specified, set it based on the DIP switch

    /* Turn off resolution bits */
    /* TODO: there gotta be a better way to do that */
    mem_mcr_shadow &= ~(VKY_MCR_RES_MASK);

    if ((width == 640) && (height == 480)) {
        mem_mcr_shadow |= VKY_MCR_RES_640x480;
        mem_resolution.width = width;
        mem_resolution.height = height;

        // Recalculate the size of the screen
        txt_mem_set_sizes();

        *tvky_mstr_ctrl = mem_mcr_shadow;
        return 0;
    }
    else if ((width == 640) && (height == 400)) {
        mem_mcr_shadow |= VKY_MCR_RES_640x400;
        mem_resolution.width = width;
        mem_resolution.height = height;

        // Recalculate the size of the screen
        txt_mem_set_sizes();

        *tvky_mstr_ctrl = mem_mcr_shadow;
        return 0;
    }
    else if ((width == 320) && (height == 240)) {
        mem_mcr_shadow |= VKY_MCR_RES_320x240;
        mem_resolution.width = width;
        mem_resolution.height = height;

        // Recalculate the size of the screen
        txt_mem_set_sizes();

        *tvky_mstr_ctrl = mem_mcr_shadow;
        return 0;
    }
    else if ((width == 320) && (height == 200)) {
        mem_mcr_shadow |= VKY_MCR_RES_320x200;
        mem_resolution.width = width;
        mem_resolution.height = height;

        // Recalculate the size of the screen
        txt_mem_set_sizes();

        *tvky_mstr_ctrl = mem_mcr_shadow;
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
static void txt_mem_set_border(short width, short height) {
    if ((width > 0) || (height > 0)) {
        mem_border_width = width;
        mem_border_height = height;
        tvky_brdr_ctrl->control = 0x01;
        tvky_brdr_ctrl->size_x = width;
        tvky_brdr_ctrl->sizy_y = height;
        
    } else {
        tvky_brdr_ctrl->control = 0;
        tvky_brdr_ctrl->size_x = 0;
        tvky_brdr_ctrl->sizy_y = 0;
    }

    // Recalculate the size of the screen
    txt_mem_set_sizes();    
}

/**
 * Set the size of the border of the screen (if supported)
 *
 * @param red the red component of the color (0 - 255)
 * @param green the green component of the color (0 - 255)
 * @param blue the blue component of the color (0 - 255)
 */
static void txt_mem_set_border_color(unsigned char red, unsigned char green, unsigned char blue) {
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
static short txt_mem_set_font(short width, short height, const unsigned char * data) {
    if (width == 8 && ((height == 8) || (height == 16))) {
        int i;

        // Calculate the starting position of the font
        // 8x8 goes into slot #0, 8x16 goes in slots 2 and 3
        int slot = (height == 8) ? 0 : 2;
        int offset = slot * 256 * 8;

        /* The size is valid... set the font */
        mem_font_size.width = width;
        mem_font_size.height = height;

        // /* Copy the font data... this assumes a width of one byte! */
        // /* TODO: generalize this for all possible font sizes */
        // for (i = 0; i < 256 * height; i++) {
        //     MEMTEXT_FONT[offset + i] = data[i];
        // }

        // if (mem_font_size.height == 16) {
        //     *MEMTEXT_MAIN_CTRL = MEMTEXT_MAIN_EN | MEMTEXT_MAIN_8x16;
        // } else {
        //     *MEMTEXT_MAIN_CTRL = MEMTEXT_MAIN_EN;
        // }

        // Recalculate the size of the screen
        txt_mem_set_sizes();

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
static void txt_mem_set_cursor(short enable, short rate, char c) {
    // TODO: Setup cursor
}

/**
 * Set if the cursor is visible or not
 *
 * @param enable 0 to hide, any other number to make visible
 */
static void txt_mem_set_cursor_visible(short enable) {
    // TODO: set cursor visibilty
}

/**
 * get the current region
 *
 * @param region pointer to a t_rect describing the rectangular region (using character cells for size and size)
 *
 * @return 0 on success, any other number means the region was invalid
 */
static short txt_mem_get_region(p_rect region) {
    region->origin.x = mem_region.origin.x;
    region->origin.y = mem_region.origin.y;
    region->size.width = mem_region.size.width;
    region->size.height = mem_region.size.height;

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
static short txt_mem_set_region(const p_rect region) {    
    if ((region->size.width == 0) || (region->size.height == 0)) {
        /* Set the region to the default (full screen) */
        mem_region.origin.x = 0;
        mem_region.origin.y = 0;
        mem_region.size.width = mem_visible_size.width;
        mem_region.size.height = mem_visible_size.height;        
    } else {
        mem_region.origin.x = region->origin.x;
        mem_region.origin.y = region->origin.y;
        mem_region.size.width = region->size.width;
        mem_region.size.height = region->size.height;
    }

    return 0;
}

/**
 * Get the default foreground and background colors for printing
 * 
 * TODO: maybe adjust to support 256 indexes
 *
 * @param pointer to the foreground the Text LUT index of the new current foreground color (0 - 255)
 * @param pointer to the background the Text LUT index of the new current background color (0 - 255)
 */
static short txt_mem_get_color(unsigned char * foreground, unsigned char * background) {
    *foreground = (mem_color & 0xff00) >> 8;
    *background = mem_color & 0xff;
    return 0;
}

/**
 * Set the default foreground and background colors for printing
 * 
 * TODO: maybe adjust to support 256 indexes
 *
 * @param foreground the Text LUT index of the new current foreground color (0 - 255)
 * @param background the Text LUT index of the new current background color (0 - 255)
 */
static short txt_mem_set_color(unsigned char foreground, unsigned char background) {
    mem_color = ((foreground & 0xff) << 8) + (background & 0xff);
    return 0;
}

/**
 * Fill a rectangular area with a 16-bit value
 * 
 * @param dest the starting address to receive the data
 * @param src the starting address of the source data
 * @param size the number of 16-bit words to copy
 */
static void dma_copy_16_1d(uint16_t * dest, uint16_t *src, uint32_t size) {
    static short count = 0;
    short index = 0;

    if (++count > 99) count = 0;
    for (int i = 0; i < 10; i++) {
        mem_text_matrix[i] = ' ';
    }

    mem_text_matrix[index++] = (count / 10) + '0';
    mem_text_matrix[index++] = (count % 10) + '0';

    index++;
    mem_text_matrix[index++] = '0';

    *DMA_CTRL = DMA_CTRL_EN | DMA_16BIT_EN;

    mem_text_matrix[index++] = '1';

    *DMA_SRC_ADDR = src;
    *DMA_DST_ADDR = dest;
    *DMA_SIZE = size;

    mem_text_matrix[index++] = '2';

    // Start the DMA operation
    *DMA_CTRL = DMA_CTRL_EN | DMA_16BIT_EN | DMA_CTRL_TRF;

    mem_text_matrix[index++] = '3';

    for (int i = 0; i < 100; i++) ;

    mem_text_matrix[index++] = '4';

    // Wait for the DMA operation to finish
    while ((*DMA_STAT & DMA_STAT_TFR_BUSY) == DMA_STAT_TFR_BUSY) ;

    mem_text_matrix[index++] = '5';

    // Shut down the DMA operation
    *DMA_CTRL = 0;

    mem_text_matrix[index++] = '6';
}

/**
 * Fill a rectangular area with a 16-bit value
 * 
 * @param dest the starting address to receive the data
 * @param src the starting address of the source data
 * @param width the width of the rectangular area to fill (number of 16-bit words)
 * @param height the height of the rectangular area to fill (number of 16-bit words)
 * @param stride the number of 16-bit words that compose a row of the over all rectangular data in memory
 */
void dma_copy_16_2d(uint16_t * dest, uint16_t *src, short width, short height, short dest_stride, short src_stride) {
    dma_2d_cmd_t dma_cmd;
    dma_cmd.destination = dest;
    dma_cmd.source = src;
    dma_cmd.width = width;
    dma_cmd.height = height;
    dma_cmd.dest_stride = dest_stride;
    dma_cmd.src_stride = src_stride;

    dma_copyw_2d(&dma_cmd);
}

/**
 * Scroll the screen for the most common case: full screen scrolls up by one full row.
 * 
 * TODO: reimplement using DMA
 * 
 */
static void txt_mem_scroll_simple() {
	uint16_t rows = mem_max_size.height;
	uint16_t columns = mem_max_size.width;
	uint16_t count = (rows - 1) * columns;

    for (int i = 0; i < count; i++) {
        mem_text_matrix[i] = mem_text_matrix[i + columns];
        mem_color_matrix[i] = mem_color_matrix[i + columns];
    }
}

/**
 * Scroll the text in the current region
 * 
 * TODO: reimplement using DMA
 * 
 * Supports the general case
 *
 * @param screen the number of the text device
 * @param horizontal the number of columns to scroll (negative is left, positive is right)
 * @param vertical the number of rows to scroll (negative is down, positive is up)
 */
static void txt_mem_scroll_complex(short horizontal, short vertical) {
	short x, x0, x1, x2, x3, dx;
    short y, y0, y1, y2, y3, dy;
    uint16_t * src = 0;
    uint16_t * dest = 0;

    if (vertical != 0) {
        short width = mem_region.size.width;
        short height = mem_region.size.height;
        short top = mem_region.origin.y;
        short left = mem_region.origin.x;

        if (vertical > 0) {
            // Scroll up
            short bottom_row = top + height - 1;
            short bottom_base = bottom_row * mem_max_size.width;

            for (int row = top; row < bottom_row; row++) {
                short dst_base = row * mem_max_size.width;
                short src_base = dst_base + mem_max_size.width;
                for (int column = left; column < left + width; column++) {
                    mem_text_matrix[dst_base + column] = mem_text_matrix[src_base + column];
                    mem_color_matrix[dst_base + column] = mem_color_matrix[src_base + column];
                }
            }

            // Clear the row
            for (short i = 0; i < width; i++) {
                mem_text_matrix[bottom_base + left + i] = ' '; 
                mem_color_matrix[bottom_base + left + i] = mem_color;             
            }
        }
    }

    // TODO: implement horizontal scrolling
 
}
/**
 * Scroll the text in the current region
 * 
 * TODO: reimplement using DMA
 *
 * @param horizontal the number of columns to scroll (negative is left, positive is right)
 * @param vertical the number of rows to scroll (negative is down, positive is up)
 */
static void txt_mem_scroll(short horizontal, short vertical) {
	// If we're scrolling up one, and the region is the full screen, use a faster scrolling routine
	if ((horizontal == 0) && (vertical == 1) &&
		(mem_region.origin.x == 0) && (mem_region.origin.y == 0) &&
		(mem_region.size.width == mem_max_size.width) && (mem_region.size.height == mem_max_size.height)) {
		txt_mem_scroll_simple();
	} else {
		txt_mem_scroll_complex(horizontal, vertical);
	}
}

/**
 * Fill the current region with a character in the current color
 * 
 * TODO: reimplement using DMA
 *
 * @param screen the number of the text device
 * @param c the character to fill the region with
 */
static void txt_mem_fill(char c) {
    int x;
    int y;

    for (y = 0; y < mem_region.size.height; y++) {
        int offset_row = ((mem_region.origin.y + y) * (int)mem_max_size.width);
        for (x = 0; x < mem_region.size.width; x++) {
            int offset = offset_row + mem_region.origin.x + x;
            mem_text_matrix[offset] = c;
            mem_color_matrix[offset] = mem_color;
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
static void txt_mem_set_xy(short x, short y) {
    /* Make sure X is within range for the current region... "print" a newline if not */
    if (x < 0) {
        x = 0;
    } else if (x >= mem_region.size.width) {
        x = 0;
        y++;
    }

    /* Make sure Y is within range for the current region... scroll if not */
    if (y < 0) {
        y = 0;
    } else if (y >= mem_region.size.height) {
        txt_mem_scroll(0, y - mem_region.size.height + 1);
        y = mem_region.size.height - 1;
    }

    mem_cursor.x = x;
    mem_cursor.y = y;

    /* Set register */
    // TODO: set 
    *MEMTEXT_CRSR_X = mem_region.origin.x + x;
    *MEMTEXT_CRSR_Y = mem_region.origin.y + y;
}

/**
 * Get the position of the cursor (x, y) relative to the current region
 *
 * @param screen the number of the text device
 * @param position pointer to a t_point record to fill out
 */
static void txt_mem_get_xy(p_point position) {
    position->x = mem_cursor.x;
    position->y = mem_cursor.y;
}

/**
 * Print a character to the current cursor position in the current color
 *
 * @param c the character to print
 */
static void txt_mem_put(char c) {
    short x;
    short y;
    unsigned int offset;

    x = mem_region.origin.x + mem_cursor.x;
    y = mem_region.origin.y + mem_cursor.y;
    offset = y * mem_max_size.width + x;
    mem_text_matrix[offset] = (c & 0xff);
    mem_color_matrix[offset] = mem_color;

    txt_mem_set_xy(mem_cursor.x + 1, mem_cursor.y);
}

/**
 * Initialize the screen
 */
static void txt_mem_init() {
	TRACE("txt_mem_init");

    char buffer[255];
    t_rect region;
    int i;

    mem_resolution.width = 0;
    mem_resolution.height = 0;
    mem_visible_size.width = 0;
    mem_visible_size.height = 0;
    mem_font_size.width = 0;
    mem_font_size.height = 0;

    /* Disable the set_sizes call for now, to avoid computing transcient unnecessary values */
    mem_enable_set_sizes = 0;

    /* Start with nothing on */
    mem_mcr_shadow = 0;

    /* Define the capabilities */

    /* Specify the screen number. We have only one so... */
    mem_caps.number = TXT_SCREEN_MEM_F256;

    /* This screen can be text, bitmap or can be put to sleep */
    mem_caps.supported_modes = TXT_MODE_TEXT | TXT_MODE_SPRITE | TXT_MODE_BITMAP | TXT_MODE_TILE | TXT_MODE_SLEEP;

    /* Supported resolutions */
    mem_caps.resolution_count = sizeof(mem_resolutions) / sizeof(t_extent);
    mem_caps.resolutions = mem_resolutions;

    /* Only 8x8 on the U */
    mem_caps.font_size_count = sizeof(mem_fonts) / sizeof(t_extent);
    mem_caps.font_sizes = mem_fonts;

    /* Initialize the color lookup tables */
    for (i = 0; i < sizeof(mem_clut)/sizeof(t_color4); i++) {
		MEMTEXT_FG[i] = mem_clut[i];
		MEMTEXT_BG[i] = mem_clut[i];        
    }

    *MEMTEXT_CHAR_PTR = mem_text_matrix;
    *MEMTEXT_COLOR_PTR = mem_color_matrix;

    /* Set the mode to text */
    txt_mem_set_mode(TXT_MODE_TEXT);

    /* Set the resolution */
#if MODEL == MODEL_FOENIX_F256K2 || MODEL == MODEL_FOENIX_F256K2X
    txt_mem_set_resolution(640, 480);
#elif MODEL == MODEL_FOENIX_FA256K2
    txt_mem_set_resolution(1024, 768);
#endif


    /* Set the default color: light grey on blue */
    txt_mem_set_color(0x07, 0x04);

    /* Set the font */
    txt_mem_set_font(8, 8, MSX_CP437_8x8_bin);             /* Use 8x8 font */

    /* Set the cursor */
    txt_mem_set_cursor(1, 0, 0xB1);

    /* Set the border */
    txt_mem_set_border(0, 0);                              /* Set up the border */
    txt_mem_set_border_color(0xf0, 0, 0);

    /*
     * Enable set_sizes, now that everything is set up initially
     * And calculate the size of the screen
     */
    mem_enable_set_sizes = 1;
    txt_mem_set_sizes();

    /* Set region to default */
    region.origin.x = 0;
    region.origin.y = 0;
    region.size.width = 0;
    region.size.height = 0;
    txt_mem_set_region(&region);

    /* Clear the screen */
    txt_mem_fill('+');

    /* Home the cursor */
    txt_mem_set_xy(0, 0);
}

/**
 * Initialize and install the driver
 *
 * @return 0 on success, any other number is an error
 */
short txt_mem_install() {
    t_txt_device device;

    device.number = TXT_SCREEN_MEM_F256;
    device.name = "MEMTEXT";

    device.init = txt_mem_init;
    device.get_capabilities = txt_mem_get_capabilities;
    device.set_mode = txt_mem_set_mode;
    device.set_sizes = txt_mem_set_sizes;
    device.set_resolution = txt_mem_set_resolution;
    device.set_border = txt_mem_set_border;
    device.set_border_color = txt_mem_set_border_color;
    device.set_font = txt_mem_set_font;
    device.set_cursor = txt_mem_set_cursor;
    device.set_cursor_visible = txt_mem_set_cursor_visible;
    device.get_region = txt_mem_get_region;
    device.set_region = txt_mem_set_region;
    device.get_color = txt_mem_get_color;
    device.set_color = txt_mem_set_color;
    device.set_xy = txt_mem_set_xy;
    device.get_xy = txt_mem_get_xy;
    device.put = txt_mem_put;
    device.scroll = txt_mem_scroll;
    device.fill = txt_mem_fill;
    device.get_sizes = txt_mem_get_sizes;

    return txt_register(&device);
}

void txt_mem_test() {
    txt_set_mode(TXT_SCREEN_MEM_F256, TXT_MODE_TEXT);
    
    t_rect region;
    region.origin.x = 5;
    region.origin.y = 5;
    region.size.width = 70;
    region.size.height = 50;
    txt_set_region(TXT_SCREEN_MEM_F256, &region);

    txt_set_xy(TXT_SCREEN_MEM_F256, 0, 0);
    txt_fill(TXT_SCREEN_MEM_F256, 'X');

    int x = 0;
    int direction = 1;

    char * message = "Hello, world!";
    int message_length = strlen(message);

    for (int i = 0; i < 10000; i++) {
        txt_set_xy(TXT_SCREEN_MEM_F256, x, region.size.height - 1);
        txt_print(TXT_SCREEN_MEM_F256, message);
        txt_scroll(TXT_SCREEN_MEM_F256, 0, 1);

        x = x + direction;
        if (x < 0) {
            direction = 1;
            x = 1;
        } else if (x + message_length >= region.size.width) {
            x = region.size.width - message_length - 1;
            direction = -1;
        }
    }

    while (1) ;
}