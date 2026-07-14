#include "utils.h"
#include "io.h"
#include "serial.h"

volatile char *fb = (volatile char *) (0x000B8000 + 0xC0000000);
unsigned int cursor_pos = 0;

// Internal state tracker for active color codes
static unsigned char active_fg = DEFAULT_FG;
static unsigned char active_bg = DEFAULT_BG;

void fb_move_cursor(unsigned short pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    fb[i] = c;
    fb[i + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F); // Corrected bitwise layout for VGA attributes
}

void putchar_color(int device, char c, unsigned char fg, unsigned char bg) {
    if (cursor_pos >= 2000) {
        // Scroll screen up
        for (int i = 0; i < 24 * 80; i++) {
            fb[i * 2] = fb[(i + 80) * 2];
            fb[i * 2 + 1] = fb[(i + 80) * 2 + 1];
        }
        // Clear last row
        for (int i = 24 * 80; i < 25 * 80; i++) {
            fb_write_cell(i * 2, ' ', fg, bg);
        }
        cursor_pos = 24 * 80; 
    }

    if (device == DEVICE_SERIAL) {
        serial_write_char(c);
    } 
    else if (device == DEVICE_FB) {
        if (c == '\n') {
            cursor_pos = (cursor_pos / 80 + 1) * 80;
        } else {
            fb_write_cell(cursor_pos * 2, c, fg, bg);
            cursor_pos++;
        }
        fb_move_cursor(cursor_pos);
    }
}
/*
in putchar why the hell are we checking for these 1 to 15 non printable characters ? 
BECAUSE THEY ACT LIKE COLOR SWITCHERS !! 

Because putchar intercepts those specific bytes and acts like a color switch, you can now embed hex codes 
directly into your text.

If you put \x02 (Hexadecimal for 2, which is VGA Green) in your string, putchar will silently switch the 
active text color to green and then print the rest of the letters using that color.
*/
void putchar(int device, char c) {
    // Elegant Feature: If char is an unprintable control byte between 1 and 15, 
    // dynamically change the active text color inline!
    if (c >= 1 && c <= 15 && c !='\n') {
        active_fg = c;
        return;
    }
    putchar_color(device, c, active_fg, active_bg);
}

void printf(int device, const char* str) {
    for (unsigned int i = 0; str[i] != '\0'; i++){
        putchar(device, str[i]);
    }
}

void printf_color(int device, const char* str, unsigned char fg, unsigned char bg) {
    unsigned char old_fg = active_fg;
    unsigned char old_bg = active_bg;
    active_fg = fg;
    active_bg = bg;
    
    for (unsigned int i = 0; str[i] != '\0'; i++){
        // If an inline color code override is found inside strings, respect it
        if (str[i] >= 1 && str[i] <= 15 && str[i] != '\n') {
            active_fg = str[i];
        } else {
            putchar_color(device, str[i], active_fg, active_bg);
        }
    }
    
    active_fg = old_fg;
    active_bg = old_bg;
}

void print_hex(unsigned int num) {
    char hex_str[11] = "0x00000000";
    char hex_chars[] = "0123456789ABCDEF";
    for (int i = 7; i >= 0; i--) {
        hex_str[i + 2] = hex_chars[num & 0x0F];
        num >>= 4;
    }
    printf(DEVICE_FB, hex_str);
}

void clear_screen() {
    active_fg = DEFAULT_FG;
    active_bg = DEFAULT_BG;
    for (int i = 0; i < 2000; i++) {
        fb_write_cell(i * 2, ' ', active_fg, active_bg);
    }
    cursor_pos = 0;
    fb_move_cursor(cursor_pos);
}

void print_lock_screen() {
    printf_color(DEVICE_FB, "\n\n\n\n\n", VGA_LIGHT_GREY, VGA_BLACK);
    
    // Explicitly rendering the title with dedicated high-contrast coloring
    printf_color(DEVICE_FB, "                \xDF\xDF\xDF\xDB\xDB\xDC         \xB0\xB0\xB0              \xDF\xDF\xDF\xDB\xDB\xDC  \xDF\xDF\xDF\xDB\xDB\xDC\n", VGA_LIGHT_GREY, VGA_BLACK);
    printf_color(DEVICE_FB, "               \xB0\xB0\xB0 \xDB\xDB\xDB         \xB1\xB1\xB1       \xDC\xDC\xDC   \xB0\xB0\xB0 \xDB\xDB\xDB \xB0\xB0\xB0 \xDB\xDB\xDB\n", VGA_LIGHT_GREY, VGA_BLACK);
    printf_color(DEVICE_FB, "               \xB1\xB1\xB1             \xB2\xB2\xB2       \xDF\xDF\xDF   \xB1\xB1\xB1 \xDB\xDB\xDB \xB1\xB1\xB1    \n", VGA_LIGHT_GREY, VGA_BLACK);
    printf_color(DEVICE_FB, "               \xDF\xB2\xB2\xDC\xDC\xDC  \xDC\xDB\xDB\xDF\xDB\xDB\xDC \xDB\xDB\xDB      \xDF\xDB\xDB\xDB   \xB2\xB2\xB2 \xDB\xDB\xDB \xDF\xB2\xB2\xDC\xDC\xDC \n", VGA_LIGHT_GREY, VGA_BLACK);
    printf_color(DEVICE_FB, "                   \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB       \xDB\xDB\xDB   \xDB\xDB\xDB \xDB\xDB\xDB     \xDB\xDB\xDB\n", VGA_LIGHT_GREY, VGA_BLACK);
    printf_color(DEVICE_FB, "                   \xDB\xDB\xDB \xDB\xDB\xDB\xDF\xDF\xDF\xDF \xDB\xDB\xDB       \xDB\xDB\xDB   \xDB\xDB\xDB \xDB\xDB\xDB     \xDB\xDB\xDB\n", VGA_LIGHT_GREY, VGA_BLACK);
    printf_color(DEVICE_FB, "               \xDC\xDC\xDC \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDC\xDC\xDC   \xDB\xDB\xDB   \xDB\xDB\xDB \xDB\xDB\xDB \xDC\xDC\xDC \xDB\xDB\xDB\n", VGA_LIGHT_GREY, VGA_BLACK);
    printf_color(DEVICE_FB, "               \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB   \xDB\xDB\xDB   \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB\n", VGA_LIGHT_GREY, VGA_BLACK);
    printf_color(DEVICE_FB, "               \xDF\xDB\xDB\xDC\xDB\xDB\xDF \xDF\xDB\xDB\xDC\xDB\xDB\xDF \xDF\xDB\xDB\xDC\xDB\xDB\xDF \xDC\xDC\xDB\xDB\xDB\xDC\xDC \xDF\xDB\xDB\xDC\xDB\xDB\xDF \xDF\xDB\xDB\xDC\xDB\xDB\xDF\n", VGA_LIGHT_GREY, VGA_BLACK);
    printf_color(DEVICE_FB, "                         \xDB\xDF      \xDF\xDB                           \n", VGA_LIGHT_GREY, VGA_BLACK);
    printf_color(DEVICE_FB, "                         \xDC       \xDC                            \n", VGA_LIGHT_GREY, VGA_BLACK);
    
    printf_color(DEVICE_FB, "\n\n\n\n", VGA_LIGHT_GREY, VGA_BLACK);
    printf_color(DEVICE_FB, "                                        \x0A[ Press ENTER to access terminal ]\x07      \n", VGA_LIGHT_GREY, VGA_BLACK);
}

