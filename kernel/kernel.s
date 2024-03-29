[bits 32]
%define ERROR_CODE nop
%define ZERO push 0

extern put_str;
extern idt_table

section .data

%macro VECTOR 2
section .text
intr%1entry:
    %2
    push ds
    push es
    push fs
    push gs
    pushad


    mov al, 0x20    ;value of OCW2
    out 0xa0, al    ;master of 8259a 
    out 0x20, al    ;slave of 8259a
    
    push %1
    call [idt_table + %1 * 4]
    jmp intr_exit

section .data
    dd intr%1entry
%endmacro

section .text
global intr_exit
intr_exit:
    add esp, 4
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 4
    iretd

section .data
global intr_entry_table
intr_entry_table:
    VECTOR 0x00, ZERO
    VECTOR 0x01, ZERO
    VECTOR 0x02, ZERO
    VECTOR 0x03, ZERO
    VECTOR 0x04, ZERO
    VECTOR 0x05, ZERO
    VECTOR 0x06, ZERO
    VECTOR 0x07, ZERO
    VECTOR 0x08, ERROR_CODE
    VECTOR 0x09, ZERO
    VECTOR 0x0a, ERROR_CODE
    VECTOR 0x0b, ERROR_CODE
    VECTOR 0x0c, ZERO
    VECTOR 0x0d, ERROR_CODE
    VECTOR 0x0e, ERROR_CODE
    VECTOR 0x0f, ZERO
    VECTOR 0x10, ZERO
    VECTOR 0x11, ERROR_CODE
    VECTOR 0x12, ZERO
    VECTOR 0x13, ZERO
    VECTOR 0x14, ZERO
    VECTOR 0x15, ZERO
    VECTOR 0x16, ZERO
    VECTOR 0x17, ZERO
    VECTOR 0x18, ERROR_CODE
    VECTOR 0x19, ZERO
    VECTOR 0x1a, ERROR_CODE
    VECTOR 0x1b, ERROR_CODE
    VECTOR 0x1c, ZERO
    VECTOR 0x1d, ERROR_CODE
    VECTOR 0x1e, ERROR_CODE
    VECTOR 0x1f, ZERO
    VECTOR 0x20, ZERO   ;Clock interrupt
    VECTOR 0x21, ZERO   ;Keyboard interrupt
    VECTOR 0x22, ZERO   ;Used for cascade
    VECTOR 0x23, ZERO   ;Serial port 2
    VECTOR 0x24, ZERO   ;Serial port 1
    VECTOR 0x25, ZERO   ;Parallel port 2
    VECTOR 0x26, ZERO   ;Floppy disk
    VECTOR 0x27, ZERO   ;Parallel port 1
    VECTOR 0x28, ZERO   ;Real time clock
    VECTOR 0x29, ZERO   ;Redirection
    VECTOR 0x2a, ZERO   ;Saved
    VECTOR 0x2b, ZERO   ;Saved
    VECTOR 0x2c, ZERO   ;PS/2 mouse
    VECTOR 0x2d, ZERO   ;Fpu
    VECTOR 0x2e, ZERO   ;Hard disk
    VECTOR 0x2f, ZERO   ;Saved

