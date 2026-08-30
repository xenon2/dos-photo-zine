#!/bin/sh
set -e

SCRIPT_DIR=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
RELEASE_DIR="$SCRIPT_DIR/RELEASE"

if [ ! -f "$RELEASE_DIR/ZINE.EXE" ]; then
    printf 'error: %s not found; prepare a release first\n' \
        "$RELEASE_DIR/ZINE.EXE" >&2
    exit 1
fi

if [ -n "${DOSBOX:-}" ]; then
    dosbox_command=$DOSBOX
elif command -v dosbox-staging >/dev/null 2>&1; then
    dosbox_command=dosbox-staging
elif command -v dosbox >/dev/null 2>&1; then
    dosbox_command=dosbox
else
    printf '%s\n' 'error: DOSBox or DOSBox Staging was not found' >&2
    exit 1
fi

exec "$dosbox_command" \
    -c "mount c \"$RELEASE_DIR\"" \
    -c 'c:' \
    -c 'zine.exe'
