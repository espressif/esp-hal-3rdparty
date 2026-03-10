/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include <stdint.h>

/*
 * Two entries placed in the test_data_table section from a DIFFERENT
 * translation unit.  This verifies the linker collects entries across
 * multiple object files.
 */
static const uint32_t entry_b1 PLACE_IN_SECTION("test_data_table") = 0xAAAAAAAA;
static const uint32_t entry_b2 PLACE_IN_SECTION("test_data_table") = 0x55555555;
