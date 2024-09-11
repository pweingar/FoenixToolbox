/**
 * @file toolbox.h
 * @brief Public-facing calls for the Foenix Toolbox
 * @version 0.1
 * @date 2024-09-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __toolbox_h__
#define __toolbox_h__

#include <stdbool.h>
#include <stdint.h>
#include "constants.h"
#include "sys_macros.h"
#include "sys_types.h"

/**
 * Quit the current user process
 *
 * NOTE: at the moment, this relaunches the CLI. In the future, this
 *       may cause execution to return to the program that started
 *       the user process.
 *
 * @param result the code to return to the kernel
 */
extern SYSTEMCALL void sys_exit(short result);

/**
 * Enable all interrupts
 */
extern SYSTEMCALL void sys_int_enable_all();

/**
 * Disable all interrupts
 */
extern SYSTEMCALL void sys_int_disable_all();

/**
 * Disable an interrupt by masking it
 *
 * @param n the number of the interrupt: n[7..4] = group number, n[3..0] = individual number.
 */
extern SYSTEMCALL void sys_int_disable(unsigned short n);

/**
 * Enable an interrupt
 *
 * @param n the number of the interrupt
 */
extern SYSTEMCALL void sys_int_enable(unsigned short n);

/**
 * Register a handler for a given interrupt.
 *
 * @param n the number of the interrupt
 * @param handler pointer to the interrupt handler to register
 *
 * @return the pointer to the previous interrupt handler
 */
extern SYSTEMCALL p_int_handler sys_int_register(unsigned short n, p_int_handler handler);

/**
 * Return true (non-zero) if an interrupt is pending for the given interrupt
 *
 * @param n the number of the interrupt: n[7..4] = group number, n[3..0] = individual number.
 *
 * @return non-zero if interrupt n is pending, 0 if not
 */
extern SYSTEMCALL short sys_int_pending(unsigned short n);

/**
 * Fill out a s_sys_info structure with the information about the current system
 *
 * @param info pointer to a s_sys_info structure to fill out
 */
extern SYSTEMCALL void sys_get_info(p_sys_info info);

/**
 * Acknowledge an interrupt (clear out its pending flag)
 *
 * @param n the number of the interrupt: n[7..4] = group number, n[3..0] = individual number.
 */
extern SYSTEMCALL void sys_int_clear(unsigned short n);

/***
 *** Channel system calls
 ***/

/**
 * Read a single byte from the channel
 *
 * @param channel the number of the channel
 *
 * @return the value read (if negative, error)
 */
extern SYSTEMCALL short sys_chan_read_b(short channel);

/**
 * Read bytes from the channel
 *
 * @param channel the number of the channel
 * @param buffer the buffer into which to copy the channel data
 * @param size the size of the buffer.
 *
 * @return number of bytes read, any negative number is an error code
 */
extern SYSTEMCALL short sys_chan_read(short channel, unsigned char * buffer, short size);

/**
 * Read a line of text from the channel
 *
 * @param channel the number of the channel
 * @param buffer the buffer into which to copy the channel data
 * @param size the size of the buffer
 *
 * @return number of bytes read, any negative number is an error code
 */
extern SYSTEMCALL short sys_chan_readline(short channel, unsigned char * buffer, short size);

/**
 * Write a single byte to the device
 *
 * @param channel the number of the channel
 * @param b the byte to write
 *
 * @return 0 on success, a negative value on error
 */
extern SYSTEMCALL short sys_chan_write_b(short channel, uint8_t b);

/**
 * Write a byte to the channel
 *
 * @param channel the number of the channel
 * @param b the byte to write
 *
 * @return number of bytes written, any negative number is an error code
 */
extern SYSTEMCALL short sys_chan_write(short channel, const uint8_t * buffer, short size);

/**
 * Return the status of the channel device
 *
 * @param channel the number of the channel
 *
 * @return the status of the device
 */
extern SYSTEMCALL short sys_chan_status(short channel);

/**
 * Ensure that any pending writes to teh device have been completed
 *
 * @param channel the number of the channel
 *
 * @return 0 on success, any negative number is an error code
 */
extern SYSTEMCALL short sys_chan_flush(short channel);

/**
 * Attempt to set the position of the channel cursor (if supported)
 *
 * @param channel the number of the channel
 * @param position the position of the cursor
 * @param base whether the position is absolute or relative to the current position
 *
 * @return 0 = success, a negative number is an error.
 */
extern SYSTEMCALL short sys_chan_seek(short channel, long position, short base);

/**
 * Issue a control command to the device
 *
 * @param channel the number of the channel
 * @param command the number of the command to send
 * @param buffer pointer to bytes of additional data for the command
 * @param size the size of the buffer
 *
 * @return 0 on success, any negative number is an error code
 */
extern SYSTEMCALL short sys_chan_ioctrl(short channel, short command, uint8_t * buffer, short size);

/**
 * Open a channel
 *
 * @param dev the device number to have a channel opened
 * @param path a "path" describing how the device is to be open
 * @param mode s the device to be read, written, both? (0x01 = READ flag, 0x02 = WRITE flag, 0x03 = READ and WRITE)
 *
 * @return the number of the channel opened, negative number on error
 */
extern SYSTEMCALL short sys_chan_open(short dev, const char * path, short mode);

/**
 * Close a channel
 *
 * @param chan the number of the channel to close
 *
 * @return nothing useful
 */
extern SYSTEMCALL short sys_chan_close(short chan);

/**
 * Swap the channel ID assignments for two channels
 *
 * Before call: channel1 = "Channel A", channel2 = "Channel B"
 * After call: channel1 = "Channel B", channel2 = "Channel A"
 *
 * @param channel1 the ID of one of the channels
 * @param channel2 the ID of the other channel
 * @return 0 on success, any other number is an error
 */
extern SYSTEMCALL short sys_chan_swap(short channel1, short channel2);

/**
 * Return the device associated with the channel
 *
 * @param channel the ID of the channel to query
 * @return the ID of the device associated with the channel, negative number for error
 */
extern SYSTEMCALL short sys_chan_device(short channel);

/**
 * Compute the size information for the text screen based on the current settings in VICKY
 * These settings are needed to correctly position text on the screen.
 *
 * @param screen the screen number 0 for channel A, 1 for channel B
 */
extern SYSTEMCALL void sys_text_setsizes(short chan);

/***
 *** Block device system calls
 ***/

/**
 * Register a block device driver
 * 
 * @param device pointer to the description of the device to register
 * @return 0 on succes, negative number on error
 */
extern SYSTEMCALL short sys_bdev_register(p_dev_block device);

/**
 * Read a block from the device
 * 
 * @param dev the number of the device
 * @param lba the logical block address of the block to read
 * @param buffer the buffer into which to copy the block data
 * @param size the size of the buffer.
 * @return number of bytes read, any negative number is an error code 
 */
extern SYSTEMCALL short sys_bdev_read(short dev, long lba, uint8_t * buffer, short size);

/**
 * Write a block to the device
 * 
 * @param dev the number of the device
 * @param lba the logical block address of the block to write
 * @param buffer the buffer containing the data to write
 * @param size the size of the buffer.
 * @return number of bytes written, any negative number is an error code 
 */
extern SYSTEMCALL short sys_bdev_write(short dev, long lba, const uint8_t * buffer, short size);

/**
 * Return the status of the block device 
 * 
 * @param dev the number of the device
 * @return the status of the device 
 */
extern SYSTEMCALL short sys_bdev_status(short dev);

/**
 * Ensure that any pending writes to the device have been completed 
 * 
 * @param dev the number of the device
 * @return 0 on success, any negative number is an error code 
 */
extern SYSTEMCALL short sys_bdev_flush(short dev);

/**
 * Issue a control command to the device 
 * 
 * @param dev the number of the device
 * @param command the number of the command to send
 * @param buffer pointer to bytes of additional data for the command
 * @param size the size of the buffer
 * @return 0 on success, any negative number is an error code 
 */
extern SYSTEMCALL short sys_bdev_ioctrl(short dev, short command, uint8_t * buffer, short size);

/*
 * File System Calls
 */

/**
 * Attempt to open a file given the path to the file and the mode.
 *
 * @param path the ASCIIZ string containing the path to the file.
 * @param mode the mode (e.g. r/w/create)
 *
 *@return the channel ID for the open file (negative if error)
 */
extern SYSTEMCALL short sys_fsys_open(const char * path, short mode);

/**
 * Close access to a previously open file.
 *
 * @param fd the channel ID for the file
 *
 * @return 0 on success, negative number on failure
 */
extern SYSTEMCALL short sys_fsys_close(short fd);

/**
 * Attempt to open a directory for scanning
 *
 * @param path the path to the directory to open
 *
 * @return the handle to the directory if >= 0. An error if < 0
 */
extern SYSTEMCALL short sys_fsys_opendir(const char * path);

/**
 * Close access to a previously open file.
 *
 * @param fd the channel ID for the file
 *
 * @return 0 on success, negative number on failure
 */
extern SYSTEMCALL short sys_fsys_close(short fd);

/**
 * Attempt to open a directory for scanning
 *
 * @param path the path to the directory to open
 *
 * @return the handle to the directory if >= 0. An error if < 0
 */
extern SYSTEMCALL short sys_fsys_opendir(const char * path);

/**
 * Close a previously open directory
 *
 * @param dir the directory handle to close
 *
 * @return 0 on success, negative number on error
 */
extern SYSTEMCALL short sys_fsys_closedir(short dir);

/**
 * Attempt to read an entry from an open directory
 *
 * @param dir the handle of the open directory
 * @param file pointer to the t_file_info structure to fill out.
 *
 * @return 0 on success, negative number on failure
 */
extern SYSTEMCALL short sys_fsys_readdir(short dir, p_file_info file);

/**
 * Open a directory given the path and search for the first file matching the pattern.
 *
 * @param path the path to the directory to search
 * @param pattern the file name pattern to search for
 * @param file pointer to the t_file_info structure to fill out
 *
 * @return the directory handle to use for subsequent calls if >= 0, error if negative
 */
extern SYSTEMCALL short sys_fsys_findfirst(const char * path, const char * pattern, p_file_info file);

/**
 * Open a directory given the path and search for the first file matching the pattern.
 *
 * @param dir the handle to the directory (returned by fsys_findfirst) to search
 * @param file pointer to the t_file_info structure to fill out
 *
 * @return 0 on success, error if negative
 */
extern SYSTEMCALL short sys_fsys_findnext(short dir, p_file_info file);

/**
 * Get the label for the drive holding the path
 *
 * @param path path to the drive
 * @param label buffer that will hold the label... should be at least 35 bytes
 * @return 0 on success, error if negative
 */
extern SYSTEMCALL short sys_fsys_get_label(const char * path, char * label);

/**
 * Set the label for the drive holding the path
 *
 * @param drive drive number
 * @param label buffer that holds the label
 * @return 0 on success, error if negative
 */
extern SYSTEMCALL short sys_fsys_set_label(short drive, const char * label);

/**
 * Create a directory
 *
 * @param path the path of the directory to create.
 *
 * @return 0 on success, negative number on failure.
 */
extern SYSTEMCALL short sys_fsys_mkdir(const char * path);

/**
 * Delete a file or directory
 *
 * @param path the path of the file or directory to delete.
 *
 * @return 0 on success, negative number on failure.
 */
extern SYSTEMCALL short sys_fsys_delete(const char * path);

/**
 * Rename a file or directory
 *
 * @param old_path he current path to the file
 * @param new_path the new path for the file
 *
 * @return 0 on success, negative number on failure.
 */
extern SYSTEMCALL short sys_fsys_rename(const char * old_path, const char * new_path);

/**
 * Change the current working directory (and drive)
 *
 * @param path the path that should be the new current
 *
 * @return 0 on success, negative number on failure.
 */
extern SYSTEMCALL short sys_fsys_set_cwd(const char * path);

/**
 * Get the current working drive and directory
 *
 * @param path the buffer in which to store the directory
 * @param size the size of the buffer in bytes
 *
 * @return 0 on success, negative number on failure.
 */
extern SYSTEMCALL short sys_fsys_get_cwd(char * path, short size);

/**
 * Load a file into memory at the designated destination address.
 *
 * If destination = 0, the file must be in a recognized binary format
 * that specifies its own loading address.
 *
 * @param path the path to the file to load
 * @param destination the destination address (0 for use file's address)
 * @param start pointer to the long variable to fill with the starting address
 *         (0 if not an executable, any other number if file is executable
 *         with a known starting address)
 *
 * @return 0 on success, negative number on error
 */
extern SYSTEMCALL short sys_fsys_load(const char * path, uint32_t destination, uint32_t * start);

/**
 * Register a file loading routine
 *
 * A file loader, takes a channel number to load from and returns a
 * short that is the status of the load.
 *
 * @param extension the file extension to map to
 * @param loader pointer to the file load routine to add
 *
 * @return 0 on success, negative number on error
 */
extern SYSTEMCALL short sys_fsys_register_loader(const char * extension, p_file_loader loader);

/**
 * Check to see if the file is present.
 * If it is not, return a file not found error.
 * If it is, populate the file info record
 *
 * @param path the path to the file to check
 * @param file pointer to a file info record to fill in, if the file is found.
 * @return 0 on success, negative number on error
 */
extern SYSTEMCALL short sys_fsys_stat(const char * path, p_file_info file);

/**
 * Memory
 */

/**
 * Return the top of system RAM... the user program must not use any
 * system memory from this address and above.
 *
 * @return the address of the first byte of reserved system RAM (one above the last byte the user program can use)
 */
extern SYSTEMCALL uint32_t sys_mem_get_ramtop();

/**
 * Reserve a block of memory at the top of system RAM.
 *
 * @param bytes the number of bytes to reserve
 * @return address of the first byte of the reserved block
 */
extern SYSTEMCALL uint32_t sys_mem_reserve(uint32_t bytes);

/*
 * Miscellaneous
 */

/**
 * Get the number of jiffies since the system last booted.
 *
 * NOTE: a jiffie is 1/60 of a second. This timer will not be
 *       100% precise, so it should be used for timeout purposes
 *       where precision is not critical.
 *
 * @return the number of jiffies since the last reset
 */
extern SYSTEMCALL uint32_t sys_time_jiffies();

/**
 * Set the time on the RTC
 *
 * @param time pointer to a t_time record containing the correct time
 */
extern SYSTEMCALL void sys_rtc_set_time(p_time time);

/**
 * Get the time on the RTC
 *
 * @param time pointer to a t_time record in which to put the current time
 */
extern SYSTEMCALL void sys_rtc_get_time(p_time time);

/**
 * Check for any keypress and return the scancode for the key
 * 
 * @return the next scan code from the keyboard... 0 if nothing pending
 */
extern SYSTEMCALL uint16_t sys_kbd_scancode();

/**
 * Return an error message given an error number
 *
 * @param err_number the error number
 * @return pointer to a string describing the error
 */
extern SYSTEMCALL const char * sys_err_message(short err_number);

/**
 * Set the keyboard translation tables
 *
 * The translation tables provided to the keyboard consist of eight
 * consecutive tables of 128 characters each. Each table maps from
 * the MAKE scan code of a key to its appropriate 8-bit character code.
 *
 * The tables included must include, in order:
 * - UNMODIFIED: Used when no modifier keys are pressed or active
 * - SHIFT: Used when the SHIFT modifier is pressed
 * - CTRL: Used when the CTRL modifier is pressed
 * - CTRL-SHIFT: Used when both CTRL and SHIFT are pressed
 * - CAPSLOCK: Used when CAPSLOCK is down but SHIFT is not pressed
 * - CAPSLOCK-SHIFT: Used when CAPSLOCK is down and SHIFT is pressed
 * - ALT: Used when only ALT is presse
 * - ALT-SHIFT: Used when ALT is pressed and either CAPSLOCK is down
 *   or SHIFT is pressed (but not both)
 *
 * @param tables pointer to the keyboard translation tables
 * @return 0 on success, negative number on error
 * 
 */
extern SYSTEMCALL short sys_kbd_layout(const char * tables);

/**
 * Load and execute an executable file
 *
 * @param path the path to the executable file
 * @param argc the number of arguments passed
 * @param argv the array of string arguments
 * @return the return result of the program
 */
extern SYSTEMCALL short sys_proc_run(const char * path, int argc, char * argv[]);

//
// Text screen calls
//

/**
 * Gets the description of a screen's capabilities
 *
 * @param screen the number of the text device
 *
 * @return a pointer to the read-only description (0 on error)
 */
extern SYSTEMCALL const p_txt_capabilities sys_txt_get_capabilities(short screen);

/**
 * Set the display mode for the screen
 *
 * @param screen the number of the text device
 * @param mode a bitfield of desired display mode options
 *
 * @return 0 on success, any other number means the mode is invalid for the screen
 */
extern SYSTEMCALL short sys_txt_set_mode(short screen, short mode);

/**
 * Set the position of the cursor to (x, y) relative to the current region
 * If the (x, y) coordinate is outside the region, it will be clipped to the region.
 * If y is greater than the height of the region, the region will scroll until that relative
 * position would be within view.
 *
 * @param screen the number of the text device
 * @param x the column for the cursor
 * @param y the row for the cursor
 */
extern SYSTEMCALL void sys_txt_set_xy(short screen, short x, short y);

/**
 * Get the position of the cursor (x, y) relative to the current region
 *
 * @param screen the number of the text device
 * @param position pointer to a t_point record to fill out
 */
extern SYSTEMCALL void sys_txt_get_xy(short screen, p_point position);

/**
 * Get the current region.
 *
 * @param screen the number of the text device
 * @param region pointer to a t_rect describing the rectangular region (using character cells for size and size)
 *
 * @return 0 on success, any other number means the region was invalid
 */
extern SYSTEMCALL short sys_txt_get_region(short screen, p_rect region);

/**
 * Set a region to restrict further character display, scrolling, etc.
 * Note that a region of zero size will reset the region to the full size of the screen.
 *
 * @param screen the number of the text device
 * @param region pointer to a t_rect describing the rectangular region (using character cells for size and size)
 *
 * @return 0 on success, any other number means the region was invalid
 */
extern SYSTEMCALL short sys_txt_set_region(short screen, p_rect region);

/**
 * Set the default foreground and background colors for printing
 *
 * @param screen the number of the text device
 * @param foreground the Text LUT index of the new current foreground color (0 - 15)
 * @param background the Text LUT index of the new current background color (0 - 15)
 */
extern SYSTEMCALL void sys_txt_set_color(short screen, unsigned char foreground, unsigned char background);

/*
 * Get the foreground and background color for printing
 *
 * Inputs:
 * screen = the screen number 0 for channel A, 1 for channel B
 * foreground = pointer to the foreground color number
 * background = pointer to the background color number
 */
extern SYSTEMCALL void sys_txt_get_color(short screen, unsigned char * foreground, unsigned char * background);

/**
 * Set if the cursor is visible or not
 *
 * @param screen the screen number 0 for channel A, 1 for channel B
 * @param is_visible TRUE if the cursor should be visible, FALSE (0) otherwise
 */
extern SYSTEMCALL void sys_txt_set_cursor_visible(short screen, short is_visible);

/**
 * Load a font as the current font for the screen
 *
 * @param screen the number of the text device
 * @param width width of a character in pixels
 * @param height of a character in pixels
 * @param data pointer to the raw font data to be loaded
 */
extern SYSTEMCALL short sys_txt_set_font(short screen, short width, short height, unsigned char * data);

/**
 * Get the display resolutions
 *
 * @param screen the screen number 0 for channel A, 1 for channel B
 * @param text_size the size of the screen in visible characters (may be null)
 * @param pixel_size the size of the screen in pixels (may be null)
 */
extern SYSTEMCALL void sys_txt_get_sizes(short screen, p_extent text_size, p_extent pixel_size);

/**
 * Set the size of the border of the screen (if supported)
 *
 * @param screen the number of the text device
 * @param width the horizontal size of one side of the border (0 - 32 pixels)
 * @param height the vertical size of one side of the border (0 - 32 pixels)
 */
extern SYSTEMCALL void sys_txt_set_border(short screen, short width, short height);

/**
 * Set the size of the border of the screen (if supported)
 *
 * @param screen the number of the text device
 * @param red the red component of the color (0 - 255)
 * @param green the green component of the color (0 - 255)
 * @param blue the blue component of the color (0 - 255)
 */
extern SYSTEMCALL void sys_txt_set_border_color(short screen, unsigned char red, unsigned char green, unsigned char blue);

/**
 * Print a character to the current cursor position in the current color
 *
 * Most character codes will result in a glyph being displayed at the current
 * cursor position, advancing the cursor one spot. There are some exceptions that
 * will be treated as control codes:
 *
 * 0x08 - BS - Move the cursor back one position, erasing the character underneath
 * 0x09 - HT - Move forward to the next TAB stop
 * 0x0A - LF - Move the cursor down one line (line feed)
 * 0x0D - CR - Move the cursor to column 0 (carriage return)
 *
 * @param screen the number of the text device
 * @param c the character to print
 */
extern SYSTEMCALL void sys_txt_put(short screen, char c);

/**
 * Print an ASCII Z string to the screen
 *
 * @param screen the number of the text device
 * @param c the ASCII Z string to print
 */
extern SYSTEMCALL void sys_txt_print(short screen, const char * message);

#endif
