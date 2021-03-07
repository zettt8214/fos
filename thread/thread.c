#include "thread.h"
#include "string.h"
#include "global.h"
#include "memory.h"
#include "interrupt.h"
#include "debug.h"
#include "print.h"

#define PG_SIZE 4096


task_pcb *main_thread; //PCB of main thread
list thread_ready_list;
list thread_all_list;
static list_elem *thread_tag;

extern void switch_to(task_pcb *cur, task_pcb *next);

task_pcb *running_thread()
{
    uint32_t esp;
    asm("mov %%esp, %0"
        : "=g"(esp));
    return (task_pcb *)(esp & 0xfffff000);
}

static void kernel_thread(thread_func *fun, void *func_arg)
{
    intr_enable();
    fun(func_arg);
}

void thread_create(task_pcb *pthread, thread_func function, void *func_arg)
{
    pthread->self_kstack -= sizeof(intr_stack);

    pthread->self_kstack -= sizeof(thread_stack);
    thread_stack *kthread_stack = (thread_stack *)pthread->self_kstack;
    kthread_stack->eip = kernel_thread;
    kthread_stack->function = function;
    kthread_stack->func_arg = func_arg;
    kthread_stack->ebp = kthread_stack->ebx = kthread_stack->edi = kthread_stack->esi = 0;
}

void init_thread(task_pcb *pthread, char *name, int prio)
{
    memset(pthread, 0, sizeof(*pthread));
    strcpy(pthread->name, name);

    if (pthread == main_thread)
    {
        pthread->status = TASK_RUNNING;
    }
    else
    {
        pthread->status = TASK_READY;
    }

    pthread->priority = prio;
    pthread->self_kstack = (uint32_t *)((uint32_t)pthread + PG_SIZE);
    pthread->ticks = prio;
    pthread->elapsed_ticks = 0;
    pthread->pgdir = NULL;
    pthread->stack_magic = STACK_MAGIC;
}

task_pcb *thread_start(char *name, int prio, thread_func function, void *func_arg)
{
    task_pcb *thread = get_kernel_pages(1);
    init_thread(thread, name, prio);
    thread_create(thread, function, func_arg);

    //join to thread list
    ASSERT(!elem_find(&thread_ready_list, &thread->general_tag));
    list_append(&thread_ready_list, &thread->general_tag);
    ASSERT(!elem_find(&thread_all_list, &thread->all_list_tag));
    list_append(&thread_all_list, &thread->all_list_tag);

    return thread;
}

static void make_main_thread()
{
    main_thread = running_thread();
    init_thread(main_thread, "main", 31);
    ASSERT(!elem_find(&thread_all_list, &main_thread->all_list_tag));
    list_append(&thread_all_list, &main_thread->all_list_tag);
}

void schedule(){
    ASSERT(intr_get_status() == INTR_OFF);
    task_pcb* cur_thread = running_thread();

    if(cur_thread->status == TASK_RUNNING){
        ASSERT(!elem_find(&thread_ready_list, &cur_thread->general_tag));
        cur_thread->status = TASK_READY;
        cur_thread->ticks = cur_thread->priority;
        list_append(&thread_ready_list, &cur_thread->general_tag);   
    }else{
        /* TODO */
    }
    ASSERT(!list_empty(&thread_ready_list));
    thread_tag = NULL;
    thread_tag = list_pop(&thread_ready_list);
    task_pcb* next_thread = elem2entry(task_pcb, general_tag, thread_tag);
    next_thread->status = TASK_RUNNING;
    switch_to(cur_thread, next_thread);
}

/* initialize environment */
void thread_init(){
    put_str("[-]thread_init start\n");
    list_init(&thread_ready_list);
    list_init(&thread_all_list);
    make_main_thread();
    put_str("[-]thread_init done\n");
}