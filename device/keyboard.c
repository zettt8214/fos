#include "keyboard.h"
#include "print.h"
#include "interrupt.h"
#include "io.h"
#include "global.h"
#include "ioqueue.h"

#define KBD_BUF_PORT 0x60 //keyboard buffer register port

/* control characters */
#define ESC '\x1b'
#define BACKSPACE '\b'
#define TAB '\t'
#define ENTER '\r'
#define DELETE '\x7f'

/* invisible characters */
#define INVISIBILE 0
#define CTRL_L INVISIBILE
#define CTRL_R INVISIBILE
#define SHIFT_L INVISIBILE
#define SHIFT_R INVISIBILE
#define ALT_L INVISIBILE
#define ALT_R INVISIBILE
#define CAPS_LOCK INVISIBILE

/* make code and break code of  invisible characters */
#define CTRL_L_MAKE 0x1d
#define CTRL_L_BREAK 0x9d
#define CTRL_R_MAKE 0xe01d
#define CTRL_R_BREAK 0xe09d
#define SHIFT_L_MAKE 0x2a
#define SHIFT_L_BREAK 0xaa
#define SHIFT_R_MAKE 0x36
#define SHIFT_R_BREAK 0xb6
#define ALT_L_MAKE 0x38
#define ALT_L_BREAK 0xb8
#define ALT_R_MAKE 0x0e38
#define ALT_R_BREAK 0x0eb8
#define CAPS_LOCK_MAKE 0x3a
#define CAPS_LOCK_BREAK 0xba

static bool ext_flag, shift_flag, ctrl_flag, alt_flags, cap_lock_flag;
Ioqueue kbd_buf;

/* code with shift, index express make code */
static char keymap[][2] = {
    /* ---------------------------------- */
    /* 0x00 */ {0, 0},
    /* 0x01 */ {ESC, ESC},
    /* 0x02 */ {'1', '!'},
    /* 0x03 */ {'2', '@'},
    /* 0x04 */ {'3', '#'},
    /* 0x05 */ {'4', '$'},
    /* 0x06 */ {'5', '%'},
    /* 0x07 */ {'6', '^'},
    /* 0x08 */ {'7', '&'},
    /* 0x09 */ {'8', '*'},
    /* 0x0A */ {'9', '('},
    /* 0x0B */ {'0', ')'},
    /* 0x0C */ {'-', '_'},
    /* 0x0D */ {'=', '+'},
    /* 0x0E */ {BACKSPACE, BACKSPACE},
    /* 0x0F */ {TAB, TAB},
    /* 0x10 */ {'q', 'Q'},
    /* 0x11 */ {'w', 'W'},
    /* 0x12 */ {'e', 'E'},
    /* 0x13 */ {'r', 'R'},
    /* 0x14 */ {'t', 'T'},
    /* 0x15 */ {'y', 'Y'},
    /* 0x16 */ {'u', 'U'},
    /* 0x17 */ {'i', 'I'},
    /* 0x18 */ {'o', 'O'},
    /* 0x19 */ {'p', 'P'},
    /* 0x1A */ {'[', '{'},
    /* 0x1B */ {']', '}'},
    /* 0x1C */ {ENTER, ENTER},
    /* 0x1D */ {CTRL_L, CTRL_L},
    /* 0x1E */ {'a', 'A'},
    /* 0x1F */ {'s', 'S'},
    /* 0x20 */ {'d', 'D'},
    /* 0x21 */ {'f', 'F'},
    /* 0x22 */ {'g', 'G'},
    /* 0x23 */ {'h', 'H'},
    /* 0x24 */ {'j', 'J'},
    /* 0x25 */ {'k', 'K'},
    /* 0x26 */ {'l', 'L'},
    /* 0x27 */ {';', ':'},
    /* 0x28 */ {'\'', '"'},
    /* 0x29 */ {'`', '~'},
    /* 0x2A */ {SHIFT_L, SHIFT_L},
    /* 0x2B */ {'\\', '|'},
    /* 0x2C */ {'z', 'Z'},
    /* 0x2D */ {'x', 'X'},
    /* 0x2E */ {'c', 'C'},
    /* 0x2F */ {'v', 'V'},
    /* 0x30 */ {'b', 'B'},
    /* 0x31 */ {'n', 'N'},
    /* 0x32 */ {'m', 'M'},
    /* 0x33 */ {',', '<'},
    /* 0x34 */ {'.', '>'},
    /* 0x35 */ {'/', '?'},
    /* 0x36	*/ {SHIFT_R, SHIFT_R},
    /* 0x37 */ {'*', '*'},
    /* 0x38 */ {ALT_L, ALT_L},
    /* 0x39 */ {' ', ' '},
    /* 0x3A */ {CAPS_LOCK, CAPS_LOCK}};

static void intr_keyboard_handle()
{
    bool break_code;
    uint16_t scancode = inb(KBD_BUF_PORT);
    if (scancode == 0xe0)
    {
        ext_flag = true;
        return;
    }
    if (ext_flag)
    {
        scancode = (scancode | 0xe000);
        ext_flag = false;
    }
    break_code = ((scancode & 0x0080) != 0);

    if (break_code)
    {
        uint16_t make_code = (scancode & 0xff7f);
        if (make_code == CTRL_L_MAKE || make_code == CTRL_R_MAKE)
        {
            ctrl_flag = false;
        }
        if (make_code == SHIFT_L_MAKE || make_code == SHIFT_R_MAKE)
        {
            shift_flag = false;
        }
        if (make_code == ALT_L_MAKE || make_code == ALT_R_MAKE)
        {
            alt_flags = false;
        }
        return;
    }
    else if ((scancode > 0x00 && scancode < 0x3b) ||
             scancode == CTRL_R_MAKE ||
             scancode == ALT_R_MAKE)
    {
        bool shift = false;
        /* handle combination of the shift key*/
        if (scancode < 0x0e || scancode == 0x29 ||
            scancode == 0x27 || scancode == 0x28 ||
            scancode == 0x33 || scancode == 0x34 ||
            scancode == 0x35 || scancode == 0x1a ||
            scancode == 0x1b || scancode == 0x2b)
        {
            if (shift_flag)
            {
                shift = true;
            }
        }
        else
        {
            /* handle letter: lower or upper */
            if (shift_flag && cap_lock_flag)
            {
                shift = false;
            }
            else if (shift_flag || cap_lock_flag)
            {
                shift = true;
            }
            else
            {
                shift = false;
            }
        }
        uint8_t index = (scancode & 0x00ff);
        char ch = keymap[index][shift];
        if (ch)
        {
            if ((ctrl_flag && ch == 'l') || (ctrl_flag && ch == 'u'))
            {
                ch -= 'a';
            }
            if (!ioqueue_full(&kbd_buf))
            {
                ioqueue_putchar(&kbd_buf, ch);
            }
            return;
        }

        if (scancode == CTRL_L_MAKE || scancode == CTRL_R_MAKE)
        {
            ctrl_flag = true;
        }
        else if (scancode == SHIFT_L_MAKE || scancode == SHIFT_R_MAKE)
        {
            shift_flag = true;
        }
        else if (scancode == ALT_L_MAKE || scancode == ALT_R_MAKE)
        {
            alt_flags = true;
        }
        else if (scancode == CAPS_LOCK_MAKE)
        {
            cap_lock_flag = !cap_lock_flag;
        }
        else
        {
            put_str("\nUnkown key\n");
        }
    }
}

void keyboard_init()
{
    put_str("[-]keyboard init start\n");
    ioqueue_init(&kbd_buf);
    register_handler(0x21, intr_keyboard_handle);
    put_str("[-]keyboard init done\n");
}
