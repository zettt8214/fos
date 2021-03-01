#ifndef _KERNEL_INTERRUPT_H
#define _KERNEL_INTERRUPT_H

typedef void* intr_handler;
void idt_init();

typedef enum intr_status {
    INTR_OFF,
    INTR_ON
}intr_status;

/* open or close the interrupt */
intr_status intr_get_status();
intr_status intr_set_status(intr_status status);
intr_status intr_enable();
intr_status intr_disable();
#endif