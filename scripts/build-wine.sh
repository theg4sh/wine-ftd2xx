#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
BUILD_DIR=${BUILD_DIR:-"$ROOT/build/wine"}
PREFIX=${PREFIX:-"$ROOT/build/install"}
JOBS=${JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf '2')}

"$ROOT/scripts/prepare-wine.sh"
mkdir -p "$BUILD_DIR" "$PREFIX"

if [[ ! -f "$BUILD_DIR/Makefile" ]]; then
    read -r -a extra_args <<< "${EXTRA_CONFIGURE_ARGS:-}"
    (
        cd "$BUILD_DIR"
        "$ROOT/wine/configure" \
            --prefix="$PREFIX" \
            --enable-archs=x86_64,i386 \
            "${extra_args[@]}" \
            --with-pthread \
            > configure.log 2>&1
    ) || {
        cat "$BUILD_DIR/configure.log"
        exit 1
    }
fi

make -C "$BUILD_DIR" -j"$JOBS"
printf '\nBuilt FTD2XX artifacts:\n'
find "$BUILD_DIR/dlls/ftd2xx" -maxdepth 3 -type f \( -name 'ftd2xx.dll' -o -name 'ftd2xx.so' \) -print
