#!/usr/bin/env python3
# Extract PNG palette (PLTE) to raw RGB palette file (768 bytes, 256*3).

from PIL import Image
import sys

if len(sys.argv) != 3:
    print("usage: png2pal.py input.png output.pal")
    sys.exit(1)

png, pal_out = sys.argv[1], sys.argv[2]
img = Image.open(png)

if img.mode != "P":
    print("ERROR: input PNG must be palettized (mode 'P')")
    sys.exit(1)

pal = img.getpalette()  # list of ints in RGB order, length may be < 768
if pal is None:
    print("ERROR: PNG has no palette")
    sys.exit(1)

# Ensure exactly 768 bytes (pad with zeros if shorter)
pal = pal[:768]
if len(pal) < 768:
    pal = pal + [0] * (768 - len(pal))

with open(pal_out, "wb") as f:
    f.write(bytes(pal))
