#include "sync.h"
#include "interrupt.h"
#include "debug.h"

void sema_init(Semaphore *psema, uint8_t value)
{
    psema->value = value;
    list_init(&psema->waiters);
}

void lock_init(Lock *plock)
{
    plock->holder = NULL;
    plock->holder_repeat_nr = 0;
    sema_init(&plock->semaphore, 1);
}

void sema_down(Semaphore *psema)
{
    IntrStatus old_status = intr_disable();
    while (psema->value == 0)
    {
        ASSERT(!elem_find(&psema->waiters, &running_thread()->general_tag));
        if (elem_find(&psema->waiters, &running_thread()->general_tag))
        {
            PANIC("sema_down: thread blocked has been in waiters_lists\n");
        }
        list_append(&psema->waiters, &running_thread()->general_tag);
        thread_block(TASK_BLOCKED);
    }
    psema->value--;
    ASSERT(psema->value == 0);
    intr_set_status(old_status);
}

void sema_up(Semaphore *psema)
{
    IntrStatus old_status = intr_disable();
    ASSERT(psema->value == 0);
    if (!list_empty(&psema->waiters))
    {
        TaskPcb *thread_blocked = elem2entry(TaskPcb, general_tag, list_pop(&psema->waiters));
        thread_unblock(thread_blocked);
    }
    psema->value++;
    ASSERT(psema->value == 1);
    intr_set_status(old_status);
}

void lock_acquire(Lock *plock)
{
    if (plock->holder != running_thread())
    {
        sema_down(&plock->semaphore);
        plock->holder = running_thread();
        ASSERT(plock->holder_repeat_nr == 0);
        plock->holder_repeat_nr = 1;
    }
    else
    {
        plock->holder_repeat_nr++;
    }
}

void lock_release(Lock *plock)
{
    ASSERT(plock->holder == running_thread());
    if (plock->holder_repeat_nr > 1)
    {
        plock->holder_repeat_nr--;
        return;
    }
    ASSERT(plock->holder_repeat_nr == 1);

    plock->holder = NULL;
    plock->holder_repeat_nr = 0;
    sema_up(&plock->semaphore);
}

