
void task_a() {
    char *msg = "A"; 
    while (1) {
        asm volatile("int $0x80" : : "a"(1), "b"(msg)); 
        asm volatile("int $0x80" : : "a"(3)); 
    }
}

void task_b() {
    char *msg = "B"; 
    while (1) {
        asm volatile("int $0x80" : : "a"(1), "b"(msg)); 
        asm volatile("int $0x80" : : "a"(3)); 
    }
}
