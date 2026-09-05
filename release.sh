#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
cd "$SCRIPT_DIR"

# Maintainer workflow: rebuild the canonical executable, regenerate all image
# data, and package exactly those files.
./build_dos-zine_bin.sh
./convert_images.sh
./package.sh
