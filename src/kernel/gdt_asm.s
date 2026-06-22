global load_gdt

section .text
load_gdt:
    ; 1. Grab the struct address passed from C (implicitly sitting on the stack)
    mov eax, [esp+4]
    
    lgdt [eax]

    ;  The CPU physically bans moving a raw number directly into 
    ; a segment register. We must move it to 'ax' first, then to the segments.
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:flush_cs

flush_cs:
    ret