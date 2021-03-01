#ifndef _LIB_STRING_H_
#define _LIB_STRING_H_
#include "stdint.h"

void memset(void* dst, uint8_t value, uint32_t size);
void memcpy(void* dst, const void* src, uint32_t size);
int memcmp(const void* mem1, const void* mem2, uint32_t size);

char* strcpy(char* dst, const char* src);
uint32_t strlen(const char* str);
int strcmp(const char* s1, const char* s2);
char* strchr(const char* str, const char ch);
char* strrchr(const char* str, const char ch);

#endif