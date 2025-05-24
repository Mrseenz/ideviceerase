#ideviceerase

A tool to securely erase iOS devices via USB, part of the libimobiledevice project.

Description
ideviceerase is a cross-platform command-line utility that securely erases all data and settings on an iOS device, returning it to factory defaults. It leverages the native protocols of iOS devices through the libimobiledevice library, enabling low-level interactions without requiring jailbreaking 
.

Features
Secure Erase : Wipes all user data, settings, and partitions.
iOS Compatibility : Works with all iOS versions supported by libimobiledevice.
CLI Interface : Simple commands for automation or manual use.
Cross-Platform : Supports Linux, macOS, and Windows

Usage

Basic Erase
ideviceerase -u <device_udid>  
Replace <device_udid> with the target device’s UDID (use idevice_id -l to list connected devices).
Options
--ecid: Erase only if the ECID matches a specific value.
--debug: Enable verbose logging for troubleshooting 
.
Warning : This operation is irreversible and will remove all data 
.

Dependencies
libimobiledevice (core library) 
libusbmuxd (USB multiplexing) 
