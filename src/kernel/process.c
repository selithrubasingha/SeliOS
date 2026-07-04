#include "paging.h"
#include "memory.h"
#include "process.h"

unsigned int user_page_directory[1024] __attribute__((aligned(4096))) ;


process_t* create_process(unsigned int module_phys_addr, unsigned int module_size) {

    // 1. malloc() the process_t struct

    // 2. new_process->page_directory = create_user_page_directory();

    // 3. allocate_frame() for code and stack

    // 4. map_page() those frames into the new directory

    // 5. memcpy the GRUB binary into the code frame

    // 6. Return the finished process struct

}