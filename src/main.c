/* This is free and unencumbered software released into the public domain.
NO ANY WARRANTY
<https://unlicense.org> */

#include <dos.h>
#include <stdio.h>

#include "vga.h"
#include "ega.h"
#include "beep.h"
#include "utils.h"

static const ViewerOps *active_viewer;

typedef enum ViewerMode {
    MODE_NONE = -2,
    MODE_QUIT = -1,
    VGA,
    EGA
} ViewerMode;

#define KEY_INFO 'i'

static int viewer_available(const ViewerOps *viewer)
{
    return viewer->image_exists(1) && viewer->hardware_available();
}


static void set_txt_mode(void)
{
    union REGS r;
    r.h.ah = 0x00;
    r.h.al = 0x03;
    int86(0x10, &r, &r);
}



static void banner_line(const char *text)
{
    printf("| %-39s |\n", text);
}

static void banner(void)
{
    char build_info[40];
    int vga_available = viewer_available(&vga_viewer_ops);
    int ega_available = viewer_available(&ega_viewer_ops);

    pc_beep(140, 50);
    pc_beep(240, 50);
    pc_beep(340, 50);

    sprintf(build_info, "Build: %s %s", __DATE__, __TIME__);

    printf("+-----------------------------------------+\n");
    banner_line("");
    banner_line("       WELCOME TO DOS PHOTO ZINE");
    banner_line("");
    printf("+-----------------------------------------+\n");
    banner_line(build_info);
    banner_line("Choose your display mode:");

    if (vga_available) {
        banner_line("[1] or [<-]  VGA  320x200 / 256 colors");
    }

    if (ega_available) {
        banner_line("[2] or [->]  EGA  640x350 / 16 colors");
    }

    if (!vga_available && !ega_available) {
        banner_line("No supported video mode or image files!");
    }

    banner_line("");
    banner_line("[ESC] Quit");
    printf("+-----------------------------------------+\n");
    banner_line("Viewer: [<-] Previous  [->] Next");
    banner_line("        [i] Info       [ESC] Exit");
    printf("+-----------------------------------------+\n");
}


static void exit_prog(void) {
    pc_beep(340, 50);
    pc_beep(240, 50);
    pc_beep(140, 50);
    printf("See you!\n");
}

static int previous_image(int current)
{
    if (current > 1)
        return current - 1;

    current = 1;
    while (active_viewer->image_exists(current))
        current++;

    return current - 1;
}

static int next_image(int current)
{
    current++;
    if (!active_viewer->image_exists(current))
        return 1;

    return current;
}

static void images_key_loop(void) {
    int current = 1;
    int key;

    for (;;) {
        active_viewer->load_palette(current);
        active_viewer->load_image(current);

        key = read_key();

        if (KEY_ASCII(key) == KEY_ESC)
            break;

        if (KEY_ASCII(key) == KEY_INFO) {
            active_viewer->show_image_info(current);
            read_key();
        }


        switch (KEY_SCAN(key))
        {
            case SCAN_LEFT:
                current = previous_image(current);
                break;

            case SCAN_RIGHT:
                current = next_image(current);
                break;
        }
       active_viewer->transition();
    }
}


static void images_auto_loop(void) {
    int current = 1;
    int key;
    int looper = 0;

     for (;;) {
        active_viewer->load_palette(current);
        active_viewer->load_image(current);

        for (looper=0;looper<10;looper++) {
            sleep_nseconds(100);
            key = read_key_nb();
            if (key != -1 && KEY_ASCII(key) == KEY_ESC) {
                looper = 200;
            }
        }
        current++;
        if (!active_viewer->image_exists(current))
            current = 1;

       active_viewer->transition();
    }

}

static void set_mode(ViewerMode mode) {
    switch (mode) {
        case VGA:
            active_viewer = &vga_viewer_ops;
            break;

        case EGA:
            active_viewer = &ega_viewer_ops;
            break;

        default:
            return;
    }

    active_viewer->set_video_mode();
}

static const char *mode_name(ViewerMode mode)
{
    switch (mode) {
        case VGA:
            return "VGA";

        case EGA:
            return "EGA";
    }

    return "unknown";
}

static ViewerMode mode_from_key(int key)
{
    if (KEY_ASCII(key) == KEY_ESC)
        return MODE_QUIT;

    if (KEY_ASCII(key) == '1' && viewer_available(&vga_viewer_ops))
        return VGA;

    if (KEY_ASCII(key) == '2' && viewer_available(&ega_viewer_ops))
        return EGA;

    if (KEY_ASCII(key) != 0)
        return MODE_NONE;

    switch (KEY_SCAN(key))
    {
        case SCAN_LEFT:
            if (viewer_available(&vga_viewer_ops))
                return VGA;
            break;

        case SCAN_RIGHT:
            if (viewer_available(&ega_viewer_ops))
                return EGA;
            break;
    }

    return MODE_NONE;
}

static ViewerMode select_mode(void)
{
    ViewerMode mode;

    for (;;) {
        mode = mode_from_key(read_key());
        if (mode != MODE_NONE)
            return mode;
    }
}

static void start_viewer(ViewerMode mode)
{
    printf("%s mode selected, switching...\n", mode_name(mode));
    set_mode(mode);

    images_key_loop();
}

int main(void)
{
    ViewerMode mode;

    banner();

    mode = select_mode();
    if (mode != MODE_QUIT)
        start_viewer(mode);

    set_txt_mode();
    exit_prog();
    return 0;
}

