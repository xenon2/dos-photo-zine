#!/bin/sh
set -e

SCRIPT_DIR=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
ROOT_DIR=$(CDPATH='' cd -- "$SCRIPT_DIR/.." && pwd)
SRC_DIR=${1:-"$ROOT_DIR/images"}
CAPTION_DIR="$ROOT_DIR/zine"

has_images=false
for file in "$SRC_DIR"/*.jpg "$SRC_DIR"/*.jpeg "$SRC_DIR"/*.JPG "$SRC_DIR"/*.JPEG; do
    if [ -f "$file" ]; then
        has_images=true
        break
    fi
done
if [ "$has_images" = false ]; then
    printf 'warning: no JPEG images found in %s; no photos will be converted\n' \
        "$SRC_DIR" >&2
fi

"$SCRIPT_DIR/convert_ega.sh" "$@"
"$SCRIPT_DIR/convert_vga.sh" "$@"

# ZINE.EXE looks for captions as zine/<image number>.txt. Keep caption
# numbering in lockstep with the image ordering used by both converters.
mkdir -p "$CAPTION_DIR"
for file in "$CAPTION_DIR"/[0-9]*.txt "$CAPTION_DIR"/[0-9]*.TXT; do
    [ -f "$file" ] || continue
    rm -f "$file"
done

i=1
for image in "$SRC_DIR"/*.jpg "$SRC_DIR"/*.jpeg "$SRC_DIR"/*.JPG "$SRC_DIR"/*.JPEG; do
    [ -f "$image" ] || continue

    caption=${image%.*}.txt
    if [ ! -f "$caption" ]; then
        caption=${image%.*}.TXT
    fi
    if [ -f "$caption" ]; then
        cp "$caption" "$CAPTION_DIR/$i.txt"
        printf 'Caption %s -> %s.txt\n' "$(basename "$caption")" "$i"
    fi

    i=$((i + 1))
done
