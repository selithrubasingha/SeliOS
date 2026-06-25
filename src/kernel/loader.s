global loader                   ; the entry symbol for ELF
extern kmain

MAGIC_NUMBER  equ 0x1BADB002        ; define the magic number constant
ALIGN_MODULES equ 0x00000001        ; NEW: tell GRUB/QEMU to align modules
CHECKSUM      equ -(MAGIC_NUMBER + ALIGN_MODULES) ; NEW: recalculate the checksum

KERNEL_STACK_SIZE equ 4096      ;  define the size of the stack (4KB)

section .text                   ; start of the text (code) section
align 4                         ; the code must be 4 byte aligned
    dd MAGIC_NUMBER             ; write the magic number to the machine code,
    dd ALIGN_MODULES            ; NEW: write the align modules instruction
    dd CHECKSUM                 ; and the checksum

loader:                         ; the loader label (defined as entry point in linker script)
    ;Save the multiboot pointer (ebx) into ecx for safekeeping
    mov ecx, ebx

    ;Get the PHYSICAL address of the page directory
    ; We subtract 0xC0000000 because paging isn't turned on yet!
    mov eax, page_directory - 0xC0000000    mov ecx, abx


.loop:
    jmp .loop                   ; loop forever

section .bss                    ; uninitialized data section
align 4096
page_directory:
    resb 4096

align 4                         ; align at 4 bytes for performance
kernel_stack:                   ; label pointing to the bottom of memory
    resb KERNEL_STACK_SIZE      ; reserve 4096 bytes for the stack

