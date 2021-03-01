#include "debug.h"
#include "string.h"
#include "global.h"

void memset(void* dst, uint8_t value, uint32_t size){
    ASSERT(dst != NULL);
    uint8_t* p_dst = (uint8_t*)dst;
    while(size > 0){
        *p_dst++ = value;
        size--;
    }
}

void memcpy(void* dst, const void* src, uint32_t size){
    ASSERT(dst != NULL && src != NULL);
    uint8_t* p_dst = (uint8_t*)dst;
    const uint8_t* p_src = (uint8_t*)src;
    while(size > 0){
        *p_dst++ = *p_src++;
        size--;
    }
}

int memcmp(const void* mem1, const void* mem2, uint32_t size){
    ASSERT(mem1 != NULL && mem2 != NULL);
    const uint8_t* p_mem1 = (uint8_t*)mem1;
    const uint8_t* p_mem2 = (uint8_t*)mem2;
    
    while(size > 0){
        if(*p_mem1 != *p_mem2){
            return *p_mem1 > *p_mem2 ? 1 : -1;
        }
        p_mem1++;
        p_mem2++;
        size--;
    }
    return 0;
}

char* strcpy(char* dst, const char* src){
    ASSERT(dst != NULL && src != NULL);
    char* s = dst;
    while(*src){
        *dst = *src;
        dst++;
        src++;
    }
    return s;
}
uint32_t strlen(const char* str){
    ASSERT(str != NULL );
    uint32_t len = 0;
    while(*str++){
        len++;
    }
    return len;
}

int strcmp(const char* s1, const char* s2){
    ASSERT(s1 != NULL && s2 != NULL)
    while(*s1  && *s1 == * s2){
        s1++;
        s2++;
    }
    if(*s1 < *s2){
        return -1;
    }else if(*s1 == *s2){
        return 0;
    }else{
        return 1;
    }
}
char* strchr(const char* str, const char ch){
    ASSERT(str != NULL);
    while(*str){
        if(*str == ch){
            return (char*)str;
        }
        str++;
    }
    return NULL;
}

char* strrchr(const char* str, const char ch){
    ASSERT(str != NULL);
    char* last_ch = NULL;
    while(*str){
        if(*str == ch){
            last_ch = (char*)str;
        }
        str++;
    }
    return last_ch;
}

char* strcat(char* dst, const char* src){
    ASSERT(dst != NULL && src != NULL);
    char* s = dst;
    while(*dst++);
    dst--;
    while((*dst++ = *src++));
    return s; 
}

uint32_t strchrs(const char* str, char ch){
    ASSERT(str != NULL);
    uint32_t ch_nums = 0;
    while(*str){
        if(*str == ch){
            ch_nums++;
        }
        str++;
    }
    return ch_nums;
}