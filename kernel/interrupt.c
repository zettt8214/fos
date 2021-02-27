#include "interrupt.h"
#include "stdint.h"
#include "global.h"
#include "io.h"
#include "print.h"

#define IDT_DESC_CNT 0x21
#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1

typedef struct gate_desc {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t unused;
    uint8_t attribute;
    uint16_t offset_high;
}gate_desc;

static gate_desc idt[IDT_DESC_CNT];
extern intr_handler intr_entry_table[IDT_DESC_CNT];

static void make_idt_desc(gate_desc* p_gdesc, uint8_t attr, intr_handler func){
    p_gdesc->offset_low = (uint32_t)func & 0x0000ffff;
    p_gdesc->selector = SELECTOR_CODE;
    p_gdesc->unused = 0;
    p_gdesc->attribute = attr;
    p_gdesc->offset_high = ((uint32_t)func & 0xffff0000) >> 16;
}

static void idt_desc_init(){
    int i;
    for(int i = 0; i < IDT_DESC_CNT; i++){
        make_idt_desc(idt + i, IDT_DESC_ATTR_DPL0, intr_entry_table[i]);
    }
    put_str("[*]idt_desc_init done\n");
}

static void pic_init(){
    /* init master 8259A chip*/
    outb(PIC_M_CTRL, 0x11); //set ICW1 -- cascade, edge trigger, need ICW4
    outb(PIC_M_DATA, 0x20); //set ICW2 -- init start interrupt verctor number
    outb(PIC_M_DATA, 0x04); //set ICW3 -- slave 8259A chip accesses IR2
    outb(PIC_M_DATA, 0x01); //set ICW4 -- 8086 mode, normal EOI

    /* init slave 8259A chip*/
    outb(PIC_S_CTRL, 0x11); 
    outb(PIC_M_DATA, 0x28);
    outb(PIC_S_DATA, 0x02);
    outb(PIC_S_DATA, 0x01);


    outb(PIC_M_DATA, 0Xfe); //set OCW1
    outb(PIC_S_DATA, 0Xff); //set OCW1
    put_str("[*]pic_init done\n");

}

void idt_init(){
    put_str("[*]idt_init start\n");
    idt_desc_init();
    pic_init();


    /* load idt */
    uint16_t idt_limit = sizeof(idt) - 1; 
    uint64_t idt_operand = (uint64_t)((uint32_t)idt << 16) + idt_limit;
    
    asm volatile ("lidt %0" : : "m"(idt_operand));
    put_str("[*]idt_init done\n");
}