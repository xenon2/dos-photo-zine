# DOS Photo Zine

A DOS photo slideshow and image viewer with two display modes:

- VGA mode 13h: 320×200, 256 colors
- EGA mode 10h: 640×350, 16 colors

The viewer detects the available image data and video hardware, then allows the
user to select VGA or EGA mode. It targets Intel 286 or newer CPUs and requires
one of the following:

- An EGA card with at least 128 KiB of video memory
- A VGA/MCGA-compatible card

It has been tested with DOSBox and a Pentium 133 laptop with SVGA graphics.

## Create a zine

The repository includes a ready-to-use `ZINE.EXE`. Open Watcom is required only
to rebuild the executable.

### Requirements

- ImageMagick 7 or newer
- Python 3
- Pillow
- DOSBox or DOSBox Staging for local testing

Check the conversion dependencies with:

```sh
./deps_check.sh
```

### 1. Add images

Place JPEG images in `images/`. Images are displayed in filename order.

To add a caption, place a text file with the same base name next to the image:

```text
photo.jpg
photo.txt
```

To customize the startup banner, edit `images/info.txt`. Multiline text is
preserved, and long lines are wrapped to fit the title screen.

### 2. Convert images

```sh
./convert_images.sh
```

This creates image data in `zine/EGA/` and `zine/VGA/`. Captions are copied to
`zine/<image number>.txt`. The script does not compile or replace `ZINE.EXE`.

### 3. Test locally

Linux:

```sh
./test_local_linux_dosbox.sh
```

Windows:

```bat
test_local_windows_dosbox.bat
```

The scripts run the root `ZINE.EXE`.

### 4. Create a package

```sh
./package.sh
```

This creates `dos-photo-zine.zip` from the existing `ZINE.EXE` and converted
image data. It does not compile the program or convert images. The resulting
files are self-contained and can be copied to a floppy disk.

To include only one graphics mode:

```sh
./package.sh --ega-only
./package.sh --vga-only
```

## Rebuild ZINE.EXE

Rebuilding requires Open Watcom C and `make`. Set `WATCOM` to the Open Watcom
installation path:

```sh
WATCOM=/path/to/open-watcom ./build_dos-zine_bin.sh
```

If the Open Watcom environment is already configured:

```sh
./build_dos-zine_bin.sh
```

A successful build replaces the root `ZINE.EXE`. See
[docs/BUILDING.md](docs/BUILDING.md) for compiler options.

To rebuild the executable, convert all images, and create the package in one
step:

```sh
./release.sh
```

## Controls

### Title screen

- Left arrow or `1`: start the VGA viewer
- Right arrow or `2`: start the EGA viewer
- Esc: quit

### Viewer

- Left/Right arrows: previous/next image
- `i`: show the image caption or information, if available
- Any other key: show graphics-mode help
- Esc: restore DOS text mode and quit

## Repository layout

```text
ZINE.EXE                    DOS viewer executable
src/                        DOS C source code
tools/                      image conversion tools
images/                     source photographs
zine/                       generated DOS image data
convert_images.sh           image conversion entry point
build_dos-zine_bin.sh       Open Watcom build script
package.sh                  ZIP packaging script
docs/                       additional documentation
```

Generated image data:

```text
zine/VGA/*.DAT
zine/VGA/*.PAL
zine/EGA/*.DAT
zine/*.txt
```

See [docs/CONVERTING_IMAGES.md](docs/CONVERTING_IMAGES.md) for image format and
compression details.

## Tested versions

- Open Watcom `wcl`: 2.0 beta, Dec 8 2023 19:09:05, 64-bit
- ImageMagick: 7.1.2-16 Beta Q16-HDRI, 2026-02-28
- Python: 3.10.12
- Pillow: 10.0.0
- WSL and Ubuntu 22.04.5 LTS

The supplied images use Creative Commons licenses.

## License

The program and source code are released under the Unlicense. See
[LICENSE](LICENSE).
