#include "print.h"
#include "init.h"

void main(){
    init_all();
    asm volatile("sti");
    while(1);
}