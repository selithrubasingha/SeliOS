global enter_user_mode

enter_user_mode:
    ; 1. Update the data segment registers
    mov ax, 0x23      ; 0x20 (User Data) | 0x3 (Ring 3)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 2. Forge the IRET stack (pushed in reverse order)
    push 0x23         ; SS: Stack Segment 
    push 0xBFFFFFFB   ; ESP: Stack Pointer
    push 0x2          ; EFLAGS: Interrupts disabled
    push 0x1B         ; CS: Code Segment (0x18 | 0x3)
    push 0x00000000   ; EIP: Instruction Pointer

    ; 3. Drop privileges and jump to User Mode!
    iret