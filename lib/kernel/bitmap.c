#include "bitmap.h"
#include "string.h"
#include "print.h"
#include "interrupt.h"
#include "debug.h"
#include "global.h"

/* initialize the bitmap */
void bitmap_init(bitmap* btmp){
    memset(btmp->bits, 0, btmp->bitmap_bytes_len);
}

/* judge whether bit_idx of the bitmap was used */
bool bitmap_scan_test(bitmap* btmp, uint32_t bit_idx){
    uint32_t byte_idx = bit_idx / 8;
    uint32_t bit_pos = bit_idx % 8;

    return btmp->bits[byte_idx] & (BITMAP_MASK << bit_pos);
}

/* find cnt unsed bit*/
int bitmap_scan(bitmap* btmp, uint32_t cnt){
    /* find the start byte to scan */
    uint32_t byte_idx = 0;
    while((btmp->bits[byte_idx] == 0xff) && (byte_idx < btmp->bitmap_bytes_len)){
        byte_idx++;
    }

    ASSERT(byte_idx < btmp->bitmap_bytes_len);
    if(byte_idx == btmp->bitmap_bytes_len){
        return -1;
    }

    /* find the start bit of start byte to scan */
    uint32_t bit_pos = 0;
    while(btmp->bits[bit_pos] & (uint8_t)(BITMAP_MASK << bit_pos)){
        bit_pos++;
    }

    uint32_t start_bit_idx = byte_idx * 8 + bit_pos; // first unused bit
    if(cnt == 1){
        return start_bit_idx;
    }

    uint32_t bits_left = btmp->bitmap_bytes_len * 8 - start_bit_idx;
    uint32_t next_bit_idx = start_bit_idx + 1;
    uint32_t count = 1;

    start_bit_idx = -1;
    while(bits_left){
        if(bitmap_scan_test(btmp,next_bit_idx)){
            count = 0;
        }else{
            count++;
        }
        if(count == cnt){
            start_bit_idx = next_bit_idx - cnt + 1;
        }
        next_bit_idx++;
        bits_left--;
    }
    return start_bit_idx;
}

void bitmap_set(bitmap* btmp, uint32_t bit_idx, int8_t value){
    ASSERT((value == 0) || (value == 1))
    uint32_t byte_idx = bit_idx / 8;
    uint32_t bit_pos = bit_idx % 8;
    
    if(value){
        btmp->bits[byte_idx] |= (BITMAP_MASK << bit_pos);
    }else{
        btmp->bits[byte_idx] &= ~(BITMAP_MASK << bit_pos);
    }
}
