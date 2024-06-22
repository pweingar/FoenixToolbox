/**
 * @file vfs.c
 * @author your name (you@domain.com)
 * @brief Implement the top-level virtual file system that supports multiple low-level file systems
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <string.h>
#include <stdlib.h>
#include "vfs.h"

#define MAX_FILE_SYSTEMS 4
#define MAX_FILE_DESCRIPTORS 8
#define MAX_DRIVES 8

static file_system_t file_systems[MAX_FILE_SYSTEMS];
static short file_system_count = 0;

static v_file_desc_t file_descriptors[MAX_FILE_DESCRIPTORS];
static short file_descriptor_count = 0;

static v_drive_binding_t drive_bindings[MAX_DRIVES];
static short drive_binding_count = 0;

/**
 * @brief Intialize the virtual file system
 * 
 * @return short 0 on success, any other number is an error
 */
short vfs_init() {
	// Clear out all the file descriptors
	for (short i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
		file_descriptors[i].status = 0;
		file_descriptors[i].public_id = i;
		file_descriptors[i].local_id = 0;
		file_descriptors[i].file_system = 0;
	}

	return 0;
}

/**
 * @brief Register a file system with the virutal file system
 * 
 * @param fs pointer to the file system driver
 * @return short 0 on success, any other number is an error
 */
short vfs_register_fs(file_system_p fs) {
	return 0;
}

/**
 * @brief Bind a drive name to a file system
 * 
 * @param drive the name of the drive (e.g. "sd0", "fd1")
 * @param id the ID of the drive in the file system
 * @param fs the file system driver
 * @return short 0 on success, any other number is an error
 */
short vfs_bind_drive(const char * drive, short id, file_system_p fs) {
	return 0;
}

short fsys_close(short fd) {
	short result = 0;

	if (fd < MAX_FILE_DESCRIPTORS) {
		v_file_desc_p desc = &file_descriptors[fd];
		if (desc->status != 0) {
			result = desc->file_system->close(desc->local_id);
			desc->status = 0;
			desc->local_id = 0;
			desc->file_system = 0;
		}
	}

	return result;
}