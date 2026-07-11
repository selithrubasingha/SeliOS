#include "timer.h"


void set_pit_count(unsigned count) {
    // Disable interrupts
    asm volatile("cli");
    
    // Command byte: Channel 0, lobyte/hibyte access, Mode 3, Binary
    outb(0x43, 0x36); // 0x36 is 00110110 in binary
    
    // Set low byte and high byte
    outb(0x40, count & 0xFF);         // Low byte
    outb(0x40, (count & 0xFF00) >> 8); // High byte
    
    // Re-enable interrupts
    asm volatile("sti");
}


void init_timer(unsigned int frequency) {
    unsigned int divisor = 1193180 / frequency;
    set_pit_count(divisor);
}
















