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



