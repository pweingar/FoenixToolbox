;;;
;;; Low-level support code for the IEC port
;;;
;;; NOTE: routines will be split into private routines and public routines.
;;;       Private routines will assume near JSRs, 8-bit accumulator and index registers, and interrupts disabled.
;;;		  Public routines will assume far JSRs and 16-bit accumulator and index registers.
;;;       Public routines will also assume Calypsi calling conventions.
;;;

				.public iecll_ioinit
				.public iecll_in
				.public iecll_eoi
				.public iecll_out
				.public iecll_talk
				.public iecll_talk_sa
				.public iecll_untalk
				.public iecll_listen
				.public iecll_listen_sa
				.public iecll_unlisten
				.public iecll_reset

#include "F256/iec_f256.h"

				.section data,data

eoi_pending:	.byte 0
rx_eoi:			.byte 0
delayed:		.byte 0
queue:			.byte 0

				.section code

;;
;; Macros
;;

assert_bit:     .macro pin
                pha
                lda IEC_OUTPUT_PORT
                and #(\pin ^ 0xff)
                sta IEC_OUTPUT_PORT
                pla
                rts
                .endm
            
release_bit:    .macro pin
                pha
                lda IEC_OUTPUT_PORT
                ora #(\pin)
                sta IEC_OUTPUT_PORT
                pla
                rts
                .endm

read_bit:       .macro pin
                pha
loop$           lda IEC_INPUT_PORT
                cmp IEC_INPUT_PORT
                bne loop$
                and #(\pin)
                cmp #1
                pla
                rts
                .endm

emit:			.macro char
				pha
				lda #(\char)
				sta long: 0xf04000
				pla
				.endm
               
;;
;; Private functions
;;

;
; Pin accessor functions to assert, release, or read a pin
;

read_SREQ:		read_bit IEC_SREQ_i
assert_SREQ:   	assert_bit IEC_SREQ_o
release_SREQ:	release_bit IEC_SREQ_o

read_ATN:		read_bit IEC_ATN_i
assert_ATN:    	assert_bit IEC_ATN_o
release_ATN:	release_bit IEC_ATN_o

read_CLOCK:		read_bit IEC_CLK_i
assert_CLOCK:   assert_bit IEC_CLK_o
release_CLOCK:	release_bit IEC_CLK_o

read_DATA:		read_bit IEC_DATA_i
assert_DATA:   	assert_bit IEC_DATA_o
release_DATA:	release_bit IEC_DATA_o

assert_RST:   	assert_bit IEC_RST_o
release_RST:	release_bit IEC_RST_o

;;
;; Routines to wait various amounts of time
;;

sleep_20us:		phx
	            ldx #20
_loop$       	dex
				bne _loop$
				plx
				rts
            
sleep_100us:	phx
            	ldx #5
_loop$	      	jsr sleep_20us
				dex
				bne _loop$
				plx
				rts                

sleep_300us:	jsr sleep_100us
            	jsr sleep_100us
            	jsr sleep_100us
            	rts
            
sleep_1ms:		jsr sleep_300us
            	jsr sleep_300us
            	jsr sleep_300us
            	jmp sleep_100us

;;
;; Code to handle the IEC port
;;

init:			jsr sleep_1ms
				jsr release_ATN
				jsr release_DATA
				jsr release_SREQ
				jsr assert_CLOCK   	; IDLE state
				jsr sleep_1ms
				jsr sleep_1ms
				jsr sleep_1ms
				
				; Bail if ATN and SRQ fail to float back up.
				; We'll have a more thorough test when we send
				; our first command.            
				nop
				nop
				nop
				jsr read_SREQ
				bcc err$
				jsr read_ATN
				bcc err$

            	jsr sleep_1ms

            	clc
            	rts

err$        	sec
            	rts     

;
; Send a command byte and release the DATA and CLOCK lines afterwards
;
; A = the command byte to send
;
atn_release_data
				jsr release_DATA
            	jsr release_CLOCK			; TODO: makes /no/ sense; maybe we can remove...
            	jsr atn_common				; NOTE: does NOT release ATN!  Does NOT release IRQs!
				rts

;
; Send a command byte and release the ATN, DATA, and CLOCK lines
;
; A = the command byte to send
;
atn_release		jsr atn_common            

				jsr release_ATN
				jsr sleep_20us
				jsr sleep_20us
				jsr sleep_20us
				jsr release_CLOCK
				jsr release_DATA
				rts

;
; Send a command byte with ATN asserted
;
; A = the command byte to send
;
atn_common
				; Assert ATN; if we aren't already in sending mode, 
				; get there:

				jsr assert_ATN
				jsr assert_CLOCK
				jsr release_DATA

				; Now give the devices ~1ms to start listening.
				jsr sleep_1ms
				
				; If no one is listening, there's nothing on
				; the bus, so signal an error.
				jsr read_DATA
				bcs err$

				; ATN bytes are technically never EOI bytes
				stz eoi_pending

				jmp send       

err$
				; Always release the ATN line on error; TODO: add post delay
				jmp release_ATN

;
; Send a byte over the IEC bus but mark it as the last byte
;
; A = the byte to send
;
send_eoi		jsr set_eoi
            	jmp send

;
; Set that the EOI byte is the next to be sent
;
set_eoi			stz eoi_pending
				dec eoi_pending
				rts

;
; Sends the queued byte with an EOI
;
flush			bit delayed
        		bpl done$
        		pha
        		lda queue
        		stz delayed
        		jsr send_eoi
        		pla
done$:		    rts

;
; Send a byte over the IEC bus
;
; A = the byte to send
;
send
				; Assumes we are in the sending state:
				; the host is asserting CLOCK and the devices are asserting DATA.

				; There must be at least 100us between bytes.
				jsr     sleep_300us

				; ; Clever cheating (PJW: removed since we disable interrupts for all this code)

				; ; Act as an ersatz listener to keep the other listeners busy
				; ; until we are ready to receive.  This is NOT part of the
				; ; IEC protocol -- we are doing this in lieu of an interrupt.
				; jsr assert_DATA

				; Release CLOCK to signal that we are ready to send
				; We can do this without disabling interrupts because
				; we are also asserting DATA.
				jsr release_CLOCK
            
        		; Now we wait for all of the listeners to acknowledge.
wait$
          		jsr sleep_20us

				; Check to see if all listeners have acknowledged
				jsr read_DATA
				bcs ready$
            
				; Other listeners are still busy; go back to sleep.
				bra wait$

ready$			bit eoi_pending
            	bpl send$
            
eoi$
				; Alas, we can't get too clever here, or the 1541 hates us.
				
				; Hard-wait the 200us for the drive to acknowledge the EOI.
				; This duration is technically unbounded, but the ersatz
				; listener trick during the EOI signal, and the drive
				; already had the opportunity to delay before starting the
				; ack, so hopefully it will stay in nominal 250us range.
        
TYE$        	jsr read_DATA
            	bcs TYE$

				; Now we're basically back to the point where we are waiting
				; for Rx ack.  The trick does work here.

				; Clear the eoi minus flag, so our next send will be data.
				lsr eoi_pending

				; The drive should hold DATA for at least 60us.  Give it
				; 20us, and then repeat our ersatz listener trick.
				jsr sleep_20us
				; jsr     assert_DATA (PJW: removed trick)
				bra wait$

send$
				; Give the listeners time to notice that the've all ack'd
				jsr sleep_20us  ; NOT on the C64

				; Now start pushing out the bits.  Note that the timing
				; is not critical, but each clock state must last at
				; least 20us (with 70us more typical for clock low).

				phx
				ldx #8
loop$
				; TODO: opt test for a frame error

          		; Clock out the next bit
				jsr assert_CLOCK
				jsr sleep_20us
				lsr a
				bcs one$

zero$       	jsr assert_DATA
            	bra clock$

one$        	jsr release_DATA
				bra clock$
            	
clock$
          		; Toggle the clock
           		jsr sleep_20us  ; TODO: Maybe extend this.

				jsr sleep_20us  ; 1541 needs this.
				jsr release_CLOCK

				jsr sleep_20us
				dex
				bne loop$
				plx
            
	          	; Finish the last bit and wait for the listeners to ack.
            	jsr release_DATA
            	jsr assert_CLOCK

				; Now wait for listener ack.  Of course, if there are
				; multiple listeners, we can only know that one ack'd.
				; This can take up to a millisecond, so another good
				; candidate for a kernel thread or interrupt.

				; TODO: ATN release timing appears to be semi-critical; we may need
				; to completely change the code below.

ack$        	jsr read_DATA
            	bcs ack$
            	clc
            	rts

;
; Wait for a byte of data to be read from the IEC port
;
recv_data  

				; Assume not EOI until proved otherwise
				stz rx_eoi

				; Wait for the sender to have a byte
wait1$      	jsr read_CLOCK
           		bcc wait1$

          		; TODO: start and check a timer

				; Signal we are ready to receive
ready$			jsr release_DATA

          		; Wait for all other listeners to signal
wait2$      	jsr read_DATA
            	bcc wait2$

				; Wait for the first bit or an EOI condition
				; Each iteration takes 6-7us
				lda #0      ; counter
wait3$      	inc a
				beq eoi$
				jsr read_CLOCK
				bcc recv$
				adc #7      ; microseconds per loop
				bcc wait3$

eoi$			lda rx_eoi
            	bmi error$

				; Ack the EOI
				jsr assert_DATA
				jsr sleep_20us
				jsr sleep_20us
				jsr sleep_20us

				; Set the EOI flag.  
				dec rx_eoi 				; TODO: error on second round

				; Go back to the ready state
				bra ready$

error$			sec
            	rts

recv$
				; Clock in the bits
				phx
				ldx #8

wait_fall$  	jsr read_CLOCK
            	bcs wait_fall$

wait_rise$  	jsr read_CLOCK
            	bcc wait_rise$
           
				jsr read_DATA
				ror a
				dex
				bne wait_fall$
				plx

				; Ack
				jsr sleep_20us
				jsr assert_DATA

				; Drives /usually/ work with a lot less, but
				; I see failures on the SD2IEC on a status check
				; after file-not-found when debugging is turned off.
				jsr sleep_20us  ; Seems to be missing the ack.
				jsr sleep_20us  ; Seems to be missing the ack.
				jsr sleep_20us  ; Seems to be missing the ack.
				jsr sleep_20us  ; Seems to be missing the ack.
            
				; Return EOI in NV
				clc
				bit rx_eoi
				ora #0
				rts

;;
;; Public Functions
;;

;
; Initialize the IEC interface
;
; extern short iecll_ioinit()
;
; Returns 0 on success, -1 if no devices found
;
iecll_ioinit	php
				sei					; Disable interrupts
				sep #0x30			; Switch to 8-bit registers

				stz delayed

            	jsr init
				bcs err$

				plp
				lda ##0
				rtl

err$			plp
				lda ##0xffff
				rtl

;
; Send a TALK command to a device
;
; extern short iecll_talk(uint8_t device)
;
; A = the number of the device to become the talker
;
iecll_talk		php
				sei
				sep #0x30
				
				ora #0x40
            	jsr flush
            	jsr atn_release_data   ; NOTE: does NOT drop ATN!

				plp
				rtl

;
; Send the secondary address to the TALK command, release ATN, and turn around control of the bus
;
; extern short iecll_talk_sa(uint8_t secondary_address)
;
; A = the secondary address to send
;
iecll_talk_sa   php
				sei
				sep #0x30
				
				jsr atn_common
           
				jsr assert_DATA
				jsr release_ATN
				jsr release_CLOCK
1$				jsr read_CLOCK
				bcs 1$ 									; TODO: should time out.
				
				plp
				rtl

;
; Send a LISTEN command to a device
;
; extern short iecll_listen(uint8_t device)
;
; A = the number of the device to become the listener
;
iecll_listen	php
				sei
				sep #0x30
				
				ora #0x20
           		jsr flush
            	jsr atn_release_data   					; NOTE: does NOT drop ATN!

				plp
				rtl
            
;
; Send the secondary address to the LISTEN command and release ATN
;
; extern short iecll_listen_sa(uint8_t secondary_address)
;
; A = the secondary address to send
;
iecll_listen_sa	php
				sei
				sep #0x30
				
				jsr atn_common
            	jsr release_ATN

				; TODO: WARNING!  No delay here!  
				; TODO: IMHO, should wait at least 100us to avoid accidental turn-around!
				; TODO: tho we do protect against this in the send code.

            	plp
				rtl

;
; Send the UNTALK command to all devices and drop ATN
;
; extern void iecll_untalk()
;
iecll_untalk	php
				sei
				sep #0x30
				
				; Detangled from C64 sources; TODO: compare with Stef's

            	lda #0x5f

				; There should never be a need to flush here, and if you
				; do manage to call IECOUT between a TALK/TALKSA and an
				; UNTALK, the C64 will flush it while ATN is asserted and 
				; the drive will be mighty confused.
				; 
				; TODO: track the state and cause calls to IECOUT to fail.

          		; pre-sets CLOCK IMMEDIATELY before the ATN ... again, TODO: makes no sense
            	jsr assert_CLOCK
            	jsr atn_release

				plp
				rtl

;
; Send the UNLISTEN command to all devices
;
; extern void iecll_unlisten()
;
iecll_unlisten 	php
				sei
				sep #0x30
				
				; Detangled from C64 sources; TODO: compare with Stef's

            	lda #0x3f
            	jsr flush
            	jsr atn_release

				plp
				rtl

;
; Try to get a byte from the IEC bus
;
; NOTE: EOI flag is set, if this character is the last to be read from the active talker.
;
; extern uint8_t iecll_in()
;
; Returns:
; A = the byte read
; 
iecll_in		php
				sei					; Disable interrupts
				sep #0x30			; Switch to 8-bit registers

				jsr recv_data

				; NOTE: we'll just read from the eoi variable in a separate function
				;       We might need to return it here folded in with the data somwhow

				plp
				and ##0x00ff
				rtl

;
; Check to see if the last byte read was an EOI byte
;
; extern short iecll_eoi()
;
; Returns:
; A = 0 if not EOI, -1 if EOI
;
iecll_eoi		php
				sep #0x30

				lda rx_eoi
				beq not_eoi$

				plp
				lda ##0xffff
				rtl

not_eoi$		plp
				lda #0
				rtl

;
; Send a byte to the IEC bus. Actually sends the previous byte and queues the current byte.
;
; extern void iecll_out(uint8_t byte)
;
; Inputs:
; A = the byte to send
;
iecll_out		php
				sei
				sep #0x30

				; Sends the byte in A.
				; Actually, sends the previous IECOUT byte, and queues
				; this one for later transmission.  This is done to
				; ensure that we can mark the last data byte with an EOI.

				clc
				bit delayed
				bpl queue$

				; Send the old byte
				pha
				lda queue
				jsr send
				pla
				stz delayed

				; Queue the new byte
queue$          sta queue
				dec delayed

				plp
				rtl

;
; Trigger the reset line on the IEC port
;
iecll_reset:	php
				sei
				sep #0x30

				jsr assert_RST
				jsr sleep_1ms
				jsr release_RST

				plp
				rtl