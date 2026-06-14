/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * QEMU AVR CPU
 *
 * Copyright (c) 2022-2023 Florian Göhler, Johannes Willbold
 */
#include "qemu/osdep.h"
#include "cpu.h"
#include "accel/tcg/cpu-ops.h"
#include "disas/dis-asm.h"
#include "exec/helper-proto.h"
#include "exec/translation-block.h"
#include "qapi/error.h"
#include "qemu/qemu-print.h"
#include "system/address-spaces.h"
#include "tcg/debug-assert.h"

static AVR32ACPU *cpu_self;
static bool first_reset = true;

/* System register indices are encoded as byte offsets divided by four. */
#define SYSREG_CONFIG0_WORD (0x0100 / 4)
#define SYSREG_CONFIG1_WORD (0x0104 / 4)
#define SYSREG_EVBA_WORD (0x0004 / 4)
#define SYSREG_RSR_INT0_WORD (0x0018 / 4)
#define SYSREG_RAR_INT0_WORD (0x0038 / 4)
#define AVR32_SR_GM_BIT 16
#define AVR32_SR_I0M_BIT 17
#define AVR32_SR_MODE_SHIFT 22
#define AVR32_MODE_INT0 2

static void avr32_cpu_disas_set_info(const CPUState *cpu,
                                     disassemble_info *info)
{
    info->mach = bfd_arch_unknown;
    info->print_insn = avr32_print_insn;
}

static void avr32a_cpu_init(Object *obj)
{
}

static void avr32b_cpu_init(Object *obj)
{
    /* TODO */
}

static void avr32_cpu_realizefn(DeviceState *dev, Error **errp)
{
    CPUState *cs = CPU(dev);
    cpu_self = AVR32A_CPU(cs);
    AVR32ACPUClass *acc = AVR32A_CPU_GET_CLASS(dev);
    Error *local_err = NULL;

    /* TODO: Custom CPU setup stuff per CPU core arch */

    cpu_exec_realizefn(cs, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }

    qemu_init_vcpu(cs);
    cpu_reset(cs);

    acc->parent_realize(dev, errp);
}

static void avr32_cpu_reset_hold(Object *obj, ResetType type)
{
    CPUState *cs = CPU(obj);
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    AVR32ACPUClass *acc = AVR32A_CPU_GET_CLASS(obj);
    CPUAVR32AState *env = &cpu->env;

    if (acc->parent_phases.hold) {
        acc->parent_phases.hold(obj, type);
    }

    env->isInInterrupt = 0;
    env->intlevel = 0;
    env->intsrc = -1;

    if (first_reset) {
        memset(env->r, 0, sizeof(env->r));
        memset(env->sflags, 1, sizeof(env->sflags));
        first_reset = false;
    }

    env->sr = 0;

    for (int i = 0; i < 32; i++) {
        env->sflags[i] = 0;
    }
    env->sflags[16] = 1;
    env->sflags[21] = 1;
    env->sflags[22] = 1;

    for (int i = 0; i < AVR32A_SYS_REG; i++) {
        env->sysr[i] = 0;
    }
    /* Advertise a small shared-TLB core with 32-byte I/D cache lines. */
    env->sysr[SYSREG_CONFIG0_WORD] = (2u << 7) | 1u;
    env->sysr[SYSREG_CONFIG1_WORD] =
        (4u << 3) | (5u << 6) | (4u << 13) | (5u << 16);
    memset(env->tlb, 0, sizeof(env->tlb));

    for (int i = 0; i < AVR32A_REG_PAGE_SIZE; i++) {
        env->r[i] = 0;
    }

    env->r[AVR32A_PC_REG] = 0x80000000;
    env->r[AVR32A_LR_REG] = 0;
    env->r[AVR32A_SP_REG] = 0;
    env->task_sp = 0;
    env->supervisor_sp = 0;
}

static ObjectClass *avr32_cpu_class_by_name(const char *cpu_model)
{
    ObjectClass *oc;
    (void)cpu_model;
    oc = object_class_by_name(AVR32A_CPU_TYPE_NAME("AVR32EXPC"));
    return oc;
}
static bool avr32_cpu_has_work(CPUState *cs)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    CPUAVR32AState *env = &cpu->env;

    return (cs->interrupt_request & CPU_INTERRUPT_HARD) &&
           cpu_interrupts_enabled(env);
}

static void avr32_cpu_dump_state(CPUState *cs, FILE *f, int flags)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    CPUAVR32AState *env = &cpu->env;

    qemu_fprintf(f, "PC:    " TARGET_FMT_lx "\n", env->r[AVR32A_PC_REG]);
    qemu_fprintf(f, "SP:    " TARGET_FMT_lx "\n", env->r[AVR32A_SP_REG]);
    qemu_fprintf(f, "LR:    " TARGET_FMT_lx "\n", env->r[AVR32A_LR_REG]);

    int i;
    for (i = 0; i < AVR32A_REG_PAGE_SIZE - 3; ++i) {
        qemu_fprintf(f, "r%d:    " TARGET_FMT_lx "\n", i, env->r[i]);
    }

    for (i = 0; i < 32; i++) {
        qemu_fprintf(f, "%s:    " TARGET_FMT_lx "\n",
                     avr32_cpu_sr_flag_names[i], env->sflags[i]);
    }

}

static void avr32_cpu_set_pc(CPUState *cs, vaddr value)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);

    cpu->env.r[AVR32A_PC_REG] = value;
}

static vaddr avr32_cpu_get_pc(CPUState *cs)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    return cpu->env.r[AVR32A_PC_REG];
}

static TCGTBCPUState avr32_get_tb_cpu_state(CPUState *cs)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);

    return (TCGTBCPUState){.pc = cpu->env.r[AVR32A_PC_REG], .flags = 0};
}

static int avr32_cpu_mmu_index(CPUState *cs, bool ifetch)
{
    return 0;
}

static bool avr32_cpu_exec_interrupt(CPUState *cs, int interrupt_request)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    CPUAVR32AState *env = &cpu->env;
    uint32_t sr = 0;
    int level = env->intlevel;

    if (!(interrupt_request & CPU_INTERRUPT_HARD) || env->intsrc < 0) {
        return false;
    }

    if (!cpu_interrupts_enabled(env) || level < 0 || level > 3 ||
        env->sflags[AVR32_SR_I0M_BIT + level]) {
        return false;
    }

    for (int i = 0; i < 32; i++) {
        sr |= (env->sflags[i] & 1) << i;
    }

    env->sysr[SYSREG_RAR_INT0_WORD + level] = env->r[AVR32A_PC_REG];
    env->sysr[SYSREG_RSR_INT0_WORD + level] = sr;
    if (((sr >> AVR32_SR_MODE_SHIFT) & 7) == 0) {
        env->task_sp = env->r[AVR32A_SP_REG];
        env->r[AVR32A_SP_REG] = env->supervisor_sp;
    }

    for (int i = 0; i < 3; i++) {
        env->sflags[AVR32_SR_MODE_SHIFT + i] =
            ((AVR32_MODE_INT0 + level) >> i) & 1;
    }
    env->sflags[AVR32_SR_GM_BIT] = 1;

    env->r[AVR32A_PC_REG] = env->sysr[SYSREG_EVBA_WORD] + env->autovector;
    cpu_reset_interrupt(cs, CPU_INTERRUPT_HARD);
    return true;
}

static void avr32_restore_state_to_opc(CPUState *cs, const TranslationBlock *tb,
                                       const uint64_t *data)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);

    /* TODO: verify whether this is correct... */
    cpu->env.r[AVR32A_PC_REG] = data[0];
}

#include "hw/core/sysemu-cpu-ops.h"

static const struct SysemuCPUOps avr32_sysemu_ops = {
    .has_work = avr32_cpu_has_work,
    .get_phys_page_debug = avr32_cpu_get_phys_page_debug,
};

static const struct TCGCPUOps avr32_tcg_ops = {
    .initialize = avr32_tcg_init,
    .translate_code = avr32_translate_code,
    .get_tb_cpu_state = avr32_get_tb_cpu_state,
    .synchronize_from_tb = avr32_cpu_synchronize_from_tb,
    .restore_state_to_opc = avr32_restore_state_to_opc,
    .mmu_index = avr32_cpu_mmu_index,
    .cpu_exec_interrupt = avr32_cpu_exec_interrupt,
    .cpu_exec_halt = avr32_cpu_has_work,
    .cpu_exec_reset = cpu_reset,
    .tlb_fill = avr32_cpu_tlb_fill,
    .do_interrupt = avr32_cpu_do_interrupt,
    .pointer_wrap = cpu_pointer_wrap_uint32,
};

void avr32_cpu_synchronize_from_tb(CPUState *cs, const TranslationBlock *tb)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    tcg_debug_assert(!tcg_cflags_has(cs, CF_PCREL));
    cpu->env.r[AVR32A_PC_REG] = tb->pc;
}

static void avr32a_cpu_class_init(ObjectClass *oc, const void *data)
{
    AVR32ACPUClass *acc = AVR32A_CPU_CLASS(oc);
    CPUClass *cc = CPU_CLASS(oc);
    DeviceClass *dc = DEVICE_CLASS(oc);
    ResettableClass *rc = RESETTABLE_CLASS(oc);

    device_class_set_parent_realize(dc, avr32_cpu_realizefn,
                                    &acc->parent_realize);
    resettable_class_set_parent_phases(rc, NULL, avr32_cpu_reset_hold, NULL,
                                       &acc->parent_phases);

    cc->class_by_name = avr32_cpu_class_by_name;

    cc->dump_state = avr32_cpu_dump_state;
    cc->set_pc = avr32_cpu_set_pc;
    cc->get_pc = avr32_cpu_get_pc;
    cc->memory_rw_debug = avr32_cpu_memory_rw_debug;
    cc->sysemu_ops = &avr32_sysemu_ops;
    cc->disas_set_info = avr32_cpu_disas_set_info;
    cc->tcg_ops = &avr32_tcg_ops;
    cc->gdb_read_register = avr32_cpu_gdb_read_register;
    cc->gdb_write_register = avr32_cpu_gdb_write_register;
    cc->gdb_adjust_breakpoint = avr32_cpu_gdb_adjust_breakpoint;
    cc->gdb_core_xml_file = "avr32a-cpu.xml";
}

static void avr32b_cpu_class_init(ObjectClass *oc, const void *data)
{
    /* TODO */
}

static const AVR32ACPUDef avr32_cpu_defs[] = {
    {
        .name = "AVR32EXPC",
        .parent_microarch = TYPE_AVR32A_CPU,
        .core_type = AVR32_EXP,
        .series_type = AVR32_EXP_S,
        .clock_speed = 66 * 1000 * 1000, /* 66 MHz */
        .audio = false,
        .aes = false,
    },
};

static void avr32_cpu_cpudef_class_init(ObjectClass *oc, const void *data)
{
    AVR32ACPUClass *acc = AVR32A_CPU_CLASS(oc);
    acc->cpu_def = data;
}

static char *avr32_cpu_type_name(const char *model_name)
{
    return g_strdup_printf(AVR32A_CPU_TYPE_NAME("%s"), model_name);
}

static void avr32_register_cpudef_type(const struct AVR32ACPUDef *def)
{
    char *cpu_model_name = avr32_cpu_type_name(def->name);
    TypeInfo ti = {
        .name = cpu_model_name,
        .parent = def->parent_microarch,
        .class_init = avr32_cpu_cpudef_class_init,
        .class_data = (void *)def,
    };

    type_register_static(&ti);
    g_free(cpu_model_name);
}

static const TypeInfo avr32_cpu_arch_types[] = {
    {
        .name = TYPE_AVR32A_CPU,
        .parent = TYPE_CPU,
        .instance_size = sizeof(AVR32ACPU),
        .instance_init = avr32a_cpu_init,
        .abstract = true,
        .class_size = sizeof(AVR32ACPUClass),
        .class_init = avr32a_cpu_class_init,
    },
    {
        .name = TYPE_AVR32B_CPU,
        .parent = TYPE_CPU,
        .instance_size = sizeof(AVR32ACPU),
        .instance_init = avr32b_cpu_init,
        .abstract = true,
        .class_size = sizeof(AVR32ACPUClass),
        .class_init = avr32b_cpu_class_init,
    },
};

static void avr32_cpu_register_types(void)
{
    int i;

    type_register_static_array(avr32_cpu_arch_types, 2);
    for (i = 0; i < ARRAY_SIZE(avr32_cpu_defs); i++) {
        avr32_register_cpudef_type(&avr32_cpu_defs[i]);
    }
}

type_init(avr32_cpu_register_types)
