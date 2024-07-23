;;;
;;; Low-level support code for the IEC port
;;;

				.public sleep_20us
				.public sleep_100us
				.public sleep_300us
				.public sleep_1ms

#include "F256/iec_f256.h"

;
; Macros
;

;
; Macro to set IEC output pins
;
; @param pins the bit masks for the pins to set
;
set_pins:		.macro pins
				lda far:IEC_OUT
				and #~(\pins)
				sta far:IEC_OUT
				.mend

;
; Macro to clear IEC output pins
;
; @param pins the bit masks for the pins to clear
;
clr_pins:		.macro pins
				lda far:IEC_OUT
				ora #(\pins)
				sta far:IEC_OUT
				.mend

;
; Macro to set and clear pins in one shot
;
; @param set_pins the bit masks for the pins to set
; @param clr_pins the bit masks for the pins to clear
;
set_clr_pins:	.macro set_pins, clr_pins
				lda far:IEC_OUT
				and #~(\set_pins)
				ora #(\clr_pins)
				sta far:IEC_OUT
				.mend

read_pin:		.macro pin
				pha
				lda far:IEC_IN
				bit #(\pin)
				pla
				.mend

;
; Routines
;

sleep_20us:		phx
	            ldx #20
_loop$       	dex
				bne _loop$
				plx
				rtl
            
sleep_100us:	phx
            	ldx #5
_loop$	      	jsl sleep_20us
				dex
				bne _loop$
				plx
				rtl                

sleep_300us:	jsl sleep_100us
            	jsl sleep_100us
            	jsl sleep_100us
            	rtl
            
sleep_1ms:		jsl sleep_300us
            	jsl sleep_300us
            	jsl sleep_300us
            	jmp sleep_100us
