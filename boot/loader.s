%include "boot.inc"

section loader vstart=LOADER_BASE_ADDR
LOADER_STACK_TOP equ   LOADER_BASE_ADDR
jmp loader_start

;create GDT
GDT_BASE:
    dd 0x00000000
    dd 0x00000000

CODE_DESC:
    dd 0x0000ffff
    dd SEG_DESC_CODE_HIGH4

DATA_STACK_DESC:
    dd 0x0000ffff
    dd SEG_DESC_DATA_HIGH4

VEDIO_DESC:
    dd 0x80000007   ;limit = (0xbffff - 0xb8000) / 4k = 0x7
    dd SEG_DESC_VIDEO_HIGH4

GDT_SIZE equ $ - GDT_BASE
GDT_LIMIT equ GDT_SIZE - 1
times 60 dq 0   ;60 reserved segement descriptors

SELECTOR_CODE equ (0x0001 << 3) + TI_GDT + RPL0
SELECTOR_DATA equ (0x0002 << 3) + TI_GDT + RPL0
SELECTOR_VEDIO equ (0x0003 << 3) + TI_GDT + RPL0

;gdt pointer 
gdt_ptr dw GDT_LIMIT
        dd GDT_BASE
message db 'loading...'

loader_start:
    mov sp, LOADER_BASE_ADDR
    mov bp, message
    mov cx, 10
    mov ax, 0x1301
    mov bx, 0x001f
    mov dx, 0x1800
    int 0x10

;---------------------- Start Protection Mode----------------

    ;open A20
    in al,0x92
    or al,00000010b
    out 0x92,al

    ;load gdt
    lgdt [gdt_ptr]
    
    ;set pe of cr0
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax

    jmp dword SELECTOR_CODE:p_mode_start

[bits 32]
p_mode_start:
    mov ax, SELECTOR_DATA
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, LOADER_STACK_TOP
    mov ax, SELECTOR_VEDIO
    mov gs, ax

    mov byte [gs:160], 'P'
    jmp $