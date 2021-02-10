%include "boot.inc"

section mbr vstart=0x7c00
  mov ax,cs
  mov ds,ax
  mov es,ax
  mov ss,ax
  mov fs,ax
  mov sp,0x7c00
  mov ax,0xb800
  mov gs,ax

;clear the screen
  mov     ax, 0600h
  mov     bx, 0700h
  mov     cx, 0               ; top left corner: (0, 0)
  mov     dx, 184fh           ; bottom right corner: (80,25),
                       
  int     10h                 ; BIOS int 10h


  ;load os loader to memory
  mov eax, LOADER_START_SECTOR
  mov bx, LOADER_BASE_ADDR
  mov cx, 4
  call rd_disk

  jmp LOADER_BASE_ADDR

;---------------------------------------------------------
;@brief: read n sectors from disk
;@param: 
;   eax: start number of sector
;   bx: write address
;   cx: sector count which will read
;---------------------------------------------------------
rd_disk:

  mov esi,eax
  mov di,cx

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
  mov [bx], ax
  add bx, 2
  loop .read_loop

  ret
;end rd_disk
;------------------------------------------------------------

  times 510-($-$$) db 0
  db 0x55,0xaa