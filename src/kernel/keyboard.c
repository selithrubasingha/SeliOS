#include "io.h"
#include "keyboard.h"
#include "serial.h"
#define KBD_DATA_PORT 0x60
#define DEVICE_FB 0

extern void printf(int device, const char* str);

/* KBDUS means US Keyboard Layout. This is a scancode table
 * used to layout a standard US keyboard. I have left some
 * comments in so you can see what code matches what key.
 */
unsigned char kbd_US[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
  '9', '0', '-', '=', '\b', /* Backspace */
  '\t',         /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,          /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
 '\'', '`',   0,        /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
  'm', ',', '.', '/',   0,              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0, /* All other keys are undefined */
};


/** read_scan_code:
 * Reads a scan code from the keyboard
 *
 * @return The scan code (NOT an ASCII character!)
 */
unsigned char read_scan_code(void)
{
    return inb(KBD_DATA_PORT);
}

void keyboard_handler(){
    unsigned char scan_code = read_scan_code();

    if (scan_code & 0x80) {
        return;
}   
    char ascii = kbd_US[scan_code];


    if (ascii != 0) {
        // Use your serial_write or fb_write to print the character
        char str[2] = {ascii, '\0'}; // Create a string with the character and null terminator
        serial_write_string(str);
        printf(DEVICE_FB, str);

    }
}
