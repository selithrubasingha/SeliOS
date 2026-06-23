
#ifndef IDT_H
#define IDT_H


struct idt {
    unsigned short size;
    unsigned int address;

} __attribute__((packed));

struct idt_entry {
    unsigned short base_low;
    unsigned short sel; // Kernel segment selector
    unsigned char  always0;
    unsigned char  flags;
    unsigned short base_high;
} __attribute__((packed));

void set_idt_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);

void init_idt();

#endif