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
static int vga_available;
static int ega_available;

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

    pc_beep(140, 50);
    pc_beep(240, 50);
    pc_beep(340, 50);

    printf("+-----------------------------------------+\n");
    banner_line("");
    print_banner_file("zine/info.txt", " DOS Photo Zine  ");
    banner_line("");
    printf("+-----------------------------------------+\n");
    banner_line("Choose your display mode:");

    if (vga_available)
        banner_line("[1] or [<-]  VGA  320x200 / 256 colors");

    if (ega_available)
        banner_line("[2] or [->]  EGA  640x350 / 16 colors");

    if (!vga_available && !ega_available)
        banner_line("No supported video mode or image files!");

    banner_line("");
    banner_line("[ESC] Quit");
    printf("+-----------------------------------------+\n");
    banner_line("Viewer: [<-] Previous  [->] Next");
    banner_line("        [i] Info       [ESC] Exit");
    printf("+-----------------------------------------+\n");


    sprintf(build_info, "Build: %s %s", __DATE__, __TIME__);
    banner_line(build_info);
    printf("+-----------------------------------------+\n");

}

static void exit_prog(void)
{
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

static void images_key_loop(void)
{
    int current = 1;
    int key;

    for (;;) {
        active_viewer->load_palette(current);
        active_viewer->load_image(current);

        key = read_key();

        if (KEY_ASCII(key) == KEY_ESC)
            break;

        if (KEY_ASCII(key) == KEY_INFO) {
            if (active_viewer->show_image_info(current))
                read_key();
            else
                graphics_no_image_info(active_viewer->set_video_mode);
            continue;
        }

        switch (KEY_SCAN(key))
        {
            case SCAN_LEFT:
                current = previous_image(current);
                active_viewer->transition();
                break;

            case SCAN_RIGHT:
                current = next_image(current);
                active_viewer->transition();
                break;

            default:
                graphics_help(active_viewer->set_video_mode);
                read_key();
                break;
        }
    }
}

static const ViewerOps *viewer_from_key(int key)
{
    if ((KEY_ASCII(key) == '1' ||
         (KEY_ASCII(key) == 0 && KEY_SCAN(key) == SCAN_LEFT)) &&
        vga_available)
        return &vga_viewer_ops;

    if ((KEY_ASCII(key) == '2' ||
         (KEY_ASCII(key) == 0 && KEY_SCAN(key) == SCAN_RIGHT)) &&
        ega_available)
        return &ega_viewer_ops;

    return NULL;
}

static const ViewerOps *select_viewer(void)
{
    const ViewerOps *viewer;
    int key;

    for (;;) {
        key = read_key();
        if (KEY_ASCII(key) == KEY_ESC)
            return NULL;

        viewer = viewer_from_key(key);
        if (viewer)
            return viewer;
    }
}

int main(void)
{
    vga_available = viewer_available(&vga_viewer_ops);
    ega_available = viewer_available(&ega_viewer_ops);
    banner();

    active_viewer = select_viewer();
    if (active_viewer) {
        active_viewer->set_video_mode();
        images_key_loop();
    }

    set_txt_mode();
    exit_prog();
    return 0;
}

