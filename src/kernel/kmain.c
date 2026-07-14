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
#include "utils.h"
#include "terminal.h"

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
    // printf(DEVICE_FB, "SeliOS Screen Router is ONLINE!\n");
    
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
        
    // printf(DEVICE_FB, "Testing Heap...\n");
    // int *test = (int *)malloc(sizeof(int));
    // if (test != NULL) {
    //     *test = 0xDEADBEEF;
    //     printf(DEVICE_FB, "Malloc success! Value: ");
    //     print_hex(*test);
    //     printf(DEVICE_FB, "\n");
    //     free(test);
    //     printf(DEVICE_FB, "Free success!\n");
    // } else {
    //     printf(DEVICE_FB, "Malloc failed!\n");
    // }   

    // Wipe all the debug logs
    clear_screen();
    
    // Print the masterpiece
    print_lock_screen();

   // 1. Wait on the lock screen until the keyboard driver updates shell_active
    while(!shell_active) {
        asm volatile("hlt");
    }

    // 2. The user pressed Enter! Clear the art and boot the terminal UI
    init_terminal();

    // 3. Keep the CPU alive forever to process terminal commands
    while(1) {
        asm volatile("hlt");
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
            // printf(DEVICE_FB, "Read from file: ");
            // printf(DEVICE_FB, buffer);
            // printf(DEVICE_FB, "\n");
        } else {
            printf(DEVICE_FB, "ERROR: hello.txt not found!\n");
        }
    } else {
        printf(DEVICE_FB, "ERROR: No GRUB modules loaded!\n");
    }


    printf(DEVICE_FB, "\n--- FIRING SYSTEM CALL FROM KERNEL MODE ---\n");
    
    // This inline assembly forces the number '1' into the EAX register
    // and then manually fires the 128 (0x80) interrupt.
    //asm volatile("int $0x80" : : "a"(1));

    //printf(DEVICE_FB, "If you see a hello message above me, the router works!\n");
    
    // printf(DEVICE_FB, "\n--- PREEMPTIVE SCHEDULING ---\n");
    // init_threads();
    
    // // Now you can use them exactly like before!
    // create_user_thread(task_a);
    // create_user_thread(task_b);


    // init_timer(100); // Initialize the timer with a frequency of 100 Hz

    while (1) {
        // asm volatile("sti"); // Just make sure interrupts are on!
        // asm volatile("hlt"); // Put the CPU to sleep until the next timer tick
    }   



}

