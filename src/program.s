
[BITS 32]             ; NEW: Tell NASM to output 32-bit machine code!
; src/program.s
mov eax, 0xDEADBEEF   ; A highly recognizable hex number
;$ means 
jmp $                 ; Infinite loop