# Razer Naga Epic control for Linux
This is my old project to control brightness of Razer Naga Epic buttons and scroll wheel from Linux.
Mouse can be connected to the dock or directly to the computer, program will work with both variants.
Sorry for bad code, I just didn't want to delete it.

## Installation

### Dependencies
* libusb-1.0-0-dev

### Compilation
You can compile binary from source using Makefile:
```
make naga
```
The binary **naga** will be placed in the current directory.

### Install
You can install compiled binary into */usr/bin/naga* with Makefile:
```
sudo make install
```

### Uninstall
To remove */usr/bin/naga* from system use:
```
sudo make uninstall
```

## Usage
```
sudo naga [-v] [-l {0-255|on|off}]
```
* **-v** — Verbose mode
  Shows additional data such as bytes sent and received from mouse
* **-l ARG** — Brightness (light) control
  Changes brightness of mouse keys and scroll wheel.

  Where ARG is:
  * A number from **0** to **255** — absolute value of brightness
  * **on** (equivalent to **255**) — sets maximum brightness (minimum battery life)
  * **off** (equivalent to **0**) — turns off lights (maximum battery life)

## Additional information
* It is possible to run *naga* without root with some udev rules but I have lost them long time ago.
* New features are not planned but pull requests are always welcome!
