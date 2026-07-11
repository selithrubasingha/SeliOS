global switch_task

;switch_task(unsigned int *current_thread, unsigned int next_thread)
switch_task:
    ; push all the general purpose registers
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp

    ; save the current thread's stack pointer
    mov eax, [esp+32]

    ;[] mean "Treat this number as a physical RAM address , and go look inside it.

    mov [eax], esp
    ;here the [eax] doesn't mean the actual eax register ... IT MEANS [ESP+32] address, go there and change the value.

    ; load the next thread's stack pointer
    mov esp, [esp+36]

    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax

    ret

