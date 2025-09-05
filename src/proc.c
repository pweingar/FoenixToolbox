/*
 * Routines to support processes
 *
 * NOTE: at the moment, FoenixMCP is single tasking only, so there is only
 *       one "process" besides the kernel.
 *
 */

#include "log_level.h"
#ifndef DEFAULT_LOG_LEVEL
    #define DEFAULT_LOG_LEVEL LOG_INFO
#endif

#include "errors.h"
#include "log.h"
#include "sys_general.h"
#include "dev/fsys.h"

static const long k_default_stack = 0x00010000;     /* For now... we're just going to put the user stack under 0x00010000 */
static int g_proc_result = 0;
uint32_t proc_shell_address = 0;

/*
 * Assembly routine: reset the supervisor stack pointer and restart the CLI
 */
extern void restart_cli();

extern void call_user(long start, long stack, int argc, char * argv[]);

// #ifdef __CALYPSI_CORE_65816__
typedef __attribute__((simple_call)) int (*user_proc)(int, char **);

void call_user(long start, long stack, int argc, char * argv[]) {
	user_proc start_point = (user_proc)start;
	g_proc_result = start_point(argc, argv);
	restart_cli();
}
// #endif

/*
 * Start a user mode process
 *
 * Inputs:
 * start = the address to start execution
 * stack = the location to start the user mode stack
 * argc = the number of parameters
 * argv = the array of parameters
 */
void proc_exec(long start, long stack, int argc, char * argv[]) {
    TRACE("proc_exec");

    log_num(LOG_INFO, "proc_exec start: ", start);
    log_num(LOG_INFO, "proc_exec stack: ", stack);

    g_proc_result = 0;
    call_user(start, stack, argc, argv);
}

typedef void (*thunk)();

/*
 * Quit the current user process
 *
 * NOTE: at the moment, this relaunches the CLI. In the future, this
 *       may cause execution to return to the program that started
 *       the user process.
 *
 * Inputs:
 */
SYSTEMCALL void proc_exit(int result) {
	INFO1("proc_exit: %d", result);
    g_proc_result = result;
	if (proc_shell_address != 0) {
		INFO("proc_exit: Attempting to call into shell");
        restart_cli();
		// call_user(proc_shell_address, k_default_stack, 0, 0);
		reboot();

	} else {
		// Otherwise ask GABE to reset the system
		INFO("proc_exit: Attempting reboot");
		reboot();
	}
}

/**
 * @brief Set the address of the code that should handle a process exiting
 * 
 * By default, the address is 0, which means that the system should reboot when the process exits
 * If any other number is provided, the code at that location will be called as a far call.
 * 
 * @param address the address of the handler code for proc_exit
 */
SYSTEMCALL void proc_set_shell(uint32_t address) {
	INFO("proc_set_shell")
	proc_shell_address = address;
}

/**
 * Return the result code of the previously running user process
 * 
 * @return the result code of the previously running user process
 */
SYSTEMCALL int proc_get_result() {
	INFO("proc_get_result")
    return g_proc_result;
}

/*
 * Find an executable binary matching the path, load it, and execute it
 *
 * Inputs:
 * path = the path to try to load
 * argc = the number of parameters
 * argv = the array of parameters
 *
 * Returns:
 * returns an error code on failure, will not return on success
 */
SYSTEMCALL short proc_run(const char * path, int argc, char * argv[]) {

    TRACE1("proc_run(\"%s\")", path);

    /* TODO: allow for commands without extensions */
    /* TODO: allow for a search PATH */
    /* TODO: allocate stack more dynamically */

    long start = 0;
    short result = fsys_load(path, 0, &start);
    if (result == 0) {
        if (start != 0) {
            proc_exec(start, k_default_stack, argc, argv);
            return 0;
        } else {
            ERROR1("Couldn't execute file: %d", result);
            return ERR_NOT_EXECUTABLE;
        }
    } else {
        ERROR1("Couldn't load file: %d", result);
        return result;
    }
}
