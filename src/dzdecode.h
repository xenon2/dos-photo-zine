#ifndef DZDECODE_H
#define DZDECODE_H

/* Called with consecutive pieces of the unpacked image. Return nonzero on success. */
typedef int (*dz_writer)(unsigned long offset, const unsigned char *data,
                         unsigned int length);

/* Decode a DZ1 stream, or copy a legacy raw DAT. */
int dz_decode_file(int fd, unsigned long expected_size, dz_writer writer);

#endif
