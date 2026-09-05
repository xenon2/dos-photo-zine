#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
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

if ask_yes_no "Include EGA files in the package?"; then
    include_ega=1
fi
if ask_yes_no "Include VGA files in the package?"; then
    include_vga=1
fi

if [ "$include_ega" -eq 0 ] && [ "$include_vga" -eq 0 ]; then
    echo "Nothing selected; packaging cancelled."
    exit 1
fi

# This is a packaging helper only: it uses ZINE.EXE and existing image data.
if [ "$include_ega" -eq 1 ] && [ "$include_vga" -eq 1 ]; then
    exec ./package.sh
elif [ "$include_ega" -eq 1 ]; then
    exec ./package.sh --ega-only
else
    exec ./package.sh --vga-only
fi
