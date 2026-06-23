
#ifndef IDT_H
#define IDT_H


struct idt {
    unsigned int address;
    unsigned short size;
} __attribute__((packed));


void init_idt();

#endif