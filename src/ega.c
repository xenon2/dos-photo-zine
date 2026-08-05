#include <dos.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <conio.h>
#include <graph.h>


#include "ega.h"

#define EGA_PTR ((unsigned char far*)0xA0000000L)

#define EGA_BYTES_PER_LINE 80
#define EGA_VRAM ((unsigned char far*)0xA0000000L)
#define PLANE_SIZE (80 * 350)

//has_ega_640x350_4bpp
int is_ega(void)
{
    union REGS r;

    r.h.ah = 0x12;
    r.h.bl = 0x10;
    int86(0x10, &r, &r);

    /*
        BL returned:
        00h = no EGA
        01h = EGA 64k
        02h = EGA 128k
        03h = EGA 256k
        VGA always reports >=02h
    */

    if (r.h.bl == 0)
        return 0;

    /* EGA 640x350 requires >= 128k */
    return (r.h.bl >= 2);
}

static void ega_set_map_mask(unsigned char mask)
{
    outp(0x3C4, 0x02);      /* Sequencer: Map Mask */
    outp(0x3C5, mask);
}

//SLOW but fun to look at
static void ega_copy_plane_to_vram(const unsigned char *src, int plane)
{
    volatile unsigned short far *vram16 = (unsigned short far *)EGA_VRAM;
    const unsigned short *src16 = (const unsigned short *)src;
    int i;

    ega_set_map_mask((unsigned char)(1 << plane));

    // CPU byte writes -> selected plane
    for (i = 0; i < PLANE_SIZE / 2; i+=4) {
        vram16[i] = src16[i];
        vram16[i+1] = src16[i+1];
        vram16[i+2] = src16[i+2];
        vram16[i+3] = src16[i+3];
    }
}

static void ega_copy_plane_to_vram_memcpy(const unsigned char *src, int plane)
{
    /* VRAM EGA: A000:0000 */
    void far *vram = (void far *)EGA_VRAM;
    ega_set_map_mask((unsigned char)(1 << plane));
    _fmemcpy(vram, src, PLANE_SIZE);
}

void load_ega_dat(int index)
{
    char filename[64];
    int fd, plane;
    int n;
    static unsigned char buf[PLANE_SIZE]; /* 28k fits in near static */

    sprintf(filename, "zine/EGA/%d.DAT", index);
    fd = open(filename, O_RDONLY | O_BINARY);
    if (fd < 0) return;

    for (plane = 0; plane < 4; plane++) {
        n = read(fd, buf, PLANE_SIZE);
        if (n != PLANE_SIZE) { close(fd); return; }
        //ega_copy_plane_to_vram(buf, plane);
        ega_copy_plane_to_vram_memcpy(buf, plane);
    }

    /* restore mask to all planes (optional) */
    ega_set_map_mask(0x0F);

    close(fd);
}

//just empty f(x)
void load_ega_palette(int index)
{

}

//just empty f(x)
void transition_ega(void)
{

}


void image_info_ega(int index)
{
    #define TXT_BUFFER 1024
    char filename[64];
    char textBuffer[TXT_BUFFER+1];          //everything else will be truncated
    int fd;

    sprintf(filename, "zine/EGA/%d.txt", index);
    fd = open(filename, O_RDONLY | O_BINARY);
    if (fd < 0) return;
    memset(textBuffer,0,sizeof(textBuffer));

    read(fd, textBuffer, TXT_BUFFER);
    close(fd);

    _settextposition(0, 0);
    printf("%s\n", textBuffer);
}

int image_ega_exists(int index)
{
    char filename[32];
    int fd;

    sprintf(filename, "zine/EGA/%d.DAT", index);
    fd = open(filename, O_RDONLY | O_BINARY);
    if (fd < 0)
        return 0;

    close(fd);
    return 1;
}

void set_ega_mode(void)
{
    union REGS r;

    /* BIOS: EGA 640x350x16 */
    r.h.ah = 0x00;
    r.h.al = 0x10;
    int86(0x10, &r, &r);

    /* Sequencer: enable all planes */
    outp(0x3C4, 0x02);
    outp(0x3C5, 0x0F);

    /* Graphics Controller: CPU writes */
    outp(0x3CE, 0x00); outp(0x3CF, 0x00);
    outp(0x3CE, 0x01); outp(0x3CF, 0x00);
    outp(0x3CE, 0x03); outp(0x3CF, 0x00);
    outp(0x3CE, 0x05); outp(0x3CF, 0x00);
    outp(0x3CE, 0x06); outp(0x3CF, 0x05);
    outp(0x3CE, 0x08); outp(0x3CF, 0xFF);

}

