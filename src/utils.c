#include <bios.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "beep.h"

#define INFO_BUFFER_SIZE 1024
#define INFO_BOX_WIDTH 38
#define INFO_LINE_WIDTH 34
#define INFO_MAX_LINES 16
#define BANNER_LINE_WIDTH 39

static void print_banner_text(const char *text)
{
    do {
        size_t length = strlen(text);

        printf("| %-39.39s |\n", text);
        text += length > BANNER_LINE_WIDTH ? BANNER_LINE_WIDTH : length;
    } while (*text);
}

void print_banner_file(const char *filename, const char *fallback)
{
    char text[INFO_BUFFER_SIZE + 1];
    FILE *file = fopen(filename, "rt");

    if (file == NULL) {
        print_banner_text(fallback);
        return;
    }

    while (fgets(text, sizeof(text), file) != NULL) {
        text[strcspn(text, "\r\n")] = '\0';
        print_banner_text(text);
    }
    fclose(file);
}

static void graphics_text_line(unsigned char row, unsigned char column,
                               const char *text)
{
    union REGS r;
    int previous_was_cr = 0;

    r.h.ah = 0x02;
    r.h.bh = 0;
    r.h.dh = row;
    r.h.dl = column;
    int86(0x10, &r, &r);

    while (*text) {
        /* DOS files normally contain CR/LF, but also render Unix LF files
           from the conversion tools at the left margin. */
        if (*text == '\n' && !previous_was_cr) {
            r.h.ah = 0x0E;
            r.h.al = '\r';
            r.h.bh = 0;
            r.h.bl = 15;
            int86(0x10, &r, &r);
        }

        r.h.ah = 0x0E;
        r.h.al = (unsigned char)*text;
        r.h.bh = 0;
        r.h.bl = 15;
        int86(0x10, &r, &r);

        previous_was_cr = (*text == '\r');
        text++;
    }
}

static void graphics_info_border(unsigned char row)
{
    char border[INFO_BOX_WIDTH + 1];
    int i;

    border[0] = '+';
    for (i = 1; i < INFO_BOX_WIDTH - 1; i++)
        border[i] = '-';
    border[INFO_BOX_WIDTH - 1] = '+';
    border[INFO_BOX_WIDTH] = '\0';
    graphics_text_line(row, 1, border);
}

static void graphics_info_line(unsigned char row, const char *text)
{
    char line[INFO_BOX_WIDTH + 1];
    int i;

    line[0] = '|';
    line[1] = ' ';
    for (i = 0; i < INFO_LINE_WIDTH; i++)
        line[i + 2] = ' ';
    for (i = 0; i < INFO_LINE_WIDTH && text[i]; i++)
        line[i + 2] = text[i];
    line[INFO_BOX_WIDTH - 2] = ' ';
    line[INFO_BOX_WIDTH - 1] = '|';
    line[INFO_BOX_WIDTH] = '\0';
    graphics_text_line(row, 1, line);
}

static void set_info_line(char *destination, const char *source)
{
    int i = 0;

    while (source[i] && i < INFO_LINE_WIDTH) {
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0';
}

static int split_info_lines(const char *text,
                            char lines[INFO_MAX_LINES][INFO_LINE_WIDTH + 1])
{
    int count = 0;
    int column;

    while (*text && count < INFO_MAX_LINES) {
        column = 0;
        while (*text && *text != '\r' && *text != '\n' &&
               column < INFO_LINE_WIDTH) {
            lines[count][column++] = *text++;
        }
        lines[count][column] = '\0';
        count++;

        /* A full line continues on the next box row. Explicit CR/LF,
           LF, and CR line endings all begin a new row. */
        if (*text == '\r') {
            text++;
            if (*text == '\n')
                text++;
        } else if (*text == '\n') {
            text++;
        }
    }

    if (*text && count == INFO_MAX_LINES)
        set_info_line(lines[count - 1], "... text truncated ...");

    return count;
}

void graphics_help(void (*reset_video_mode)(void))
{
    /* Resetting the active graphics mode gives the BIOS text a clean,
       readable background without temporarily returning to text mode. */
    reset_video_mode();

    graphics_text_line(5, 2,  " * DOS PHOTO ZINE - HELP *");
    graphics_text_line(8, 2,  "[ < ] LEFT ARROW   Previous image");
    graphics_text_line(10, 2, "[ > ] RIGHT ARROW  Next image");
    graphics_text_line(12, 2, "[ i ]              Image information");
    graphics_text_line(14, 2, "[ESC]              Quit");
    graphics_text_line(18, 2, "Press any key to return");
}

int graphics_image_info(void (*reset_video_mode)(void), int index)
{
    char filename[32];
    char text[INFO_BUFFER_SIZE + 1];
    char lines[INFO_MAX_LINES][INFO_LINE_WIDTH + 1];
    int fd;
    int length;
    int line_count;
    int row;
    int i;

    /* Captions are shared by all display modes. */
    sprintf(filename, "zine/%d.txt", index);
    fd = open(filename, O_RDONLY | O_BINARY);
    if (fd < 0)
        return 0;

    length = read(fd, text, INFO_BUFFER_SIZE);
    close(fd);
    if (length <= 0)
        return 0;

    text[length] = '\0';
    line_count = split_info_lines(text, lines);
    if (line_count == 0)
        return 0;

    //reset_video_mode();
    row = 1;
    graphics_info_border((unsigned char)row++);
    graphics_info_line((unsigned char)row++, "IMAGE INFORMATION");
    graphics_info_border((unsigned char)row++);
    for (i = 0; i < line_count; i++)
        graphics_info_line((unsigned char)row++, lines[i]);
    graphics_info_border((unsigned char)row++);
    graphics_info_line((unsigned char)row++, "Press any key to return");
    graphics_info_border((unsigned char)row);
    return 1;
}

void graphics_no_image_info(void (*reset_video_mode)(void))
{
    //reset_video_mode();
    graphics_text_line(10, 2, "No image information available.");
    graphics_text_line(12, 2, "Returning to image...");
    sleep_nseconds(1500);
}

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
