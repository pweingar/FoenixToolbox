;;;
;;; A simple client program for the Foenix Toolbox using 64TASS
;;;

			.cpu "65816"

			.include "toolbox.s"
			.include "macros.s"

kbd_opt_data = $f01dc0		; 8-bit
kbd_opt_stat = $f01dc1		; 8-bit
kbd_opt_cnt = $f01dc2		; 16-bit

vky_text = $f04000

* = $10000

			.virtual 0,d
save_a		.word ?
kbd_stat	.byte ?
scancode	.word ?
offset		.word ?
code		.word ?
count		.word ?
tmp			.word ?
			.endv

header:		.byte $f8, $16		; Signature
			.byte 0				; Version
			.dword start		; Starting address
			.dword 0			; Icon address
			.dword 0			; Icon palette address
			.null "optical"		; Name of the file

start:		clc
			xce

			setdbr `start
			setaxl

wait_line:	lda #80
			sta offset

			setas
			lda @l kbd_opt_stat
			jsr puthex8

			lda #' '
			jsr putc

wait:		setas
			lda @l kbd_opt_stat
			and #$01
			cmp #$01
			beq wait

			setal
			lda @l kbd_opt_cnt
			sta count

			setas
			lda @l kbd_opt_data
			jsr puthex8
			
			setas
			lda @l kbd_opt_data
			jsr puthex8

			setas
			lda #' '
			jsr putc

			setal
			dec count
			dec count
			beq wait_line

			bra wait
			
loop:		nop
			bra loop

;
; Print the character in A
;
putc:		.proc
			php
			phx

			ldx offset
			sta vky_text,x
			
			inc offset

			plx
			plp
			rts
			.pend

hexdigits:	.null '0123456789ABCDEF'

;
; Print the 4-bit hex digit in A
;
puthex4:	.proc
			phx
			php

			setaxs
			and #$0f
			tax

			lda hexdigits,x
			jsr putc

			plp
			plx
			rts
			.pend

;
; Print the 8-bit hex number in A
;
puthex8:	.proc
			php

			setas
			sta save_a

			lsr a
			lsr a
			lsr a
			lsr a
			and #$0f
			jsr puthex4

			lda save_a
			and #$0f
			jsr puthex4

			plp
			rts
			.pend

;
; Print the 16-bit hex number in A
;
puthex16:	.proc
			php

			setaxl
			sta code

			.rept 8
			lsr a
			.next
			and #$00ff
			jsr puthex8

			setaxl
			lda code
			and #$00ff
			jsr puthex8

			plp
			rts
			.pend

message: 	.null "Hello, Foenix Toolbox (64TASS)!",13,10

