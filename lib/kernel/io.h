#ifndef _LIB_KERNEL_HDIO_H
#define _LIB_KERNEL_HDIO_H
#include "stdint.h"

/* write a byte data to a port*/
static inline void outb(uint16_t port, uint8_t data){
    asm volatile("outb %b0, %w1;": : "a"(data), "Nd"(port));
}

/* write word_cnt word to a port*/
static inline void outw(uint16_t port, const void* addr, uint32_t word_cnt){
    asm volatile (" \
    cld; \
    rep outsw" 
    : "+S"(addr), "+c"(word_cnt) : "d"(port));
}

/* read a byte data from a port*/
static inline uint8_t inb(uint16_t port){
    uint8_t data;
    asm volatile("\
        inb %w1, %b0;" 
    : "=a"(data) : "Nd"(port));
    return data;
}

/* read word_cnt word from a port*/
static inline void inw(uint16_t port, const void* addr, uint32_t word_cnt){
    asm volatile (" \
    cld; \
    rep insw" 
    : "+D"(addr), "+c"(word_cnt) : "d"(port) : "memory");
}
#endif