#include "memory.h"

void set_bit(unsigned int index) {
    bitmap[index / 8] = bitmap[index / 8]|(1 << (index % 8));
}

void clear_bit(unsigned int index) {
    bitmap[index / 8] = bitmap[index / 8] & ~(1 << (index % 8));
}

int test_bit(unsigned int index) {
    return (bitmap[index / 8] & (1 << (index % 8))) != 0;
}

unsigned int allocate_frame(){

    for (unsigned int i = 0; i < BITMAP_SIZE*8 ;i++){
        
        if (!test_bit(i)){
            set_bit(i);
            return i*4096; // Return the physical address of the allocated frame
        }

    }
    return 0; // Return 0 if no free frame is found
}


void init_memory(unsigned int total_ram_bytes,unsigned int kernel_physical_start, unsigned int kernel_physical_end) {
    //  Pessimism. Loop through the whole bitmap and set EVERY bit to 1 (Used)
    for (unsigned int i = 0; i < BITMAP_SIZE * 8; i++) {
        set_bit(i);
    }
    
    // Loop through the available RAM (from GRUB) and set those bits to 0 (Free)
    unsigned int total_frames = total_ram_bytes / 4096; // 4KB per frame
    for (unsigned int i = 0; i < total_frames; i++) {
        clear_bit(i);
    }
    
    
    // Protect the VIP! Calculate which frames the Kernel is sitting in, 
    // and set those specific bits back to 1 (Used)!
    unsigned int kernel_start_frame = kernel_physical_start / 4096;
    unsigned int kernel_end_frame = (kernel_physical_end + 4095) / 4096;
    for (unsigned int i = kernel_start_frame; i <= kernel_end_frame; i++) {
        set_bit(i);
    }

    // Protect the first 1MB of RAM!
    // The first 1MB (Frames 0 to 255) contains the BIOS and your VGA Screen!
    // If we don't protect it, your OS will hand out the screen memory to apps.
    for (unsigned int i = 0; i < 256; i++) {
        set_bit(i);
    }
}
