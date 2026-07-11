
void task_a() {
    char *msg = "A"; 
    while (1) {
        // Turn the timer back on so we can be preempted!
        asm volatile("sti"); 
        asm volatile("int $0x80" : : "a"(1), "b"(msg)); 
    }
}

void task_b() {
    char *msg = "B"; 
    while (1) {
        // Turn the timer back on so we can be preempted!
        asm volatile("sti"); 
        asm volatile("int $0x80" : : "a"(1), "b"(msg)); 
    }
}