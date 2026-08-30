#!/bin/sh
set -e

SCRIPT_DIR=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
cd "$SCRIPT_DIR"

# build.sh writes the intermediate executable here; never leave it behind.
trap 'rm -f main.exe' 0

rm -rf RELEASE dos-photo-zine.zip
mkdir -p RELEASE/ZINE/EGA RELEASE/ZINE/VGA

sh ./build.sh
sh ./tools/convert_all.sh


cp main.exe RELEASE/ZINE.EXE
for file in zine/EGA/*; do
    [ -f "$file" ] || continue
    cp "$file" RELEASE/ZINE/EGA/
done
for file in zine/VGA/*; do
    [ -f "$file" ] || continue
    cp "$file" RELEASE/ZINE/VGA/
done
cp README.md RELEASE/README.TXT
[ -f LICENSE ] && cp LICENSE RELEASE/

( cd RELEASE && zip -qr ../dos-photo-zine.zip . )

echo "Release prepared in RELEASE/ and dos-photo-zine.zip"
