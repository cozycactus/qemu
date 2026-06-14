/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * QEMU AVR32 Example board
 *
 * Copyright (c) 2022-2023 Florian Göhler
 */
#ifndef HW_AVR32_AVR32EXPC_H
#define HW_AVR32_AVR32EXPC_H

#include "chardev/char-fe.h"
#include "hw/core/sysbus.h"
#include "qom/object.h"
#include "target/avr32/cpu.h"

#define TYPE_AVR32EXP_MCU "AVR32EXP"
#define TYPE_AVR32EXPS_MCU "AVR32EXPS"
#define AVR32EXP_USART_RX_FIFO_SIZE 64

typedef struct AVR32EXPMcuState AVR32EXPMcuState;
DECLARE_INSTANCE_CHECKER(AVR32EXPMcuState, AVR32EXP_MCU, TYPE_AVR32EXP_MCU)

typedef struct AVR32EXPUSARTState {
    MemoryRegion iomem;
    CharFrontend chr;
    struct AVR32EXPMcuState *mcu;
    uint32_t imr;
    uint32_t rpr;
    uint32_t rcr;
    uint32_t tpr;
    uint32_t tcr;
    uint32_t rnpr;
    uint32_t rncr;
    uint32_t tnpr;
    uint32_t tncr;
    uint32_t ptsr;
    uint32_t rx_status;
    uint8_t rx_byte;
    bool rx_ready;
    uint8_t rx_fifo[AVR32EXP_USART_RX_FIFO_SIZE];
    unsigned rx_fifo_head;
    unsigned rx_fifo_len;
    unsigned irq;
} AVR32EXPUSARTState;

struct AVR32EXPMcuState {
    /*< private > */
    SysBusDevice parent_obj;

    /*< public > */
    AVR32ACPU cpu;
    MemoryRegion flash;
    MemoryRegion sram;
    MemoryRegion ram;
    MemoryRegion ram_p1_alias;
    MemoryRegion ram_p2_alias;
    MemoryRegion intc_iomem;
    uint32_t intc_intpr[33];
    uint32_t intc_intreq[33];
    int intc_pending_irq;
    AVR32EXPUSARTState usart[4];
    MemoryRegion test_exit;
    uint32_t test_result;
    uint32_t test_expected;
};

#endif /* HW_AVR32_AVR32EXPC_H */
