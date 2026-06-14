/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * QEMU AVR32 Boot
 *
 * Copyright (c) 2022-2023 Florian Göhler, Johannes Willbold
 */
#ifndef HW_AVR32_BOOT_H
#define HW_AVR32_BOOT_H

#include "cpu.h"
#include "elf.h"
#include "hw/core/boards.h"

/**
 * avr32_load_firmware:   load an image into a memory region
 *
 * @cpu:        Handle a AVR CPU object
 * @ms:         A MachineState
 * @mr:         Memory Region to load into
 * @firmware:   Path to the firmware file (raw binary or ELF format)
 *
 * Load a firmware supplied by the machine or by the user  with the
 * '-bios' command line option, and put it in target memory.
 *
 * Returns: true on success, false on error.
 */
bool avr32_load_firmware(AVR32ACPU *cpu, MachineState *ms, MemoryRegion *mr,
                         const char *firmware);
bool avr32_load_elf_file(AVR32ACPU *cpu, const char *filename,
                         MemoryRegion *program_mr);
void avr32_copy_sections(int e_shnum, FILE *file, Elf32_Shdr **sh_table,
                         char *sh_strtable, MemoryRegion *program_mr);
void avr32_copy_text_section(int e_shnum, FILE *file, Elf32_Shdr **sh_table,
                             char *sh_strtable, FILE *output);
void avr32_copy_data_section(int e_shnum, FILE *file, Elf32_Shdr **sh_table,
                             char *sh_strtable, FILE *output);

#endif /* HW_AVR32_BOOT_H */
