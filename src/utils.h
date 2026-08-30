#ifndef UTILS_H
#define UTILS_H

#define KEY_ASCII(key) (((key) & 0xFF))
#define KEY_SCAN(key)  (((key) >> 8) & 0xFF)

#define KEY_ESC    27
#define SCAN_LEFT  0x4B
#define SCAN_RIGHT 0x4D

void graphics_help(void (*reset_video_mode)(void));
int graphics_image_info(void (*reset_video_mode)(void), int index);
void graphics_no_image_info(void (*reset_video_mode)(void));
void sleep_nseconds(unsigned int nseconds);
void clear_keyboard_buffer(void);
int read_key_nb(void);
int read_key(void);

#endif /* UTILS_H */
