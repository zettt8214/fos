#include "ioqueue.h"
#include "debug.h"
#include "interrupt.h"

void ioqueue_init(Ioqueue *ioq)
{
    ioq->head = ioq->tail = 0;
    ioq->producer = ioq->consumer = NULL;
    lock_init(&ioq->lock);
}

static int32_t next_pos(int32_t pos)
{
    return (pos + 1) % BUFFZISE;
}

bool ioqueue_full(Ioqueue *ioq){
    ASSERT(intr_get_status() == KINTROFF);
    return next_pos(ioq->head) == ioq->tail;
}

bool ioqueue_empty(Ioqueue *ioq){
    ASSERT(intr_get_status() == KINTROFF);
    return ioq->head == ioq->tail;
}

char ioqueue_getchar(Ioqueue *ioq){
    ASSERT(intr_get_status() == KINTROFF);
    while(ioqueue_empty(ioq)){
        lock_acquire(&ioq->lock);
        ioq->consumer = running_thread();
        thread_block(TASK_BLOCKED);
        lock_release(&ioq->lock);
    }
    char byte = ioq->buf[ioq->tail];
    ioq->tail = next_pos(ioq->tail);

    if(ioq->producer != NULL){  // the waiting producer
        thread_unblock(ioq->producer);
        ioq->producer = NULL;
    }
    return byte;
}

void ioqueue_putchar(Ioqueue *ioq, char byte){
    ASSERT(intr_get_status() == KINTROFF);
    while(ioqueue_full(ioq)){
        lock_acquire(&ioq->lock);
        ioq->producer = running_thread();
        thread_block(TASK_BLOCKED);
        lock_release(&ioq->lock);
    }

    ioq->buf[ioq->head] = byte;
    ioq->head = next_pos(ioq->head);

    if(ioq->consumer != NULL){
        thread_unblock(ioq->consumer);
    }
}