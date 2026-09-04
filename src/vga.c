#include <dos.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <conio.h>

#include "vga.h"
#include "dzdecode.h"
#include "utils.h"

#define VGA_PTR ((unsigned char far*)0xA0000000L)
#define IMG_SIZE 64000
#define SCREEN_W 320
#define SCREEN_H 200

const ViewerOps vga_viewer_ops = {
    set_vga_mode,
    load_vga_palette,
    load_vga_dat,
    image_vga_exists,
    transition_vga,
    image_info_vga,
    is_vga
};

// has_vga_320x200_256
int is_vga(void)
{
    union REGS r;

    r.x.ax = 0x1A00;
    int86(0x10, &r, &r);

    /* AL == 1Ah → VGA or MCGA */
    return (r.h.al == 0x1A);
}

/* Wait for start of vertical retrace (one frame sync) */
static void wait_vsync(void)
{
    /* wait until NOT in retrace */
    while (inp(0x3DA) & 0x08) { }
    /* wait until in retrace */
    while (!(inp(0x3DA) & 0x08)) { }
}

static void wait_frames(int n)
{
    while (n-- > 0) wait_vsync();
}


void transition_vga(void) {};

void transition_vga2(void)
{
    unsigned char far *vram = (unsigned char far*)0xA0000000L;
    int y, x;
    int frames_per_line = 1;

    if (frames_per_line < 0) frames_per_line = 0;

    for (y = 0; y < SCREEN_H; y+=2) {
        for (x = 0; x < SCREEN_W; x+=1)
            vram[y * SCREEN_W + x] = 0;

        if (y%50==0)
            wait_frames(frames_per_line);
    }

    for (y = 1; y < SCREEN_H; y+=2) {
        for (x = 0; x < SCREEN_W; x+=1)
            vram[y * SCREEN_W + x] = 0;

        if (y%9==0)
            wait_frames(frames_per_line);
    }
}


void load_vga_palette(int index)
{
    char filename[32];
    int fd, i;
    unsigned char pal[768];
    sprintf(filename, "zine/VGA/%d.PAL", index);
    fd = open(filename, O_RDONLY | O_BINARY);
    if (fd < 0) return;
    read(fd, pal, 768);
    close(fd);
    outp(0x3C8, 0);
    for (i = 0; i < 256; i++) {
        unsigned char r = pal[i*3 + 0];
        unsigned char g = pal[i*3 + 1];
        unsigned char b = pal[i*3 + 2];
        outp(0x3C9, r >> 2);
        outp(0x3C9, g >> 2);
        outp(0x3C9, b >> 2);
    }
}


static int write_vga(unsigned long offset, const unsigned char *data,
                     unsigned int length)
{
    _fmemcpy(VGA_PTR + offset, data, length);
    return 1;
}

void load_vga_dat(int index)
{
    char filename[32];
    int fd;
    sprintf(filename, "zine/VGA/%d.DAT", index);

    fd = open(filename, O_RDONLY | O_BINARY);
    if (fd < 0)
        return;

    dz_decode_file(fd, IMG_SIZE, write_vga);
    close(fd);
}

int image_vga_exists(int index)
{
    char filename[32];
    int fd;

    sprintf(filename, "zine/VGA/%d.DAT", index);
    fd = open(filename, O_RDONLY | O_BINARY);
    if (fd < 0)
        return 0;

    close(fd);
    return 1;
}

int image_info_vga(int index)
{
    return graphics_image_info(set_vga_mode, index);
}

void set_vga_mode(void)
{
    union REGS r;
    r.h.ah = 0x00;
    r.h.al = 0x13;
    int86(0x10, &r, &r);
}


