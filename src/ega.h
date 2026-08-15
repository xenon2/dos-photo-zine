#ifndef EGA_H
#define EGA_H

#include "viewer.h"

void load_ega_dat(int index);
int image_ega_exists(int index);
void load_ega_palette(int index);
void transition_ega(void);
void set_ega_mode(void);
int is_ega(void);
void image_info_ega(int index);

extern const ViewerOps ega_viewer_ops;

#endif /* EGA_H */
