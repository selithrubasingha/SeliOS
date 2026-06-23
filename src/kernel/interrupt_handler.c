#include "interrupt_handler.h"

void interrupt_handler(struct cpu_state cpu, unsigned int intr_no, struct stack_state stack) {
    // The CPU is paused. Decide what to do based on 'intr_no'
    
    if (intr_no == 0) {
        // Use your serial_write or fb_write to print "DIVIDE BY ZERO ERROR!"
        // Then loop infinitely to halt the OS so it doesn't execute broken code.
    }
    else if (intr_no == 33) {
        // The Keyboard interrupted us!
        // keyboard_handler(); // (You will write this in Phase 5!)
    }
    else if (intr_no >= 32 && intr_no <= 47) {
        // It's a hardware interrupt, but not the keyboard.
        // For now, we can just ignore it.
    }
    else {
        // Some other random CPU exception happened.
        // Print "Unhandled Exception: [intr_no]"
    }
}