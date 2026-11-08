# Scripts and project layout

## Layout

- `wine/` — Wine source submodule, pinned by the selected branch to Wine 9.22, 10.20, or 11.21.
- `ftd2xx/api.json` — export ordinals, C signatures, implementation class, required symbols, and shared-layout overrides.
- `ftd2xx/generate.py` — generates repetitive PE wrappers, native thunks, WoW64 conversions, dispatch tables, and the spec file.
- `ftd2xx/*_prelude.*` — editable headers and common declarations.
- `ftd2xx/pe_custom.c` — PE-side custom APIs, Windows event threads, and `DllMain`.
- `ftd2xx/unix_custom.c` — native handle/event registries and custom marshalling.
- `ftd2xx/patch_wine.py` — registers the generated module in the selected Wine version.
- `ftd2xx/generated/` — complete generated module; do not edit these files directly.
- `scripts/` — generation, preparation, and build entry points.
- `build/` — ignored out-of-tree build products.

## `scripts/generate.sh`

Runs `ftd2xx/generate.py` and recreates the module in `ftd2xx/generated`.

Generation is deterministic. Change `api.json` or an editable template and rerun this script. Do not patch generated C files: the next generation will overwrite them.

The generator is intentionally not a general C parser. A new API needs manifest entries and, where necessary, explicit pointer or callback semantics in custom source. Nested pointers and variable-shaped arguments must not be inferred from `winedump` output.

## `scripts/prepare-wine.sh`

1. Initializes the submodule when `wine/configure` is absent.
2. Runs generation.
3. Copies the generated module into `wine/dlls/ftd2xx`.
4. Adds `dlls/ftd2xx` registration to `configure.ac` and the generated `configure`.

Preparation is idempotent. The registration edits are the only changes to tracked Wine files. The script edits `configure` directly to avoid introducing a dependency on a particular Autoconf version; it does not run `autoreconf`.

To select another major series, check out `wine-9.22`, `wine-10.20`, or `wine-11.21` in the superproject, run `git submodule update --init --depth 1 wine`, and use a fresh `BUILD_DIR`. Each branch pins the newest release in its major series. The anchors used by `patch_wine.py` are deliberately checked: an incompatible version fails instead of silently producing an incomplete build.

## `scripts/build-wine.sh`

Runs preparation, configures Wine when `build/wine/Makefile` does not yet exist, and builds Wine and the wrapper.

Supported environment variables:

- `JOBS` — parallel make jobs; defaults to the host CPU count.
- `BUILD_DIR` — build directory; defaults to `build/wine`.
- `PREFIX` — optional installation prefix; defaults to `build/install`.
- `EXTRA_CONFIGURE_ARGS` — additional whitespace-separated Wine configure options.

Example:

```sh
JOBS=8 EXTRA_CONFIGURE_ARGS="--disable-tests" ./build.sh
```

Omit `--disable-tests` for contribution validation. If configure options change, use a new `BUILD_DIR` or remove the previous build directory explicitly.

## `build.sh`

Convenience wrapper for `scripts/build-wine.sh`.

## `runner.sh`

Runs an arbitrary Windows program with the local Wine build and FTD2XX wrapper. Pass the program and its arguments after the script name. It checks the Wine executable and native FTDI library before starting. It warns about FTDI interfaces owned by `ftdi_sio` and prints their exact unbind commands, but never changes kernel-driver bindings. See [runtime setup](runtime.md) for supported environment overrides.

## `install.sh`

Installs the completed Wine build with `make install`, then adds `runner.sh` and the documentation. `PREFIX` defaults to `/opt/wine-ftd2xx`; `DESTDIR` is available for package staging. `BUILD_DIR` and `JOBS` have the same meaning as in the build script.

The installer checks the i386 PE DLL and Unix library before installing. It does not install the proprietary FTDI library or the Windows application, and it does not invoke `sudo` itself.

## `install-system.sh`

Installs only the three FTD2XX module files beside an existing system Wine's drivers. The wrapper must first be built from the branch matching the system Wine major version. `WINE_BIN` defaults to `wine` from `PATH`; `WINE_LIBDIR` overrides automatic library-directory detection, and `DESTDIR` supports package staging. Existing FTD2XX modules are preserved unless `FORCE=1` is explicitly set.

## `ftd2xx/patch_wine.py`

Normally invoked by preparation rather than directly. Its direct form is:

```sh
python3 ftd2xx/patch_wine.py --wine wine --module ftd2xx/generated
```

It verifies all required generated files and the registration anchors before copying the module.
