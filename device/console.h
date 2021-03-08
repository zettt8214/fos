#ifndef _DEVICE_CONSOLE_H_
#define _DEVICE_CONSOLE_H_
#include "stdint.h"

void console_init();
void console_acquire();
void console_release();
void console_put_str(char *str);
void console_put_int(uint32_t num);
void console_put_char(uint8_t ch);
#endif