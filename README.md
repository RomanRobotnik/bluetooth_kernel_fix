# DualSense Bluetooth Fix

A small Linux kernel module that adjusts the Bluetooth security setting for the first HCI controller (`hci0`). It is intended to work around a DualSense Bluetooth connection race condition.

## How it works

When the module is loaded, it resolves the kernel symbols `hci_dev_get` and `hci_dev_put` using kprobes. If `hci0` is available, it sets `hci0->min_enc_key_size` to `0` and releases the controller reference.

The module logs its result to the kernel log:

- Success: `DualSense Fix: min_enc_key_size for hci0 set to 0`
- Missing controller: `DualSense Fix: hci0 not found`
- Missing symbols: `DualSense Fix: Could not resolve bluetooth symbols`

## Requirements

- Linux with Bluetooth support
- A Bluetooth controller available as `hci0`
- Kernel headers and build tools for the running kernel
- `make`, `gcc`, `kmod`, and `systemd`
- Root access for installation

Because this module accesses kernel Bluetooth internals, it may require changes for different kernel versions. Use it only on systems where you understand and accept that risk.

## Build

Build the module for the currently running kernel:

```sh
make
```

Remove generated build artifacts:

```sh
make clean
```

The build produces `dualsense_fix.ko`.

## Install

Build the module first, then run:

```sh
./install.sh
```

The installer:

1. Copies `dualsense_fix.ko` to `/lib/modules/$(uname -r)/extra/`.
2. Runs `depmod`.
3. Installs `dualsense-fix.service` in `/etc/systemd/system/`.
4. Enables and starts the service.

The script uses `sudo` for operations that require root privileges.

Check the service status:

```sh
systemctl status dualsense-fix.service
```

Check the module log messages:

```sh
dmesg | grep 'DualSense Fix'
```

## Uninstall

Stop and disable the service, remove the installed files, and refresh the module dependency cache:

```sh
sudo systemctl disable --now dualsense-fix.service
sudo rm -f /etc/systemd/system/dualsense-fix.service
sudo rm -f /lib/modules/$(uname -r)/extra/dualsense_fix.ko
sudo depmod -a
```

If the module is currently loaded, unload it before removing the file:

```sh
sudo modprobe -r dualsense_fix
```

## Files

- `dualsense_fix.c` - Kernel module source
- `Makefile` - Out-of-tree kernel module build rules
- `install.sh` - Installation script
- `dualsense-fix.service` - systemd unit that loads the module at boot
- `.gitignore` - Ignores generated build and editor artifacts

## License

The module declares the GPL license through `MODULE_LICENSE("GPL")`.
