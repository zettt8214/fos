TI_GDT equ 0
RPL0 equ 0
SELECTOR_VEDIO equ (0x0003 << 3) + TI_GDT + RPL0

[bits 32]
section .text
;----------------------------------------
;@brief put a char to screen
;----------------------------------------
global put_char
put_char:
    pushad
    mov ebp, esp
    mov ax, SELECTOR_VEDIO
    mov gs, ax

    ; get position of cursor
    mov dx, 0x03d4
    mov al, 0x0e    ;index ->  high 8 bit of cursor
    out dx, al      ;set index
    mov dx, 0x03d5
    in al, dx
    mov ah, al

    mov dx, 0x03d4
    mov al, 0x0f    ;index ->  low 8 bit of cursor
    out dx, al      ;set index
    mov dx, 0x03d5
    in al, dx

    mov bx, ax      ;save position of cursor

    mov ecx, [ebp + 36] ;pushad: push 4 regs = 32, 
                        ;32 add bytes of return address = 36
    cmp cl, 0x0d
    jz .handle_CR

    cmp cl, 0x0a
    jz .handle_LF

    cmp cl, 0x08
    jz .handle_backspace

    jmp .handle_other

.handle_backspace:
    dec bx
    shl bx, 1
    
    mov byte [gs:bx], 0x20
    inc bx
    mov byte [gs:bx], 0x07
    shr bx, 1

    jmp .set_cursor

.handle_other:
    shl bx, 1

    mov byte [gs:bx], cl
    inc bx
    mov byte [gs:bx], 0x07
    shr bx, 1
    inc bx

    cmp bx, 2000
    jl .set_cursor
    jmp .roll_screen

.handle_CR:
.handle_LF:
    xor dx, dx
    mov ax, bx
    mov si, 80
    div si
    sub bx, dx  ;start position of current line 

    add bx, 80
    cmp bx, 2000
    jl .set_cursor

.roll_screen:
    cld
    mov ecx, 960
    mov esi, 0xc00b80a0
    mov edi, 0xc00b8000
    rep movsd

    mov ebx, 3840
    mov ecx, 80
.cls:
    mov word [gs:ebx], 0x7020
    add ebx, 2
    loop .cls
    mov bx, 1920

.set_cursor:
    mov dx, 0x03d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x03d5
    mov al, bh
    out dx, al

    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    mov al, bl
    out dx, al

    popad
    ret