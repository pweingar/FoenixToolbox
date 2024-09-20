;;;
;;; Definitions to connect a 64TASS assembly program to the Foenix Toolbox
;;;

		.include "bindings.s"

;
; Allocate space on the stack for parameters (count = number of bytes)
;
; Affects: A
;
alloc	.macro count

		.switch \count
		.case 2
		pea #0

		.case 4
		pea #0
		pea #0

		.case 6
		pea #0
		pea #0
		pea #0

		.default
		sta #0,d

		tsc
		sec
		sbc #(\count)
		tcs

		lda #0,d
		.endswitch
		.endm

;
; Remove space from the stack previously used for parameters (count = number of bytes)
;
; Affects: Y, #0,D, #1,D
;
free	.macro count

		.switch \count
		.case 2
		ply

		.case 4
		ply
		ply

		.case 6
		ply
		ply
		ply

		.default
		sta #0,d

		tsc
		clc
		adc #(\count)
		tcs

		lda #0,d
		.endswitch
		.endm