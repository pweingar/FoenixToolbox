/**
 * @file sys_macros.h
 * @brief Macros needed for the Foenix Toolbox
 * @version 0.1
 * @date 2024-09-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _sys_macros_h_
#define _sys_macros_h_

/*
 * Define the machine-specific system call function prefix
 */

#ifdef __CALYPSI_CORE_65816__
//
// System calls on the 65816 pass parameters primarily on stack using the Calypsi
// simple call convention and save/restore the direct page and data bank registers.
//
#define SYSTEMCALL __attribute__((simple_call)) __attribute__((saveds))
#else
#define SYSTEMCALL
#endif

#endif