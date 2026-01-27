/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "psa/crypto_driver_common.h"
#include "psa_crypto_driver_esp_sha_contexts.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_HMAC_TRANSPARENT_DRIVER_ENABLED)
typedef struct {
    uint8_t opad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
    psa_algorithm_t alg;
    esp_sha_hash_operation_t esp_sha_ctx;
} esp_hmac_transparent_operation_t;
#endif /* ESP_HMAC_TRANSPARENT_DRIVER_ENABLED */

#ifdef __cplusplus
}
#endif
