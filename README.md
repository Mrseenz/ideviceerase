# ideviceerase

A command-line utility to erase all content and settings on an iOS device.

## Description

`ideviceerase` is a cross-platform tool that interfaces with iOS devices via USB to perform a full erase operation, effectively returning the device to its factory settings. It utilizes the `libimobiledevice` library to communicate with the device.

## Erase Functionality

The tool performs an "Erase All Content and Settings" operation on the target iOS device. This is achieved by connecting to the device and issuing a command to the `com.apple.diagnostics_relay` service. Specifically, it sends a PList dictionary containing the `{"Request": "MobileObliterator"}` command, which instructs the device to wipe all user data, settings, and stored content.

This process is equivalent to selecting "Erase All Content and Settings" from the device's own settings menu.

## Features

*   **Full Device Erase**: Securely wipes all user data, applications, and settings.
*   **iOS Compatibility**: Works with iOS versions supported by the linked `libimobiledevice` library.
*   **Command-Line Interface**: Allows for easy scripting and automation.
*   **Cross-Platform**: Can be compiled and run on Linux, macOS, and Windows (with appropriate library installations).

## Usage

### Basic Erase Command

To erase an iOS device, you need its UDID. You can typically find the UDID using `idevice_id -l`.

```bash
./ideviceerase -u <device_udid>
```

Replace `<device_udid>` with the actual UDID of your target device.

### Options

*   `-u, --udid <device_udid>`: (Mandatory) Specifies the UDID of the target iOS device.
*   `--ecid <value>`: (Optional) Specifies the ECID of the target device. This option is parsed but not currently used to gate the erase operation.
*   `--debug`: (Optional) Enables verbose debug output, showing communication details with the device, including PList messages.

## WARNING

This operation is **DESTRUCTIVE** and **IRREVERSIBLE**. All data on the target device will be permanently erased.

*   **BACK UP YOUR DEVICE** before using this tool if you need to preserve any data.
*   Ensure you are targeting the correct device.
*   Use with extreme caution.

## Dependencies

To compile and run `ideviceerase`, you will need the following libraries installed:

*   **libimobiledevice**: A library that provides protocols to communicate with iOS devices.
*   **libplist**: A library for handling Apple's Property List (PList) format, used for communication.
*   **libusbmuxd**: A library that handles USB communication with iOS devices via the usbmux daemon.

Ensure that development headers for these libraries are also available if compiling from source (e.g., `libimobiledevice-dev`, `libplist-dev`, `libusbmuxd-dev` on Debian/Ubuntu systems).

## Compilation

A `Makefile` is provided. Typically, you can compile the tool by running:

```bash
make
```

This will produce the `ideviceerase` executable.

## Disclaimer

This tool interacts with iOS devices at a low level. The developers are not responsible for any damage or data loss that may occur from using this software. Understand the risks before proceeding.
