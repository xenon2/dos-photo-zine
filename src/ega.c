#include <dos.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <conio.h>
#include <graph.h>


#include "ega.h"
#include "dzdecode.h"

#define EGA_PTR ((unsigned char far*)0xA0000000L)

#define EGA_BYTES_PER_LINE 80
#define EGA_VRAM ((unsigned char far*)0xA0000000L)
#define PLANE_SIZE (EGA_BYTES_PER_LINE * 350)
#define PACKED_SIZE (640L * 350L / 2L)
#define DECODE_BLOCK_SIZE 4096

/* One lookup contributes two pixels to all four output planes. */
static unsigned long ega_unpack_by_position[4][256];
static unsigned char ega_plane_buffer[4][DECODE_BLOCK_SIZE / 4];

const ViewerOps ega_viewer_ops = {
    set_ega_mode,
    load_ega_palette,
    load_ega_dat,
    image_ega_exists,
    transition_ega,
    image_info_ega,
    is_ega
};

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

static void ega_init_unpack_tables(void)
{
    unsigned int value;

    for (value = 0; value < 256; value++) {
        unsigned int first = value >> 4;
        unsigned int second = value & 0x0F;
        unsigned int plane, position;
        unsigned long expanded = 0;

        for (plane = 0; plane < 4; plane++) {
            unsigned int pair = (((first >> plane) & 1) << 1) |
                                ((second >> plane) & 1);
            expanded |= (unsigned long)pair << (plane * 8);
        }

        for (position = 0; position < 4; position++)
            ega_unpack_by_position[position][value] =
                expanded << (6 - position * 2);
    }
}

static int write_ega_packed(unsigned long offset, const unsigned char *data,
                            unsigned int length)
{
    unsigned int output_offset;
    unsigned int count;
    unsigned int i;
    unsigned int plane;

    if ((offset & 3) || (length & 3) || length > DECODE_BLOCK_SIZE)
        return 0;

    output_offset = (unsigned int)(offset / 4);
    count = length / 4;
    if ((unsigned long)output_offset + count > PLANE_SIZE)
        return 0;

    for (i = 0; i < count; i++) {
        unsigned int at = i * 4;
        unsigned long expanded =
            ega_unpack_by_position[0][data[at]] |
            ega_unpack_by_position[1][data[at + 1]] |
            ega_unpack_by_position[2][data[at + 2]] |
            ega_unpack_by_position[3][data[at + 3]];

        ega_plane_buffer[0][i] = (unsigned char)expanded;
        ega_plane_buffer[1][i] = (unsigned char)(expanded >> 8);
        ega_plane_buffer[2][i] = (unsigned char)(expanded >> 16);
        ega_plane_buffer[3][i] = (unsigned char)(expanded >> 24);
    }

    for (plane = 0; plane < 4; plane++) {
        ega_set_map_mask((unsigned char)(1 << plane));
        _fmemcpy(EGA_VRAM + output_offset, ega_plane_buffer[plane], count);
    }

    return 1;
}

void load_ega_dat(int index)
{
    char filename[64];
    int fd;

    sprintf(filename, "zine/EGA/%d.DAT", index);
    fd = open(filename, O_RDONLY | O_BINARY);
    if (fd < 0) return;

    dz_decode_file(fd, PACKED_SIZE, write_ega_packed);

    /* restore mask to all planes */
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

    ega_init_unpack_tables();

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

