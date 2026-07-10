global load_idt
extern interrupt_handler    ; Link to the C function you will write later

; load_idt - Loads the interrupt descriptor table (IDT).
; stack: [esp + 4] the address of the first entry in the IDT
;        [esp    ] the return address
load_idt:
    mov     eax, [esp+4]    ; load the address of the IDT into register eax
    lidt    [eax]           ; load the IDT (FIXED BOOK TYPO)
    ret                     ; return to the calling function


; MACROS (The Catcher Templates)

%macro no_error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push    dword 0                     ; push 0 as error code
    push    dword %1                    ; push the interrupt number
    jmp     common_interrupt_handler    ; jump to the common handler
%endmacro

%macro error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push    dword %1                    ; push the interrupt number
    jmp     common_interrupt_handler    ; jump to the common handler
%endmacro

; THE COMMON HANDLER

common_interrupt_handler:               ; the common parts of the generic interrupt handler
    ; save the registers manually
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    esi
    push    edi
    push    ebp

    ; call the C function
    call    interrupt_handler

    ; restore the registers manually (in exact reverse order!)
    pop     ebp
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax

    ; restore the esp (cleans up the 2 dwords we pushed in the macro)
    add     esp, 8

    ; return to the code that got interrupted
    iret

; GENERATE THE HANDLERS

no_error_code_interrupt_handler 0       ; create handler for interrupt 0
no_error_code_interrupt_handler 1       ; create handler for interrupt 1
error_code_interrupt_handler 7        ; FIXED BOOK TYPO: Match macro name
no_error_code_interrupt_handler 33
no_error_code_interrupt_handler 32
no_error_code_interrupt_handler 128