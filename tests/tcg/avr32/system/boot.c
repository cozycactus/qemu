/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <stdint.h>
#include "boot.h"

#define PASS_MARK 0x600dca7eU
#define FAIL_MARK 0xbad00badU
#define QEMU_TEST_EXIT_BASE 0xfffff000U

/* Volatile objects below model MMIO/result area shared with QEMU. */

extern int main(void);

static volatile uint32_t *const qemu_test_exit =
    (volatile uint32_t *)QEMU_TEST_EXIT_BASE;

void __sys_outc(char c)
{
    (void)c;
}

__attribute__((noreturn)) void _exit(int code)
{
    qemu_test_exit[0] = (uint32_t)code;
    qemu_test_exit[1] = 0U;
    qemu_test_exit[2] = code == 0 ? PASS_MARK : FAIL_MARK;

    for (;;) {
    }
}

static void init_sections(void)
{
    uint8_t *src = __data_load;
    uint8_t *dst = __data_start;

    while (dst < __data_end) {
        *dst++ = *src++;
    }

    for (dst = __bss_start; dst < __bss_end; dst++) {
        *dst = 0;
    }
}

__attribute__((noreturn)) void __crt_start(void)
{
    init_sections();
    _exit(main());
}

__attribute__((section(".text.entry"), used, naked)) void _start(void)
{
    __asm__ volatile("lda.w sp, __stack_top\n\t"
                     "call __crt_start\n\t"
                     "rjmp .\n\t");
}
