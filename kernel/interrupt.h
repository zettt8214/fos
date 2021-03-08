#ifndef _KERNEL_INTERRUPT_H
#define _KERNEL_INTERRUPT_H
#include "stdint.h"

typedef void *intr_handler;
void idt_init();

typedef enum IntrStatus
{
    KINTROFF,
    KINTRON
} IntrStatus;

/* open or close the interrupt */
IntrStatus intr_get_status();
IntrStatus intr_set_status(IntrStatus status);
IntrStatus intr_enable();
IntrStatus intr_disable();
void register_handler(uint8_t vec_nr, intr_handler function);
#endif