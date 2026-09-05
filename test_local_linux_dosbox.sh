#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)

if [ ! -f "$ROOT_DIR/ZINE.EXE" ]; then
    printf 'error: %s not found; restore it or run ./build_dos-zine_bin.sh\n' \
        "$ROOT_DIR/ZINE.EXE" >&2
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

printf '%s\n' 'Testing the canonical local executable: ZINE.EXE'
exec "$dosbox_command" \
    -c "mount c \"$ROOT_DIR\"" \
    -c 'c:' \
    -c 'zine.exe'
