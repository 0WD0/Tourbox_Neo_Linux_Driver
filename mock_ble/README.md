# Mock BLE Wrapper for TourBox Console

This is a mock implementation of the `libBLEWrapper.dll` used by TourBox Console. It provides dummy implementations of the Bluetooth Low Energy (BLE) functions to allow the TourBox Console software to run under Wine without crashing due to Bluetooth-related errors.

## Purpose

The original `libBLEWrapper.dll` attempts to access Windows Bluetooth APIs that don't work properly under Wine, causing the application to crash with "Invalid memory access" errors. This mock version provides stub implementations that return success values without actually trying to access Bluetooth hardware.

## Building

To build the mock DLL, you need MinGW-w64 cross-compiler installed on your Linux system.

On Arch Linux, install it with:

```bash
sudo pacman -S mingw-w64-gcc
```

Then build the DLL:

```bash
make
```

## Installation

You can install the mock DLL to your Wine TourBox Console directory with:

```bash
make install
```

This will copy the DLL to `~/.wine/drive_c/Program Files/TourBox Console/win64libs/`.

Alternatively, you can manually copy the DLL to replace the original:

1. Backup the original DLL:
   ```bash
   cp ~/.wine/drive_c/Program\ Files/TourBox\ Console/win64libs/libBLEWrapper.dll ~/.wine/drive_c/Program\ Files/TourBox\ Console/win64libs/libBLEWrapper.dll.bak
   ```

2. Copy the mock DLL:
   ```bash
   cp libBLEWrapper.dll ~/.wine/drive_c/Program\ Files/TourBox\ Console/win64libs/
   ```

## Logging

The mock DLL creates a log file named `ble_wrapper_log.txt` in the current directory when TourBox Console is run. This log records which BLE functions are being called, which can be useful for debugging.

## Limitations

This mock implementation:

- Does not actually connect to any Bluetooth devices
- Always reports a successful connection
- Returns dummy data for device scanning
- May not implement all functions used by TourBox Console

It's intended as a workaround to allow the software to run, not as a full replacement for the Bluetooth functionality.

## Future Improvements

If you need actual Bluetooth functionality, a more comprehensive solution would be to:

1. Implement a bridge between the Windows BLE API calls and Linux BlueZ
2. Use a USB connection to the TourBox device instead of Bluetooth
3. Reverse-engineer the TourBox protocol and implement a native Linux driver
