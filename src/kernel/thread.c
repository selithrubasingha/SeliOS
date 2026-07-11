#include "thread.h"

#define NULL ((void *)0)
// global variable current task index

thread_t *current_thread = NULL;

thread_t main_thread ;
void init_threads() {

    main_thread.esp = 0; // Initialize the main thread's stack pointer to 0
    main_thread.next_thread = &main_thread; // No next thread yet
    current_thread = &main_thread; // Set the current thread to the main thread
    
}