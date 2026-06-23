
#include "idt.h"

// Link to the assembly function we just wrote
extern void load_idt(unsigned int idt_address);
extern void interrupt_handler_33();

struct idt_entry idt_entries[256]; // Our 256 rules
struct idt idt_ptr;                // The book's pointer struct

void set_idt_gate(int index, unsigned int base, unsigned short sel, unsigned char flags) {
    idt_entries[index].base_low    = (base & 0xFFFF);   
    idt_entries[index].base_high   = (base >> 16) & 0xFFFF;
    idt_entries[index].sel         = sel;
    idt_entries[index].always0     = 0;
    idt_entries[index].flags       = flags;

}

void init_idt() {
    // 1. Set up the book's pointer struct
    idt_ptr.size = (sizeof(struct idt_entry) * 256) - 1;
    idt_ptr.address  = (unsigned int)&idt_entries;
    
    // Plug the keyboard into slot 33. 
    // 0x08 is the Kernel Code Segment. 0x8E means "Present, Ring 0".
    set_idt_gate(33, (unsigned int)interrupt_handler_33, 0x08, 0x8E);

    // 2. Trigger the assembly code!
    load_idt((unsigned int)&idt_ptr);
}

