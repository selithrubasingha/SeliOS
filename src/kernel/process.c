
#include "paging.h"
#include "memory.h"
#include "process.h"
#define NULL ((void *)0)
unsigned int user_page_directory[1024] __attribute__((aligned(4096))) ;


/*
THE ULTIMATE THING THAT HAPPENS INSIDE CREATE PROCESS 

1) Build the empty universe: You create a new Page Directory. The top 1GB has the kernel, but the bottom 3GB is completely blank.

2) Put on the headset: You switch CR3. You are now standing inside the new, blank universe.

3) Build the house: Now you allocate the frames and map them to 0x00000000.

4) Move the tenant in: Now you memcpy the GRUB binary into 0x00000000.
*/
process_t* create_process(unsigned int module_phys_addr, unsigned int module_size) {

    // 1. malloc() the process_t struct
    process_t *new_process = (process_t *)malloc(sizeof(process_t));
    if (!new_process) {
        return NULL;
    }


    // 2. new_process->page_directory = create_user_page_directory();
    new_process->page_directory = create_user_page_directory();

    /* * CONTEXT SWITCH: Load the new Page Directory into the CPU hardware.
 * - asm volatile: Tells the GCC optimizer "DO NOT delete or move this instruction!"
 * - mov %0, %%cr3: Updates CR3 (the CPU's physical page directory pointer).
 * - : :          : Inline assembly syntax (we have no output variables).
 * - "r"(...)     : Tells GCC to pick any free CPU register to pass our physical address into %0.
 * NOTE: The exact instant this executes, the bottom 3GB of virtual memory swaps to the new sandbox!
 */
    asm volatile("mov %0, %%cr3" : : "r"(new_process->page_directory));


    // 3. allocate_frame() for code and stack

    unsigned int num_pages = (module_size + 4095) / 4096; // Calculate the number of pages needed for the module

        
    for (unsigned int i = 0; i < num_pages; i++) {
        unsigned int frame = allocate_frame();
        if (frame == 0) {
            return NULL; // Out of memory!
        }
        
        // Map directly into the user's empty sandbox starting at 0x00000000
        // We use map_page_user so the U/S bit is set to 1!
        unsigned int virtual_addr = 0x00000000 + (i * 4096);
        map_user_page((unsigned int *)0xC0500000, virtual_addr, frame); 
    }

    // 5. memcpy the GRUB binary into the code frame
    unsigned int stack_frame = allocate_frame();
    map_user_page((unsigned int *)0xC0500000, 0xBFFFF000, stack_frame);

    // 6. Return the finished process struct
    memcpy((void *)0x00000000, (void *)(module_phys_addr + 0xC0000000), module_size);
    return new_process;

}

