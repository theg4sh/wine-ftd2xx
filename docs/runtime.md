# Runtime setup

## Native library

Use a 64-bit Linux `libftd2xx.so`. A 32-bit Linux FTDI library is neither used nor required.

The default soname is `libftd2xx.so`, resolved by the system loader. Set `LD_LIBRARY_PATH` when it is installed in a nonstandard directory, or set `FTD2XX_LIBRARY` to an explicit path:

```sh
export FTD2XX_LIBRARY=/usr/lib/libftd2xx.so
```

## USB access

The user running Wine needs permission to access the FTDI USB device. Configure the appropriate udev rule for the device.

FTDI's D2XX library can conflict with the kernel `ftdi_sio` driver and related serial drivers. The runner checks sysfs and warns for every FTDI interface owned by `ftdi_sio`, including the interface name and an exact `sudo tee .../unbind` command. Unbind or configure the target device as required by the FTDI documentation. The project deliberately does not run `sudo rmmod` or change kernel state from its build and launch scripts.

## Run a Windows program from the build tree

Use the generic runner and pass the Windows program followed by its arguments:

```sh
wine-ftd2xx/runner.sh ./application.exe
```

It selects the local `build/wine/wine`, exports `FTD2XX_LIBRARY`, and starts the program with a user-owned `WINEPREFIX` (default `$HOME/.wine-ftd2xx`). It does not unload kernel drivers.

Optional overrides include `WINE_BIN`, `WINE_BUILD_DIR`, `WINEPREFIX`, `FTD2XX_LIBRARY`, `WINEDEBUG`, and `LD_LIBRARY_PATH`.

The relevant build products are:

```text
build/wine/dlls/ftd2xx/i386-windows/ftd2xx.dll
build/wine/dlls/ftd2xx/ftd2xx.so
```

The build also produces an x86-64 PE `ftd2xx.dll`, but the i386 application through WoW64 is the supported and prioritized scenario. A separate `ftd2xx64.dll` module is not currently generated.

## Installation

Build first and then run the installer with sufficient permission for the target prefix:

```sh
./build.sh
sudo ./install.sh
```

The default installation prefix is `/opt/wine-ftd2xx`. Override `PREFIX` for another location or `DESTDIR` for package staging:

```sh
PREFIX="$HOME/.local/wine-ftd2xx" ./install.sh
DESTDIR=/tmp/package-root PREFIX=/opt/wine-ftd2xx ./install.sh
```

The installer deploys Wine, the generated FTD2XX module, `bin/runner.sh`, and documentation. It does not install the proprietary FTDI library or the target application. Run the installed generic runner as:

```sh
/opt/wine-ftd2xx/bin/runner.sh ./application.exe
```

### Install beside system Wine

The wrapper can be installed without replacing or reinstalling Wine. Select the branch for the installed Wine major version, build it in a clean directory, and install only its modules:

```sh
git switch wine-10.20
git submodule update --init --depth 1 wine
./build.sh
sudo ./install-system.sh
```

The example supports Wine 10.x, including a system `wine-10.4` executable. Use `wine-9.22` for Wine 9.x or `wine-11.21` for Wine 11.x. The installer checks this major-version match, then places the PE DLLs in `i386-windows` and `x86_64-windows`, and the Unix library in `x86_64-unix`. Afterwards, run applications with the normal `wine` command.

Automatic detection supports standard `lib/wine`, `lib64/wine`, and Debian multiarch directories. Override it when necessary, or stage a package without writing to the host:

```sh
WINE_LIBDIR=/usr/lib/wine DESTDIR=/tmp/package-root ./install-system.sh
```

The installer does not invoke `sudo` itself and refuses to overwrite existing FTD2XX modules unless `FORCE=1` is set. Reinstall or update the wrapper whenever the system Wine major version changes.

Avoid EEPROM erase or programming operations during smoke tests. End-to-end acquisition requires the real target hardware; a successful build or DLL load alone does not validate device I/O.
