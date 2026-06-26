ENTRY(loader)           /* the name of the entry symbol */

SECTIONS {
    /* the "." dot is what we call a location counter ... it doens't stay the same in the top and bottom of the file */

    . = 0xC0100000;         /* the code should be relocated to 3GB + 1MB */

    /* these labels get exported to the code files */
    kernel_virtual_start = .; /* meaning this variable = 0xc0100000 */
    kernel_physical_start = . - 0xC0000000; /* 0xC000000 - 0xC0100000 = 0x00100000 !! */

    /* align at 4 KB and load at 1 MB */
    .text ALIGN (0x1000) : AT(ADDR(.text)-0xC0000000)
    {
        *(.text)            /* all text sections from all files */
    }

    /* align at 4 KB and load at 1 MB + . */
    .rodata ALIGN (0x1000) : AT(ADDR(.rodata)-0xC0000000)
    {
        *(.rodata*)         /* all read-only data sections from all files */
    }

    /* align at 4 KB and load at 1 MB + . */
    .data ALIGN (0x1000) : AT(ADDR(.data)-0xC0000000)
    {
        *(.data)            /* all data sections from all files */
    }

    /* align at 4 KB and load at 1 MB + . */
    .bss ALIGN (0x1000) : AT(ADDR(.bss)-0xC0000000)
    {
        *(COMMON)           /* all COMMON sections from all files */
        *(.bss)             /* all bss sections from all files */
    }

    kernel_virtual_end = .; /* the location counter has moved we don't know where though */
    kernel_physical_end = . - 0xC0000000;
}


