#include "io.h"
#include "serial.h"
#include "idt.h"
#include "pic.h"
#include "multiboot.h"
#define DEVICE_FB     0
#define DEVICE_SERIAL 1

/* The I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

// Colors from the example
#define FB_GREEN     2
#define FB_DARK_GREY 8

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
    if (device == DEVICE_SERIAL) {
        serial_write_char(c);
    } 
    else if (device == DEVICE_FB) {
        fb_write_cell(cursor_pos * 2, c, FB_GREEN, FB_DARK_GREY);
        
        cursor_pos++;
        
        fb_move_cursor(cursor_pos);
    }
}
void printf(int device ,const char* str)
{
    for (unsigned int i = 0; str[i] != '\0'; i++){
        putchar(device, str[i]);
    }
}

void kmain(unsigned int ebx) {
    serial_init();
    init_idt(); // 1. Setup the emergency phonebook
    pic_init(); // 2. Remap the hardware secretary
    
    printf(DEVICE_FB, "SeliOS Screen Router is ONLINE!\n");
    
    printf(DEVICE_SERIAL, "SeliOS Serial Router is ONLINE!\n");

    asm volatile("sti"); // STI stands for "Set Interrupt flag"

    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;

    // check if GRUB.QEMU actually loaded any modules
    if (mbinfo->mods_count > 0) {
        multiboot_module_t *modules = (multiboot_module_t *) (mbinfo->mods_addr+ 0xC0000000);

        unsigned int address_of_module = modules->mod_start+ 0xC0000000;

        printf(DEVICE_FB, "Module loaded at address: 0x");

        typedef void (*call_module_t)(void);
        call_module_t start_program = (call_module_t) address_of_module;

        start_program(); // Call the module's entry point
    }

    while (1){
        
    }
}

