/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * QEMU AVR CPU
 *
 * Copyright (c) 2022-2023 Florian Göhler
 */
DEF_HELPER_1(raise_illegal_instruction, noreturn, env)
DEF_HELPER_1(debug, noreturn, env)
DEF_HELPER_1(break, noreturn, env)
DEF_HELPER_1(avr32_sleep, noreturn, env)
DEF_HELPER_0(avr32_count, i32)
DEF_HELPER_1(avr32_tlb_flush, void, env)
DEF_HELPER_3(avr32_tlb_miss, noreturn, env, i32, i32)
DEF_HELPER_1(avr32_tlbs, void, env)
DEF_HELPER_1(avr32_tlbw, void, env)
DEF_HELPER_1(avr32_rete, void, env)
DEF_HELPER_4(macsathhw, void, env, i32, i32, i32)

#ifndef QEMU_AVR32_HELPER
#define QEMU_AVR32_HELPER
#include "tcg/tcg.h"
#define sflagC 0
#define sflagZ 1
#define sflagN 2
#define sflagV 3
#define sflagQ 4
#define sflagL 5
#define sflagT 14
#define sflagR 15
#define sflagGM 16
#define sflagEM 21

#endif /* QEMU_AVR32_HELPER */
