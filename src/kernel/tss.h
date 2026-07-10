
// got this using intel manual ! 
// We use __attribute__((packed)) so GCC doesn't add invisible padding bytes!
typedef struct {
    unsigned int prev_tss;   // The previous TSS (Unused for us)
    unsigned int esp0;       // The ring 0 stack pointer! (CRITICAL)
    unsigned int ss0;        // The ring 0 stack segment! (CRITICAL)
    unsigned int esp1;       // Unused
    unsigned int ss1;        // Unused
    unsigned int esp2;       // Unused
    unsigned int ss2;        // Unused
    unsigned int cr3;        // Unused
    unsigned int eip;        // Unused
    unsigned int eflags;     // Unused
    unsigned int eax;        // Unused
    unsigned int ecx;        // Unused
    unsigned int edx;        // Unused
    unsigned int ebx;        // Unused
    unsigned int esp;        // Unused
    unsigned int ebp;        // Unused
    unsigned int esi;        // Unused
    unsigned int edi;        // Unused
    unsigned int es;         // Unused         
    unsigned int cs;         // Unused
    unsigned int ss;         // Unused
    unsigned int ds;         // Unused
    unsigned int fs;         // Unused
    unsigned int gs;         // Unused
    unsigned int ldt;        // Unused
    unsigned short trap;     // Unused
    unsigned short iomap_base; // Unused
} __attribute__((packed)) tss_entry_t;