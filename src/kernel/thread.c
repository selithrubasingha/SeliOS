#include "thread.h"

#define NULL ((void *)0)
// global variable current task index


// Make sure C knows about your assembly function!
extern void switch_task(unsigned int *old_esp_ptr, unsigned int new_esp);

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

//what's different here is that we are creating a user thread that will run in ring 3 (user mode) instead of ring 0 
//(kernel mode). This means we need to set up the stack and registers differently to ensure that when the thread is 
//scheduled, it transitions correctly from kernel mode to user mode.
void create_user_thread(void (*thread_function)()) {
    thread_t *new_thread = (thread_t *)malloc(sizeof(thread_t));

    // Allocate a kernel stack for this thread to use during interrupts
    unsigned int *kernel_stack = (unsigned int *)malloc(4096);
    unsigned int *stack_top = kernel_stack + 1024; // Start at the top

    // --- 1. FORGE THE IRET FRAME (Ring 3 Transition) ---
    // The CPU expects these 5 things exactly in this order to jump to Ring 3
    
    // User Data Segment (SS). 0x20 is the base GDT offset, + 3 for Ring 3 = 0x23
    *(--stack_top) = 0x23; 
    
    // User Stack Pointer (ESP). We'll reuse the kernel stack for this basic test, 
    // but in a real OS, you'd malloc a separate user stack here.
    *(--stack_top) = (unsigned int)kernel_stack + 4096; 
    
    // EFLAGS. 0x202 enables interrupts (IF flag) so the hardware timer still works!
    *(--stack_top) = 0x202; 
    
    // User Code Segment (CS). 0x18 is the base GDT offset, + 3 for Ring 3 = 0x1B
    *(--stack_top) = 0x1B; 
    
    // User EIP (The actual function pointer)
    *(--stack_top) = (unsigned int)thread_function; 

    // --- 2. FORGE THE SWITCH_TASK REGISTERS ---
    // Push the 7 dummy registers just like you did for kernel threads
    *(--stack_top) = 0; // EAX 
    *(--stack_top) = 0; // EBX
    *(--stack_top) = 0; // ECX
    *(--stack_top) = 0; // EDX
    *(--stack_top) = 0; // ESI
    *(--stack_top) = 0; // EDI
    *(--stack_top) = 0; // EBP

    // --- 3. LINK IT INTO THE RUN QUEUE ---
    new_thread->esp = (unsigned int)stack_top;
    
    if (current_thread == NULL) {
        // If this is the very first thread ever, make it point to itself
        new_thread->next_thread = new_thread;
        current_thread = new_thread;
    } else {
        // Squeeze it into the existing circle
        new_thread->next_thread = current_thread->next_thread;
        current_thread->next_thread = new_thread;
    }
}

void thread_yield() {
    
    thread_t *old_thread = current_thread;
    current_thread = current_thread->next_thread; // Move to the next thread in the circular list

    //call switch task assembly function
    switch_task(&old_thread->esp, current_thread->esp); // Switch to the next thread





}