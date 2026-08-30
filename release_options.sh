#!/bin/sh
set -e

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$SCRIPT_DIR"

ask_yes_no()
{
    while :; do
        printf '%s [y/n]: ' "$1"
        IFS= read -r answer
        case "$answer" in
            y|Y|yes|YES|Yes) return 0 ;;
            n|N|no|NO|No)    return 1 ;;
            *) echo "Please answer y or n." ;;
        esac
    done
}

include_ega=0
include_vga=0

if ask_yes_no "Include EGA files in the release?"; then
    include_ega=1
fi

if ask_yes_no "Include VGA files in the release?"; then
    include_vga=1
fi

if [ "$include_ega" -eq 0 ] && [ "$include_vga" -eq 0 ]; then
    echo "Nothing selected; release cancelled."
    exit 1
fi

rm -rf RELEASE dos-photo-zine.zip
mkdir -p RELEASE/ZINE

sh ./build.sh

if [ "$include_ega" -eq 1 ]; then
    mkdir -p RELEASE/ZINE/EGA
    sh ./tools/convert_ega.sh
    cp zine/EGA/* RELEASE/ZINE/EGA/
fi

if [ "$include_vga" -eq 1 ]; then
    mkdir -p RELEASE/ZINE/VGA
    sh ./tools/convert_vga.sh
    cp zine/VGA/* RELEASE/ZINE/VGA/
fi

for file in zine/*.txt; do
    [ -f "$file" ] || continue
    cp "$file" RELEASE/ZINE/
done

cp main.exe RELEASE/ZINE.EXE
cp README.md RELEASE/README.TXT
[ -f LICENSE ] && cp LICENSE RELEASE/

( cd RELEASE && zip -qr ../dos-photo-zine.zip . )

echo "Release prepared in RELEASE/ and dos-photo-zine.zip"
