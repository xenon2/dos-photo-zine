#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
cd "$SCRIPT_DIR"

include_ega=1
include_vga=1

usage()
{
    cat <<EOF
Usage: $0 [--ega-only | --vga-only]

Package the canonical ./ZINE.EXE and already-converted files in ./zine.
This command does not compile the program or convert images.
EOF
}

while [ "$#" -gt 0 ]; do
    case "$1" in
        --ega-only)
            include_ega=1
            include_vga=0
            ;;
        --vga-only)
            include_ega=0
            include_vga=1
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            printf 'error: unknown option: %s\n' "$1" >&2
            usage >&2
            exit 1
            ;;
    esac
    shift
done

if [ ! -f ZINE.EXE ]; then
    printf '%s\n' 'error: ZINE.EXE not found; restore the supplied binary or run ./build_dos-zine_bin.sh' >&2
    exit 1
fi

if [ "$include_ega" -eq 1 ] && ! find zine/EGA -type f -name '*.DAT' -print -quit 2>/dev/null | grep -q .; then
    printf '%s\n' 'error: no converted EGA images; run ./convert_images.sh first' >&2
    exit 1
fi
if [ "$include_vga" -eq 1 ] && ! find zine/VGA -type f -name '*.DAT' -print -quit 2>/dev/null | grep -q .; then
    printf '%s\n' 'error: no converted VGA images; run ./convert_images.sh first' >&2
    exit 1
fi

stage_dir=$(mktemp -d "${TMPDIR:-/tmp}/dos-photo-zine.XXXXXX")
trap 'rm -rf "$stage_dir"' EXIT HUP INT TERM

cp ZINE.EXE "$stage_dir/ZINE.EXE"
mkdir -p "$stage_dir/ZINE"

if [ "$include_ega" -eq 1 ]; then
    mkdir -p "$stage_dir/ZINE/EGA"
    cp zine/EGA/*.DAT "$stage_dir/ZINE/EGA/"
fi
if [ "$include_vga" -eq 1 ]; then
    mkdir -p "$stage_dir/ZINE/VGA"
    cp zine/VGA/*.DAT zine/VGA/*.PAL "$stage_dir/ZINE/VGA/"
fi
for file in zine/*.txt; do
    [ -f "$file" ] || continue
    cp "$file" "$stage_dir/ZINE/"
done
cp README.md "$stage_dir/README.TXT"
[ ! -f LICENSE ] || cp LICENSE "$stage_dir/"

rm -f dos-photo-zine.zip
( cd "$stage_dir" && zip -qr "$SCRIPT_DIR/dos-photo-zine.zip" . )
printf '%s\n' 'Package created: dos-photo-zine.zip'
