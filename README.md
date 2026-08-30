# DOS Photo Zine

A tiny DOS photo zine / image viewer made for fun. It supports:

- VGA mode 13h: 320x200, 256 colors
- EGA mode 10h: 640x350, 16 colors, planar data
- Tiny streaming LZSS image decompressor (4 KiB window)

The program auto-detects available image data and video hardware, then lets you pick VGA or EGA from the title screen.
I've created it to see how my modern images could look in retro aesthetics. Best effect is in EGA mode.
Sadly I do not have access to real EGA hardware so I can only enjoy the effect under DosBox or on retro VGA laptop.
Program was tested on real Pentium 133/SVGA retro laptop.
Supplied images licensed as Creative Commons.

## Controls

Title screen:

- Left arrow or `1` - VGA viewer
- Right arrow or `2` - EGA viewer
- Esc - quit

Viewer:

- Left / Right arrows - previous / next image
- `i` - show image info/caption, when available
- Esc - return to DOS text mode and quit

## Repository layout

```text
src/            DOS C source code
tools/          image conversion scripts
assets/source/  source photos used by the converters
zine/           generated runtime image data used by the DOS viewer
dosbox/         DOSBox helper configs/scripts
docs/           extra project notes
```

Runtime data is expected at:

```text
zine/VGA/*.DAT
zine/VGA/*.PAL
zine/EGA/*.DAT
zine/EGA/*.txt
```

## Requirements

Build:

- Open Watcom C
- `make`

Image conversion:

- ImageMagick 7+
- Python 3
- Pillow

Testing:

- DOSBox or DOSBox Staging

Tested tool versions in this environment:

- Open Watcom `wcl`: 2.0 beta, Dec 8 2023 19:09:05, 64-bit
- ImageMagick: 7.1.2-16 Beta Q16-HDRI, 2026-02-28
- Python: 3.10.12
- Pillow: 10.0.0

## Build

```sh
WATCOM=/path/to/open-watcom ./build.sh
```

If your Open Watcom environment already defines `WATCOM`, `./build.sh` is
enough. Alternatively, with the environment configured manually:

```sh
make
```

## Convert images

Put `.jpg` images into `assets/source/`, then run:

```sh
./tools/convert_all.sh
```

This regenerates `zine/EGA/` and `zine/VGA/`.

## Prepare release package

```sh
./release.sh
```

This creates:

- `RELEASE/`
- `dos-photo-zine.zip`

## License

Public domain / Unlicense. See `LICENSE`.
