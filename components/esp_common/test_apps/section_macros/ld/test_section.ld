/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Linker script for the section macros test on Linux host builds.
 * Collects all uint32_t entries placed in the .test_data_table section.
 */

SECTIONS
{
    .test_data_table :
    {
        PROVIDE(_test_data_table_start = .);
        KEEP(*(SORT(.test_data_table*)))
        PROVIDE(_test_data_table_end = .);
    }
}
INSERT AFTER .data;
