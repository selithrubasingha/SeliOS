// program.c
// program.c

#include "kernel/utils.h"
#include "kernel/thread.h"
// Tell program.c to look for the master switch in kmain.c
extern volatile int matrix_active;

void task_a() {
    while (1) {
        asm volatile("sti"); // Keep the timer hardware enabled
        
        if (matrix_active) {
            // VIP Access: Call the kernel functions directly!
            printf(DEVICE_FB, "\x0A" "1   \x07");
            
            // Artificial delay so the human eye can see the matrix
            for(volatile int i=0; i<800000; i++); 
        } else {
            // VIP Access: Yield directly!
            thread_yield();
        }
    }
}

void task_b() {
    while (1) {
        asm volatile("sti"); // Keep the timer hardware enabled
        
        if (matrix_active) {
            // VIP Access: Call the kernel functions directly!
            printf(DEVICE_FB, "\x0A" "  0 \x07");
            
            for(volatile int i=0; i<800000; i++); 
        } else {
            // VIP Access: Yield directly!
            thread_yield();
        }
    }
}

// void task_a() {
//     char *msg = "A"; 
//     while (1) {
//         // Turn the timer back on so we can be preempted!
//         asm volatile("sti"); 
//         asm volatile("int $0x80" : : "a"(1), "b"(msg)); 
//     }
// }

// void task_b() {
//     char *msg = "B"; 
//     while (1) {
//         // Turn the timer back on so we can be preempted!
//         asm volatile("sti"); 
//         asm volatile("int $0x80" : : "a"(1), "b"(msg)); 
//     }
// }