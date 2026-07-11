#include "interrupt_handler.h"
#include "keyboard.h"
#include "pic.h"
#include "thread.h"


#define DEVICE_FB     0

void interrupt_handler(struct cpu_state cpu, unsigned int intr_no, struct stack_state stack) {
    // The CPU is paused. Decide what to do based on 'intr_no'

    if (intr_no == 128) {
        
        // Look at eax to see what the user program wants!
        switch (cpu.eax) {
            case 1:
                // They want to print! 
                // (Hint: The string pointer they want to print is probably sitting in regs->ebx)
                printf(DEVICE_FB, (char *)cpu.ebx);
                break;
            case 2:
                // They want to read a file!
                break;
            case 3:
                // They want to yield the CPU to another thread!
                thread_yield();
                break;
            default:
                printf(0,"Unknown system call!\n");
        }
    }
    
    if (intr_no == 0) {
        // Use your serial_write or fb_write to print "DIVIDE BY ZERO ERROR!"
        // Then loop infinitely to halt the OS so it doesn't execute broken code.
    }
    else if (intr_no == 33) {
        keyboard_handler();       // 1. Read the key and print it
        pic_acknowledge(intr_no); // 2. CRUCIAL: Thank the PIC secretary!
    }
    else if (intr_no >= 32 && intr_no <= 47) {
        pic_acknowledge(intr_no);
        // It's a hardware interrupt, but not the keyboard.
        if (intr_no == 32) {
            // It's the timer interrupt! Call the scheduler to switch threads.
            thread_yield();
        }
    }
    else {
        // Some other random CPU exception happened.
        // Print "Unhandled Exception: [intr_no]"
    }
}