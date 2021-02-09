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
  mov     cx, 0               ; 左上角: (0, 0)
  mov     dx, 184fh           ; 右下角: (80,25),
                       ; VGA文本模式中，一行只能容纳80个字符，共25行
                       ; 下标从0开始，所以0x18=24，0x4f=79
  int     10h                 ; int 10h

; 输出背景色绿色，前景色红色，并且跳动的字符串"1 MBR"
  mov byte [gs:0x00],'1'
  mov byte [gs:0x01],0xA4   ; A表示绿色背景闪烁，4表示前景色为红色

  mov byte [gs:0x02],' '
  mov byte [gs:0x03],0xA4

  mov byte [gs:0x04],'M'
  mov byte [gs:0x05],0xA4

  mov byte [gs:0x06],'B'
  mov byte [gs:0x07],0xA4

  mov byte [gs:0x08],'R'    
  mov byte [gs:0x09],0xA4



  mov eax, LOADER_START_SECTOR
  mov bx, LOADER_BASE_ADDR
  mov cx, 1
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