/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <stdint.h>

#define EXPECTED_STACK_MIX 0x6be5b5a0U

static uint32_t data_words[2] = { 0x11223344U, 0x55667788U };
static uint8_t bss_bytes[16];

static uint32_t stack_mix(uint32_t depth, uint32_t seed)
{
    /* volatile forces real stack slots, which is what this test exercises. */
    volatile uint32_t local[4];

    local[0] = seed ^ 0x01020304U;
    local[1] = seed + depth * 0x11111111U;
    local[2] = (seed << 3) | (seed >> 29);
    local[3] = local[0] + local[1] + local[2] + depth;

    if (depth == 0) {
        return local[3] ^ local[1];
    }

    return stack_mix(depth - 1, local[3]) ^ local[depth & 3];
}

int main(void)
{
    if (data_words[0] != 0x11223344U || data_words[1] != 0x55667788U) {
        return 1;
    }

    data_words[1] += 1U;
    if (data_words[1] != 0x55667789U) {
        return 2;
    }

    for (unsigned int i = 0; i < sizeof(bss_bytes); i++) {
        if (bss_bytes[i] != 0U) {
            return 3;
        }
    }

    bss_bytes[3] = 0xa5U;
    if (bss_bytes[3] != 0xa5U) {
        return 4;
    }

    if (stack_mix(6U, 0x89abcdefU) != EXPECTED_STACK_MIX) {
        return 5;
    }

    return 0;
}
