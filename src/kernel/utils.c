#include "utils.h"
#include "io.h"
#include "serial.h"
#include "idt.h"
#include "pic.h"
#include "multiboot.h"
#include "gdt.h"
#include "memory.h"
#include "paging.h"
#include "alloc.h"
#include "initrd.h"
#include "string.h"
#include "fs.h"
#include "thread.h"
#include "timer.h"
// Tell kmain to look for these symbols in another file
extern void task_a();
extern void task_b();


// Global framebuffer pointer (with volatile to prevent compiler optimization)
volatile char *fb = (volatile char *) (0x000B8000+ 0xC0000000);

unsigned int cursor_pos = 0;

/** fb_move_cursor:
 * Moves the cursor of the framebuffer to the given position
 *
 * @param pos The new position of the cursor
 */
void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

/** fb_write_cell:
 * Writes a character with the given foreground and background to position i
 * in the framebuffer.
 *
 * @param i  The location in the framebuffer
 * @param c  The character
 * @param fg The foreground color
 * @param bg The background color
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
    fb[i] = c;
    fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

void putchar(int device, char c) {

    // If we hit the bottom of the screen (80 cols * 25 rows = 2000)
    if (cursor_pos >= 2000) {
        // 1. Move all rows up by one (shift everything back by 80 characters)
        // (You can write a simple loop or use memcpy if you have it)
        for (int i = 0; i < 24 * 80; i++) {
            fb[i * 2] = fb[(i + 80) * 2];         // Copy character
            fb[i * 2 + 1] = fb[(i + 80) * 2 + 1]; // Copy color
        }
        
        // 2. Clear out the very last row with blank spaces
        for (int i = 24 * 80; i < 25 * 80; i++) {
            fb_write_cell(i * 2, ' ', FB_GREEN, FB_DARK_GREY);
        }
        
        // 3. Reset the cursor to the start of the final row!
        cursor_pos = 24 * 80; 
    }
    
    if (device == DEVICE_SERIAL) {
        serial_write_char(c);
    } 
    else if (device == DEVICE_FB) {
        // Did we receive a newline character?
        if (c == '\n') {
            // Move cursor to the start of the next row (Next multiple of 80)
            cursor_pos = (cursor_pos / 80 + 1) * 80;
        } else {
            // It's a normal character, print it!
            fb_write_cell(cursor_pos * 2, c, FB_GREEN, FB_DARK_GREY);
            cursor_pos++;
        }
        
        fb_move_cursor(cursor_pos);
    }
}


void printf(int device ,const char* str)
{
    for (unsigned int i = 0; str[i] != '\0'; i++){
        putchar(device, str[i]);
    }
}


// NEW: A helper function to print integers as Hexadecimal strings!
void print_hex(unsigned int num) {
    char hex_str[11] = "0x00000000";
    char hex_chars[] = "0123456789ABCDEF";
    
    for (int i = 7; i >= 0; i--) {
        hex_str[i + 2] = hex_chars[num & 0x0F];
        num >>= 4;
    }
    printf(DEVICE_FB, hex_str);
}

// THE C TRICK: Pretend the linker labels are empty functions
//extern void kernel_physical_start(void);
//extern void kernel_physical_end(void);

void clear_screen() {
    // Fill all 2000 VGA character slots with blank spaces
    for (int i = 0; i < 2000; i++) {
        fb_write_cell(i * 2, ' ', FB_GREEN, FB_DARK_GREY);
    }
    // Reset the global cursor back to the top left (0,0)
    cursor_pos = 0;
    fb_move_cursor(cursor_pos);
}

void print_lock_screen() {
    // Drop down a few lines for vertical centering
    printf(DEVICE_FB, "\n\n\n\n\n");
    
    // The fully translated, pre-padded CP437 ASCII Art!
    printf(DEVICE_FB, "                \xDF\xDF\xDF\xDB\xDB\xDC         \xB0\xB0\xB0              \xDF\xDF\xDF\xDB\xDB\xDC  \xDF\xDF\xDF\xDB\xDB\xDC\n");
    printf(DEVICE_FB, "               \xB0\xB0\xB0 \xDB\xDB\xDB         \xB1\xB1\xB1       \xDC\xDC\xDC   \xB0\xB0\xB0 \xDB\xDB\xDB \xB0\xB0\xB0 \xDB\xDB\xDB\n");
    printf(DEVICE_FB, "               \xB1\xB1\xB1             \xB2\xB2\xB2       \xDF\xDF\xDF   \xB1\xB1\xB1 \xDB\xDB\xDB \xB1\xB1\xB1    \n");
    printf(DEVICE_FB, "               \xDF\xB2\xB2\xDC\xDC\xDC  \xDC\xDB\xDB\xDF\xDB\xDB\xDC \xDB\xDB\xDB      \xDF\xDB\xDB\xDB   \xB2\xB2\xB2 \xDB\xDB\xDB \xDF\xB2\xB2\xDC\xDC\xDC \n");
    printf(DEVICE_FB, "                   \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB       \xDB\xDB\xDB   \xDB\xDB\xDB \xDB\xDB\xDB     \xDB\xDB\xDB\n");
    printf(DEVICE_FB, "                   \xDB\xDB\xDB \xDB\xDB\xDB\xDF\xDF\xDF\xDF \xDB\xDB\xDB       \xDB\xDB\xDB   \xDB\xDB\xDB \xDB\xDB\xDB     \xDB\xDB\xDB\n");
    printf(DEVICE_FB, "               \xDC\xDC\xDC \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDC\xDC\xDC   \xDB\xDB\xDB   \xDB\xDB\xDB \xDB\xDB\xDB \xDC\xDC\xDC \xDB\xDB\xDB\n");
    printf(DEVICE_FB, "               \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB   \xDB\xDB\xDB   \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB \xDB\xDB\xDB\n");
    printf(DEVICE_FB, "               \xDF\xDB\xDB\xDC\xDB\xDB\xDF \xDF\xDB\xDB\xDC\xDB\xDB\xDF \xDF\xDB\xDB\xDC\xDB\xDB\xDF \xDC\xDC\xDB\xDB\xDB\xDC\xDC \xDF\xDB\xDB\xDC\xDB\xDB\xDF \xDF\xDB\xDB\xDC\xDB\xDB\xDF\n");
    printf(DEVICE_FB, "                         \xDB\xDF      \xDF\xDB                           \n");
    printf(DEVICE_FB, "                         \xDC       \xDC                            \n");
    
    // Spacing for the prompt at the bottom
    printf(DEVICE_FB, "\n\n\n\n");
    printf(DEVICE_FB, "                                        [ Press ENTER to access terminal ]      \n");
}

