/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include <stdint.h>

/*
 * Three entries placed in the test_data_table section from translation unit A.
 * The test verifies these values are discoverable at link time.
 */
static const uint32_t entry_a1 PLACE_IN_SECTION("test_data_table") = 0xDEADBEEF;
static const uint32_t entry_a2 PLACE_IN_SECTION("test_data_table") = 0xCAFEBABE;
static const uint32_t entry_a3 PLACE_IN_SECTION("test_data_table") = 0x12345678;
