#!/bin/sh
set -eu

# WATCOM is supplied by the user so the installation can live anywhere.
if [ -z "${WATCOM:-}" ]; then
    printf '%s\n' 'error: set WATCOM to the Open Watcom installation directory' >&2
    printf '%s\n' 'example: WATCOM=/opt/watcom ./build_dos-zine_bin.sh' >&2
    exit 1
fi

if [ ! -d "$WATCOM/h" ] || [ ! -d "$WATCOM/lib286/dos" ]; then
    printf 'error: %s does not look like an Open Watcom installation\n' "$WATCOM" >&2
    exit 1
fi

# Respect an already configured compiler; otherwise select the Linux host binary.
if ! command -v wcl >/dev/null 2>&1; then
    case "$(uname -s):$(uname -m)" in
        Linux:x86_64|Linux:amd64)
            watcom_bin=binl64
            ;;
        Linux:i?86)
            watcom_bin=binl
            ;;
        *)
            printf '%s\n' 'error: wcl is not on PATH for this host platform' >&2
            exit 1
            ;;
    esac

    if [ ! -x "$WATCOM/$watcom_bin/wcl" ]; then
        printf 'error: compiler not found: %s/%s/wcl\n' "$WATCOM" "$watcom_bin" >&2
        exit 1
    fi
    PATH="$WATCOM/$watcom_bin:$PATH"
fi

INCLUDE="$WATCOM/h"
# lib286 contains Watcom's 16-bit runtime used by 386 real-mode (-3 -ms) code.
LIB="$WATCOM/lib286;$WATCOM/lib286/dos"
export WATCOM PATH INCLUDE LIB

# Allow invocation outside the repository and pass Make overrides through.
cd "$(dirname "$0")"
MAKE=${MAKE:-make}
"$MAKE" clean
# Rebuild even when the shipped executable is newer than the checked-out sources.
"$MAKE" -B "$@"
printf '%s\n' 'Program built: ZINE.EXE (the canonical local test executable)'
