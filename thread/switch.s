[bits 32]
section .text
global switch_to
switch_to:
    push esi
    push edi
    push ebx
    push ebp

    mov eax, [esp + 20] ;parameter cur_thread
    mov [eax], esp ;save esp to cur_thread->self_kstack

    ;set next_thread
    mov eax, [esp + 24]
    mov esp, [eax]

    pop ebp
    pop ebx
    pop edi
    pop esi
    ret
