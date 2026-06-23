#include "pic.h"
#include "io.h"



/**
 * pic_init:
 * Initializes and remaps the Programmable Interrupt Controllers (PICs).
 * 
 * 
 * * THE PROBLEM: 
 * By default, the PIC maps hardware interrupts (timer, keyboard) to CPU 
 * interrupts 0-7. This is fatal because the CPU reserves 0-31 for hardware 
 * exceptions (like Divide by Zero). We must remap them to start at 32.
 * 
 * 
 * * THE MAGIC NUMBERS (from the Intel 8259A Datasheet):
 * 
 * * - 0x11 (ICW1 - Wake Up): 
 * 
 * Binary: 0001 0001. 
 * Bit 4 = 1 (Wipes settings / Initialization mode). 
 * Bit 0 = 1 (Tells PIC to wait for an ICW4 byte at the end).
 * 
 * * - ICW2 (The Base Offsets / Remap):
 * 
 * PIC1 gets 0x20 (32). The chip physically does: 32 + IRQ_Line.
 * PIC2 gets 0x28 (40). The chip physically does: 40 + IRQ_Line.
 * 
 * * - 0x04 (ICW3 for PIC 1 - Wiring Map): 
 * 
 * Binary: 0000 0100 (Bitmask). 
 * Bit 2 is flipped to '1'. This tells the Master PIC (PIC1) that the 
 * Slave PIC (PIC2) is physically wired into its IRQ 2 slot.
 * 
 * * - 0x02 (ICW3 for PIC 2 - Slave Identity): 
 * 
 * Binary: 0000 0010 (Literal number 2).
 * Tells the Slave PIC: "You are Slave 2, matching the slot you are 
 * plugged into on the Master."
 * 
 * * - 0x01 (ICW4 - Environment):
 * 
 * Binary: 0000 0001.
 * Bit 0 = 1 (8086/88 Mode). Tells the PIC to format its electrical 
 * pulses for x86 CPUs, rather than older 1970s processors.
 */
void pic_init() {
    // 1. Send the Initialization Command Word (ICW1) to both PICs
    // 0x11 means: "Wake up! Wait for 3 more setup bytes."
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    // 2. ICW2: The Offsets (The Remap!)
    // Tell PIC1 to start at Interrupt 32 (0x20)
    outb(PIC1_DATA, PIC1_START_INTERRUPT);
    // Tell PIC2 to start at Interrupt 40 (0x28)
    outb(PIC2_DATA, PIC2_START_INTERRUPT);

    // 3. ICW3: Cascading (The Wiring)
    // Tell PIC1 that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC1_DATA, 0x04);
    // Tell PIC2 its cascade identity (0000 0010)
    outb(PIC2_DATA, 0x02);

    // 4. ICW4: The Environment
    // 0x01 means: "We are operating in 32-bit x86 mode."
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    
    // Setup complete!
}



/** 
 * Acknowledges an interrupt from either PIC 1 or PIC 2.
 */
void pic_acknowledge(unsigned int interrupt) {
    // If it's not a hardware interrupt, we don't need to acknowledge it
    if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT) {
        return;
    }

    // FIXED BUG: If it comes from PIC 2, we MUST acknowledge PIC 2
    if (interrupt >= PIC2_START_INTERRUPT) {
        outb(PIC2_COMMAND, PIC_ACK);
    }

    // FIXED BUG: We MUST ALWAYS acknowledge PIC 1, because PIC 2 
    // routes its signals through PIC 1!
    outb(PIC1_COMMAND, PIC_ACK);
}