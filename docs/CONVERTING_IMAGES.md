# Converting images

Put source `.jpg` files in `assets/source/`, then run:

```sh
./tools/convert_all.sh
```

Generated viewer data is written to:

- `zine/VGA/` as raw 320x200 index data plus 256-color palettes
- `zine/EGA/` as planar 640x350 16-color EGA data

The converter requires ImageMagick, Python 3, and Pillow.

Tested versions in this environment:

```text
ImageMagick 7.1.2-16 Beta Q16-HDRI, 2026-02-28
Python 3.10.12
Pillow 10.0.0
```
