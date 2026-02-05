/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/regi2c_impl.h"
#include "hal/assert.h"

uint8_t _regi2c_impl_read(uint8_t block, uint8_t host_id, uint8_t reg_add)
{
    // TODO: [ESP32S31] IDF-14680
    return 0;
}

uint8_t _regi2c_impl_read_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb)
{
    // TODO: [ESP32S31] IDF-14680
    return 0;
}

void _regi2c_impl_write(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data)
{
    // TODO: [ESP32S31] IDF-14680
}

void _regi2c_impl_write_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data)
{
    // TODO: [ESP32S31] IDF-14680
}
