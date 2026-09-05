# Converting images

Put source `.jpg` files in `images/`. An image can have an optional caption in
a `.txt` file with the same base name (for example, `photo.jpg` and
`photo.txt`). Then run:

```sh
./convert_images.sh
```

This does not invoke Open Watcom or replace `ZINE.EXE`. Generated viewer data
is written to:

- `zine/VGA/` as compressed 320x200 index data plus 256-color palettes
- `zine/EGA/` as compressed packed 4bpp 640x350 16-color EGA data
- `zine/<image number>.txt` for captions matched to source images

## DAT compression

`tools/dzcompress.py` implements a small LZSS codec using only the Python 3
standard library. Its 4 KiB history window and 3--18 byte matches keep the DOS
decoder (`src/dzdecode.c`) small. It uses about 5 KiB of static workspace.
Decoding is streamed in 512-byte blocks directly to VGA memory, so no
complete 64/112 KiB image buffer is needed.
The EGA stream stores two 4-bit pixels per byte. During decoding, each
512-byte block is transposed into four small plane buffers and copied directly
to EGA video memory.

The DAT layout is:

```text
"DZ1\\0" | uint32 little-endian unpacked size | flag groups
```

Each flag controls eight items, least-significant bit first. A set bit is one
literal byte. A clear bit is a two-byte code: 12 bits of `distance - 1` and
4 bits of `length - 3`. Distances are 1--4096 and lengths are 3--18.

The conversion scripts compress DAT files automatically. To compress one raw
file on Linux without Pillow or other external libraries:

```sh
python3 tools/dzcompress.py image.raw image.DAT
```

If compression would enlarge the file, the tool writes it raw instead. The
DOS loader accepts both DZ1 and legacy raw DAT files.

Image conversion requires ImageMagick, Python 3, and Pillow. The compression
step itself requires only Python 3.

Tested versions in this environment:

```text
ImageMagick 7.1.2-16 Beta Q16-HDRI, 2026-02-28
Python 3.10.12
Pillow 10.0.0
```
