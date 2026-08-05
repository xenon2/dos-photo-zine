#include <dos.h>
#include <conio.h>

#include "beep.h"

/* PIT base frequency */
#define PIT_FREQ 1193182UL

static void delay_ms(unsigned int ms)
{
    /* BIOS wait: INT 15h, AH=86h
       CX:DX = microseconds */
    union REGS r;
    unsigned long us = (unsigned long)ms * 1000UL;

    r.h.ah = 0x86;
    r.x.cx = (unsigned int)(us >> 16);
    r.x.dx = (unsigned int)(us & 0xFFFF);
    int86(0x15, &r, &r);
}

void pc_beep(unsigned int frequency_hz, unsigned int duration_ms)
{
    unsigned int divisor;
    unsigned char tmp;

    if (frequency_hz == 0)
        return;

    divisor = (unsigned int)(PIT_FREQ / frequency_hz);

    /* program PIT channel 2 */
    outp(0x43, 0xB6);              /* ch2, lobyte/hibyte, square wave */
    outp(0x42, divisor & 0xFF);
    outp(0x42, divisor >> 8);

    /* enable speaker */
    tmp = inp(0x61);
    outp(0x61, tmp | 0x03);

    delay_ms(duration_ms);

    /* disable speaker */
    tmp = inp(0x61);
    outp(0x61, tmp & ~0x03);
}
