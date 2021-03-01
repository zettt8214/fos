#include "debug.h"
#include "string.h"
#include "global.h"

void memset(void* dst, uint8_t value, uint32_t size){
    ASSERT(dst != NULL);
    uint8_t* p_dst = (uint8_t*)dst;
    while(size--){
        *p_dst++ = value;
    }
}

void memcpy(void* dst, const void* src, uint32_t size){
    
}