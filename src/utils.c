#include <bios.h>
#include <dos.h>

#include "utils.h"
#include "beep.h"

void sleep_nseconds(unsigned int nseconds)
{
    union REGS r;
    unsigned long usec;

    /* 1 nsekunda = 1 000 µs */
    usec = (unsigned long)nseconds * 1000UL;

    while (usec) {
        unsigned long chunk = usec;

        /* BIOS limit: max ~65 ms per one call */
        if (chunk > 60000UL)
            chunk = 60000UL;

        r.h.ah = 0x86;
        r.x.cx = (unsigned short)(chunk >> 16);
        r.x.dx = (unsigned short)(chunk & 0xFFFF);
        int86(0x15, &r, &r);

        usec -= chunk;
    }
}

void clear_keyboard_buffer(void)
{
    while (_bios_keybrd(_KEYBRD_READY) != 0)
        _bios_keybrd(_KEYBRD_READ);
}

int read_key_nb(void)
{
    if (_bios_keybrd(_KEYBRD_READY) == 0)
        return -1;

    return _bios_keybrd(_KEYBRD_READ);
}

int read_key(void)
{
    int key;

    pc_beep(2340, 15);
    key = _bios_keybrd(_KEYBRD_READ);

    /* Treat a burst/key-repeat as one action. */
    clear_keyboard_buffer();
    return key;
}
