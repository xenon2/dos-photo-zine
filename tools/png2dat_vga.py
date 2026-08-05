#!/usr/bin/env python3
# Extract raw palette indices from an indexed PNG (mode 'P') into a 64000-byte DAT.

from PIL import Image
import sys

if len(sys.argv) != 3:
    print("usage: png2dat.py input.png output.dat")
    sys.exit(1)

png, dat = sys.argv[1], sys.argv[2]
img = Image.open(png)

if img.mode != "P":
    print("ERROR: input PNG must be palettized (mode 'P')")
    sys.exit(1)

data = img.tobytes()

with open(dat, "wb") as f:
    f.write(data)

w, h = img.size
if len(data) != w * h:
    print("ERROR: size mismatch")
    sys.exit(1)
