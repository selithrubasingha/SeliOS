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


void kmain(unsigned int ebx) {
    // --- 1. INITIALIZE PAGING FIRST ---
    // The kernel is running in higher-half. We switch to our custom Paging setup.
    extern void kernel_physical_start(void);
    extern void kernel_physical_end(void);
    unsigned int phys_start = (unsigned int) &kernel_physical_start;
    unsigned int phys_end   = (unsigned int) &kernel_physical_end;
    init_paging(phys_start, phys_end);


    init_gdt();
    serial_init();
    init_idt(); // 1. Setup the emergency phonebook
    pic_init(); // 2. Remap the hardware secretary
    printf(DEVICE_FB, "SeliOS Screen Router is ONLINE!\n");
    
    printf(DEVICE_SERIAL, "SeliOS Serial Router is ONLINE!\n");

    asm volatile("sti"); // STI stands for "Set Interrupt flag"

    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;


    // --- PAGE FRAME ALLOCATOR PREP ---
    
    // 1. Calculate the Kernel boundaries using the C Trick!
    // unsigned int phys_start = (unsigned int) &kernel_physical_start;
    // unsigned int phys_end   = (unsigned int) &kernel_physical_end;
    unsigned int kernel_size = phys_end - phys_start;

    

    // 2. Read the total RAM from GRUB!
    // GRUB gives us memory in Kilobytes. Multiply by 1024 to get raw Bytes!
    unsigned int total_ram_bytes = (mbinfo->mem_lower + mbinfo->mem_upper) * 1024;

    // Setup the Physical Memory Manager!
    init_memory(total_ram_bytes, phys_start, phys_end);

    // Print out the intelligence we just gathered!
    // printf(DEVICE_FB, "Total RAM Available: ");
    // print_hex(total_ram_bytes);
    // printf(DEVICE_FB, " Bytes\n");

    // printf(DEVICE_FB, "Kernel Size: ");
    // print_hex(kernel_size);
    // printf(DEVICE_FB, " Bytes\n");

    // Let's ask the manager for two rooms!
    unsigned int frame1 = allocate_frame();
    unsigned int frame2 = allocate_frame();

    // printf(DEVICE_FB, "First allocated frame: ");
    // print_hex(frame1);
    // printf(DEVICE_FB, "\nSecond allocated frame: ");
    // print_hex(frame2);
    // printf(DEVICE_FB, "\n");
        
    printf(DEVICE_FB, "Testing Heap...\n");
    int *test = (int *)malloc(sizeof(int));
    if (test != NULL) {
        *test = 0xDEADBEEF;
        printf(DEVICE_FB, "Malloc success! Value: ");
        print_hex(*test);
        printf(DEVICE_FB, "\n");
        free(test);
        printf(DEVICE_FB, "Free success!\n");
    } else {
        printf(DEVICE_FB, "Malloc failed!\n");
    }   
    
    // check if GRUB/QEMU actually loaded any modules (Our initrd.img!)
    if (mbinfo->mods_count > 0) {
        // Find the module array provided by GRUB
        multiboot_module_t *modules = (multiboot_module_t *) (mbinfo->mods_addr + 0xC0000000);

        // 1. Find the virtual memory address where GRUB dropped your blob
        unsigned int initrd_location = modules->mod_start + 0xC0000000;
        printf(DEVICE_FB, "Initrd loaded at memory address...\n");

        // 2. Initialize the RAM Disk driver you wrote!
        fs_node_t *fs_root = initialize_initrd(initrd_location);

        // 3. Search for the file using the VFS switchboard
        fs_node_t *hello_node = finddir_fs(fs_root, "hello.txt");

        if (hello_node != NULL) {
            char buffer[256];
            
            // 4. Read the file using the VFS switchboard
            unsigned int bytes_read = read_fs(hello_node, 0, 255, buffer);
            buffer[bytes_read] = '\0'; // Null-terminate the string safely
            
            // 5. Print it to the screen! 
            printf(DEVICE_FB, "Read from file: ");
            printf(DEVICE_FB, buffer);
            printf(DEVICE_FB, "\n");
        } else {
            printf(DEVICE_FB, "ERROR: hello.txt not found!\n");
        }
    } else {
        printf(DEVICE_FB, "ERROR: No GRUB modules loaded!\n");
    }


    printf(DEVICE_FB, "\n--- FIRING SYSTEM CALL FROM KERNEL MODE ---\n");
    
    // This inline assembly forces the number '1' into the EAX register
    // and then manually fires the 128 (0x80) interrupt.
    asm volatile("int $0x80" : : "a"(1));

    printf(DEVICE_FB, "If you see a hello message above me, the router works!\n");
   
    init_threads();
    
    // Now you can use them exactly like before!
    create_user_thread(task_a);
    create_user_thread(task_b);

    while (1) {
        asm volatile("int $0x80" : : "a"(3));
    }   



}

