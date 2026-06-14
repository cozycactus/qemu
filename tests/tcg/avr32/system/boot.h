/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef AVR32_TEST_BOOT_H
#define AVR32_TEST_BOOT_H

#include <stdint.h>

extern uint8_t __data_load[];
extern uint8_t __data_start[];
extern uint8_t __data_end[];
extern uint8_t __bss_start[];
extern uint8_t __bss_end[];

#endif /* AVR32_TEST_BOOT_H */
