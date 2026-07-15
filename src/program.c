// program.c

// Tell program.c to look for the master switch in kmain.c
extern volatile int matrix_active;

void task_a() {
    // Print a Light Green '1' with trailing spaces
    char *msg = "\x0A" "1   \x07"; 
    while (1) {
        if (matrix_active) {
            // System call to print (eax = 1)
            asm volatile("int $0x80" : : "a"(1), "b"(msg));
            
            // Artificial delay so the human eye can see the matrix
            for(volatile int i=0; i<800000; i++); 
        } else {
            // SAFE YIELD: Ask the kernel to switch threads (eax = 3)
            asm volatile("int $0x80" : : "a"(3)); 
        }
    }
}

void task_b() {
    // Print a Light Green '0'
    char *msg = "\x0A" "  0 \x07"; 
    while (1) {
        if (matrix_active) {
            // System call to print (eax = 1)
            asm volatile("int $0x80" : : "a"(1), "b"(msg));
            
            for(volatile int i=0; i<800000; i++); 
        } else {
            // SAFE YIELD: Ask the kernel to switch threads (eax = 3)
            asm volatile("int $0x80" : : "a"(3)); 
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