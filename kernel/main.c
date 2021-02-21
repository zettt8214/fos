#include "print.h"
void main(){
    put_str("hello world!\n");
    put_char('\n');
    put_int(0);
    put_char('\n');
    put_int(0x123c);
    put_char('\n');
    put_int(0xffffffff);
    put_char('\n');
    put_int(0xffffab12);
    while(1);
}