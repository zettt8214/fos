#ifndef _LIB_KERNEL_BITMAP_H_
#define _LIB_KERNEL_BITMAP_H_
#include "global.h"

#define BITMAP_MASK 1
typedef struct bitmap {
    uint32_t bitmap_bytes_len;
    uint8_t* bits;
}bitmap;

void bitmap_init(bitmap* btmp);
bool bitmap_scan_test(bitmap* btmp, uint32_t bit_idx);
int bitmap_scan(bitmap* btmp, uint32_t cnt);
void bitmap_set(bitmap* btmp, uint32_t bit_idx, int8_t value);

#endif