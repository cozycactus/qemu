/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * QEMU AVR CPU
 *
 * Copyright (c) 2022-2023 Florian Göhler
 */
#ifndef QEMU_AVR32_HELPER_CP_INST_H
#define QEMU_AVR32_HELPER_CP_INST_H
#include "cpu.h"
#include "exec/helper-gen.h"
#include "exec/helper-proto.h"
#include "exec/log.h"
#include "exec/translator.h"
#include "qemu/qemu-print.h"
#include "tcg/tcg-op.h"
#include "tcg/tcg.h"

int checkCondition(int condition, TCGv returnReg, TCGv cpu_r[],
                   TCGv cpu_sflags[]);

void set_v_flag_add(TCGv op1, TCGv op2, TCGv result, TCGv cpu_sflags[]);
void set_c_flag_add(TCGv op1, TCGv op2, TCGv result, TCGv cpu_sflags[]);

void set_v_flag_cp(TCGv op1, TCGv op2, TCGv result, TCGv cpu_sflags[]);
void set_c_flag_cp(TCGv op1, TCGv op2, TCGv result, TCGv cpu_sflags[]);

void set_flags_cpc(TCGv op1, TCGv op2, TCGv result, TCGv cpu_sflags[]);

void cpw_instruction(TCGv Rd, TCGv Rs, TCGv cpu_sflags[]);

#endif /* QEMU_AVR32_HELPER_CP_INST_H */
