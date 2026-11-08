#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
PREFIX=${PREFIX:-/opt/wine-ftd2xx}
DESTDIR=${DESTDIR:-}
BUILD_DIR=${BUILD_DIR:-"$ROOT/build/wine"}
JOBS=${JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf '2')}

if [[ ! -f "$BUILD_DIR/Makefile" || ! -x "$BUILD_DIR/wine" ]]; then
    printf 'Wine has not been built in %s; run %s/build.sh first.\n' "$BUILD_DIR" "$ROOT" >&2
    exit 1
fi
if [[ ! -f "$BUILD_DIR/dlls/ftd2xx/i386-windows/ftd2xx.dll" || ! -f "$BUILD_DIR/dlls/ftd2xx/ftd2xx.so" ]]; then
    printf 'The generated FTD2XX module is missing from %s; finish the build first.\n' "$BUILD_DIR" >&2
    exit 1
fi

make -C "$BUILD_DIR" -j"$JOBS" install prefix="$PREFIX" DESTDIR="$DESTDIR"
install -D -m 0755 "$ROOT/runner.sh" "$DESTDIR$PREFIX/bin/runner.sh"
for document in "$ROOT"/docs/*.md; do
    install -D -m 0644 "$document" "$DESTDIR$PREFIX/share/doc/wine-ftd2xx/$(basename "$document")"
done
install -D -m 0644 "$ROOT/README.md" "$DESTDIR$PREFIX/share/doc/wine-ftd2xx/README.md"

printf 'Installed Wine and the FTD2XX runner in %s\n' "$DESTDIR$PREFIX"
printf 'Run %s/bin/runner.sh <windows-program> [arguments...].\n' "$PREFIX"
