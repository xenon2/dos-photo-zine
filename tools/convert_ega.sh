#!/bin/sh
set -e

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT_DIR=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
SRC_DIR=${1:-"$ROOT_DIR/assets/source"}
OUT_DIR=${2:-"$ROOT_DIR/zine/EGA"}
TMP_PAL="$OUT_DIR/ega16.png"

W=640
H=350
SIZE=$((80 * H * 4))

echo "EGA converter: $SRC_DIR -> $OUT_DIR"
mkdir -p "$OUT_DIR"
rm -f "$OUT_DIR"/*

magick -size 16x1 xc:black \
  -fill "rgb(0,0,170)"       -draw "point 1,0" \
  -fill "rgb(0,170,0)"       -draw "point 2,0" \
  -fill "rgb(0,170,170)"     -draw "point 3,0" \
  -fill "rgb(170,0,0)"       -draw "point 4,0" \
  -fill "rgb(170,0,170)"     -draw "point 5,0" \
  -fill "rgb(170,85,0)"      -draw "point 6,0" \
  -fill "rgb(170,170,170)"   -draw "point 7,0" \
  -fill "rgb(85,85,85)"      -draw "point 8,0" \
  -fill "rgb(85,85,255)"     -draw "point 9,0" \
  -fill "rgb(85,255,85)"     -draw "point 10,0" \
  -fill "rgb(85,255,255)"    -draw "point 11,0" \
  -fill "rgb(255,85,85)"     -draw "point 12,0" \
  -fill "rgb(255,85,255)"    -draw "point 13,0" \
  -fill "rgb(255,255,85)"    -draw "point 14,0" \
  -fill "rgb(255,255,255)"   -draw "point 15,0" \
  PNG8:"$TMP_PAL"

i=1
for f in "$SRC_DIR"/*.jpg "$SRC_DIR"/*.jpeg "$SRC_DIR"/*.JPG "$SRC_DIR"/*.JPEG; do
  [ -f "$f" ] || continue
  echo "Processing $(basename "$f") -> $i.DAT"

  tmp24="$OUT_DIR/tmp_$i.png"
  tmp8="$OUT_DIR/$i.png"

  magick "$f" \
    -resize 640x480 \
    -gravity center \
    -background black \
    -extent 640x480 \
    -resize 640x350! \
    -dither FloydSteinberg \
    -remap "$TMP_PAL" \
    PNG24:"$tmp24"

  magick "$tmp24" \
    -type Palette \
    -dither None \
    -remap "$TMP_PAL" \
    PNG8:"$tmp8"

  "$SCRIPT_DIR/png2dat_ega.py" "$tmp8" "$OUT_DIR/$i.DAT"

  rm "$tmp8" "$tmp24"

  [ "$(wc -c < "$OUT_DIR/$i.DAT")" -eq "$SIZE" ] || exit 1

  i=$((i + 1))
done

rm -f "$TMP_PAL"
echo "Done."
