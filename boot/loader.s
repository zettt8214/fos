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

total_mem_bytes dd 0    ; total_mem_bytes denotes memory size

SELECTOR_CODE equ (0x0001 << 3) + TI_GDT + RPL0
SELECTOR_DATA equ (0x0002 << 3) + TI_GDT + RPL0
SELECTOR_VEDIO equ (0x0003 << 3) + TI_GDT + RPL0

;gdt pointer 
gdt_ptr dw GDT_LIMIT
        dd GDT_BASE

ards_buf times 244 db 0
ards_nr dw 0

loader_start:
;get memory size
;---------------------- int 0x15 -------------------------- 
    xor ebx, ebx
    mov edx, 0x534d4150
    mov di, ards_buf

.e820_men_get_loop:
    mov eax, 0x0000e820
    mov ecx, 20
    int 0x15
    
    jc .try_0xe801

    add di, cx
    inc word [ards_nr]
    cmp ebx, 0
    jnz .e820_men_get_loop

    mov cx, [ards_nr]
    mov ebx, ards_buf
    xor edx, edx

.find_max_mem_area:
    mov eax, [ebx]
    add eax, [ebx + 8]
    add ebx, 20
    cmp edx, eax
    jge .next_ards
    mov edx, eax

.next_ards:
    loop .find_max_mem_area
    jmp .mem_get_ok


.try_0xe801:
    mov ax, 0xe801
    int 0x15
    jc .try_0x88

    mov cx, 1024
    mul cx
    shl edx, 16
    and eax, 0x0000ffff
    or edx, eax
    add edx, 0x100000
    mov esi, edx

    xor eax, eax
    mov ax, bx
    mov ecx, 0x10000
    mul ecx
    add esi, eax

    mov edx, esi
    jmp .mem_get_ok

.try_0x88:
    mov ah, 0x88
    int 0x15
    jc .error_hlt

    mov cx, 1024
    mul cx
    shl edx, 16
    and eax, 0x0000ffff
    or edx, eax
    add edx, 0x100000
    jmp .mem_get_ok

.mem_get_ok:
    mov [total_mem_bytes], edx

;start protection mode
.start_protection_mode:
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

.error_hlt:
    hlt

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

;--------------------load kernel-----------------------------
    mov eax, KERNEL_START_SECTOR
    mov ebx, KERNEL_BIN_BASE_ADDR
    mov ecx, 500

    call rd_disk_32


;-------------------start paging mechanism--------------------
    call setup_page
    sgdt [gdt_ptr]
    mov ebx, [gdt_ptr + 2]

    ;move VEDIO segment to high 1G virtual address space
    or dword [ebx + 0x18 + 4], 0xc0000000

    add dword [gdt_ptr + 2], 0xc0000000
    add esp, 0xc0000000

    mov eax, PAGE_DIR_TABLE_POS
    mov cr3, eax 
    
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    lgdt [gdt_ptr]
    mov byte [gs:160], 'V'

    jmp SELECTOR_CODE:enter_kernel

enter_kernel:
    call kernel_init
    mov esp, 0xc009f000
    jmp KERNEL_ENTRY_POINT


;-------------------------------------------------------
;@brief : setup page directory table and page table
;-------------------------------------------------------
setup_page:
    mov ecx, 4096
    mov esi, 0
.clear_page_dir:
    mov byte [PAGE_DIR_TABLE_POS + esi], 0
    inc esi
    loop .clear_page_dir

;create Page Directory Entry
.create_pde:
    mov eax, PAGE_DIR_TABLE_POS
    add eax, 0x1000     ;position of first page table - pyhsical memory 0~0x3fffff
    mov ebx, eax
    or eax, PG_US_U | PG_RW_W | PG_P

    mov dword [PAGE_DIR_TABLE_POS], eax ;first entry

    ;high 1G space will be used for kernel (0xc0000000 ~ 0xffffffff) - pyhsical memory 0~0x3fffff
    mov dword [PAGE_DIR_TABLE_POS + 0xc00], eax
    
    mov dword [PAGE_DIR_TABLE_POS + 4096], eax

;create Page table entry
    mov ecx, 256
    mov esi, 0
    mov edx , 0
    or edx, PG_US_U | PG_RW_W | PG_P
.create_pte:
    mov [ebx + esi * 4], edx
    add edx, 4096
    inc esi
    loop .create_pte

    mov eax, PAGE_DIR_TABLE_POS
    add eax, 0x2000
    or eax, PG_US_U | PG_RW_W | PG_P
    mov ebx, PAGE_DIR_TABLE_POS
    mov ecx, 254
    mov esi, 769
.create_kernel_pde:
    mov [ebx + esi * 4], eax
    inc esi
    add eax, 0x1000
    loop .create_kernel_pde
    ret
;end setup_page

;---------------------------------------------------------
;@brief: read n sectors from disk
;@param: 
;   eax: start number of sector
;   bx: write address
;   cx: sector count which will read
;---------------------------------------------------------
rd_disk_32:

  mov esi,eax
  mov edi,ecx

  ;set sector numbers
  mov dx, 0x1f2
  mov al, cl
  out dx, al

  mov eax, esi
  ;write lba addr to 0x1f3 ~ 0x1f6
  mov dx, 0x1f3
  out dx, al

  mov dx, 0x1f4
  shr eax, 8
  out dx, al

  mov dx, 0x1f5
  shr eax, 8
  out dx, al

  mov dx, 0x1f6
  shr eax, 8
  and al, 00001111b ;set 23~24 bits of lbs
  or al, 11100000b  ;set lbs mode
  out dx, al

  ;set read command
  mov dx, 0x1f7
  mov al, 0x20  ;read
  out dx, al

  ;examine the status of disk
.not_ready:
  in al, dx
  and al, 0x88 
  cmp al, 0x08 
  jnz .not_ready

  ;read data from port 0x1f0
  mov ax, di
  mov dx, 256 
  mul dx
  mov cx, ax  ;total times, 2 bytes per time
  mov dx, 0x1f0

.read_loop:
  in ax, dx
  mov [ebx], ax
  add ebx, 2
  loop .read_loop

  ret
;end rd_disk

;---------------------------------------------------------
;@brief: copy kernel to target address
;---------------------------------------------------------
kernel_init:
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx

    mov dx,[KERNEL_BIN_BASE_ADDR + 42]      ;e_phentsize
    mov ebx, [KERNEL_BIN_BASE_ADDR + 28]    ;e_phoff
    add ebx, KERNEL_BIN_BASE_ADDR           ;e_phoff + base address -> Elf32_Phdr
    mov cx, [KERNEL_BIN_BASE_ADDR + 44]     ;e_phnum

.each_segment:
    cmp byte [ebx + 0], PT_NULL
    je .PTNULL

    push dword [ebx + 16]           ;p_filesz
    mov eax, [ebx + 4]              ;p_offset
    add eax, KERNEL_BIN_BASE_ADDR   
    push eax
    push dword [ebx + 8]            ;p_vaddr

    call mem_cpy
    add esp, 12

.PTNULL:
    add ebx, edx
    loop .each_segment

    ret
;end kernel_init

;---------------------------------------------------------
;@brief: copy menmory from source to destination
;@param: dst    destination address
;        src    source address
;        size   memory size
;---------------------------------------------------------
mem_cpy:
    cld
    push ebp
    mov ebp, esp
    push ecx            ;save ecx

    mov edi, [ebp + 8]  ;dst
    mov esi, [ebp + 12] ;src
    mov ecx, [ebp + 16] ;size
    rep movsb

    pop ecx
    pop ebp

    ret