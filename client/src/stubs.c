/**
 * @file stubs.c
 * @brief Stubs for Calypsi I/O routines
 * @version 0.1
 * @date 2024-09-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "include/toolbox.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stubs.h>

#define MAX_FD 16

struct s_file_descriptor {
	bool is_open;
	int public_fd;
	short toolbox_fd;
};

static bool is_inited = false;
static struct s_file_descriptor file_descriptor[MAX_FD];

static void init() {
	if (!is_inited) {
		is_inited = true;

		// Define stdin
		file_descriptor[0].is_open = true;
		file_descriptor[0].public_fd = 0;
		file_descriptor[0].toolbox_fd = 0;

		// Define stdout
		file_descriptor[1].is_open = true;
		file_descriptor[1].public_fd = 0;
		file_descriptor[1].toolbox_fd = 0;

		// Define stderr
		file_descriptor[2].is_open = true;
		file_descriptor[2].public_fd = 0;
		file_descriptor[2].toolbox_fd = 0;

		for (int i = 3; i < MAX_FD; i++) {
			file_descriptor[i].is_open = false;
			file_descriptor[i].public_fd = 0;
			file_descriptor[i].toolbox_fd = 0;
		}
	}
}

/**
 * @brief Find a free file descriptor
 * 
 * @return int the index of the available (closed) file descriptor (-1 for error)
 */
static int find_fd() {
	for (int i = 3; i < MAX_FD; i++) {
		if (!file_descriptor[i].is_open) {
			// Found one that is closed... return it's public ID
			return file_descriptor[i].public_fd;
		}
	}

	// Return an error
	return -1;
}

/****************************************************************************
 * Name: _Stub_open
 *
 * Description:
 *   Open a file.
 *   The oflag argument are POSIX style mode flags, e.g O_RDONLY which
 *   are defined in fcntl.h.
 *   This function is variadic as it optionally can take a mode_t that
 *   are permissions, e.g 0666. If the file system does not handle
 *   permissions you can ignore that this function is variadic.
 *   The return file descriptor shall be a positive number, larger
 *   than 2 (as 0-2 are used for stdin, stdout and stderr).
 *   The actual number does not matter and they need not to be
 *   consequtive, multiple numeric series with gaps between can be used.
 *
 * Return the obtained file descriptor or EOF (-1) on failure and set
 * errno according to the error.
 *
 ****************************************************************************/

int _Stub_open(const char *path, int oflag, ...) {
	int fd = find_fd();
	if (fd >= 0) {
		int mode = 0;

		if ((oflag & O_RDONLY) == O_RDONLY) {
			mode = FSYS_READ | FSYS_OPEN_EXISTING;
		}

		if ((oflag & O_WRONLY) == O_WRONLY) {
			mode = FSYS_WRITE;
		}

		if ((oflag & O_RDWR) == O_RDWR) {
			mode = FSYS_READ | FSYS_WRITE;
		}

		if ((oflag & O_CREAT) == O_CREAT) {
			mode |= FSYS_CREATE_NEW | FSYS_CREATE_ALWAYS;
		}

		short toolbox_fd = sys_fsys_open(path, mode);
		if (toolbox_fd >= 0) {
			file_descriptor[fd].is_open = true;
			file_descriptor[fd].toolbox_fd = toolbox_fd;

			return fd;
		} else {
			return -1;
		}

	} else {
		errno = ENFILE;
		return -1;
	}
}

/****************************************************************************
 * Name: _Stub_close
 *
 * Description:
 *   Close a file
 *
 * Return 0 if operation was OK, EOF otherwise and set errno according to
 * the error.
 * Note: This will only be invoked for streams opened by _Stub_open(),
 *       there is no need to check for the standard descriptor 0-2.
 *
 ****************************************************************************/

int _Stub_close(int fd) {
	if (file_descriptor[fd].is_open) {
		sys_fsys_close(file_descriptor[fd].toolbox_fd);
		file_descriptor[fd].toolbox_fd = 0;
		file_descriptor[fd].is_open = false;
	}
	
	return 0;
}

/****************************************************************************
 * Name: _Stub_read
 *
 * Description:
 *   Read from a file
 *
 * Returns the number of characters read. Return -1 on failure and set
 * errno according to the error.
 *
 ****************************************************************************/

size_t _Stub_read(int fd, void *buf, size_t count) {
	if (file_descriptor[fd].is_open) {
		short n = sys_chan_read(file_descriptor[fd].toolbox_fd, (unsigned char *)buf, (short)count);
		return n;
	} else {
		return -1;
	}
}

/****************************************************************************
 * Name: _Stub_write
 *
 * Description:
 *   Write to a file
 *
 * Returns the number of characters actually written. Return -1 on failure and
 * set errno according to the error.
 *
 ****************************************************************************/

size_t _Stub_write(int fd, const void *buf, size_t count) {
	if (file_descriptor[fd].is_open) {
		short n = sys_chan_write(file_descriptor[fd].toolbox_fd, (unsigned char *)buf, (short)count);
		return n;
	} else {
		return -1;
	}
}

/****************************************************************************
 * Name: _Stub_rename
 *
 * Description:
 *   Rename a file or directory
 *
 * Return 0 on success, -1 otherwise and set errno according to the
 * error.
 *
 ****************************************************************************/

int _Stub_rename(const char *oldpath, const char *newpath) {
	short result = sys_fsys_rename(oldpath, newpath);
	return result;
}

/****************************************************************************
 * Name: _Stub_remove
 *
 * Description:
 *   Remove a file or directory
 *
 * Return 0 on success, -1 otherwise and set errno according to the
 * error.
 *
 ****************************************************************************/

int _Stub_remove(const char *path) {
	short result = sys_fsys_delete(path);
	return result;
}
