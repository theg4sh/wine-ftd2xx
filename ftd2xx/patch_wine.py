#!/usr/bin/env python3
import argparse
import shutil
from pathlib import Path


def insert_after(path, anchor, addition):
    text = path.read_text()
    if addition in text:
        return False
    if anchor not in text:
        raise RuntimeError(f"Cannot find registration anchor {anchor!r} in {path}")
    path.write_text(text.replace(anchor, anchor + addition, 1))
    return True


def main():
    parser = argparse.ArgumentParser(description="Register the generated FTD2XX module in a Wine source tree")
    parser.add_argument("--wine", type=Path, required=True)
    parser.add_argument("--module", type=Path, required=True)
    args = parser.parse_args()
    wine = args.wine.resolve()
    module = args.module.resolve()

    if not (wine / "configure").is_file() or not (wine / "dlls").is_dir():
        raise RuntimeError(f"{wine} is not a Wine source tree")
    for name in ("ftd2xx.c", "unixlib.c", "unixlib.h", "ftd2xx.spec", "Makefile.in"):
        if not (module / name).is_file():
            raise RuntimeError(f"Generated module is missing {name}; run scripts/generate.sh first")

    registrations = [
        (wine / "configure.ac",
         "WINE_CONFIG_MAKEFILE(dlls/fontsub)\n",
         "WINE_CONFIG_MAKEFILE(dlls/ftd2xx)\n"),
        (wine / "configure",
         "enable_fontsub\n",
         "enable_ftd2xx\n"),
        (wine / "configure",
         "wine_fn_config_makefile dlls/fontsub enable_fontsub\n",
         "wine_fn_config_makefile dlls/ftd2xx enable_ftd2xx\n"),
    ]
    for path, anchor, addition in registrations:
        text = path.read_text()
        if addition not in text and anchor not in text:
            raise RuntimeError(f"Cannot find registration anchor {anchor!r} in {path}")
    for path, anchor, addition in registrations:
        insert_after(path, anchor, addition)

    destination = wine / "dlls" / "ftd2xx"
    destination.mkdir(exist_ok=True)
    for source in module.iterdir():
        if source.is_file():
            shutil.copy2(source, destination / source.name)
    print(f"Registered generated FTD2XX module in {wine}")


if __name__ == "__main__":
    main()
