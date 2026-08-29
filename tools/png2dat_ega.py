#!/usr/bin/env python3
from PIL import Image
import sys

if len(sys.argv) != 3:
    print("usage: png2dat_ega.py input.png output.dat")
    sys.exit(1)

png, outdat = sys.argv[1], sys.argv[2]

img = Image.open(png)

if img.mode != "P":
    print("ERROR: PNG must be palettized (mode P)")
    sys.exit(1)

w, h = img.size
if w != 640 or h != 350:
    print("ERROR: expected 640x350")
    sys.exit(1)

pixels = img.tobytes()

# --- read palette ---
palette = img.getpalette()
if palette is None:
    print("ERROR: PNG has no palette")
    sys.exit(1)

# --- canonical EGA palette ---
EGA_PALETTE = [
    (0,   0,   0),      # 0
    (0,   0,   170),    # 1
    (0,   170, 0),      # 2
    (0,   170, 170),    # 3
    (170, 0,   0),      # 4
    (170, 0,   170),    # 5
    (170, 85,  0),      # 6
    (170, 170, 170),    # 7
    (85,  85,  85),     # 8
    (85,  85,  255),    # 9
    (85,  255, 85),     # 10
    (85,  255, 255),    # 11
    (255, 85,  85),     # 12
    (255, 85,  255),    # 13
    (255, 255, 85),    # 14
    (255, 255, 255),   # 15
]

# --- build RGB -> EGA index map ---
rgb_to_ega = {}
for i in range(256):
    rgb = tuple(palette[i*3:i*3+3])
    if rgb in EGA_PALETTE:
        rgb_to_ega[i] = EGA_PALETTE.index(rgb)

if not rgb_to_ega:
    print("ERROR: no EGA colors found in PNG palette")
    sys.exit(1)

# Packed 4bpp: high nibble is the left pixel, low nibble is the right pixel.
def write_packed(outdat):
    out = bytearray((w * h) // 2)

    i = 0
    for y in range(h):
        for x in range(0, w, 2):
            p0 = pixels[y*w + x]
            p1 = pixels[y*w + x + 1]

            if p0 not in rgb_to_ega or p1 not in rgb_to_ega:
                print("ERROR: non-EGA color used")
                sys.exit(1)

            ega0 = rgb_to_ega[p0]
            ega1 = rgb_to_ega[p1]

            out[i] = (ega0 << 4) | ega1
            i += 1

    with open(outdat, "wb") as f:
        f.write(out)

write_packed(outdat)
