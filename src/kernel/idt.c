
#include "idt.h"

// Link to the assembly function we just wrote
extern void load_idt(unsigned int idt_address);

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
}

