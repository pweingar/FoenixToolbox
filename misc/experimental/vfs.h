/**
 * @file vfs.h
 * @author your name (you@domain.com)
 * @brief Implement the top-level virtual file system that supports multiple low-level file systems
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __vfs_h__
#define __vfs_h__

#include "sys_types.h"

//
// Structures
//

/**
 * @brief Define the structure for a file system driver
 * 
 */
typedef struct file_system_s {
	const char * name;

	short (* open)(const char * path, short mode);
	short (* close)(short fd);
	short (* opendir)(const char * path);
	short (* closedir)(short dir);
	short (* readdir)(short dir, p_file_info file);
	short (* findfirst)(const char * path, const char * pattern, p_file_info file);
	short (* findnext)(short dir, p_file_info file);
	short (* stat)(const char * path, p_file_info file);
	short (* getlabel)(char * path, char * label);
	short (* setlabel)(short drive, const char * label);
	short (* mkfs)(short drive, char * label);
	short (* mkdir)(const char * path);
	short (* unlink)(const char * path);
	short (* rename)(const char * old_path, const char * new_path);
} file_system_t, *file_system_p;

/**
 * @brief Structure defining a top-level file descriptor
 * 
 */
typedef struct v_file_desc_s {
	short status;					// 0 = closed, 1 = open
	short public_id;				// The ID of this file descriptor as used by callers
	short local_id;					// The ID of the matching file descriptor in the file system
	file_system_p file_system;		// The file system handling this file descriptor
} v_file_desc_t, *v_file_desc_p;

/**
 * @brief Structure to bind a drive to a file system
 * 
 */
typedef struct v_drive_binding_s {
	const char * drive;				// The name of the drive (e.g. "sd0", "fd1", "rom")
	short drive_id;					// The ID of the drive in the file system
	file_system_p file_system;		// The file system that handles the drive
} v_drive_binding_t, *v_drive_binding_p;

//
// Calls
//

/**
 * @brief Intialize the virtual file system
 * 
 * @return short 0 on success, any other number is an error
 */
extern short vfs_init();

/**
 * @brief Register a file system with the virutal file system
 * 
 * @param fs pointer to the file system driver
 * @return short 0 on success, any other number is an error
 */
extern short vfs_register_fs(file_system_p fs);

/**
 * @brief Bind a drive name to a file system
 * 
 * @param drive the name of the drive (e.g. "sd0", "fd1")
 * @param id the ID of the drive in the file system
 * @param fs the file system driver
 * @return short 0 on success, any other number is an error
 */
extern short vfs_bind_drive(const char * drive, short id, file_system_p fs);