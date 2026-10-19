# Lenovo Control 0.3.0

Unofficial Lenovo IdeaPad Linux utility (Arch Linux / KDE Plasma). Not affiliated with Lenovo.

## Features
- Qt 6 GUI (`lenovo-control-gui`)
- Performance profile and battery conservation controls via `pkexec`
- Desktop notifications for changes through a systemd user service

## Install on Arch Linux
Download the release ZIP, extract it, and run `makepkg -Csi` in the extracted `lenovo-control` directory. This package builds from a checksum-verified local source tarball.

## Commands
`lenovo-control status`
`lenovo-control profiles`
`lenovo-control profile balanced`
`lenovo-control conservation on`
`systemctl --user enable --now lenovo-control.service`

## Compatibility
Requires `/sys/firmware/acpi/platform_profile` and/or the IdeaPad ACPI `conservation_mode` interface. Not all Lenovo models expose these controls. Check `lenovo-control status` before using controls.

## Security
The GUI and CLI run as your user. Only the fixed-operation helper runs through Polkit authorization. No passwordless Polkit rule is installed. Review helper code before using.

## Uninstall
`systemctl --user disable --now lenovo-control.service` then `sudo pacman -Rns lenovo-control`.
