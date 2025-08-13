/**
 * A simple serial port based CLI to be used in testing.
 */

#ifndef __test_cli__
#define __test_cli__

/**
 * Attempt to execute a command line
 * 
 * @param chan the channel to write to
 * @param line the command text to try to process
 * @return 0 on success, negative number is an error
 */
extern short test_cli_execute(short chan, char * line);

/**
 * Startup the test CLI
 * 
 * @return 0 on successful exit, negative number is an error
 */
extern short test_cli_repl();

#endif