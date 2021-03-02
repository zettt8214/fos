#include "memory.h"
#include "print.h"
#include "debug.h"
#include "string.h"

#define PG_SIZE 4096

/*
*   Kernel main thread's pcb is in 0xc009e000, top stack is in 0xc009f000.
*   One page size bitmap represents 128M memory, this OS supports 512M, 
*   so bitmap uses 4 pages. And the start virtual address of bitmap is 0xc009a000.
*/

#define MEM_BITMAP_BASE 0xc009a000
#define K_HEAP_START 0xc0100000
#define PDE_IDX(addr) ((addr & 0xffc00000) >> 22)
#define PTE_IDX(addr) ((addr & 0x003ff000) >> 12)

pool kernel_pool, user_pool;
virtual_addr kernel_vaddr;

/* get pg_cnt pages virtual address */
static void * vaddr_get(pool_flag pf, uint32_t pg_cnt){
    int vaddr_start = 0, bit_idx_start = -1;
    uint32_t cnt = 0;
    if(pf == KERNEL_POOL){
        bit_idx_start = bitmap_scan(&kernel_vaddr.vaddr_bitmap, pg_cnt);
        if(bit_idx_start == -1){
            return NULL;
        }
        while(cnt < pg_cnt){
            bitmap_set(&kernel_vaddr.vaddr_bitmap, bit_idx_start + cnt, 1);
            cnt++;
        }
        vaddr_start = kernel_vaddr.vaddr_start + bit_idx_start * PG_SIZE;
    }else{
        /* TODO */
    }
    return (void*)vaddr_start;
}

/* get the pte pointer of vaddr*/
uint32_t* pte_ptr(uint32_t vaddr){
    uint32_t* pte = (uint32_t*) (0xffc00000 + ((vaddr & 0xffc00000) >> 10) + PTE_IDX(vaddr) * 4); //the last item of pdt point pdt itself
    return pte;
}

/* get the pde pointer of vaddr*/
uint32_t* pde_ptr(uint32_t vaddr) {
   uint32_t* pde = (uint32_t*)((0xfffff000) + PDE_IDX(vaddr) * 4);
   return pde;
}

/* get a physical page in mem_pool*/
static void* palloc(pool* mem_pool){
    int bit_idx = bitmap_scan(&mem_pool->pool_bitmap, 1);
    if(bit_idx == -1){
        return NULL;
    }
    bitmap_set(&mem_pool->pool_bitmap, bit_idx, 1);
    uint32_t page_phyaddr = (mem_pool->phy_addr_start + bit_idx * PG_SIZE);
    return (void*)page_phyaddr;
}


/*  maping  vaddr with page_phyaddr */
static void page_table_add(void* _vaddr, void* _page_phyaddr ){
   
    uint32_t vaddr = (uint32_t)_vaddr;
    uint32_t page_phyaddr = (uint32_t)_page_phyaddr;
    uint32_t* pde = pde_ptr(vaddr);
    uint32_t* pte = pte_ptr(vaddr);

    if(*pde & 0x00000001){  //flag P
        ASSERT(!(*pte & 0x00000001));
        if(!(*pte & 0x00000001)){
            *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
        }else{
            PANIC("pte repeat");
            *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
        }
    }else{
        uint32_t pt_phyaddr = (uint32_t)palloc(&kernel_pool);
        *pde = (pt_phyaddr | PG_US_S | PG_RW_W | PG_P_1);
        memset((void*)((int)pte & 0xfffff000), 0, PG_SIZE);
        ASSERT(!(*pte & 0x00000001));
        *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
    }
}
/* get pg_cnt pages and return start vritual address */
void* malloc_page(pool_flag pf,uint32_t pg_cnt){
    put_str("malloc_page\n");
    ASSERT(pg_cnt > 0 && pg_cnt < 3840);

    void* vaddr_start = vaddr_get(pf, pg_cnt); 
    if(vaddr_start == NULL){
        return NULL;
    }
    uint32_t vaddr = (uint32_t)vaddr_start;
    uint32_t cnt = pg_cnt;
    pool* mem_pool = pf & KERNEL_POOL ? &kernel_pool : &user_pool;
    while(cnt--){
        void* page_phyaddr = palloc(mem_pool);
        if(page_phyaddr == NULL){
            return NULL;
        }
        page_table_add((void*)vaddr, page_phyaddr);
        vaddr += PG_SIZE;
    }
    return vaddr_start;
}

void* get_kernel_pages(uint32_t pg_cnt){
    void* vaddr = malloc_page(KERNEL_POOL, pg_cnt);
    if(vaddr != NULL){
        memset(vaddr, 0, pg_cnt * PG_SIZE);
    }
    return vaddr;
}
static void mem_pool_init(uint32_t all_mem){
    put_str("[-]mem_pool_init start\n");
    uint32_t page_table_size = PG_SIZE * 256; //the memory used for page table and page directory table in loader.s 
    uint32_t used_mem = page_table_size + 0x100000; // 0x1000000 used for kernel
    uint32_t free_mem = all_mem - used_mem;
    uint16_t free_pages = free_mem / PG_SIZE;
    uint16_t kernel_free_pages = free_pages / 2;
    uint16_t user_free_pages = free_pages - kernel_free_pages;

    uint32_t kernel_bitmap_length = kernel_free_pages / 8;
    uint32_t user_bitmap_length = user_free_pages / 8;

    uint32_t kernel_pool_start = used_mem;
    uint32_t user_pool_start = kernel_pool_start + kernel_free_pages * PG_SIZE;

    kernel_pool.phy_addr_start = kernel_pool_start;
    kernel_pool.pool_size = kernel_free_pages * PG_SIZE;
    kernel_pool.pool_bitmap.bitmap_bytes_len = kernel_bitmap_length;
    kernel_pool.pool_bitmap.bits = (void*)MEM_BITMAP_BASE;

    user_pool.phy_addr_start = user_pool_start;
    user_pool.pool_size = user_pool_start;
    user_pool.pool_bitmap.bitmap_bytes_len = user_bitmap_length;
    user_pool.pool_bitmap.bits = (void*)(MEM_BITMAP_BASE + kernel_bitmap_length);

    put_str("[*]kernel_pool.phy_addr_start: ");
    put_int((int)kernel_pool_start);
    put_str(" kernel_pool.pool_bitmap.bits: ");
    put_int((int)kernel_pool.pool_bitmap.bits);
    put_char('\n');
    put_str("[*]user_pool.phy_addr_start: ");
    put_int((int)user_pool_start);
    put_str(" user_pool.pool_bitmap.bits: ");
    put_int((int)user_pool.pool_bitmap.bits);
    put_char('\n');

    bitmap_init(&kernel_pool.pool_bitmap);
    bitmap_init(&user_pool.pool_bitmap);

    kernel_vaddr.vaddr_bitmap.bitmap_bytes_len = kernel_bitmap_length;
    kernel_vaddr.vaddr_bitmap.bits = (void*)(MEM_BITMAP_BASE + kernel_bitmap_length + user_bitmap_length);
    kernel_vaddr.vaddr_start = K_HEAP_START;
    bitmap_init(&kernel_vaddr.vaddr_bitmap);
    put_str("[+]mem_pool_init done\n");
}

void mem_init(){
    put_str("[-]mem_init start\n");
    uint32_t total_mem_bytes = (*(uint32_t*)0xb03); //physical address of total_mem_bytes in loaders.s , saving total memory bytes;
    mem_pool_init(total_mem_bytes);
    put_str("[+]mem_init done\n");
}