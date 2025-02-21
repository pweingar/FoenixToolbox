/****************************************************************************
 *
 * Copyright Håkan Thörngren
 *
 * This file is part of the Calypsi C library.
 * Permission to use with the Calypsi tool chain is hereby granted.
 *
 ****************************************************************************/

;;; Startup variant, change attribute value if you make your own
              .rtmodel cstartup,"a2560"

              .rtmodel version, "1"
              .rtmodel core, "*"

              ;; External declarations
              .section sstack
              .section stack
              .section heap
              .section data_init_table

              .extern main, exit

#ifdef __CALYPSI_DATA_MODEL_SMALL__
              .extern _NearBaseAddress
#endif

#ifdef __CALYPSI_TARGET_SYSTEM_FOENIX__
              .extern _Gavin
              .extern _Beatrix
              .extern _Vicky
              .extern _VickyBaseVRAM

# define GavinLow   0x00b00000
# define BeatrixLow 0x00b20000
# define VickyLow   0x00b40000
# define VickyBaseVRAMLow 0x00c00000

# define GavinHigh   0xfec00000
# define BeatrixHigh 0xfec20000
# define VickyHigh   0xfec40000
# define VickyBaseVRAMHigh 0x00800000
#endif

#include "macros.h"

;;; ***************************************************************************
;;;
;;; The reset vector. This uses the entry point label __program_root_section
;;; which by default is what the linker will pull in first.
;;;
;;; ***************************************************************************

              .section reset
              .pubweak __program_root_section
__program_root_section:
              .long   .sectionEnd sstack + 1
              .long   __program_start

;;; ***************************************************************************
;;;
;;; __program_start - actual start point of the program
;;;
;;; Initialize sections and call main().
;;; You can override this with your own routine, or tailor it as needed.
;;; The easiest way to make custom initialization is to provide your own
;;; __low_level_init which gets called after stacks have been initialized.
;;;
;;; ***************************************************************************

              .section libcode, noreorder
              .pubweak __program_start
              .align  2
__program_start:
              move.l  #.sectionEnd stack + 1,a0
              move.l  a0,usp
#ifdef __CALYPSI_DATA_MODEL_SMALL__
              lea.l   _NearBaseAddress.l,a4
#endif
              call     __low_level_init
              tst.l   d0            ; stay in supervisor?
              bne.s   10$           ; yes
              andi.w  #~0x2700,sr   ; no, drop out of supervisor,
                                    ; enable interrupts
10$:

;;; Initialize data sections if needed.
              .section libcode, noroot, noreorder
              .pubweak __data_initialization_needed
              .extern __initialize_sections
              .align  2
__data_initialization_needed:
              move.l  #(.sectionStart data_init_table),a0
              move.l  #(.sectionEnd data_init_table),a1
              call    __initialize_sections

;;; **** Initialize streams if needed.
              .section libcode, noroot, noreorder
              .pubweak __call_initialize_global_streams
              .extern __initialize_global_streams
__call_initialize_global_streams:
              call    __initialize_global_streams

;;; **** Initialize heap if needed.
              .section libcode, noroot, noreorder
              .pubweak __call_heap_initialize
              .extern __heap_initialize, __default_heap
__call_heap_initialize:
              move.l  #.sectionSize heap,d0
              move.l  #__default_heap,a0
              move.l  #.sectionStart heap,a1
              call    __heap_initialize

              .section libcode, noroot, noreorder
#ifdef __CALYPSI_TARGET_SYSTEM_FOENIX__
              .pubweak _Gavin_initialize
_Gavin_initialize:
              move.l  #GavinLow,a0  ; assume A2560U system
              move.l  #BeatrixLow,a2
              move.l  #VickyLow,a1
              move.l  #VickyBaseVRAMLow,d0
              cmp.w   #0x4567,0x0010(a0) ; check byte order
              beq.s   20$
              move.l  #GavinHigh,a0 ; no, assume A2560K 32-bit
              move.l  #BeatrixHigh,a2
              move.l  #VickyHigh,a1
              move.l  #VickyBaseVRAMHigh,d0
20$:
              ;; keep base pointer to Gavin
#ifdef __CALYPSI_DATA_MODEL_SMALL__
              move.l  a0,(.near _Gavin,A4)
              move.l  a2,(.near _Beatrix,A4)
              move.l  a1,(.near _Vicky,A4)
              move.l  d0,(.near _VickyBaseVRAM,A4)
#else
              move.l  a0,_Gavin
              move.l  a2,_Beatrix
              move.l  a1,_Vicky
              move.l  d0,_VickyBaseVRAM
#endif // __CALYPSI_DATA_MODEL_SMALL__
#endif // __CALYPSI_TARGET_SYSTEM_FOENIX__

              .section libcode, root, noreorder
              moveq.l #0,d0         ; argc = 0
              call    main
              jump    exit

;;; ***************************************************************************
;;;
;;; __low_level_init - custom low level initialization
;;;
;;; This default routine just returns doing nothing. You can provide your own
;;; routine, either in C or assembly for doing custom low leve initialization.
;;;
;;; ***************************************************************************

              .section libcode
              .pubweak __low_level_init
              .align  2
__low_level_init:
              moveq.l #1,d0         ; switch to user mode
              rts
