#ifndef _LIB_KERNEL_BITMAP_H_
#define _LIB_KERNEL_BITMAP_H_
#include "global.h"

#define BITMAP_MASK 1
typedef struct BitMap {
    uint32_t bitmap_bytes_len;
    uint8_t* bits;
}BitMap;

void bitmap_init(BitMap* btmp);
bool bitmap_scan_test(BitMap* btmp, uint32_t bit_idx);
int bitmap_scan(BitMap* btmp, uint32_t cnt);
void bitmap_set(BitMap* btmp, uint32_t bit_idx, int8_t value);
#endif