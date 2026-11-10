#include <stdint.h>
#include <stdlib.h>

#include "errors.h"
#include "indicators.h"

//
// Types
//

typedef struct ind_handler_node_s {
    ind_handler handler;
    struct ind_handler_node_s * prev;
    struct ind_handler_node_s * next;
} ind_handler_node_t, *ind_handler_node_p;

//
// Module variables
//

static ind_handler_node_p handlers = 0;

//
// Functions
//

/**
 * Function provided by the machine specific indicator code to initialize that part of the code
 */
extern void ind_init_lowlevel();

/**
 * Get the node for the indicator handler provided
 * 
 * @param handler pointer to the function to search for
 * @return pointer to the handler node for that handler function, 0 if not found
 */
static ind_handler_node_p ind_find_handler_node(ind_handler handler) {
    if (handlers) {
        for (ind_handler_node_p node = handlers; node; node = node->next) {
            if (node->handler == handler) {
                return node;
            }
        }
    }

    // If we got here, the handler was never registered
    return 0;
}

/**
 * Register another function as a handler for indicator changes
 * 
 * NOTE: this handler does not replace existing functions but is called in addition
 *       to other code
 * 
 * @param handler function to call when an indicator changes
 * @return 0 on success, negative on failure
 */
short ind_register(ind_handler handler) {
    if (ind_find_handler_node(handler) == 0) {
        // The handler has not yet been registered... add it to the list
        ind_handler_node_p node = (ind_handler_node_p)malloc(sizeof(ind_handler_node_t));
        if (node) {
            node->handler = handler;
            node->prev = 0;

            if (handlers) {
                // There are already handlers... add this to the list
                node->next = handlers;
                handlers->prev = node;
                handlers = node;

            } else {
                // This is the first handler registered
                handlers = node;
                node->next = 0;
            }

            return 0;
        } else {
            return ERR_OUT_OF_MEMORY;
        }

    } else {
        // The handler is already registered, just ignore it
        return 0;
    }
}

/**
 * Remove a previously registered handler
 */
void ind_remove(ind_handler handler) {
    ind_handler_node_p node = ind_find_handler_node(handler);
    if (node) {
        // We found the node... unlink it from the list
        if (node->next) {
            node->next->prev = node->prev;
        }

        if (node->prev) {
            node->prev->next = node->next;
        }

        if (handlers == node) {
            handlers = node->next;
        }

        // Return the node to the heap
        free(node);
    }
}

/*
 * Set an indicator to the given state
 *
 * NOTE: this version requires that the low-level indicator handlers have been installed
 *
 * @param ind_number the number of the indicator to change
 * @param state the state the indicator should take (on, off, error)
 */
extern void ind_set(short ind_number, short state) {
    for (ind_handler_node_p node = handlers; node; node = node->next) {
        if (node->handler) {
            node->handler(ind_number, state);
        }
    }
}

/*
 * Initialize the indicators
 *
 * Generally, this means the indicators will all be turned off
 */
void ind_init() {
    // Make sure our handlers list is empty
    handlers = 0;

    // Call the machine-specific initialization code
    // This should register the indicator handler for the base machine
    ind_init_lowlevel();
}