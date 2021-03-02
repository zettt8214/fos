
#ifndef _KERNEL_MEMORY_H_
#define _KERNEL_MEMORY_H_
#include "stdint.h"
#include "bitmap.h"

typedef struct virtual_addr {
    bitmap vaddr_bitmap;
    uint32_t vaddr_start;
}virtual_addr;

typedef struct pool {
    bitmap pool_bitmap;
    uint32_t phy_addr_start;
    uint32_t pool_size;
}pool;

typedef enum pool_flag{
    KERNEL_POOL = 1,
    USER_POOL = 2
}pool_flag;

/* flages of page table and page directory table */
#define PG_P_1 1
#define PG_P_0 0
#define PG_RW_R 0
#define PG_RW_W 2
#define PG_US_S 0
#define PG_US_U 4

void mem_init();
void* get_kernel_pages(uint32_t pg_cnt);
void* malloc_page(pool_flag pf,uint32_t pg_cnt);

#endif