/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * QEMU AVR32 gdbstub
 *
 * Copyright (c) 2023 Florian Göhler
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "gdbstub/helpers.h"

int avr32_cpu_gdb_read_register(CPUState *cs, GByteArray *mem_buf, int n)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    CPUAVR32AState *env = &cpu->env;

    uint32_t val = 0;
    val |= ((env->r[n] & 0x000000FF) << 24);
    val |= ((env->r[n] & 0x0000FF00) << 8);
    val |= ((env->r[n] & 0x00FF0000) >> 8);
    val |= ((env->r[n] & 0xFF000000) >> 24);

    printf("Sending to GDB: 0x%04x", val);

    return gdb_get_reg32(mem_buf, val);
}

int avr32_cpu_gdb_write_register(CPUState *cs, uint8_t *mem_buf, int n)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    CPUAVR32AState *env = &cpu->env;

    printf("Writing GDB input to register %i", n);

    uint32_t val = 0;
    uint32_t memval = (uint32_t)ldl_p(mem_buf);
    val |= ((memval & 0x000000FF) << 24);
    val |= ((memval & 0x0000FF00) << 8);
    val |= ((memval & 0x00FF0000) >> 8);
    val |= ((memval & 0xFF000000) >> 24);
    env->r[n] = val;

    return 0;
}

vaddr avr32_cpu_gdb_adjust_breakpoint(CPUState *cpu, vaddr addr)
{
    /*
     * This is due to some strange GDB behavior.  Let's assume main has address
     * 0x100:
     *   b main    - sets breakpoint at address 0x00000100 (code)
     *   b *0x100  - sets breakpoint at address 0x00800100 (data)
     *
     * Force all breakpoints into code space.
     */
    return addr;
}
