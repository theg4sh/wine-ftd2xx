# Wrapper architecture and API coverage

## PE and Unix split

Wine builds `ftd2xx.dll` as a PE builtin and `ftd2xx.so` as its native Unix companion. The PE side exports the D2XX ABI and calls the Unix side with `WINE_UNIX_CALL`. The Unix side loads `libftd2xx.so` with `dlopen` and resolves functions with `dlsym`.

No FTDI archive is linked into the wrapper. The build therefore needs only Wine's headers and pthreads; the proprietary FTDI library is a runtime dependency.

The manifest marks each export as:

- `simple` — scalar arguments, opaque handles, and flat buffers can be forwarded automatically.
- `custom` — a hand-written PE or Unix implementation is required.
- `unsupported` — the PE export returns an explicit error and never passes an unsafe structure to native code.
- `stub` — Wine supplies an unimplemented stub for a private initializer export.

Required native symbols prevent attachment when absent. Optional symbols return `FT_NOT_SUPPORTED` per call instead of making the entire DLL unavailable.

## WoW64 ABI rules

The application never receives a native `FT_HANDLE`. It receives a nonzero 32-bit token. The Unix registry maps tokens to native handles, protects lookup with a mutex, and uses references so a closing handle is not freed while another thunk uses it. Close removes the token from the live registry.

Flat buffers are converted between 32-bit and native pointers in generated WoW64 thunks. Scalar fields have fixed-width types. `FT_STATUS` is returned in the parameter block rather than being confused with the `NTSTATUS` transport result.

Custom conversions cover:

- `FT_Open` and `FT_OpenEx` handle outputs.
- `FT_GetDeviceInfoDetail` handle output.
- `FT_GetDeviceInfoList`, including the different 32-bit and 64-bit structure strides.
- `FT_ListDevices` arrays of string-buffer pointers.
- `FT_OpenEx` arguments that are either integers or pointers depending on flags.

## Event notification

FTDI's native `EVENT_HANDLE` is a pthread condition-variable structure, not a Windows event. The Unix side owns that native object and provides wait, stop, and destroy calls.

For a non-null Windows event, the PE side creates a worker thread. The thread waits through the Unix library and calls `SetEvent` on the application's Windows handle. Close, registration replacement, and DLL detach wake the worker, join it, and release the native event. No Windows API is called from a native FTDI callback thread.

A null Windows event still creates the native FTDI event object, as required by the Linux library, but does not create a PE worker.

## Implemented surface

The implemented synchronous subset includes open/close, read/write, device information, UART and bit-mode configuration, queue/status operations, timeouts, EEPROM word/user-area operations, event notification, and vendor command buffers.

The following are intentionally unsupported in this version:

- `FT_IoCtl` and Win32 COMM/overlapped functions.
- `FT_EE_Program`, `FT_EE_Read`, and their `Ex` variants.
- `FT_EEPROM_Read` and `FT_EEPROM_Program`.
- The private `_FT_Initialise@0` and `_FT_Finalise@0` exports, which remain Wine stubs.
- `ftd2xx64.dll` as a separate DLL name.

`FT_EraseEE`, `FT_WriteEE`, `FT_EE_UAWrite`, and `FT_EE_WriteConfig` are forwarded to the native library and can modify device contents. They require explicit hardware validation and should not be exercised during ordinary smoke tests.
