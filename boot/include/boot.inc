;--------------------loader and kernel----------------------
LOADER_BASE_ADDR equ 0x900
LOADER_START_SECTOR equ 0x2
PAGE_DIR_TABLE_POS  equ 0x100000
;--------------------Segment Descriptor -----------------
;
;-------------------------------------------------------------------------------------------------------------------------
;|              |    31~24       |  23  |  22  | 21   | 20   | 19~16       | 15   | 14~13 |  12  | 11~8 |      7~0       |
;| ------------ | ---------------| ---- | ---- | ---- | ---- | ----------  | ---- | ----- | ---- | ---- | ---------------|
;| High 32 bits | Segment Base   |  G   | D/B  | L    | AVL  |   Segment   | P    |  DPL  |   S  | Type | Segment Base   |
;|              | Address(31~24) |      |      |      |      | Limit(19~16)|      |       |      |      | Address(23~16) |
;| ------------ | -------------- | ---- | ---- | ---- | ---- | ----------- | ---- | ----- | ---- | ---- | -------------- |
;| Low 32 bits  |                Segment Base Address(15~0)                |           Segment Limit(15~0)               |
;-------------------------------------------------------------------------------------------------------------------------
SEG_DESC_G_4K   equ 1_00000000000000000000000b
SEG_DESC_D_32   equ  1_0000000000000000000000b
SEG_DESC_L      equ   0_000000000000000000000b
SEG_DESC_AVL    equ    0_00000000000000000000b
SEG_DESC_Limit_CODE2    equ 1111_0000000000000000b;
SEG_DESC_Limit_DATA2    equ 1111_0000000000000000b
SEG_DESC_Limit_VEDIO2   equ 0000_0000000000000000b
SEG_DESC_P      equ 1_000000000000000b
SEG_DESC_DPL_0  equ 00_0000000000000b
SEG_DESC_DPL_1  equ 01_0000000000000b
SEG_DESC_DPL_2  equ 10_0000000000000b
SEG_DESC_DPL_3  equ 11_0000000000000b
SEG_DESC_S_CODE equ   1_000000000000b
SEG_DESC_S_DATA equ   SEG_DESC_S_CODE
SEG_DESC_S_sys  equ   0_000000000000b
SEG_DESC_TYPE_CODE  equ 1000_00000000b
SEG_DESC_TYPE_DATA  equ 0010_00000000b

SEG_DESC_CODE_HIGH4     equ (0x00 << 24) + SEG_DESC_G_4K + SEG_DESC_D_32 + \
SEG_DESC_L + SEG_DESC_AVL + SEG_DESC_Limit_CODE2 + SEG_DESC_P + SEG_DESC_DPL_0 + \
SEG_DESC_S_CODE + SEG_DESC_TYPE_CODE + 0x00

SEG_DESC_DATA_HIGH4     equ (0x00 << 24) + SEG_DESC_G_4K + SEG_DESC_D_32 + \
SEG_DESC_L + SEG_DESC_AVL + SEG_DESC_Limit_DATA2 + SEG_DESC_P + SEG_DESC_DPL_0 + \
SEG_DESC_S_DATA + SEG_DESC_TYPE_DATA + 0x00

SEG_DESC_VIDEO_HIGH4    equ (0x00 << 24) + SEG_DESC_G_4K + SEG_DESC_D_32 + \
SEG_DESC_L + SEG_DESC_AVL + SEG_DESC_Limit_VEDIO2 + SEG_DESC_P + SEG_DESC_DPL_0 + \
SEG_DESC_S_DATA + SEG_DESC_TYPE_DATA + 0x0b

;--------------------------selector-------------------------
RPL0    equ 00b
RPL1    equ 01b
RPL2    equ 10b
RPL3    equ 11b
TI_GDT  equ 000b
TI_LDT  equ 100b

;-------------------------- page table ---------------------
PG_P    equ 1b
PG_RW_R equ 00b
PG_RW_W equ 10b
PG_US_S equ 000b
PG_US_U equ 100b