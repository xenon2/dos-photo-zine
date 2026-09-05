# DOS Photo Zine

A tiny DOS photo zine and image viewer supporting:

- VGA mode 13h: 320x200, 256 colors
- EGA mode 10h: 640x350, 16 colors, planar data
- Streaming LZSS image decompression with a 4 KiB window

The viewer detects the available image data and video hardware, then lets you
choose VGA or EGA mode. The minimum CPU target is an Intel 80286. It requires
an EGA card with at least 128 KiB of video memory, or a VGA/MCGA-compatible
card. It has been tested with DOSBox and on a real Pentium 133/SVGA laptop.

## You do not need to compile the DOS program

The repository includes a ready-to-use `ZINE.EXE`. You only need Open Watcom
if you want to rebuild that executable from the C source.

To create a zine with your own images, follow the quick-start instructions
below.

## Quick start: create your own zine

Image conversion requires:

- ImageMagick 7+
- Python 3
- Pillow

### 1. Add images

Place JPEG images in the `images/` directory. Files are displayed in filename
order. To add a caption, place a `.txt` file with the same base name beside its
image (for example, `photo.jpg` and `photo.txt`).

### 2. Convert images

```sh
./convert_images.sh
```

This creates the DOS image data in `zine/EGA/` and `zine/VGA/`. Matching
caption files are copied to `zine/<image number>.txt` for `ZINE.EXE` to read.
To customize the startup banner, add `images/info.txt`; multiline text is
preserved and long lines are wrapped inside the title-screen box. It does
not compile or replace `ZINE.EXE`.

### 3. Test locally

On Linux:

```sh
./test_local_linux_dosbox.sh
```

On Windows:

```bat
test_local_windows_dosbox.bat
```

The test scripts always run the root `ZINE.EXE`, so there is no ambiguity about
which executable is being tested. DOSBox or DOSBox Staging is required.

### 4. Create the ZIP package

```sh
./package.sh
```

This packages the existing `ZINE.EXE` and converted image data as
`dos-photo-zine.zip`. Packaging does not compile the program or reconvert
images, and its temporary staging directory is removed automatically.

To package only one graphics mode, use:

```sh
./package.sh --ega-only
./package.sh --vga-only
```

## Optional: rebuild ZINE.EXE

Rebuilding the DOS viewer requires Open Watcom C and `make`. Set `WATCOM` to
your Open Watcom installation and run:

```sh
WATCOM=/path/to/open-watcom ./build_dos-zine_bin.sh
```

If your Open Watcom environment is already configured:

```sh
./build_dos-zine_bin.sh
```

A successful build replaces the root `ZINE.EXE`. Local testing and packaging
will therefore use the newly built program. See
[docs/BUILDING.md](docs/BUILDING.md) for compiler options.

Maintainers can rebuild the executable, convert all images, and package them in
one operation:

```sh
./release.sh
```

## Controls

Title screen:

- Left arrow or `1` — VGA viewer
- Right arrow or `2` — EGA viewer
- Esc — quit

Viewer:

- Left/Right arrows — previous/next image
- `i` — show image information or caption, when available
- Any other key — show the graphics-mode help screen
- Esc — return to DOS text mode and quit

## Repository layout

```text
ZINE.EXE                    supplied or locally built DOS viewer
src/                        DOS C source code
tools/                      image conversion tools
images/                     source photographs
zine/                       generated DOS image data
convert_images.sh           image conversion entry point
build_dos-zine_bin.sh       optional Open Watcom program build
package.sh                  ZIP packaging without compilation
docs/                       additional project documentation
```

Runtime image data is stored as:

```text
zine/VGA/*.DAT
zine/VGA/*.PAL
zine/EGA/*.DAT
zine/*.txt
```

See [docs/CONVERTING_IMAGES.md](docs/CONVERTING_IMAGES.md) for details about
the image format and compression.

## Tested tool versions

- Open Watcom `wcl`: 2.0 beta, Dec 8 2023 19:09:05, 64-bit
- ImageMagick: 7.1.2-16 Beta Q16-HDRI, 2026-02-28
- Python: 3.10.12
- Pillow: 10.0.0

Supplied images are licensed under Creative Commons.

## License

The program and source code are public domain under the Unlicense. See
[LICENSE](LICENSE).
