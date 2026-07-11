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

void create_thread(void (*thread_function)()){
    thread_t *new_thread = (thread_t *)malloc(sizeof(thread_t));

    //allocate for the stack
    unsigned int *stack = (unsigned int *)malloc(4096); // Allocate 4KB for the stack

    unsigned int *stack_top = stack + 1024; // Stack grows downwards, so start at the top . since stack is 4 bytes . 1024 is enough 

    //writing to the stack to simulate a context switch
    *(--stack_top) = (unsigned int)thread_function; // Return address for the thread function

    //EBP, EDI, ESI, EDX, ECX, EBX, and EAX
    *(--stack_top) = 0; // EAX 
    *(--stack_top) = 0; // EBX
    *(--stack_top) = 0; // ECX
    *(--stack_top) = 0; // EDX
    *(--stack_top) = 0; // ESI
    *(--stack_top) = 0; // EDI
    *(--stack_top) = 0; // EBP (Lowest address, popped first!)



    new_thread->esp = (unsigned int)stack_top; // Set the new thread's stack pointer
    new_thread->next_thread = current_thread->next_thread; // Link the new thread into the circular list
    current_thread->next_thread = new_thread; // Update the current thread's next pointer to point to the new thread





}