#include "interrupt.h"
#include "stdint.h"
#include "global.h"
#include "io.h"
#include "print.h"

#define IDT_DESC_CNT 0x30
#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1

#define EFLAGS_IF 0x00000200
#define GET_EFLAGES(EFLAG_VAR) asm volatile("pushfl; popl %0" \
                                            : "=g"(EFLAG_VAR))

typedef struct gate_desc
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t unused;
    uint8_t attribute;
    uint16_t offset_high;
} gate_desc;

static gate_desc idt[IDT_DESC_CNT];
char *intr_name[IDT_DESC_CNT];
intr_handler idt_table[IDT_DESC_CNT];
extern intr_handler intr_entry_table[IDT_DESC_CNT];

static void make_idt_desc(gate_desc *p_gdesc, uint8_t attr, intr_handler func)
{
    p_gdesc->offset_low = (uint32_t)func & 0x0000ffff;
    p_gdesc->selector = SELECTOR_CODE;
    p_gdesc->unused = 0;
    p_gdesc->attribute = attr;
    p_gdesc->offset_high = ((uint32_t)func & 0xffff0000) >> 16;
}

static void idt_desc_init()
{
    int i;
    for (i = 0; i < IDT_DESC_CNT; i++)
    {
        make_idt_desc(idt + i, IDT_DESC_ATTR_DPL0, intr_entry_table[i]);
    }
    put_str("[*]idt_desc_init done\n");
}

static void pic_init()
{
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

    // outb(PIC_M_DATA, 0xfe); //set OCW1
    // outb(PIC_S_DATA, 0xff); //set OCW1

    /* keyboard interrupt */
    outb(PIC_M_DATA, 0xfc); //set OCW1
    outb(PIC_S_DATA, 0xff); //set OCW1
    put_str("[*]pic_init done\n");
}
static void general_intr_handler(uint8_t vec_nr)
{
    if (vec_nr == 0x27 || vec_nr == 0x2f)
    {
        return;
    }
    set_cursor(0);
    int cursor_pos = 0;
    while (cursor_pos < 320)
    {
        put_char(' ');
        cursor_pos++;
    }

    set_cursor(0);
    put_str("!!!!!!!      excetion message begin  !!!!!!!!\n");
    set_cursor(88);
    put_str(intr_name[vec_nr]);
    if (vec_nr == 14)
    { // Pagefault
        int page_fault_vaddr = 0;
        asm("movl %%cr2, %0"
            : "=r"(page_fault_vaddr));
        put_str("\npage fault addr is ");
        put_int(page_fault_vaddr);
    }
    put_str("\n!!!!!!!      excetion message end    !!!!!!!!\n");
    while (1)
        ;
}

static void exception_init()
{
    int i;
    for (i = 0; i < IDT_DESC_CNT; i++)
    {
        idt_table[i] = general_intr_handler;
        intr_name[i] = "Unknow";
    }
    intr_name[0] = "#DE Divide Error";
    intr_name[1] = "#DB Debug Exception";
    intr_name[2] = "NMI Interrupt";
    intr_name[3] = "#BP Breakpoint Exception";
    intr_name[4] = "#OF Overflow Exception";
    intr_name[5] = "#BR BOUND Range Exceeded Exception";
    intr_name[6] = "#UD Invalid Opcode Exception";
    intr_name[7] = "#NM Device Not Available Exception";
    intr_name[8] = "#DF Double Fault Exception";
    intr_name[9] = "Coprocessor Segment Overrun";
    intr_name[10] = "#TS Invalid TSS Exception";
    intr_name[11] = "#NP Segment Not Present";
    intr_name[12] = "#SS Stack Fault Exception";
    intr_name[13] = "#GP General Protection Exception";
    intr_name[14] = "#PF Page-Fault Exception";
    // intr_name[15] unused
    intr_name[16] = "#MF x87 FPU Floating-Point Error";
    intr_name[17] = "#AC Alignment Check Exception";
    intr_name[18] = "#MC Machine-Check Exception";
    intr_name[19] = "#XF SIMD Floating-Point Exception";
}

void idt_init()
{
    put_str("[*]idt_init start\n");
    idt_desc_init();
    exception_init();
    pic_init();

    /* load idt */
    uint16_t idt_limit = sizeof(idt) - 1;
    uint64_t idt_operand = (uint64_t)((uint32_t)idt << 16) + idt_limit;

    asm volatile("lidt %0"
                 :
                 : "m"(idt_operand));
    put_str("[*]idt_init done\n");
}

IntrStatus intr_get_status()
{
    uint32_t eflags = 0;
    GET_EFLAGES(eflags);
    return (EFLAGS_IF & eflags) ? KINTRON : KINTROFF;
}

IntrStatus intr_enable()
{
    IntrStatus old_status;
    if ((old_status = intr_get_status()))
    {
        return old_status;
    }
    else
    {
        asm volatile("sti");
        return old_status;
    }
}

IntrStatus intr_disable()
{
    IntrStatus old_status;
    if ((old_status = intr_get_status()))
    {
        asm volatile("cli"
                     :
                     :
                     : "memory");
        return old_status;
    }
    else
    {
        return old_status;
    }
}

IntrStatus intr_set_status(IntrStatus status)
{
    return status ? intr_enable() : intr_disable();
}

void register_handler(uint8_t vec_nr, intr_handler function)
{
    idt_table[vec_nr] = function;
}