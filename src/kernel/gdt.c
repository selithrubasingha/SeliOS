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