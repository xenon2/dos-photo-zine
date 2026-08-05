/* This is free and unencumbered software released into the public domain.
NO ANY WARRANTY
<https://unlicense.org> */

#include <dos.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>   // malloc, free

#include "vga.h"
#include "ega.h"
#include "beep.h"
#include "utils.h"

// f(x) pointers for image loading etc...
void (*load_image)(int);
void (*load_palette)(int);
int (*image_exists)(int);
void (*transition)(void);
void (*image_info)(int);


int is_ega_avail = 0;
int is_vga_avail = 0;

enum modes {
    MODE_NONE = -2,
    MODE_QUIT = -1,
    VGA,
    EGA
};

#define KEY_ASCII(key)      ((key) & 0xFF)
#define KEY_SCAN(key)       (((key) >> 8) & 0xFF)

#define KEY_ESC             27
#define KEY_INFO            'i'

#define SCAN_LEFT           0x4B
#define SCAN_RIGHT          0x4D

// detect "video modes" and detect folders
// if no VGA or EGA folders present - marked as unavailable mode
void detect_modes(void) {
    // find if dir with files present (at least 1.dat)
    // and check bios for modes
    if (image_vga_exists(1) && is_vga()) {
        is_vga_avail = 1;
    }

    if (image_ega_exists(1) && is_ega()) {
        is_ega_avail = 1;
    }
}


void set_txt_mode(void)
{
    union REGS r;
    r.h.ah = 0x00; //0x83h
    r.h.al = 0x03;
    int86(0x10, &r, &r);
}



void banner(void) {
    const char build_info[] = "= Build: " __DATE__ " " __TIME__ "   =\n";
    pc_beep(140, 50);
    pc_beep(240, 50);
    pc_beep(340, 50);

    printf("=================================\n");
    printf(build_info);
    printf("=================================\n");
    printf("= Super Basic DOS Photo Zine    =\n");
    if (is_vga_avail) {
    printf("= [<-]  VGA 320x200 8bit        =\n");
    }

    if (is_ega_avail) {
    printf("= [->]  EGA 640x350 4bit        =\n");
    }

    if (!is_vga_avail && !is_ega_avail) {
        printf("= No video mode or no files  :( =\n");
    }

    printf("= [ESC] - quit                  =\n");
    printf("=================================\n");
    printf("= Keys when viewing:            =\n");
    printf("= [<-] [->] [i] [Esc]           =\n");
    printf("=================================\n");
}


void exit_prog(void) {
    pc_beep(340, 50);
    pc_beep(240, 50);
    pc_beep(140, 50);
    printf("See you!\n");
}

void images_key_loop(void) {
    int current = 1;    // image index
    int key;            // kb key

    for (;;) {
        load_palette(current);
        load_image(current);

        key = read_key();

        if (KEY_ASCII(key) == KEY_ESC)
            break;

        if (KEY_ASCII(key) == KEY_INFO) {
            image_info(current);
            read_key();
        }


        switch (KEY_SCAN(key))
        {
            case SCAN_LEFT:
                current--;
                if (current < 1) {
                    current = 1;
                    while (image_exists(current))
                        current++;
                    current--;  // last
                }
                break;

            case SCAN_RIGHT:
                current++;
                if (!image_exists(current))
                    current = 1;
                break;
        }
       transition();
    }
}


void images_auto_loop(void) {
    int current = 1;    // image index
    int key;            // kb key
    int looper;

     for (;looper<100;) {
        load_palette(current);
        load_image(current);

        //
        for (looper=0;looper<10;looper++) {
            sleep_nseconds(100);
            key = read_key_nb();
            if (key != -1 && KEY_ASCII(key) == KEY_ESC) {
                looper = 200;
            }
        }
        current++;
        if (!image_exists(current))
            current = 1;

       transition();
    }

}

// Set video mode + set f(x) pointers/handlers
// for specific video mode
void set_mode(int mode) {
    switch (mode) {
        case VGA:
            set_vga_mode();
            load_image = load_vga_dat;
            load_palette = load_vga_palette;
            image_exists = image_vga_exists;
            transition = transition_vga;
            image_info = image_info_vga;
            break;

        case EGA:
            set_ega_mode();
            load_image   = load_ega_dat;
            load_palette = load_ega_palette;
            image_exists = image_ega_exists;
            transition = transition_ega;
            image_info = image_info_ega;
            break;
    };
}

const char *mode_name(int mode)
{
    switch (mode) {
        case VGA:
            return "VGA";

        case EGA:
            return "EGA";
    }

    return "unknown";
}

int mode_from_key(int key)
{
    if (KEY_ASCII(key) == KEY_ESC)
        return MODE_QUIT;

    if (KEY_ASCII(key) == '1' && is_vga_avail)
        return VGA;

    if (KEY_ASCII(key) == '2' && is_ega_avail)
        return EGA;

    if (KEY_ASCII(key) != 0)
        return MODE_NONE;

    switch (KEY_SCAN(key))
    {
        case SCAN_LEFT:
            if (is_vga_avail)
                return VGA;
            break;

        case SCAN_RIGHT:
            if (is_ega_avail)
                return EGA;
            break;
    }

    return MODE_NONE;
}

int select_mode(void)
{
    int mode;

    for (;;) {
        mode = mode_from_key(read_key());
        if (mode != MODE_NONE)
            return mode;
    }
}

void start_viewer(int mode)
{
    printf("%s mode selected, switching...\n", mode_name(mode));
    set_mode(mode);

    images_key_loop();
    //images_auto_loop();
}

int main(void)
{
    int mode;

    detect_modes();
    banner();

    mode = select_mode();
    if (mode != MODE_QUIT)
        start_viewer(mode);

    set_txt_mode();
    exit_prog();
    return 0;
}

