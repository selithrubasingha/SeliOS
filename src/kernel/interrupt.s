global load_idt
extern interrupt_handler    ; Link to the C function you will write later

; load_idt - Loads the interrupt descriptor table (IDT).
; stack: [esp + 4] the address of the first entry in the IDT
;        [esp    ] the return address
load_idt:
    mov     eax, [esp+4]    ; load the address of the IDT into register eax
    lidt    [eax]           ; load the IDT (FIXED BOOK TYPO)
    ret                     ; return to the calling function
