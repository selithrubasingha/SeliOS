//if not defined : if the compiler has not seen this code before ... load it
#ifndef GDT_H
//give this a name 
#define GDT_H
//these help so that this code is not loaded more than once by the compiler (load is not the correct word here though)


struct gdt {
    unsigned int address;
    unsigned short size;
} __attribute__((packed));

void init_gdt();

#endif