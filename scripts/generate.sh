#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
python3 "$ROOT/ftd2xx/generate.py" --output "$ROOT/ftd2xx/generated"
