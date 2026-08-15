#!/bin/sh
set -e


rm -rf RELEASE dos-photo-zine.zip
mkdir -p RELEASE/ZINE/EGA RELEASE/ZINE/VGA

sh ./build.sh
sh ./tools/convert_all.sh


cp main.exe RELEASE/ZINE.EXE
cp zine/EGA/* RELEASE/ZINE/EGA/
cp zine/VGA/* RELEASE/ZINE/VGA/
cp README.md RELEASE/README.TXT
[ -f LICENSE ] && cp LICENSE RELEASE/

( cd RELEASE && zip -qr ../dos-photo-zine.zip . )

echo "Release prepared in RELEASE/ and dos-photo-zine.zip"
