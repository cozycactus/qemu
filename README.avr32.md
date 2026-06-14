SPDX-License-Identifier: GPL-2.0-or-later

# Downstream AVR32 QEMU branch

This fork carries a downstream, experimental revival of the AVR32 system target
for QEMU 11.0.1.

AVR32 is a discontinued architecture. This branch is intended for preservation,
local testing, and legacy firmware bring-up rather than immediate upstream QEMU
inclusion.

## Stable downstream entry points

- Branch: `avr32/qemu-11.0.1`
- Release tag: `avr32-qemu-11.0.1-0`
- Review artifact: <https://github.com/cozycactus/qemu/pull/1>

To check out the downstream branch:

```sh
git clone https://github.com/cozycactus/qemu.git qemu-avr32
cd qemu-avr32
git checkout avr32/qemu-11.0.1
```

Or check out the fixed release tag:

```sh
git checkout avr32-qemu-11.0.1-0
```

## Supported machine

- `avr32example-board`

The board model includes flash, SRAM, SDRAM, an interrupt controller,
Atmel-style USARTs, and a QEMU-only test-exit MMIO device used by the TCG
system tests.

## Build QEMU

A minimal AVR32-only build does not require the AVR32 cross compiler:

```sh
./configure --target-list=avr32-softmmu --disable-werror
ninja -C build qemu-system-avr32
./build/qemu-system-avr32 -machine help
```

Expected machine-list output includes:

```text
avr32example-board   AVR32 Example Board
```

## Run bare-metal firmware

Raw firmware images can be loaded with `-bios`:

```sh
./build/qemu-system-avr32 \
    -M avr32example-board \
    -bios firmware.bin \
    -nographic
```

The board maps:

- flash at `0x80000000`
- SRAM at `0x00000000`
- SDRAM at `0x10000000`
- QEMU test-exit MMIO at `0xfffff000`

The test-exit device uses three 32-bit words:

- offset `0x0`: result
- offset `0x4`: expected result
- offset `0x8`: status marker

Writing `0x600dca7e` to the status marker exits QEMU with status 0. Any other
status marker exits with status 1.

## AVR32 toolchain for tests

The QEMU binary can be built without an AVR32 compiler, but the AVR32 TCG tests
build freestanding AVR32 firmware and therefore need AVR32 GCC/binutils.

For macOS, this fork was tested with the preserved toolchain release here:

<https://github.com/cozycactus/avr32-toolchain-macos-arm64/releases/tag/v2026.06.03>

Available release assets include macOS Apple Silicon and x86_64 archives. For
Apple Silicon:

```sh
curl -LO https://github.com/cozycactus/avr32-toolchain-macos-arm64/releases/download/v2026.06.03/avr32-tools-src-macos-arm64-20260603.tar.gz
tar -xzf avr32-tools-src-macos-arm64-20260603.tar.gz -C "$HOME"
export PATH="$HOME/avr32-tools-src/bin:$PATH"
avr32-gcc --version
```

The local validation used:

```text
avr32-gcc 4.4.7
GNU binutils 2.23.1
```

Linux or other host users can use any AVR32 cross toolchain that provides an
`avr32-gcc`, `avr32-ld`, and related binutils capable of building freestanding
firmware. The original source-build reference is:

<https://github.com/denravonska/avr32-toolchain>

## Run the AVR32 TCG tests

Configure QEMU with the target-specific cross prefix, then run the softmmu tests:

```sh
./configure --target-list=avr32-softmmu --disable-werror \
    --cross-prefix-avr32="$HOME/avr32-tools-src/bin/avr32-"
ninja -C build qemu-system-avr32
make -C build/tests/tcg/avr32-softmmu run
```

The current test set covers:

- CPU smoke execution
- runtime startup for stack, `.data`, and `.bss` initialization
- arithmetic instructions
- branches
- shifts
- load/store instructions
- the common system `hello` test

A direct smoke run of the generated `hello` firmware looks like:

```sh
./build/qemu-system-avr32 \
    -M avr32example-board \
    -bios build/tests/tcg/avr32-softmmu/hello \
    -nographic
```

## Known limitations

- This is a downstream legacy branch, not an upstream-maintained QEMU target.
- The only exposed machine is `avr32example-board`.
- Device coverage is intentionally small and centered on firmware/test bring-up.
- Full regression testing depends on an external AVR32 cross toolchain.
- The implementation should be considered experimental until more real firmware
  and architecture conformance tests are added.

## Local verification performed

At the time this branch was prepared, local verification included:

```sh
git diff --check
ninja -C build qemu-system-avr32
make -C build/tests/tcg/avr32-softmmu run
./build/qemu-system-avr32 -machine help
./build/qemu-system-avr32 -M avr32example-board \
    -bios build/tests/tcg/avr32-softmmu/runtime-init \
    -nographic
```

The fork currently has no GitHub Actions checks wired up for this branch.
