/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * QEMU AVR CPU
 *
 * Copyright (c) 2022-2023 Florian Göhler, Johannes Willbold
 */
#ifndef AVR32A_CPU_QOM_H
#define AVR32A_CPU_QOM_H

#include "hw/core/cpu.h"
#include "qom/object.h"

#define TYPE_AVR32A_CPU "avr32a-cpu"
#define TYPE_AVR32B_CPU "avr32b-cpu"

OBJECT_DECLARE_CPU_TYPE(AVR32ACPU, AVR32ACPUClass, AVR32A_CPU)

typedef struct AVR32ACPUDef AVR32ACPUDef;

/**
 *  AVR32ACPUClass:
 *  @parent_realize: The parent class' realize handler.
 *  @parent_phases: The parent class' reset phase handlers.
 *
 *  A AVR32 CPU model.
 */
struct AVR32ACPUClass {
    /*< private > */
    CPUClass parent_class;

    /*< public > */
    DeviceRealize parent_realize;
    ResettablePhases parent_phases;

    const AVR32ACPUDef *cpu_def;
};

#endif /* AVR32A_CPU_QOM_H */
