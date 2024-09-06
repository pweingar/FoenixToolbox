;;;
;;; A simple client program for the Foenix Toolbox using 64TASS
;;;

			.cpu "65816"

			.include "macros.s"
			.include "toolbox.s"
			.include "tb_params.s"

* = $10000

header:		.byte $f8, $16		; Signature
			.byte 0				; Version
			.dword start		; Starting address
			.dword 0			; Icon address
			.dword 0			; Icon palette address
			.null "hello"		; Name of the file

start:		clc
			xce

			setdbr `start
			setaxl

			alloc 6				; Set aside parameter space for sys_chan_write

			lda #33				; Size of the message
			sta tb.chan_write.size

			lda #`message		; Pointer to the message
			sta tb.chan_write.buffer+2
			lda #<>message
			sta tb.chan_write.buffer

			lda #0				; Channel #0

			jsl sys_chan_write	; sys_chan_write(0, message, strlen(message))

			free 6				; Reclaim parameter space from sys_chan_write

loop:		nop
			bra loop

message: 	.null "Hello, Foenix Toolbox (64TASS)!",13,10

