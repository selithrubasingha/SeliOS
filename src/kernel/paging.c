
//page directory
unsigned int page_directory[1024] __attribute__((aligned(4096))) ;

//page table
unsigned int first_page_table[1024] __attribute__((aligned(4096))) ;

void map_page(unsigned int virtual_address, unsigned int physical_address){

    unsigned int page_directory_index = virtual_address >> 22; 
    unsigned int page_table_index = (virtual_address >> 12) & 0x03FF;

    // Set the page table entry
    first_page_table[page_table_index] = (physical_address |3);

    // Set the page directory entry
    // Corrected line in map_page:
    unsigned int physical_pt_addr = ((unsigned int)first_page_table) - 0xC0000000;
    page_directory[page_directory_index] = (physical_pt_addr | 3);    
}

/*
The Math Behind | 3
If we want to map a page so that it actually exists (Present = 1) and we can write to it (Read/Write = 2), we just add the values of those flags together!
1 + 2 = 3.

In binary, the number 3 is 0000 0000 0000 0011.

When you use the Bitwise OR operator (physical_address | 3), it perfectly pastes those two 1s into the empty zeros at the very bottom of the physical address.

You are telling the CPU: "Here is the physical address, AND by the way, it is Present and Writeable!"
*/