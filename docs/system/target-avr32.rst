.. SPDX-License-Identifier: GPL-2.0-or-later

.. _AVR32-System-emulator:

AVR32 System emulator
---------------------

QEMU can emulate the 32-bit AVR32 architecture in system mode with the
``qemu-system-avr32`` executable. The target currently provides the
``avr32example-board`` machine, which models an AVR32 example system with
flash, SRAM, SDRAM, an interrupt controller, and Atmel-style USARTs.

Building
========

Configure a minimal AVR32-only build with:

.. code-block:: shell

  ./configure --target-list=avr32-softmmu --disable-werror
  make -C build qemu-system-avr32

The resulting binary should report an AVR32 machine:

.. code-block:: shell

  build/qemu-system-avr32 -machine help

Bare-metal firmware
===================

Raw firmware images can be loaded with ``-bios``:

.. code-block:: shell

  build/qemu-system-avr32 -M avr32example-board -bios firmware.bin -nographic

The board maps flash at ``0x80000000``, SRAM at ``0x00000000``, and SDRAM at
``0x10000000``. The QEMU test firmware uses a test-exit MMIO device at
``0xfffff000``. The firmware writes a result word at offset ``0x0``, an
expected word at offset ``0x4``, and a status marker at offset ``0x8``.
Writing ``0x600dca7e`` to the status register exits QEMU with status 0; any
other status marker exits with status 1.

TCG tests
=========

The AVR32 system tests require an AVR32 GCC/binutils toolchain. Configure QEMU
with a target-specific cross prefix, for example:

.. code-block:: shell

  ./configure --target-list=avr32-softmmu --disable-werror \
      --cross-prefix-avr32=/path/to/avr32-
  make -C build qemu-system-avr32
  make -C build/tests/tcg/avr32-softmmu run

The tests build freestanding firmware and run it on ``avr32example-board``.
They cover a CPU smoke test, arithmetic instructions, branches, shifts,
load/store instructions, and the common system ``hello`` test.
