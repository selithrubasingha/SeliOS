
/*
Interrupts 0 to 31: The Fatal CPU Exceptions
Intel hardcoded the first 32 interrupts inside the silicon. If one of these fires, it means your OS or a program did something illegal and crashed.

0: Divide by Zero Error

6: Invalid Opcode (Tried to run a broken instruction)

8: Double Fault (An error happened while trying to handle an error!)

13: General Protection Fault (You tried to touch memory without the right VIP wristband)

14: Page Fault (Memory doesn't exist)

Interrupts 32 to 47: The Hardware (IRQs)
This is the domain of the PIC (the secretary microchip we will program in Phase 4).

32: The System Timer (Ticks automatically)

33: The Keyboard!

36: COM1 (The Serial Port you set up earlier!)

44: The PS/2 Mouse
*/


// This maps to the registers pushed inside common_interrupt_handler
struct cpu_state {
    unsigned int ebp;
    unsigned int edi;
    unsigned int esi;
    unsigned int edx;
    unsigned int ecx;
    unsigned int ebx;
    unsigned int eax;
} __attribute__((packed));

// This maps to what the macro and the CPU hardware pushed automatically
struct stack_state {
    unsigned int error_code;    // Pushed by the macro
    unsigned int eip;           // Pushed by the CPU hardware
    unsigned int cs;            // Pushed by the CPU hardware
    unsigned int eflags;        // Pushed by the CPU hardware
} __attribute__((packed));

void interrupt_handler(struct cpu_state cpu, unsigned int intr_no, struct stack_state stack) ;