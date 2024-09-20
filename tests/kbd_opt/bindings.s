
;
; extern SYSTEMCALL void sys_exit(short result);
; 
; result goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_exit = $000000


;
; extern SYSTEMCALL void sys_int_enable_all();
;
; 0 bytes needed for the stack parameters
;
sys_int_enable_all = $ffe004


;
; extern SYSTEMCALL void sys_int_disable_all();
;
; 0 bytes needed for the stack parameters
;
sys_int_disable_all = $ffe008


;
; extern SYSTEMCALL void sys_int_disable(unsigned short n);
; 
; n goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_int_disable = $ffe00c


;
; extern SYSTEMCALL void sys_int_enable(unsigned short n);
; 
; n goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_int_enable = $ffe010


;
; extern SYSTEMCALL p_int_handler sys_int_register(unsigned short n, p_int_handler handler);
; 
; n goes in A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_int_register = $ffe014

int_register    .namespace
                .virtual 1,s
handler         .dword ?      ; pointer to the interrupt handler to register
                .endv
                .endn

;
; extern SYSTEMCALL short sys_int_pending(unsigned short n);
; 
; n goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_int_pending = $ffe018


;
; extern SYSTEMCALL void sys_get_info(p_sys_info info);
; 
; info goes in X[15..0]:A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_get_info = $ffe01c


;
; extern SYSTEMCALL void sys_int_clear(unsigned short n);
; 
; n goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_int_clear = $ffe020


;
; extern SYSTEMCALL short sys_chan_read_b(short channel);
; 
; channel goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_chan_read_b = $ffe024


;
; extern SYSTEMCALL short sys_chan_read(short channel, unsigned char * buffer, short size);
; 
; channel goes in A[15..0]
;
; 6 bytes needed for the stack parameters
;
sys_chan_read = $ffe028

chan_read       .namespace
                .virtual 1,s
buffer          .dword ?      ; the buffer into which to copy the channel data
size            .word ?       ; the size of the buffer.
                .endv
                .endn

;
; extern SYSTEMCALL short sys_chan_readline(short channel, unsigned char * buffer, short size);
; 
; channel goes in A[15..0]
;
; 6 bytes needed for the stack parameters
;
sys_chan_readline = $ffe02c

chan_readline   .namespace
                .virtual 1,s
buffer          .dword ?      ; the buffer into which to copy the channel data
size            .word ?       ; the size of the buffer
                .endv
                .endn

;
; extern SYSTEMCALL short sys_chan_write_b(short channel, uint8_t b);
; 
; channel goes in A[15..0]
;
; 1 bytes needed for the stack parameters
;
sys_chan_write_b = $ffe030

chan_write_b    .namespace
                .virtual 1,s
b               .byte ?       ; the byte to write
                .endv
                .endn

;
; extern SYSTEMCALL short sys_chan_write(short channel, const uint8_t * buffer, short size);
; 
; channel goes in A[15..0]
;
; 6 bytes needed for the stack parameters
;
sys_chan_write = $ffe034

chan_write      .namespace
                .virtual 1,s
buffer          .dword ?      ; 
size            .word ?       ; 
                .endv
                .endn

;
; extern SYSTEMCALL short sys_chan_status(short channel);
; 
; channel goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_chan_status = $ffe038


;
; extern SYSTEMCALL short sys_chan_flush(short channel);
; 
; channel goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_chan_flush = $ffe03c


;
; extern SYSTEMCALL short sys_chan_seek(short channel, long position, short base);
; 
; channel goes in A[15..0]
;
; 6 bytes needed for the stack parameters
;
sys_chan_seek = $ffe040

chan_seek       .namespace
                .virtual 1,s
position        .dword ?      ; the position of the cursor
base            .word ?       ; whether the position is absolute or relative to the current position
                .endv
                .endn

;
; extern SYSTEMCALL short sys_chan_ioctrl(short channel, short command, uint8_t * buffer, short size);
; 
; channel goes in A[15..0]
;
; 8 bytes needed for the stack parameters
;
sys_chan_ioctrl = $ffe044

chan_ioctrl     .namespace
                .virtual 1,s
command         .word ?       ; the number of the command to send
buffer          .dword ?      ; pointer to bytes of additional data for the command
size            .word ?       ; the size of the buffer
                .endv
                .endn

;
; extern SYSTEMCALL short sys_chan_open(short dev, const char * path, short mode);
; 
; dev goes in A[15..0]
;
; 6 bytes needed for the stack parameters
;
sys_chan_open = $ffe048

chan_open       .namespace
                .virtual 1,s
path            .dword ?      ; a "path" describing how the device is to be open
mode            .word ?       ; s the device to be read, written, both? (0x01 = READ flag, 0x02 = WRITE flag, 0x03 = READ and WRITE)
                .endv
                .endn

;
; extern SYSTEMCALL short sys_chan_close(short chan);
; 
; chan goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_chan_close = $ffe04c


;
; extern SYSTEMCALL short sys_chan_swap(short channel1, short channel2);
; 
; channel1 goes in A[15..0]
;
; 2 bytes needed for the stack parameters
;
sys_chan_swap = $ffe050

chan_swap       .namespace
                .virtual 1,s
channel2        .word ?       ; the ID of the other channel
                .endv
                .endn

;
; extern SYSTEMCALL short sys_chan_device(short channel);
; 
; channel goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_chan_device = $ffe054


;
; extern SYSTEMCALL void sys_text_setsizes(short chan);
; 
; chan goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_text_setsizes = $000000


;
; extern SYSTEMCALL short sys_bdev_register(p_dev_block device);
; 
; device goes in X[15..0]:A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_bdev_register = $ffe05c


;
; extern SYSTEMCALL short sys_bdev_read(short dev, long lba, uint8_t * buffer, short size);
; 
; dev goes in A[15..0]
;
; 10 bytes needed for the stack parameters
;
sys_bdev_read = $ffe060

bdev_read       .namespace
                .virtual 1,s
lba             .dword ?      ; the logical block address of the block to read
buffer          .dword ?      ; the buffer into which to copy the block data
size            .word ?       ; the size of the buffer.
                .endv
                .endn

;
; extern SYSTEMCALL short sys_bdev_write(short dev, long lba, const uint8_t * buffer, short size);
; 
; dev goes in A[15..0]
;
; 10 bytes needed for the stack parameters
;
sys_bdev_write = $ffe064

bdev_write      .namespace
                .virtual 1,s
lba             .dword ?      ; the logical block address of the block to write
buffer          .dword ?      ; the buffer containing the data to write
size            .word ?       ; the size of the buffer.
                .endv
                .endn

;
; extern SYSTEMCALL short sys_bdev_status(short dev);
; 
; dev goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_bdev_status = $ffe068


;
; extern SYSTEMCALL short sys_bdev_flush(short dev);
; 
; dev goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_bdev_flush = $ffe06c


;
; extern SYSTEMCALL short sys_bdev_ioctrl(short dev, short command, uint8_t * buffer, short size);
; 
; dev goes in A[15..0]
;
; 8 bytes needed for the stack parameters
;
sys_bdev_ioctrl = $ffe070

bdev_ioctrl     .namespace
                .virtual 1,s
command         .word ?       ; the number of the command to send
buffer          .dword ?      ; pointer to bytes of additional data for the command
size            .word ?       ; the size of the buffer
                .endv
                .endn

;
; extern SYSTEMCALL short sys_fsys_open(const char * path, short mode);
; 
; path goes in X[15..0]:A[15..0]
;
; 2 bytes needed for the stack parameters
;
sys_fsys_open = $ffe074

fsys_open       .namespace
                .virtual 1,s
mode            .word ?       ; the mode (e.g. r/w/create)
                .endv
                .endn

;
; extern SYSTEMCALL short sys_fsys_close(short fd);
; 
; fd goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_fsys_close = $ffe078


;
; extern SYSTEMCALL short sys_fsys_opendir(const char * path);
; 
; path goes in X[15..0]:A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_fsys_opendir = $ffe07c


;
; extern SYSTEMCALL short sys_fsys_closedir(short dir);
; 
; dir goes in A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_fsys_closedir = $ffe080


;
; extern SYSTEMCALL short sys_fsys_readdir(short dir, p_file_info file);
; 
; dir goes in A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_fsys_readdir = $ffe084

fsys_readdir    .namespace
                .virtual 1,s
file            .dword ?      ; pointer to the t_file_info structure to fill out.
                .endv
                .endn

;
; extern SYSTEMCALL short sys_fsys_findfirst(const char * path, const char * pattern, p_file_info file);
; 
; path goes in X[15..0]:A[15..0]
;
; 8 bytes needed for the stack parameters
;
sys_fsys_findfirst = $ffe088

fsys_findfirst  .namespace
                .virtual 1,s
pattern         .dword ?      ; the file name pattern to search for
file            .dword ?      ; pointer to the t_file_info structure to fill out
                .endv
                .endn

;
; extern SYSTEMCALL short sys_fsys_findnext(short dir, p_file_info file);
; 
; dir goes in A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_fsys_findnext = $ffe08c

fsys_findnext   .namespace
                .virtual 1,s
file            .dword ?      ; pointer to the t_file_info structure to fill out
                .endv
                .endn

;
; extern SYSTEMCALL short sys_fsys_get_label(const char * path, char * label);
; 
; path goes in X[15..0]:A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_fsys_get_label = $ffe090

fsys_get_label  .namespace
                .virtual 1,s
label           .dword ?      ; buffer that will hold the label... should be at least 35 bytes
                .endv
                .endn

;
; extern SYSTEMCALL short sys_fsys_set_label(short drive, const char * label);
; 
; drive goes in A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_fsys_set_label = $ffe094

fsys_set_label  .namespace
                .virtual 1,s
label           .dword ?      ; buffer that holds the label
                .endv
                .endn

;
; extern SYSTEMCALL short sys_fsys_mkdir(const char * path);
; 
; path goes in X[15..0]:A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_fsys_mkdir = $ffe098


;
; extern SYSTEMCALL short sys_fsys_delete(const char * path);
; 
; path goes in X[15..0]:A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_fsys_delete = $ffe09c


;
; extern SYSTEMCALL short sys_fsys_rename(const char * old_path, const char * new_path);
; 
; old_path goes in X[15..0]:A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_fsys_rename = $ffe0a0

fsys_rename     .namespace
                .virtual 1,s
new_path        .dword ?      ; the new path for the file
                .endv
                .endn

;
; extern SYSTEMCALL short sys_fsys_set_cwd(const char * path);
; 
; path goes in X[15..0]:A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_fsys_set_cwd = $ffe0a4


;
; extern SYSTEMCALL short sys_fsys_get_cwd(char * path, short size);
; 
; path goes in X[15..0]:A[15..0]
;
; 2 bytes needed for the stack parameters
;
sys_fsys_get_cwd = $ffe0a8

fsys_get_cwd    .namespace
                .virtual 1,s
size            .word ?       ; the size of the buffer in bytes
                .endv
                .endn

;
; extern SYSTEMCALL short sys_fsys_load(const char * path, uint32_t destination, uint32_t * start);
; 
; path goes in X[15..0]:A[15..0]
;
; 8 bytes needed for the stack parameters
;
sys_fsys_load = $ffe0ac

fsys_load       .namespace
                .virtual 1,s
destination     .dword ?      ; the destination address (0 for use file's address)
start           .dword ?      ; pointer to the long variable to fill with the starting address
                .endv
                .endn

;
; extern SYSTEMCALL short sys_fsys_register_loader(const char * extension, p_file_loader loader);
; 
; extension goes in X[15..0]:A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_fsys_register_loader = $ffe0b0

fsys_register_loader	.namespace
                .virtual 1,s
loader          .dword ?      ; pointer to the file load routine to add
                .endv
                .endn

;
; extern SYSTEMCALL short sys_fsys_stat(const char * path, p_file_info file);
; 
; path goes in X[15..0]:A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_fsys_stat = $ffe0b4

fsys_stat       .namespace
                .virtual 1,s
file            .dword ?      ; pointer to a file info record to fill in, if the file is found.
                .endv
                .endn

;
; extern SYSTEMCALL uint32_t sys_mem_get_ramtop();
;
; 0 bytes needed for the stack parameters
;
sys_mem_get_ramtop = $ffe0b8


;
; extern SYSTEMCALL uint32_t sys_mem_reserve(uint32_t bytes);
; 
; bytes goes in X[15..0]:A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_mem_reserve = $ffe0bc


;
; extern SYSTEMCALL uint32_t sys_time_jiffies();
;
; 0 bytes needed for the stack parameters
;
sys_time_jiffies = $ffe0c0


;
; extern SYSTEMCALL void sys_rtc_set_time(p_time time);
; 
; time goes in X[15..0]:A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_rtc_set_time = $ffe0c4


;
; extern SYSTEMCALL void sys_rtc_get_time(p_time time);
; 
; time goes in X[15..0]:A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_rtc_get_time = $ffe0c8


;
; extern SYSTEMCALL uint16_t sys_kbd_scancode();
;
; 0 bytes needed for the stack parameters
;
sys_kbd_scancode = $ffe0cc


;
; extern SYSTEMCALL short sys_kbd_layout(const char * tables);
; 
; tables goes in X[15..0]:A[15..0]
;
; 0 bytes needed for the stack parameters
;
sys_kbd_layout = $ffe0d4


;
; extern SYSTEMCALL short sys_proc_run(const char * path, int argc, char * argv[]);
; 
; path goes in X[15..0]:A[15..0]
;
; 8 bytes needed for the stack parameters
;
sys_proc_run = $ffe0d8

proc_run        .namespace
                .virtual 1,s
argc            .dword ?      ; the number of arguments passed
argv            .dword ?      ; the array of string arguments
                .endv
                .endn

;
; extern SYSTEMCALL short sys_txt_set_mode(short screen, short mode);
; 
; screen goes in A[15..0]
;
; 2 bytes needed for the stack parameters
;
sys_txt_set_mode = $ffe0e0

txt_set_mode    .namespace
                .virtual 1,s
mode            .word ?       ; a bitfield of desired display mode options
                .endv
                .endn

;
; extern SYSTEMCALL void sys_txt_set_xy(short screen, short x, short y);
; 
; screen goes in A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_txt_set_xy = $ffe0e8

txt_set_xy      .namespace
                .virtual 1,s
x               .word ?       ; the column for the cursor
y               .word ?       ; the row for the cursor
                .endv
                .endn

;
; extern SYSTEMCALL void sys_txt_get_xy(short screen, p_point position);
; 
; screen goes in A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_txt_get_xy = $ffe0ec

txt_get_xy      .namespace
                .virtual 1,s
position        .dword ?      ; pointer to a t_point record to fill out
                .endv
                .endn

;
; extern SYSTEMCALL short sys_txt_get_region(short screen, p_rect region);
; 
; screen goes in A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_txt_get_region = $ffe0f0

txt_get_region  .namespace
                .virtual 1,s
region          .dword ?      ; pointer to a t_rect describing the rectangular region (using character cells for size and size)
                .endv
                .endn

;
; extern SYSTEMCALL short sys_txt_set_region(short screen, p_rect region);
; 
; screen goes in A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_txt_set_region = $ffe0f4

txt_set_region  .namespace
                .virtual 1,s
region          .dword ?      ; pointer to a t_rect describing the rectangular region (using character cells for size and size)
                .endv
                .endn

;
; extern SYSTEMCALL void sys_txt_set_color(short screen, unsigned char foreground, unsigned char background);
; 
; screen goes in A[15..0]
;
; 2 bytes needed for the stack parameters
;
sys_txt_set_color = $ffe0f8

txt_set_color   .namespace
                .virtual 1,s
foreground      .byte ?       ; the Text LUT index of the new current foreground color (0 - 15)
background      .byte ?       ; the Text LUT index of the new current background color (0 - 15)
                .endv
                .endn

;
; extern SYSTEMCALL void sys_txt_get_color(short screen, unsigned char * foreground, unsigned char * background);
; 
; screen goes in A[15..0]
;
; 8 bytes needed for the stack parameters
;
sys_txt_get_color = $ffe0fc

txt_get_color   .namespace
                .virtual 1,s
foreground      .dword ?      ; the Text LUT index of the new current foreground color (0 - 15)
background      .dword ?      ; the Text LUT index of the new current background color (0 - 15)
                .endv
                .endn

;
; extern SYSTEMCALL void sys_txt_set_cursor_visible(short screen, short is_visible);
; 
; screen goes in A[15..0]
;
; 2 bytes needed for the stack parameters
;
sys_txt_set_cursor_visible = $ffe100

txt_set_cursor_visible	.namespace
                .virtual 1,s
is_visible      .word ?       ; TRUE if the cursor should be visible, FALSE (0) otherwise
                .endv
                .endn

;
; extern SYSTEMCALL short sys_txt_set_font(short screen, short width, short height, unsigned char * data);
; 
; screen goes in A[15..0]
;
; 8 bytes needed for the stack parameters
;
sys_txt_set_font = $ffe104

txt_set_font    .namespace
                .virtual 1,s
width           .word ?       ; width of a character in pixels
height          .word ?       ; of a character in pixels
data            .dword ?      ; pointer to the raw font data to be loaded
                .endv
                .endn

;
; extern SYSTEMCALL void sys_txt_get_sizes(short screen, p_extent text_size, p_extent pixel_size);
; 
; screen goes in A[15..0]
;
; 8 bytes needed for the stack parameters
;
sys_txt_get_sizes = $ffe108

txt_get_sizes   .namespace
                .virtual 1,s
text_size       .dword ?      ; the size of the screen in visible characters (may be null)
pixel_size      .dword ?      ; the size of the screen in pixels (may be null)
                .endv
                .endn

;
; extern SYSTEMCALL void sys_txt_set_border(short screen, short width, short height);
; 
; screen goes in A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_txt_set_border = $ffe10c

txt_set_border  .namespace
                .virtual 1,s
width           .word ?       ; the horizontal size of one side of the border (0 - 32 pixels)
height          .word ?       ; the vertical size of one side of the border (0 - 32 pixels)
                .endv
                .endn

;
; extern SYSTEMCALL void sys_txt_set_border_color(short screen, unsigned char red, unsigned char green, unsigned char blue);
; 
; screen goes in A[15..0]
;
; 3 bytes needed for the stack parameters
;
sys_txt_set_border_color = $ffe110

txt_set_border_color	.namespace
                .virtual 1,s
red             .byte ?       ; the red component of the color (0 - 255)
green           .byte ?       ; the green component of the color (0 - 255)
blue            .byte ?       ; the blue component of the color (0 - 255)
                .endv
                .endn

;
; extern SYSTEMCALL void sys_txt_put(short screen, char c);
; 
; screen goes in A[15..0]
;
; 1 bytes needed for the stack parameters
;
sys_txt_put = $ffe114

txt_put         .namespace
                .virtual 1,s
c               .byte ?       ; the character to print
                .endv
                .endn

;
; extern SYSTEMCALL void sys_txt_print(short screen, const char * message);
; 
; screen goes in A[15..0]
;
; 4 bytes needed for the stack parameters
;
sys_txt_print = $ffe118

txt_print       .namespace
                .virtual 1,s
message         .dword ?      ; the ASCII Z string to print
                .endv
                .endn
