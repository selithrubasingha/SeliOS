#include "paging.h"
#include "memory.h"
//page directory
unsigned int page_directory[1024] __attribute__((aligned(4096))) ;

//page table
unsigned int first_page_table[1024] __attribute__((aligned(4096))) ;

// void map_page(unsigned int virtual_address, unsigned int physical_address){

//     unsigned int page_directory_index = virtual_address >> 22; 
//     unsigned int page_table_index = (virtual_address >> 12) & 0x03FF;

//     // Set the page table entry
//     first_page_table[page_table_index] = (physical_address |3);

//     // Set the page directory entry
//     // Corrected line in map_page:
//     unsigned int physical_pt_addr = ((unsigned int)first_page_table) - 0xC0000000;
//     page_directory[page_directory_index] = (physical_pt_addr | 3);    
// }

// /*
// The Math Behind | 3
// If we want to map a page so that it actually exists (Present = 1) and we can write to it (Read/Write = 2), we just add the values of those flags together!
// 1 + 2 = 3.

// In binary, the number 3 is 0000 0000 0000 0011.

// When you use the Bitwise OR operator (physical_address | 3), it perfectly pastes those two 1s into the empty zeros at the very bottom of the physical address.

// You are telling the CPU: "Here is the physical address, AND by the way, it is Present and Writeable!"
// */

// void init_paging(unsigned int kernel_physical_start, unsigned int kernel_physical_end) {
//     //mapping the VGA BUFFER page
//     map_page(0xC00B8000,0x000B8000);

//     //mapping the kernel
//     for (unsigned int addr = kernel_physical_start; addr < kernel_physical_end; addr += 4096) {
//         unsigned int virtual_address = addr + 0xC0000000; // Map to the higher half
//         map_page(virtual_address, addr);
//     }

//     // 3. Convert Page Directory virtual address to physical for the CPU hardware
//     unsigned int physical_pd_addr = ((unsigned int)page_directory) - 0xC0000000;
    
//     // 4. Load the physical address into CR3
//     asm volatile("mov %0, %%cr3" : : "r"(physical_pd_addr));

//     // 5. Turn on Paging (Set the PG bit in CR0)
//     // We haven't done this part yet! The CPU won't actually use our 
//     // page directory until we flip this master switch.
//     unsigned int cr0;
//     asm volatile("mov %%cr0, %0" : "=r"(cr0));
//     cr0 |= 0x80000000; // Flip the PG (Paging) bit
//     asm volatile("mov %0, %%cr0" : : "r"(cr0));// Load the page directory into the CR3 register
//     asm volatile("mov %0, %%cr3" : : "r"(page_directory));

// }



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