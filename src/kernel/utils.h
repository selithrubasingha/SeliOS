#ifndef UTILS_H
#define UTILS_H

#define DEVICE_FB     0
#define DEVICE_SERIAL 1

/* The I/O ports */
#define FB_COMMAND_PORT          0x3D4
#define FB_DATA_PORT             0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND     14
#define FB_LOW_BYTE_COMMAND      15

/* CP437 Standard VGA Colors */
#define VGA_BLACK          0
#define VGA_BLUE           1
#define VGA_GREEN          2
#define VGA_CYAN           3
#define VGA_RED            4
#define VGA_MAGENTA        5
#define VGA_BROWN          6
#define VGA_LIGHT_GREY     7
#define VGA_DARK_GREY      8
#define VGA_LIGHT_BLUE     9
#define VGA_LIGHT_GREEN    10
#define VGA_LIGHT_CYAN     11
#define VGA_LIGHT_RED      12
#define VGA_LIGHT_MAGENTA  13
#define VGA_YELLOW         14
#define VGA_WHITE          15

/* Default SeliOS Theme */
#define DEFAULT_FG         VGA_GREEN
#define DEFAULT_BG         VGA_DARK_GREY

extern unsigned int cursor_pos;

void fb_move_cursor(unsigned short pos);
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);
void putchar(int device, char c);
void putchar_color(int device, char c, unsigned char fg, unsigned char bg);
void printf(int device, const char* str);
void printf_color(int device, const char* str, unsigned char fg, unsigned char bg);
void print_hex(unsigned int num);
void clear_screen();
void print_lock_screen();

#endif
