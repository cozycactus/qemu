# SPDX-License-Identifier: GPL-2.0-or-later
avr32_ss = ss.source_set()
avr32_ss.add(when: 'CONFIG_AVR32EXP_MCU', if_true: files('avr32exp.c'))
avr32_ss.add(when: 'CONFIG_AVR32EXAMPLE_BOARD', if_true: files(
  'avr32example_board.c',
  'boot.c',
))

hw_arch += {'avr32': avr32_ss}
