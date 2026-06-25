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
    mov eax, page_directory - 0xC0000000       ; Create the Maps (Using physical eax, not the virtual label)
    mov dword [eax], 0x00000083             ; Identity Map (Virtual 0 -> Physical 0)
    mov dword [eax + (768 * 4)], 0x00000083 ; Penthouse Map (Virtual 3GB -> Physical 0)

    ; Turn on Paging! (From the book snippet)
    mov cr3, eax            ; Give CPU the physical address of our directory

    mov edx, cr4            
    or  edx, 0x00000010     ; Set PSE (4MB Pages)
    mov cr4, edx            

    mov edx, cr0            
    or  edx, 0x80000000     ; Set PG (Paging Enabled!)
    mov cr0, edx  

    ; Teleport to the Higher Half! (Using eax instead of ebx to protect Multiboot)
    lea eax, [higher_half]
    jmp eax

higher_half:
    ; We are now officially running at 3GB! The VR Headset is ON!

    ; Step 5: Burn the Bridge (Unmap the Identity Map)
    mov dword [page_directory], 0
    invlpg [0]

    ; NOW it is safe to setup our stack, because the virtual addresses work!
    mov esp, kernel_stack + KERNEL_STACK_SIZE 

    ; The Multiboot pointer (in ecx) is a physical address. We must upgrade
    ; it to a virtual address so our C code can read it!
    add ecx, 0xC0000000
    push ecx      ; Push the upgraded Multiboot pointer as an argument 
    
    ; Jump out of assembly and into your C code!
    call kmain

     
.loop:
    jmp .loop                   ; loop forever

section .bss                    ; uninitialized data section
align 4096
page_directory:
    resb 4096

align 4                         ; align at 4 bytes for performance
kernel_stack:                   ; label pointing to the bottom of memory
    resb KERNEL_STACK_SIZE      ; reserve 4096 bytes for the stack

