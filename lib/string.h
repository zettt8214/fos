#ifndef _LIB_STRING_H_
#define _LIB_STRING_H_
#include "stdint.h"

void memset(void* dst, uint8_t value, uint32_t size);
void memcpy(void* dst, const void* src, uint32_t size);
void memcmp(const void* mem1, const void* mem2, uint32_t size);


#endif