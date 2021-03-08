#ifndef _THREAD_SYNC_H_
#define _THREAD_SYNC_H_
#include "list.h"
#include "stdint.h"
#include "thread.h"

typedef struct Semaphore
{
    uint8_t value;
    List waiters;
} Semaphore;

typedef struct Lock
{
    TaskPcb* holder;
    Semaphore semaphore;
    uint32_t holder_repeat_nr;

} Lock;
void sema_init(Semaphore *psema, uint8_t value);
void lock_init(Lock *plock);
void sema_down(Semaphore *psema);
void sema_up(Semaphore *psema);
void lock_acquire(Lock* plock);
void lock_release(Lock* plock);
#endif