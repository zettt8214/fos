
#ifndef _KERNEL_MEMORY_H_
#define _KERNEL_MEMORY_H_
#include "stdint.h"
#include "bitmap.h"

typedef struct VirtualAddr {
    BitMap vaddr_bitmap;
    uint32_t vaddr_start;
}VirtualAddr;

typedef struct Pool {
    BitMap pool_bitmap;
    uint32_t phy_addr_start;
    uint32_t pool_size;
}Pool;

typedef enum PoolFlag{
    KKERNEL_POOL = 1,
    KUSER_POOL = 2
}PoolFlag;

/* flages of page table and page directory table */
#define PG_P_1 1
#define PG_P_0 0
#define PG_RW_R 0
#define PG_RW_W 2
#define PG_US_S 0
#define PG_US_U 4

void mem_init();
void* get_kernel_pages(uint32_t pg_cnt);
void* malloc_page(PoolFlag pf,uint32_t pg_cnt);

#endif