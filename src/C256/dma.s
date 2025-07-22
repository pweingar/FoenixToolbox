;;;
;;; DMA routines for the F256*X machines
;;;

                    .public dma_copyw_2d

                    .extern _Dp

;
; DAM Constants
;

DMA_CTRL            .equlab 0xf01f00        ; Main controller for the DMA
DMA_CTRL_EN         .equ 0x01               ; Enable the DMA engine
DMA_CTRL_2D         .equ 0x02               ; Enable a 2D transfer (as opposed to linear)
DMA_CTRL_FILL       .equ 0x04               ; Fill rather than copy
DMA_CTRL_IEN        .equ 0x08               ; Interrupt Enable
DMA_16BIT_EN        .equ 0x40               ; Enable 16-bit wide transfers (F256*X only)
DMA_CTRL_TRF        .equ 0x80               ; Start the transfer

DMA_STAT            .equlab 0xf01f01        ; DMA engine status
DMA_STAT_BUSY       .equ 0x80               ; DMA engine is busy with a transfer

DMA_FILL_VALUE      .equlab 0xf01f01        ; Value to use for a fill operation
DMA_SRC_ADDR        .equlab 0xf01f04        ; Starting source address for a copy operation
DMA_DST_ADDR        .equlab 0xf01f08        ; Starting destination address for all transfers
DMA_SIZE            .equlab 0xf01f0c        ; 32-bit size for a linear transfer (in bytes)
DMA_SIZE_X          .equlab 0xf01f0c        ; 16-bit width for a 2D transfer (in bytes)
DMA_SIZE_Y          .equlab 0xf01f0e        ; 16-bit height for a 2D transfer (in rows)
DMA_SRC_STRIDE      .equlab 0xf01f10        ; Size of the row for the containing rectangle at the source (in bytes)
DMA_DST_STRIDE      .equlab 0xf01f12        ; Size of the row for the containing rectangle at the destination (in bytes)

; DMA_CTRL            .equlab 0x021f00        ; Main controller for the DMA
; DMA_CTRL_EN         .equ 0x01               ; Enable the DMA engine
; DMA_CTRL_2D         .equ 0x02               ; Enable a 2D transfer (as opposed to linear)
; DMA_CTRL_FILL       .equ 0x04               ; Fill rather than copy
; DMA_CTRL_IEN        .equ 0x08               ; Interrupt Enable
; DMA_16BIT_EN        .equ 0x40               ; Enable 16-bit wide transfers (F256*X only)
; DMA_CTRL_TRF        .equ 0x80               ; Start the transfer

; DMA_STAT            .equlab 0x021f01        ; DMA engine status
; DMA_STAT_BUSY       .equ 0x80               ; DMA engine is busy with a transfer

; DMA_FILL_VALUE      .equlab 0x021f01        ; Value to use for a fill operation
; DMA_SRC_ADDR        .equlab 0x021f04        ; Starting source address for a copy operation
; DMA_DST_ADDR        .equlab 0x021f08        ; Starting destination address for all transfers
; DMA_SIZE            .equlab 0x021f0c        ; 32-bit size for a linear transfer (in bytes)
; DMA_SIZE_X          .equlab 0x021f0c        ; 16-bit width for a 2D transfer (in bytes)
; DMA_SIZE_Y          .equlab 0x021f0e        ; 16-bit height for a 2D transfer (in rows)
; DMA_SRC_STRIDE      .equlab 0x021f10        ; Size of the row for the containing rectangle at the source (in bytes)
; DMA_DST_STRIDE      .equlab 0x021f12        ; Size of the row for the containing rectangle at the destination (in bytes)

VKY_RASTER_ROW      .equlab 0xf0101a

;
; Structure to hold the information describing the 2D DMA transfer to perform
;
; typedef struct dma_2d_cmd_s {
;     uint16_t * destination;
;     uint16_t * source;
;     uint16_t width;
;     uint16_t height;
;     uint16_t dest_stride
;     uint16_t src_stride
; } dma_2d_cmd_t, *dma_2d_cmd_p;

DMA_CMD_DEST        .equ 0
DMA_CMD_SRC         .equ 4
DMA_CMD_WIDTH       .equ 8
DMA_CMD_HEIGHT      .equ 10
DMA_CMD_DEST_STRIDE .equ 12
DMA_CMD_SRC_STRIDE  .equ 14

				    .section farcode

;
; Copy a 2D block of 16-bit words from one area of memory to another
;
; void dma_copyw_2d(void * dest, void * src, short width, short height, short dest_stride, short src_stride)
;
dma_copyw_2d        phy
                    php

                    sep #0x30

                    ; Wait for any pending DMA transfer to complete
wait1               lda long:DMA_STAT
                    and #DMA_STAT_BUSY
                    cmp #DMA_STAT_BUSY
                    beq wait1

                    ; Enable 16-bit 2D transfer
                    lda #DMA_CTRL_EN | DMA_CTRL_2D | DMA_16BIT_EN
                    sta long:DMA_CTRL

                    ; Set the destination address
                    ldy #DMA_CMD_DEST
                    lda [.tiny _Dp],y
                    sta long:DMA_DST_ADDR

                    ldy #DMA_CMD_DEST+1
                    lda [.tiny _Dp],y
                    sta long:DMA_DST_ADDR+1
            
                    ldy #DMA_CMD_DEST+2
                    lda [.tiny _Dp],y
                    sta long:DMA_DST_ADDR+2

                    ; Set the source address
                    ldy #DMA_CMD_SRC
                    lda [.tiny _Dp],y
                    sta long:DMA_SRC_ADDR

                    ldy #DMA_CMD_SRC+1
                    lda [.tiny _Dp],y
                    sta long:DMA_SRC_ADDR+1
                    
                    ldy #DMA_CMD_SRC+2
                    lda [.tiny _Dp],y
                    sta long:DMA_SRC_ADDR+2

                    ; Set the size of the block
                    rep #0x30
                    ldy ##DMA_CMD_WIDTH
                    lda [.tiny _Dp],y
                    asl a
                    sta long:DMA_SIZE_X

                    ldy ##DMA_CMD_HEIGHT
                    lda [.tiny _Dp],y
                    sta long:DMA_SIZE_Y

                    ; Set the size of the enclosing rectangles
                    ldy ##DMA_CMD_SRC_STRIDE
                    lda [.tiny _Dp],y
                    asl a
                    sta long:DMA_SRC_STRIDE

                    ldy ##DMA_CMD_DEST_STRIDE
                    lda [.tiny _Dp],y
                    asl a
                    sta long:DMA_DST_STRIDE

                    ; Wait for the raster to get to the end of the display
wait_rast           lda long:VKY_RASTER_ROW
                    cmp ##480
                    bcc wait_rast

                    sep #0x30

                    ; Start the transfer
                    lda #DMA_CTRL_EN | DMA_CTRL_2D | DMA_16BIT_EN | DMA_CTRL_TRF
                    sta long:DMA_CTRL

                    ; Wait (maybe not necessary) for the transfer to kick off
                    nop
                    nop
                    nop
                    nop
                    nop
                    nop
                    nop
                    nop

                    ; Wait for the transfer to complete
wait2               lda long:DMA_STAT
                    and #DMA_STAT_BUSY
                    cmp #DMA_STAT_BUSY
                    beq wait2

                    plp
                    ply

                    rtl
