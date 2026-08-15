#ifndef VGA_H
#define VGA_H

#include "viewer.h"

/* VGA image handling */

void load_vga_palette(int index);
void load_vga_dat(int index);
int  image_vga_exists(int index);

void set_vga_mode(void);
void transition_vga(void);

int is_vga(void);

void image_info_vga(int index);

extern const ViewerOps vga_viewer_ops;

#endif /* VGA_H */
