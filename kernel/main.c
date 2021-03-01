#include "print.h"
#include "init.h"
#include "debug.h"

int main(){
    init_all();
    asm volatile("sti");
    ASSERT(1 == 2);
    while(1);
    return 0;
}