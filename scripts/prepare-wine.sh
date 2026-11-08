#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)

if [[ ! -f "$ROOT/wine/configure" ]]; then
    git -C "$ROOT" submodule update --init --depth 1 wine
fi

"$ROOT/scripts/generate.sh"
python3 "$ROOT/ftd2xx/patch_wine.py" --wine "$ROOT/wine" --module "$ROOT/ftd2xx/generated"

printf 'Wine source: '
git -C "$ROOT/wine" describe --always --dirty
