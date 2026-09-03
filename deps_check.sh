#!/bin/sh

# Verify that the programs required to build, convert, and test this project
# are available. The script exits with status 1 when a dependency is missing.

missing_count=0

print_found() {
    printf '  [OK]      %s\n' "$1"
}

print_missing() {
    printf '  [MISSING] %s\n' "$1"
    missing_count=$((missing_count + 1))
}

# Check whether at least one of the supplied command names is available.
# Usage: check_tool "Display name" command [alternative-command ...]
check_tool() {
    display_name=$1
    shift

    for command_name in "$@"; do
        if command -v "$command_name" >/dev/null 2>&1; then
            print_found "$display_name ($command_name)"
            return 0
        fi
    done

    print_missing "$display_name"
    return 1
}

check_pillow() {
    if python3 -c 'from PIL import Image' >/dev/null 2>&1; then
        print_found "Pillow Python package"
    else
        print_missing "Pillow Python package"
        printf '            Install with: python3 -m pip install Pillow\n'
    fi
}

printf 'Build dependencies\n'
printf '%s\n' '------------------'
check_tool "Make" make
check_tool "Open Watcom C compiler" wcl

printf '\nImage conversion dependencies\n'
printf '%s\n' '-----------------------------'
check_tool "ImageMagick 7" magick
if check_tool "Python 3" python3; then
    check_pillow
fi

printf '\nTesting dependencies\n'
printf '%s\n' '--------------------'
check_tool "DOSBox" \
    dosbox-staging \
    dosbox \
    DOSBox \
    dosbox-staging.exe \
    dosbox.exe

printf '\n'
if [ "$missing_count" -eq 0 ]; then
    printf 'All project dependencies are available.\n'
    exit 0
fi

printf 'Missing dependencies: %s\n' "$missing_count"
exit 1
