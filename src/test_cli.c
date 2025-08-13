/**
 * A simple serial port based CLI to be used in testing.
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "errors.h"
#include "log.h"
#include "sys_types.h"
#include "utilities.h"
#include "dev/channel.h"
#include "dev/fsys.h"

//
// Macros
//

#define LINE_LEN        80
#define CTRL_CHAR(c)    ((c) & 0xbf)

//
// Constants
//

#define BELL    0x07

enum kbd_state_e {
    state_default = 0,
    state_esc,
    state_csi,
    state_digit
};

//
// Module variables
//

static char buffer[LINE_LEN];
static char input_line[LINE_LEN];

//
// Code
//

/**
 * Test code to talk directly to the wizfi
 */
short test_cli_wizfi(short chan) {
    short wizfi = chan_open(CDEV_COM2, 0, 0);
    if (wizfi >= 0) {
        do {
            if (chan_status(wizfi) & CDEV_STAT_READABLE) {
                uint8_t b = chan_read_b(wizfi);
                chan_write_b(chan, b);
                
            } else if (chan_status(chan) & CDEV_STAT_READABLE) {
                uint8_t b = chan_read_b(chan);
                chan_write_b(wizfi, b); 
            }
        } while (1);
    } else {
        return wizfi;
    }
}

short test_cli_type(short chan, char * path) {
    short n = 0;
    short fd = 0;
    uint8_t file_line[LINE_LEN];

    // Check for the path
    if ((path == 0) || (strlen(path) == 0)) {
        return FSYS_ERR_NO_PATH;
    }

    sprintf(buffer, "Contents of %s\r\n", path);
    chan_write(chan, (uint8_t *)buffer, n);

    fd = fsys_open(path, FSYS_READ);
    if (fd < 0) {
        return fd;
    } else {
        do {
            n = chan_readline(fd, file_line, LINE_LEN);
            if (n > 0) {
                chan_write(chan, file_line, n);
            } else {
                break;
            }
        } while (1);
    }

    fsys_close(fd);
    return n;
}

/**
 * Command to print a directory listing
 */
short test_cli_directory(short chan, char * path) {
    short result = 0;
    t_file_info file_info;
    bool keep_reading = true;

    // Try to get the path to open
    if ((path == 0) || (strlen(path) == 0)) {
        // Default to /sd0
        path = "/sd0";
    }

    sprintf(buffer, "Contents of %s:\r\n", path);
    chan_write(chan, (uint8_t *)buffer, strlen(buffer));

    // Try to open the directory
    short dir = fsys_opendir(path);
    if (dir < 0) {
        return dir;
    }

    while (keep_reading) {
        result = fsys_readdir(dir, &file_info);
        if (result < 0) {
            keep_reading = false;
        } else {
            if (file_info.name[0] == 0) {
                keep_reading = false;
            } else if (((file_info.attributes & FSYS_AM_HID) == 0) && (file_info.name[0] != '.')) {
                if (file_info.attributes & FSYS_AM_DIR) {
                    sprintf(buffer, "%s/\r\n", file_info.name);
                    chan_write(chan, (uint8_t *)buffer, strlen(buffer));

                } else {
                    sprintf(buffer, "%s\r\n", file_info.name);
                    chan_write(chan, (uint8_t *)buffer, strlen(buffer));
                }
            }
        }
    }

    // Close the directory
    fsys_closedir(dir);

    return result;
}

short test_cli_find_word(char * text, short initial, char * word, short word_size) {
    short i = initial;

    memset(word, 0, word_size);

    // Skip over initial white space
    while (text[i] && isspace(text[i])) {
        i++;
    }

    // Return -1 if there is no word
    if (text[i] == 0) {
        return -1;
    }

    // Copy the word
    short j = 0;
    while (text[i] && !isspace(text[i]) && (j < word_size)) {
        word[j++] = text[i++];
        word[j] = 0;
    }

    return i;
}

/**
 * Attempt to execute a command line
 * 
 * @param chan the channel to write to
 * @param line the command text to try to process
 * @return 0 on success, negative number is an error
 */
short test_cli_execute(short chan, char * line) {
    short result = 0;
    char * tok_cursor = 0;
    char token[LINE_LEN];

    short next = test_cli_find_word(line, 0, token, LINE_LEN);
    if (strlen(token) > 0) {
        if (strcmp(token, "dir") == 0) {
            next = test_cli_find_word(line, next, token, LINE_LEN);
            return test_cli_directory(chan, token);

        } else if (strcmp(token, "type") == 0) {
            next = test_cli_find_word(line, next, token, LINE_LEN);
            return test_cli_type(chan, token);

        } else if (strcmp(token, "wizfi") == 0) {
            return test_cli_wizfi(chan);

        }
    } else {
        sprintf(buffer, "no token found in [%s]\r\n", line);
        chan_write(chan, (uint8_t *)buffer, strlen(buffer)); 
    }

    return result;
}

short test_cli_readline(short chan, char * line, short size) {
    short index = 0;
    short code = 0;
    enum kbd_state_e state = state_default;

    memset(line, 0, size);

    do {
        char c = chan_read_b(chan);
        if (c) {
            switch(state) {
                case state_esc:
                    if (c == '[') {
                        state = state_csi;
                    } else {
                        state = state_default;
                    }
                    break;

                case state_csi:
                    switch(c) {
                        case 'D':
                            // Move back
                            if (index > 0) {
                                sprintf(buffer, "\e[D");
                                chan_write(chan, (uint8_t *)buffer, strlen(buffer));
                                index -= 1;
                            } else {
                                chan_write_b(chan, BELL);
                            }
                            state = state_default;
                            break;

                        case 'C':
                            // Move forward
                            if (index < strlen(line)) {
                                sprintf(buffer, "\e[C");
                                chan_write(chan, (uint8_t *)buffer, strlen(buffer));
                                index += 1;
                            } else {
                                chan_write_b(chan, BELL);
                            }
                            state = state_default;
                            break;

                        case 'H':
                            // HOME key
                            sprintf(buffer, "\e[%dD", index);
                            chan_write(chan, (uint8_t *)buffer, strlen(buffer));
                            index = 0;
                            state = state_default;
                            break;

                        case 'F':
                            // END key
                            sprintf(buffer, "\e[%dD\e[%dC", index, strlen(line));
                            chan_write(chan, (uint8_t *)buffer, strlen(buffer));
                            index = strlen(line) - 1;
                            state = state_default;
                            break;

                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            code = c - '0';
                            state = state_digit;
                            break;

                        default:
                            state = state_default;
                            break;
                    }
                    break;

                case state_digit:
                    // We have a digit as part of the sequence \e[#~
                    if (isdigit(c)) {
                        code = code * 10;
                        code = code + c - '0';
                    } else if (c == '~') {
                        switch(code) {
                            case 1:
                                // HOME key
                                sprintf(buffer, "\e[132D\e[2C");
                                chan_write(chan, (uint8_t *)buffer, strlen(buffer));
                                index = 0;
                                state = state_default;
                                break;

                            case 4:
                            case 8:
                                // END key
                                sprintf(buffer, "\e[132D\e[%dC", strlen(line)+2);
                                chan_write(chan, (uint8_t *)buffer, strlen(buffer));
                                index = strlen(line);
                                state = state_default;
                                break;

                            default:
                                state = state_default;
                                break;
                        }
                    } else {
                        state = state_default;
                    }
                    break;

                default:
                    if (c == '\r') {
                        return index;

                    } else if (c == '\e') {
                        state = state_esc;

                    } else if (iscntrl(c)) {
                        char ch = c | 0x40;
                        switch (ch) {
                            case 'K':
                                sprintf(buffer, "\e[2K\r> ");
                                chan_write(chan, (uint8_t *)buffer, strlen(buffer));
                                index = 0;
                                memset(line, 0, size);
                                break;

                            default:
                                break;
                        }

                    } else if (isprint(c)) {
                        if (index < size - 1) {
                            // Echo the character back
                            chan_write_b(chan, c);
                            line[index++] = c;
                        } else {
                            chan_write_b(chan, BELL);
                        }
                    }
                    break;
            }
        }
    } while (1);
}

/**
 * Startup the test CLI
 * 
 * @return 0 on successful exit, negative number is an error
 */
short test_cli_repl() {
    short output = 0;

    output = chan_open(CDEV_COM1, 0, 0);
    if (output >= 0) {
        sprintf(buffer, "\e[2J\e[HFoenix Test Command Line Interpreter\r\n");
        chan_write(output, (uint8_t *)buffer, strlen(buffer));
        sprintf(buffer, "HOME = move to start of line, END = move to end of line, CTRL-K = delete line\r\n");
        chan_write(output, (uint8_t *)buffer, strlen(buffer));
        sprintf(buffer, "LEFT = move to one character left, RIGHT = move one character right\r\n");
        chan_write(output, (uint8_t *)buffer, strlen(buffer));

        do {
            sprintf(buffer, "\r\n> ");
            chan_write(output, (uint8_t *)buffer, strlen(buffer));

            short n = test_cli_readline(output, input_line, LINE_LEN);
            if (strlen(input_line) > 0) {
                sprintf(buffer, "\r\n");
                chan_write(output, (uint8_t *)buffer, strlen(buffer));
                short result = test_cli_execute(output, input_line);
                if (result < 0) {
                    sprintf(buffer, "Error: %s (%d)\r\n", err_message(result), result);
                    chan_write(output, (uint8_t *)buffer, strlen(buffer));
                }
            }
        } while (1);

        chan_close(output);
        return 0;

    } else {
        return output;
    }
}