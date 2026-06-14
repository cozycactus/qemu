.. SPDX-License-Identifier: GPL-2.0-or-later

.. _AVR32-System-emulator:

AVR32 System emulator
---------------------

QEMU can emulate the 32-bit AVR32 architecture in system mode with the
``qemu-system-avr32`` executable. The target currently provides the
``avr32example-board`` machine, which models an AVR32 example system with
flash, SRAM, SDRAM, an interrupt controller, and Atmel-style USARTs.

Status
======

This target is a downstream, experimental revival for legacy AVR32 firmware
work. AVR32 is a discontinued architecture, so this branch is intended for
preservation, local testing, and bring-up of old firmware rather than immediate
upstream QEMU inclusion.

The implementation is useful for bare-metal tests and simple firmware boot
experiments, but it should not yet be treated as a complete AVR32 platform
model. Users should expect missing devices and incomplete architectural corner
cases.

Supported machine
=================

``avr32example-board``
  Example AVR32 board model with flash at ``0x80000000``, SRAM at
  ``0x00000000``, SDRAM at ``0x10000000``, an interrupt controller,
  Atmel-style USARTs, and a QEMU-only test-exit device.

Building
========

Configure a minimal AVR32-only build with:

.. code-block:: shell

  ./configure --target-list=avr32-softmmu --disable-werror
  ninja -C build qemu-system-avr32

The resulting binary should report the AVR32 machine:

.. code-block:: shell

  build/qemu-system-avr32 -machine help

Expected output includes:

.. code-block:: text

  avr32example-board   AVR32 Example Board

Bare-metal firmware
===================

Raw firmware images can be loaded with ``-bios``:

.. code-block:: shell

  build/qemu-system-avr32 -M avr32example-board -bios firmware.bin -nographic

The QEMU test firmware uses a test-exit MMIO device at ``0xfffff000``. The
firmware writes a result word at offset ``0x0``, an expected word at offset
``0x4``, and a status marker at offset ``0x8``. Writing ``0x600dca7e`` to the
status register exits QEMU with status 0; any other status marker exits with
status 1.

TCG tests
=========

The AVR32 system tests require an AVR32 GCC/binutils toolchain. Configure QEMU
with a target-specific cross prefix, for example:

.. code-block:: shell

  ./configure --target-list=avr32-softmmu --disable-werror \
      --cross-prefix-avr32=/path/to/avr32-
  ninja -C build qemu-system-avr32
  make -C build/tests/tcg/avr32-softmmu run

The tests build freestanding firmware and run it on ``avr32example-board``.
They cover:

* CPU smoke execution
* runtime startup for stack, ``.data``, and ``.bss`` initialization
* arithmetic instructions
* branches
* shifts
* load/store instructions
* the common system ``hello`` test

Known limitations
=================

* This is a downstream legacy target, not an upstream-maintained QEMU target.
* The only board currently exposed is ``avr32example-board``.
* Device coverage is intentionally small and centered on firmware/test bring-up.
* The test suite depends on an external AVR32 cross toolchain.
* The implementation should be considered experimental until more real firmware
  and architecture conformance tests are added.
