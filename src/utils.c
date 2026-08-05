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

int read_key_nb(void)
{
    union REGS r;
    r.h.ah = 0x01;          /* check */
    int86(0x16, &r, &r);
    if (r.x.ax == 0)
        return -1;
    r.h.ah = 0x00;          /* read */
    int86(0x16, &r, &r);
    return r.x.ax;
}

int read_key(void)
{
    union REGS r;
    pc_beep(2340, 15);
    r.h.ah = 0x00;
    int86(0x16, &r, &r);
    return r.x.ax;
}
