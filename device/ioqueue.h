#ifndef _DEVICE_IOQUEUE_H_
#define _DEVICE_IOQUEUE_H_
#include "stdint.h"
#include "thread.h"
#include "sync.h"

#define BUFFZISE 64

typedef struct Ioqueue
{
    Lock lock;
    TaskPcb* producer;
    TaskPcb* consumer;
    char buf[BUFFZISE];
    int32_t head;
    int32_t tail;
} Ioqueue;
void ioqueue_init(Ioqueue *ioq);
bool ioqueue_full(Ioqueue *ioq);
bool ioqueue_empty(Ioqueue *ioq);
char ioqueue_getchar(Ioqueue *ioq);
void ioqueue_putchar(Ioqueue *ioq, char byte);
#endif