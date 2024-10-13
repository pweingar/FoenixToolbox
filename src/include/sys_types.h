//
// Types used in FoenixMCP system calls
//

#ifndef _sys_types_h_
#define _sys_types_h_

#include <stdbool.h>
#include <stdint.h>
#include "constants.h"

/**
 * @struct s_extent
 *
 * An extent or size of a rectangular area
 */
typedef struct s_extent {
    short width;        /**< The width of the region */
    short height;       /**< The height of the region */
} t_extent, *p_extent;

/**
 * @struct s_point
 *
 * A point on a plane
 */
typedef struct s_point {
    short x;                /**< The column of the point */
    short y;                /**< The row of the point */
} t_point, *p_point;

/**
 * @struct s_rect
 *
 * A rectangle on the screen
 */
typedef struct s_rect {
    t_point origin;         /**< The upper-left corner of the rectangle */
    t_extent size;          /**< The size of the rectangle */
} t_rect, *p_rect;

//
// A color (BGR)
//
typedef struct s_color3 {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} t_color3;

//
// A color entry for a color lookup table (BGRA)
//
typedef struct s_color4 {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
} t_color4;

/*
 * Function types
 */

typedef void  (*FUNC_V_2_V)();

/*
 * Type declaration for an interrupt handler
 */

typedef void (*p_int_handler)();

/*
 * Structure to describe the hardware
 */

typedef struct s_sys_info {
    uint16_t mcp_version;     		/* Current version of the MCP kernel */
    uint16_t mcp_rev;         		/* Current revision, or sub-version of the MCP kernel */
    uint16_t mcp_build;       		/* Current vuild # of the MCP kernel */
    uint16_t model;           		/* Code to say what model of machine this is */
    uint16_t sub_model;         	/* 0x00 = PB, 0x01 = LB, 0x02 = CUBE */
    const char * model_name;        /* Human readable name of the model of the computer */
    uint16_t cpu;             		/* Code to say which CPU is running */
    const char * cpu_name;          /* Human readable name for the CPU */
    uint32_t cpu_clock_khz;     	/* Speed of the CPU clock in KHz */
    unsigned long fpga_date;        /* YYYYMMDD */    
    uint16_t fpga_model;       		/* FPGA model number */
    uint16_t fpga_version;    		/* FPGA version */
    uint16_t fpga_subver;     		/* FPGA sub-version */
    uint32_t system_ram_size;		/* The number of bytes of system RAM on the board */
    bool has_floppy;                /* TRUE if the board has a floppy drive installed */
    bool has_hard_drive;            /* TRUE if the board has a PATA device installed */
    bool has_expansion_card;        /* TRUE if an expansion card is installed on the device */
    bool has_ethernet;              /* TRUE if an ethernet port is present */
    uint16_t screens;         		/* How many screens are on this computer */
} t_sys_info, *p_sys_info;

/*
 * Structure defining a channel device's functions
 */


/*
 * Structure defining a channel
 */

typedef struct s_channel {
    short number;                   // The number of the channel
    short dev;                      // The number of the channel's device
    uint8_t data[32];   // A block of state data that the channel code can use for its own purposes
} t_channel, *p_channel;

typedef struct s_dev_chan {
    short number;           // The number of the device (assigned by registration)
    char * name;            // The name of the device
    short (*init)();        // Initialize the device
    short (*open)(t_channel * chan, const uint8_t * path, short mode); //  -- open a channel for the device
    short (*close)(t_channel * chan);	// Called when a channel is closed
    short (*read)(t_channel * chan, uint8_t * buffer, short size);	// Read a a buffer from the device
    short (*readline)(t_channel * chan, uint8_t * buffer, short size);	// Read a line of text from the device
    short (*read_b)(t_channel * chan);      //  -- read a single uint8_t from the device
    short (*write)(t_channel * chan, const uint8_t * buffer, short size);	// Write a buffer to the device
    short (*write_b)(t_channel * chan, const uint8_t b) ; // Write a single uint8_t to the device
    short (*status)(t_channel * chan); // Get the status of the device
    short (*flush)(t_channel * chan); // Ensure that any pending writes to teh device have been completed
    short (*seek)(t_channel * chan, long position, short base); // Attempt to move the "cursor" position in the channel
    short (*ioctrl)(t_channel * chan, short command, uint8_t * buffer, short size); // Issue a control command to the device
} t_dev_chan, *p_dev_chan;

/*
 * Structure defining a block device's functions
 */

typedef struct s_dev_block {
    short number;           														// The number of the device (assigned by registration)
    char * name;            														// The name of the device
	void * data;																	// Device-specific data block
    short (*init)(struct s_dev_block *);        									// Initialize the device
    short (*read)(struct s_dev_block *, long lba, uint8_t * buffer, short size);	// Read a block from the device
    short (*write)(struct s_dev_block *, long lba, const uint8_t * buffer, short size);	// Write a block to the device
    short (*status)(struct s_dev_block *);      									// Get the status of the device
    short (*flush)(struct s_dev_block *);      										// Ensure that any pending writes to the device have been completed
    short (*ioctrl)(struct s_dev_block *, short command, unsigned char * buffer, short size);	// Issue a control command to the device
} t_dev_block, *p_dev_block;

/*
 * Type for directory information about a file
 */

typedef struct s_file_info {
    long size;
    unsigned short date;
    unsigned short time;
    unsigned char attributes;
    char name[MAX_PATH_LEN];
} t_file_info, * p_file_info;

/*
 * Pointer type for file loaders
 *
 * short loader(short chan, destination, start);
 */

typedef short (*p_file_loader)(short chan, long destination, long * start);

/*
 * Type to describe the current time
 */

typedef struct s_time {
    short year;
    short month;
    short day;
    short hour;
    short minute;
    short second;
    short is_pm;
    short is_24hours;
} t_time, *p_time;

/*
 * A description of a screen's capabilities
 */
 
typedef struct s_txt_capabilities {
    short number;               /**< The unique ID of the screen */
    short supported_modes;      /**< The display modes supported on this screen */
    short font_size_count;      /**< The number of supported font sizes */
    p_extent font_sizes;        /**< Pointer to a list of t_extent listing all supported font sizes */
    short resolution_count;     /**< The number of supported display resolutions */
    p_extent resolutions;       /**< Pointer to a list of t_extent listing all supported display resolutions (in pixels) */
} t_txt_capabilities, *p_txt_capabilities;

#endif