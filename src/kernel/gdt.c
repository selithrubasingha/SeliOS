#include "gdt.h"

// Link to the assembly function we just wrote
extern void load_gdt(unsigned int gdt_address);

struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access;
    unsigned char  granularity;
    unsigned char  base_high;
} __attribute__((packed));

struct gdt_entry gdt_entries[5]; // Our 5 rules
struct gdt gdt_ptr;              // The book's pointer struct

void set_entry(int index, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran) {
    gdt_entries[index].base_low    = (base & 0xFFFF);
    gdt_entries[index].base_middle = (base >> 16) & 0xFF;
    gdt_entries[index].base_high   = (base >> 24) & 0xFF;
    gdt_entries[index].limit_low   = (limit & 0xFFFF);
    gdt_entries[index].granularity = (limit >> 16) & 0x0F;
    gdt_entries[index].granularity |= gran & 0xF0;
    gdt_entries[index].access      = access;
}

void init_gdt() {
    // 1. Set up the book's pointer struct
    gdt_ptr.size = (sizeof(struct gdt_entry) * 5) - 1;
    gdt_ptr.address  = (unsigned int)&gdt_entries;

    // 2. Build the 5 flat-model rules
    set_entry(0, 0, 0, 0, 0);                // Null
    set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Kernel Code
    set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Kernel Data
    set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User Code
    set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User Data

    // 3. Trigger the assembly code!
    load_gdt((unsigned int)&gdt_ptr);
}

