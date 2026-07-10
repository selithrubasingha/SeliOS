#include "gdt.h"
#include "tss.h"
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

struct gdt_entry gdt_entries[6]; // Our 5 rules
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
    gdt_ptr.size = (sizeof(struct gdt_entry) * 6) - 1;
    gdt_ptr.address  = (unsigned int)&gdt_entries;

    // 2. Build the 5 flat-model rules
    set_entry(0, 0, 0, 0, 0);                // Null
    set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Kernel Code
    set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Kernel Data
    set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User Code
    set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User Data

    //TSS argument 
    set_entry(5, (unsigned int)&tss_entry, sizeof(tss_entry_t) - 1, 0x89, 0x00); // in 0x89 the 5th bit is 0 
    /*
    0 (Granularity Bit): "Do NOT multiply by 4KB. Measure my Limit in exact,
     single bytes."

    0 (Size Bit): "Leave as default." (The CPU already knows it is 32-bit 
    from the 0x89 access byte).

    0000 (Limit Top Bits): The limit we passed for the TSS was sizeof(tss_entry_t) - 1,
     which equals 103 (or 0x67 in hex). Because 0x67 is so small, it easily fits in the
      lower 16 bits of the limit, meaning the upper 4 bits are safely 0000.
    */

    // 3. Trigger the assembly code!
    load_gdt((unsigned int)&gdt_ptr);

    //ltr function use to store 0x28
    asm volatile("ltr %%ax" : : "a"(0x28));
}

