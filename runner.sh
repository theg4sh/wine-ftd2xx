#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)

if [[ $# -lt 1 ]]; then
    printf 'Usage: %s <windows-program> [arguments...]\n' "${0##*/}" >&2
    exit 2
fi

if [[ -n ${WINE_BIN:-} ]]; then
    WINE=$WINE_BIN
elif [[ -n ${WINE_BUILD_DIR:-} ]]; then
    WINE=$WINE_BUILD_DIR/wine
elif [[ -x "$ROOT/build/wine/wine" ]]; then
    WINE=$ROOT/build/wine/wine
elif [[ -x "$ROOT/wine" ]]; then
    WINE=$ROOT/wine
else
    printf 'Wine was not found; build the project, install it, or set WINE_BIN.\n' >&2
    exit 1
fi

FTD2XX_LIBRARY=${FTD2XX_LIBRARY:-libftd2xx.so}
WINEPREFIX=${WINEPREFIX:-"$HOME/.wine-ftd2xx"}
WINEDEBUG=${WINEDEBUG:-+loaddll}

if [[ ! -x "$WINE" ]]; then
    printf 'Wine is not executable at %s; set WINE_BIN or WINE_BUILD_DIR.\n' "$WINE" >&2
    exit 1
fi
if [[ "$FTD2XX_LIBRARY" == */* && ! -e "$FTD2XX_LIBRARY" ]]; then
    printf 'The 64-bit FTDI library was not found at %s; set FTD2XX_LIBRARY.\n' "$FTD2XX_LIBRARY" >&2
    exit 1
fi

export WINEPREFIX WINEDEBUG FTD2XX_LIBRARY

exec "$WINE" "$@"
