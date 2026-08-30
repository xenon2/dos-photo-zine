#!/bin/sh
set -e

SCRIPT_DIR=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
ROOT_DIR=$(CDPATH='' cd -- "$SCRIPT_DIR/.." && pwd)
SRC_DIR=${1:-"$ROOT_DIR/images"}

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
