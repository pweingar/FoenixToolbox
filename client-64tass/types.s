;;;
;;; Types used by the Foenix Toolbox functions
;;;

tb			.namespace

;
; An extent or size of a rectangular area
;
s_extent	.struct
width		.word ?	; The width of the region
height		.word ?	; The height of the region
			.ends

;
; A point on a plane
;
s_point		.struct
x			.word ?	; The column of the point
y			.word ?	; The row of the point
			.ends

;
; A rectangle on the screen
;
s_rect		.struct
origin		.dstruct s_point	; The upper-left corner of the rectangle
size		.dstruct s_extent 	; The size of the rectangle
			.ends

;
; A color (BGR)
;
s_color3	.struct
blue		.byte ?
green		.byte ?
red			.byte ?
			.ends

;
; A color entry for a color lookup table (BGRA)
;
s_color4	.struct
blue		.byte ?
green		.byte ?
red			.byte ?
alpha		.byte ?
			.ends

;
; Type to describe the current time
;
s_time		.struct
year		.word ?
month		.word ?
day			.word ?
hour		.word ?
minute		.word ?
second		.word ?
is_pm		.word ?
is_24hours	.word ?
			.ends

;
; A description of a screen's capabilities
;
s_txt_capabilities	.struct
number				.word ?		; The unique ID of the screen
supported_modes		.word ?		; The display modes supported on this screen
font_size_count		.word ?		; The number of supported font sizes
font_sizes			.dword ?	; Pointer to a list of t_extent listing all supported font sizes
resolution_count	.word ?		; The number of supported display resolutions
resolutions			.dword ?	; Pointer to a list of t_extent listing all supported display resolutions (in pixels)
					.ends

;
; Structure to describe the hardware
;
s_sys_info			.struct
mcp_version			.word ?		; Current version of the MCP kernel 
mcp_rev				.word ?		; Current revision, or sub-version of the MCP kernel 
mcp_build			.word ?		; Current vuild # of the MCP kernel 
model				.word ?		; Code to say what model of machine this is 
sub_model			.word ?		; 0x00 = PB, 0x01 = LB, 0x02 = CUBE 
model_name			.dword ?	; Human readable name of the model of the computer 
cpu					.word ?		; Code to say which CPU is running 
cpu_name			.dword ?	; Human readable name for the CPU 
cpu_clock_khz		.dword ?	; Speed of the CPU clock in KHz 
fpga_date			.dword ?	; YYYYMMDD     
fpga_model			.word ?		; FPGA model number 
fpga_version		.word ?		; FPGA version 
fpga_subver			.word ?		; FPGA sub-version 
system_ram_size		.dword ?	; The number of bytes of system RAM on the board 
has_floppy			.byte ?		; TRUE if the board has a floppy drive installed 
has_hard_drive		.byte ?		; TRUE if the board has a PATA device installed 
has_expansion_card	.byte ?		; TRUE if an expansion card is installed on the device 
has_ethernet		.byte ?		; TRUE if an ethernet port is present 
screens				.word ?		; How many screens are on this computer 

			.endn