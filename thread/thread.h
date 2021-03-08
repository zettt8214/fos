#ifndef _THREAD_THREAD_H_
#define _THREAD_THREAD_H_
#include "stdint.h"
#include "list.h"

#define STACK_MAGIC 0x19870916
typedef void thread_func(void *);

typedef enum TaskStatus
{
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_WAITING,
    TASK_HANGING,
    TASK_DIED
} TaskStatus;

typedef struct IntrStack
{
    uint32_t vec_nr;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t err_code;
    void (*eip)(void);
    uint32_t cs;
    uint32_t eflags;
    void *esp;
    uint32_t ss;

} IntrStack;

typedef struct TreadStack
{
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;

    void (*eip)(thread_func *func, void *func_arg);
    /* below used for the first time when thread be called */
    void(*unused_retaddr);
    thread_func *function;
    void *func_arg;

} TreadStack;

typedef struct TaskPcb
{
    uint32_t *self_kstack;
    TaskStatus status;
    char name[16];
    uint8_t priority;
    uint8_t ticks;          //the time of task executing in cpu at every turn
    uint32_t elapsed_ticks; //total ticks of task executing in cpu
    ListElem general_tag;
    ListElem all_list_tag;
    uint32_t *pgdir; //page table virtul address
    uint32_t stack_magic;
} TaskPcb;
void thread_create(TaskPcb *pthread, thread_func function, void *func_arg);
void init_thread(TaskPcb *pthread, char *name, int prio);
TaskPcb *thread_start(char *name, int prio, thread_func function, void *func_arg);
TaskPcb *running_thread();
void thread_init();
void schedule();
void thread_block(TaskStatus stat);
void thread_unblock(TaskPcb *pthread);
#endif