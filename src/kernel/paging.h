#ifndef PAGING_H
#define PAGING_H

// Maps a virtual address to a physical frame
void map_page(unsigned int virtual_address, unsigned int physical_address);

// Initializes the page directory, maps the kernel/VGA, and enables paging
void init_paging(unsigned int phys_start, unsigned int phys_end);

#endif