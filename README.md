# wine-ftd2xx

A manifest-generated FTDI D2XX wrapper for running 32-bit Windows applications on 64-bit Wine and Linux.

The primary path is:

```text
i386 Windows application
  -> i386 PE ftd2xx.dll
  -> Wine WoW64 Unix-call thunk
  -> x86-64 ftd2xx.so
  -> x86-64 libftd2xx.so
```

Wine is included as a pinned Git submodule. `main` tracks the newest release, currently 11.21. The latest release in each supported major series has its own branch:

- `wine-9.22`
- `wine-10.20`
- `wine-11.21`

Check out the branch matching the Wine major version you intend to run, initialize the submodule, and use a fresh build directory. These branches change the submodule reference; the generation and registration scripts apply the wrapper to that release.

Generated module sources are kept in `ftd2xx/generated`; editable inputs are directly under `ftd2xx`.

## Build

Install Wine's build dependencies, MinGW-w64, Python 3, and the 64-bit FTDI D2XX Linux library, then run:

```sh
./build.sh
```

See [scripts and build options](docs/scripts.md), [architecture and API coverage](docs/architecture.md), and [runtime setup](docs/runtime.md).

## Status

The synchronous D2XX API subset, device enumeration, opaque handle mapping, and event notification are implemented. The remaining exports return explicit unsupported errors. This tree does not implement `ftd2xx64.dll`, Win32 COMM/overlapped APIs, or EEPROM structures containing nested pointers.

The wrapper is experimental. Validate it with the target hardware before using EEPROM write or erase functions.
