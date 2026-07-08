#include "paging.h"
#include "memory.h"
#define NULL ((void *)0)

//page directory
unsigned int page_directory[1024] __attribute__((aligned(4096))) ;
unsigned int user_page_directory[1024] __attribute__((aligned(4096))) ;

//page table
unsigned int first_page_table[1024] __attribute__((aligned(4096))) ;


void map_page(unsigned int virtual_address, unsigned int physical_address) {
    unsigned int pd_index = virtual_address >> 22; 
    unsigned int pt_index = (virtual_address >> 12) & 0x03FF;

    // Only map the Page Table if it's not already mapped
    if (!(page_directory[pd_index] & 0x1)) {
        unsigned int pt_phys = ((unsigned int)first_page_table) - 0xC0000000;
        page_directory[pd_index] = (pt_phys | 3); // Bit 0=Present, Bit 1=Read/Write
    }

    // Map the physical frame to the Page Table line
    first_page_table[pt_index] = (physical_address | 3);
}


void map_user_page(unsigned int *target_directory, unsigned int virtual_address, unsigned int physical_address) {
    unsigned int pd_index = virtual_address >> 22; 
    unsigned int pt_index = (virtual_address >> 12) & 0x03FF;

    // 1. Check if a Page Table already exists for this memory region
    if (!(target_directory[pd_index] & 0x1)) {
        // We need a new table! Allocate a raw physical frame for it.
        unsigned int new_table_phys = allocate_frame();

        // Temporarily map it to our workbench (0xC0600000) so C can touch it.
        // We can use the kernel's normal map_page for the workbench.
        map_page(0xC0600000, new_table_phys);
        unsigned int *pt_ptr = (unsigned int *) 0xC0600000;

        // Zero out the new table so there's no random garbage data
        for (int i = 0; i < 1024; i++) {
            pt_ptr[i] = 0;
        }

        // Link the new table into the user's directory. 
        // | 7 means: Present (1) + Read/Write (2) + User Mode (4)
        target_directory[pd_index] = (new_table_phys | 7);
    }

    // 2. We know the table exists now. We need its physical address 
    // to map it to our workbench so we can write the final frame into it.
    // (We use & 0xFFFFF000 to strip away the | 7 flags and get the raw address)
    unsigned int table_phys = target_directory[pd_index] & 0xFFFFF000;

    // Put the table on the workbench
    map_page(0xC0600000, table_phys);
    unsigned int *pt_ptr = (unsigned int *) 0xC0600000;

    // 3. Write the actual physical memory frame into the table!
    // Again, | 7 ensures the user program is legally allowed to access it.
    pt_ptr[pt_index] = (physical_address | 7);
}




void init_paging(unsigned int phys_start, unsigned int phys_end) {
    // 1. Clear directory
    for(int i = 0; i < 1024; i++) page_directory[i] = 0;

    // 2. IDENTITY MAP (0x00000000 to 0x00400000)
    // This maps the first 4MB of RAM to the first 4MB of Virtual Memory.
    // This allows the CPU to breathe safely during the switch.
    for (unsigned int addr = 0; addr < 0x400000; addr += 4096) {
        map_page(addr, addr); 
    }

    // 3. Map the VGA buffer (0xB8000 -> 0xC00B8000)
    map_page(0xC00B8000, 0x000B8000);
    
    // 4. Map the kernel
    for (unsigned int addr = phys_start; addr < phys_end; addr += 4096) {
        map_page(addr + 0xC0000000, addr);
    }

    // 5. Convert PD virtual address to physical address
    unsigned int pd_phys = ((unsigned int)page_directory) - 0xC0000000;

    // 6. Load physical address of PD into CR3
    asm volatile("mov %0, %%cr3" : : "r"(pd_phys));

    // 7. Turn on Paging
    unsigned int cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; 
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}


unsigned int create_user_page_directory() {
    /*
    The C Code is Trapped: The moment you flipped that CR0 bit in your init_paging function, your C code was permanently 
    trapped in the virtual matrix. A C pointer like 0xC0500000 is completely fake.

The Bridge (Mapping Once): When you call map_page(0xC0500000, phys_dir_addr), you are configuring the CPU hardware. You are
 telling it: "Hey, if my C code ever tries to read or write to this fake address, secretly redirect it to this real
  physical frame."

The Array Math: You only need to map it once because a physical frame is exactly 4096 bytes long. An unsigned int 
in C is 4 bytes. 1024 * 4 = 4096. So, when your C code loops from dir_ptr[0] to dir_ptr[1023], the CPU perfectly 
translates that into filling the entire 4KB physical frame, byte for byte, all routed through that single virtual
 starting address!

The Hardware Reality: The CPU's CR3 register is a physical piece of silicon. It doesn't understand the C code's 
virtual illusion. That is exactly why the function must return the raw, unmapped physical address—so you can hand 
it directly to the hardware later when it's time to run the user program.
    */

    
    // 1. Get a raw physical frame for the new directory. 
    // It is always 4KB, so no size parameter is needed.
    unsigned int phys_dir_addr = allocate_frame();
    if (phys_dir_addr == 0) {
        return 0; // Out of memory!
    }

    // 2. We cannot write to 'phys_dir_addr' directly because paging is active.
    // So, we temporarily map this single physical frame to a safe virtual address 
    // (e.g., 0xC0500000) using your existing kernel map_page function.
    unsigned int temp_virtual_addr = 0xC0500000;
    map_page(temp_virtual_addr, phys_dir_addr);

    // 3. Create a C pointer to that virtual address so we can safely edit the frame.
    unsigned int *dir_ptr = (unsigned int *) temp_virtual_addr;

    // 4. Zero out the entire directory so there is no garbage data.
    for (int i = 0; i < 1024; i++) {
        dir_ptr[i] = 0;
    }

    // 5. Copy the kernel's higher-half mappings (from index 768 to 1023).
    // This perfectly clones the top 1GB of memory (Kernel, Heap, VGA).
    for (int i = 768; i < 1024; i++) {
        dir_ptr[i] = page_directory[i];
    }
       
    // 6. Return the PHYSICAL address. The process struct needs this 
    // so we can eventually load it into the CR3 register!
    return phys_dir_addr;
}