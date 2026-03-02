/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "esp_attr.h"

/*
 * Test for the portable link-time section macros defined in esp_attr.h.
 *
 * Two separate translation units (test_entries_a.c and test_entries_b.c)
 * place uint32_t values into the "test_data_table" section using
 * PLACE_IN_SECTION().  This main function uses _SECTION_ATTR_SYMBOL_DECL_GENERIC
 * and _SECTION_START/_SECTION_END to iterate the collected entries and verify:
 *   1. The total count matches the expected number (5 entries).
 *   2. Every expected value is present in the section.
 */

_SECTION_ATTR_SYMBOL_DECL_GENERIC(uint32_t, test_data_table)

/* Expected values — order is linker-determined, so we check membership */
static const uint32_t expected_values[] = {
    0xDEADBEEF,
    0xCAFEBABE,
    0x12345678,
    0xAAAAAAAA,
    0x55555555,
};

#define EXPECTED_COUNT (sizeof(expected_values) / sizeof(expected_values[0]))

void app_main(void)
{
    const uint32_t *start = _SECTION_START(test_data_table);
    const uint32_t *end   = _SECTION_END(test_data_table);

    /* --- Check 1: entry count -------------------------------------------- */
    size_t count = (size_t)(end - start);
    printf("Section entry count: %zu (expected %zu)\n", count, (size_t)EXPECTED_COUNT);

    if (count != EXPECTED_COUNT) {
        printf("FAIL: entry count mismatch\n");
        exit(1);
    }

    /* --- Check 2: every expected value is present ------------------------ */
    for (size_t i = 0; i < EXPECTED_COUNT; i++) {
        int found = 0;
        for (size_t j = 0; j < count; j++) {
            if (start[j] == expected_values[i]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("FAIL: expected value 0x%08X not found in section\n", expected_values[i]);
            exit(1);
        }
    }

    /* --- Check 3: no unexpected values ----------------------------------- */
    for (size_t j = 0; j < count; j++) {
        int known = 0;
        for (size_t i = 0; i < EXPECTED_COUNT; i++) {
            if (start[j] == expected_values[i]) {
                known = 1;
                break;
            }
        }
        if (!known) {
            printf("FAIL: unexpected value 0x%08X found in section\n", start[j]);
            exit(1);
        }
    }

    printf("SUCCESS: All %zu section entries verified.\n", count);
}
