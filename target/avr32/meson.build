# SPDX-License-Identifier: GPL-2.0-or-later
gen = [
  decodetree.process('insn.decode', extra_args: [ '--decode', 'decode_insn',
                                                  '--varinsnwidth', '32'  ])
]

avr32_ss = ss.source_set()
avr32_system_ss = ss.source_set()
avr32_common_system_ss = ss.source_set()

avr32_ss.add(gen)
avr32_ss.add(files(
  'cpu.c',
  'disas.c',
  'helper.c',
  'helper_conditions.c',
  'helper_elf.c',
  'translate.c'
  ))

avr32_common_system_ss.add(files('gdbstub.c'))
avr32_system_ss.add(files('machine.c'))

target_arch += {'avr32': avr32_ss}
target_system_arch += {'avr32': avr32_system_ss}
target_common_system_arch += {'avr32': avr32_common_system_ss}
