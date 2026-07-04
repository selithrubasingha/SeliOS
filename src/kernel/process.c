#include "paging.h"
#include "memory.h"

unsigned int user_page_directory[1024] __attribute__((aligned(4096))) ;
