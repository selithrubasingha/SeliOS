
#ifndef MEMORY_H
#define MEMORY_H


//set bitmap size to 4096
#define BITMAP_SIZE 4096


void init_memory(unsigned int total_ram_bytes, unsigned int kernel_physical_start, unsigned int kernel_physical_end);
unsigned int allocate_frame();
void free_frame(unsigned int physical_address);
void *memcpy(void *dest, const void *src, unsigned int n);
#endif