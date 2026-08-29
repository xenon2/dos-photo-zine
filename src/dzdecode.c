#include <io.h>
#include <fcntl.h>
#include <string.h>

#include "dzdecode.h"

#define WINDOW_SIZE 4096
#define INPUT_SIZE 4096
#define OUT_SIZE 4096

typedef struct input_buffer {
    int fd;
    unsigned int at;
    unsigned int size;
    unsigned char data[INPUT_SIZE];
} input_buffer;

static int get_byte(input_buffer *in)
{
    if (in->at == in->size) {
        int count = read(in->fd, in->data, sizeof(in->data));
        if (count <= 0) return -1;
        in->at = 0;
        in->size = (unsigned int)count;
    }
    return in->data[in->at++];
}

static unsigned long read_u32(const unsigned char *p)
{
    return (unsigned long)p[0] | ((unsigned long)p[1] << 8) |
           ((unsigned long)p[2] << 16) | ((unsigned long)p[3] << 24);
}

static int flush_output(dz_writer writer, unsigned long *written,
                        unsigned char *out, unsigned int *used)
{
    if (*used && !writer(*written, out, *used)) return 0;
    *written += *used;
    *used = 0;
    return 1;
}

int dz_decode_file(int fd, unsigned long expected_size, dz_writer writer)
{
    unsigned char header[8];
    static unsigned char window[WINDOW_SIZE];
    static unsigned char out[OUT_SIZE];
    static input_buffer in;
    unsigned long written = 0;
    unsigned long unpacked, remaining;
    unsigned int win_at = 0, used = 0;
    int flags = 0, bits = 0;
    int a, b, value;

    if (read(fd, header, sizeof(header)) != sizeof(header)) return 0;

    if (memcmp(header, "DZ1\0", 4) != 0) {
        if (lseek(fd, 0L, SEEK_SET) < 0) return 0;
        while (written < expected_size) {
            unsigned int want = (expected_size - written > OUT_SIZE) ?
                                OUT_SIZE : (unsigned int)(expected_size - written);
            int count = read(fd, out, want);
            if (count != (int)want || !writer(written, out, want)) return 0;
            written += want;
        }
        return 1;
    }

    unpacked = read_u32(header + 4);
    if (unpacked != expected_size) return 0;

    memset(window, 0, sizeof(window));
    in.fd = fd;
    in.at = in.size = 0;
    /* Keep 32-bit accounting out of the match byte loop on a 16-bit CPU. */
    remaining = unpacked;

    while (remaining != 0) {
        unsigned int distance, length;

        if (bits == 0) {
            flags = get_byte(&in);
            if (flags < 0) return 0;
            bits = 8;
        }

        if (flags & 1) {
            value = get_byte(&in);
            if (value < 0) return 0;

            window[win_at] = (unsigned char)value;
            win_at = (win_at + 1) & (WINDOW_SIZE - 1);
            out[used++] = (unsigned char)value;
            remaining--;

            if (used == OUT_SIZE && !flush_output(writer, &written, out, &used))
                return 0;
        } else {
            unsigned int copy_at, available, chunk, i;

            a = get_byte(&in);
            b = get_byte(&in);
            if (a < 0 || b < 0) return 0;
            distance = ((unsigned int)a | ((unsigned int)(b & 15) << 8)) + 1;
            length = ((unsigned int)b >> 4) + 3;
            if (remaining < length) length = (unsigned int)remaining;
            copy_at = (win_at - distance) & (WINDOW_SIZE - 1);

            /* Copy to the next output boundary without testing it per byte. */
            while (length != 0) {
                available = OUT_SIZE - used;
                chunk = length < available ? length : available;

                for (i = 0; i < chunk; i++) {
                    unsigned char byte = window[copy_at];
                    copy_at = (copy_at + 1) & (WINDOW_SIZE - 1);
                    window[win_at] = byte;
                    win_at = (win_at + 1) & (WINDOW_SIZE - 1);
                    out[used + i] = byte;
                }

                used += chunk;
                remaining -= chunk;
                length -= chunk;
                if (used == OUT_SIZE &&
                    !flush_output(writer, &written, out, &used)) return 0;
            }
        }

        flags >>= 1;
        bits--;
    }

    return flush_output(writer, &written, out, &used) && written == unpacked;
}
