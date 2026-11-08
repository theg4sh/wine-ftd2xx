#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR=${BUILD_DIR:-"$ROOT/build/wine"}
DESTDIR=${DESTDIR:-}
WINE_BIN=${WINE_BIN:-$(command -v wine || true)}

if [[ -z "$WINE_BIN" || ! -x "$WINE_BIN" ]]; then
    printf 'System Wine was not found; set WINE_BIN to its executable.\n' >&2
    exit 1
fi

system_version=$($WINE_BIN --version)
if [[ $system_version =~ wine-([0-9]+) ]]; then
    system_major=${BASH_REMATCH[1]}
else
    printf 'Cannot determine the Wine version from %s.\n' "$system_version" >&2
    exit 1
fi
if [[ ! -f "$ROOT/wine/VERSION" ]] || [[ ! $(<"$ROOT/wine/VERSION") =~ Wine\ version\ ([0-9]+) ]]; then
    printf 'Cannot determine the wrapper Wine version; initialize the wine submodule.\n' >&2
    exit 1
fi
build_major=${BASH_REMATCH[1]}
if [[ $system_major != "$build_major" ]]; then
    printf 'Version mismatch: system Wine is %s, but this wrapper was built for Wine %s.x.\n' \
        "$system_version" "$build_major" >&2
    printf 'Check out the branch for the matching major version and rebuild in a clean BUILD_DIR.\n' >&2
    exit 1
fi

for artifact in i386-windows/ftd2xx.dll x86_64-windows/ftd2xx.dll; do
    if [[ ! -f "$BUILD_DIR/dlls/ftd2xx/$artifact" ]]; then
        printf 'Missing %s; build the wrapper first.\n' "$BUILD_DIR/dlls/ftd2xx/$artifact" >&2
        exit 1
    fi
done
if [[ ! -f "$BUILD_DIR/dlls/ftd2xx/ftd2xx.so" ]]; then
    printf 'Missing %s; build the wrapper first.\n' "$BUILD_DIR/dlls/ftd2xx/ftd2xx.so" >&2
    exit 1
fi

if [[ -z ${WINE_LIBDIR:-} ]]; then
    wine_path=$(readlink -f "$WINE_BIN")
    wine_prefix=$(dirname -- "$(dirname -- "$wine_path")")
    for candidate in "$wine_prefix/lib/wine" "$wine_prefix/lib64/wine" \
            "$wine_prefix/lib/x86_64-linux-gnu/wine"; do
        if [[ -d "$candidate/x86_64-unix" && -d "$candidate/i386-windows" ]]; then
            WINE_LIBDIR=$candidate
            break
        fi
    done
fi
if [[ -z ${WINE_LIBDIR:-} || ! -d "$WINE_LIBDIR/x86_64-unix" || ! -d "$WINE_LIBDIR/i386-windows" ]]; then
    printf 'Cannot locate the system Wine module directories; set WINE_LIBDIR.\n' >&2
    exit 1
fi

for relative in i386-windows/ftd2xx.dll x86_64-windows/ftd2xx.dll x86_64-unix/ftd2xx.so; do
    destination=$DESTDIR$WINE_LIBDIR/$relative
    if [[ -e "$destination" && ${FORCE:-0} != 1 ]]; then
        printf '%s already exists; set FORCE=1 to replace it.\n' "$destination" >&2
        exit 1
    fi
done

install -D -m 0644 "$BUILD_DIR/dlls/ftd2xx/i386-windows/ftd2xx.dll" \
    "$DESTDIR$WINE_LIBDIR/i386-windows/ftd2xx.dll"
install -D -m 0644 "$BUILD_DIR/dlls/ftd2xx/x86_64-windows/ftd2xx.dll" \
    "$DESTDIR$WINE_LIBDIR/x86_64-windows/ftd2xx.dll"
install -D -m 0755 "$BUILD_DIR/dlls/ftd2xx/ftd2xx.so" \
    "$DESTDIR$WINE_LIBDIR/x86_64-unix/ftd2xx.so"

printf 'Installed only the FTD2XX modules for %s in %s.\n' "$system_version" "$DESTDIR$WINE_LIBDIR"
printf 'Start applications with the matching system Wine; no private prefix is required.\n'
