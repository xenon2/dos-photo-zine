#!/bin/sh
set -e

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT_DIR=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
SRC_DIR=${1:-"$ROOT_DIR/assets/source"}
OUT_DIR=${2:-"$ROOT_DIR/zine/VGA"}

W=320
H=200
SIZE=$((W * H))

echo "VGA converter: $SRC_DIR -> $OUT_DIR"
mkdir -p "$OUT_DIR"
rm -f "$OUT_DIR"/*

i=1
for f in "$SRC_DIR"/*.jpg "$SRC_DIR"/*.jpeg "$SRC_DIR"/*.JPG "$SRC_DIR"/*.JPEG; do
  [ -f "$f" ] || continue
  echo "Processing $(basename "$f") -> $i.DAT + $i.PAL"

  tmp="$OUT_DIR/$i.png"

  magick "$f" \
    -resize 320x240 \
    -gravity center \
    -background black \
    -extent 320x240 \
    -resize 320x200! \
    -dither Riemersma \
    -colors 256 \
    PNG8:"$tmp"

  "$SCRIPT_DIR/png2dat_vga.py" "$tmp" "$OUT_DIR/$i.DAT"
  "$SCRIPT_DIR/png2pal.py" "$tmp" "$OUT_DIR/$i.PAL"

  rm "$tmp"

  [ "$(wc -c < "$OUT_DIR/$i.DAT")" -eq "$SIZE" ] || exit 1
  [ "$(wc -c < "$OUT_DIR/$i.PAL")" -eq 768 ] || exit 1

  i=$((i + 1))
done

echo "Done."
