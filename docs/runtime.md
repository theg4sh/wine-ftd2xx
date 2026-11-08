# Runtime setup

## Native library

Use a 64-bit Linux `libftd2xx.so`. A 32-bit Linux FTDI library is neither used nor required.

The default soname is `libftd2xx.so`, resolved by the system loader. Set `LD_LIBRARY_PATH` when it is installed in a nonstandard directory, or set `FTD2XX_LIBRARY` to an explicit path:

```sh
export FTD2XX_LIBRARY=/usr/lib/libftd2xx.so
```

## USB access

The user running Wine needs permission to access the FTDI USB device. Configure the appropriate udev rule for the device.

FTDI's D2XX library can conflict with the kernel `ftdi_sio` driver and related serial drivers. Unbind or configure the device as required by the FTDI documentation. The project deliberately does not run `sudo rmmod` or change kernel state from its build and launch scripts.

## Run from the build tree

From the directory containing `Osa103.exe`:

```sh
FTD2XX_LIBRARY=/usr/lib/libftd2xx.so \
WINEDEBUG=+loaddll,+ftd2xx \
wine-ftd2xx/build/wine/wine ./Osa103.exe
```

Use a dedicated `WINEPREFIX` for testing when appropriate. The relevant build products are:

```text
build/wine/dlls/ftd2xx/i386-windows/ftd2xx.dll
build/wine/dlls/ftd2xx/ftd2xx.so
```

The build also produces an x86-64 PE `ftd2xx.dll`, but the i386 application through WoW64 is the supported and prioritized scenario. A separate `ftd2xx64.dll` module is not currently generated.

Avoid EEPROM erase or programming operations during smoke tests. End-to-end acquisition requires the real Osa103 hardware; a successful build or DLL load alone does not validate device I/O.
