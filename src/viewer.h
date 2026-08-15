#ifndef VIEWER_H
#define VIEWER_H

typedef struct ViewerOps {
    void (*set_video_mode)(void);
    void (*load_palette)(int index);
    void (*load_image)(int index);
    int  (*image_exists)(int index);
    void (*transition)(void);
    void (*show_image_info)(int index);
    int  (*hardware_available)(void);
} ViewerOps;

#endif /* VIEWER_H */
