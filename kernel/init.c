#include "init.h"
#include "print.h"
#include "interrupt.h"
#include "memory.h"
#include "thread.h"
#include "timer.h"
#include "console.h"
#include "keyboard.h"

void init_all()
{
    put_str("[-]init_all\n");
    idt_init();
    mem_init();
    thread_init();
    timer_init();
    console_init();
    keyboard_init();
    put_str("[+]init_all done\n");
}