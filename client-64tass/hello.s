;;;
;;; A simple client program for the Foenix Toolbox using 64TASS
;;;

			.cpu "65816"

			.include "toolbox.s"

setdbr      .macro          ; Set the B (Data bank) register 
			pea #((\1) * 256) + (\1)
			plb
			plb
			.databank \1
			.endm 

setaxl      .macro
			rep #$30        ; set A&X long 
			.al
			.xl
			.endm

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

