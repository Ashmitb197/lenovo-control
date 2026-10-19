# Lenovo Control

An unofficial, open-source Lenovo Vantage-inspired utility for Linux.

Lenovo Control provides a graphical interface and command-line tools for managing supported Lenovo laptop features, monitoring hardware, and controlling power settings.

> **Version:** 0.4.0 (Development)
>
> **Status:** Active development on the `dev/v0.4.0` branch.
>
> **Tested hardware:** Lenovo IdeaPad Gaming 3 with Intel Core i5-11300H, Intel Iris Xe Graphics, and NVIDIA GeForce GTX 1650.

## Features

### Performance Management

Switch between supported firmware performance profiles:

- **Quiet** — Reduced power consumption (`low-power`)
- **Balanced** — Everyday usage (`balanced`)
- **Performance** — Higher performance (`performance`)

Available profiles depend on the laptop's firmware and Linux kernel support.

### Battery Conservation

Enable or disable Lenovo battery conservation mode on supported laptops.

This feature uses the `ideapad_acpi` Linux driver and requires compatible hardware.

### Native Desktop Interface

Version 0.4.0 introduces a native graphical interface built with:

- C++20
- Qt 6
- Qt Quick / QML
- CMake and Ninja

The interface includes:

- System Overview
- Performance Profiles
- Battery Management
- Hardware Monitoring
- Live monitoring cards
- RAM and battery progress indicators
- Vector icons and animated controls
- Responsive layouts
- Dark-themed interface

### Hardware Monitoring

The Hardware page currently supports:

| Component | Available information |
|---|---|
| CPU | Model, utilization, temperature |
| Intel GPU | Iris Xe model, current and maximum clock frequencies |
| NVIDIA GPU | Model, utilization, temperature |
| NVIDIA VRAM | Used and total memory |
| System RAM | Used and total memory |
| Battery | Charge percentage and charging status |
| Battery Health | Full-charge capacity compared with design capacity |
| Battery Capacity | Full-charge and original design capacity in Wh |
| Battery Cycles | Reported charge cycle count |

Hardware information is read from Linux system interfaces such as `/proc` and `/sys`.

NVIDIA monitoring uses `nvidia-smi` when available.

Unsupported or unavailable readings are displayed as unavailable.

**Note:** Intel GPU clock monitoring is implemented; Intel GPU utilization monitoring is not yet available.

## Screenshots

Screenshots of the v0.4.0 native interface will be added as development progresses.

## Requirements

### Development dependencies

For Arch Linux:

```bash
sudo pacman -S --needed \
    base-devel \
    cmake \
    ninja \
    qt6-base \
    qt6-declarative \
    qt6-svg

```

For NVIDIA monitoring, a working NVIDIA driver installation providing `nvidia-smi` is required.

### Hardware compatibility

Lenovo Control is primarily developed and tested on the Lenovo IdeaPad Gaming 3.

Supported functionality depends on Linux kernel drivers, Lenovo firmware, and available sysfs interfaces.

## Building v0.4.0

Clone the development branch:

```bash
git clone --branch dev/v0.4.0 https://github.com/Ashmitb197/lenovo-control.git
cd lenovo-control
```

Configure the native application:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
```

Compile:

```bash
cmake --build build -j 4
```

Launch:

```bash
./build/lenovo-control-native
```

### Hardware control backend

The native v0.4.0 interface currently reuses the existing Python `lenovo-control` command-line utility and its Polkit helper for privileged hardware changes.

Building the native application does not automatically install this backend.

Performance profile switching and battery conservation require the existing CLI and helper to be installed and configured correctly.

Read-only hardware monitoring can work independently where the relevant system interfaces are accessible.

## Command-Line Interface

Check status:

```bash
lenovo-control status
```

List supported performance profiles:

```bash
lenovo-control profiles
```

Change performance profile:

```bash
lenovo-control profile balanced
lenovo-control profile performance
lenovo-control profile low-power
```

Battery conservation:

```bash
lenovo-control conservation on
lenovo-control conservation off
```

Hardware-changing commands may require Polkit authorization.

## Project Structure

```text
lenovo-control/
├── assets/
│   ├── lenovo-control
│   ├── lenovo-control-helper
│   └── lenovo-control-gui
├── src/
│   ├── main.cpp
│   ├── HardwareController.h
│   └── HardwareController.cpp
├── qml/
│   └── Main.qml
├── CMakeLists.txt
├── PKGBUILD
└── README.md
```

## Architecture

The v0.4.0 application consists of two parts.

**Native C++ and Qt frontend**

- Qt Quick / QML interface
- C++ HardwareController
- CPU and memory monitoring
- NVIDIA GPU monitoring through asynchronous `nvidia-smi` queries
- Battery health and cycle monitoring
- Periodic hardware status updates

**Existing Python control backend**

- Command-line utility
- Privileged Polkit helper
- Lenovo firmware interfaces for performance and battery conservation

The native application currently depends on the existing CLI for hardware-changing operations.

## Settings and Preferences

Lenovo Control v0.4.0 supports:

- Persistent settings using Qt QSettings
- Monitoring refresh intervals of 1, 2, 3, or 5 seconds
- Celsius and Fahrenheit temperature display
- Synchronized CPU, Intel GPU, NVIDIA GPU, RAM, and battery polling
- Approximately 60 seconds of monitoring history

Preferences are saved automatically and restored when the application restarts.

## Development Roadmap

- [x] Python command-line utility
- [x] Performance profile switching
- [x] Battery conservation
- [x] Native C++20 / Qt 6 interface
- [x] Responsive QML dashboard
- [x] CPU utilization and temperature
- [x] NVIDIA GPU monitoring
- [x] Battery health and charge cycles
- [x] Intel Iris Xe GPU frequency monitoring
- [ ] Intel GPU utilization monitoring
- [x] Real-time hardware history graphs
- [x] Dedicated live monitoring page
- [x] Advanced battery information
- [x] Settings and preferences
- [ ] System tray integration
- [ ] Standalone native installation package
- [ ] Broader hardware compatibility testing

## Limitations

- Some functionality requires compatible Lenovo firmware and kernel drivers.
- NVIDIA telemetry requires a working `nvidia-smi` installation.
- Intel GPU frequency monitoring is available on tested hardware; utilization monitoring is not yet implemented.
- The native v0.4.0 application is still under development.
- A standalone native installer is not yet available.

## Disclaimer

Lenovo Control is an unofficial open-source community project. It is not affiliated with or endorsed by Lenovo.

Hardware compatibility varies by model. The software is provided without warranty.

## Contributing

Bug reports, compatibility testing, documentation improvements, and pull requests are welcome.

When reporting an issue, include your laptop model, Linux distribution, kernel version, and relevant error output.

## Repository

GitHub: https://github.com/Ashmitb197/lenovo-control

Development branch: `dev/v0.4.0`
