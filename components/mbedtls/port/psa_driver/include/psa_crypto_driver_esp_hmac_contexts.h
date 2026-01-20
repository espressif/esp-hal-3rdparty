/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_SHA_DRIVER_ENABLED)

#include "psa_crypto_driver_esp_sha_contexts.h"
#include "psa_crypto_driver_esp_sha.h"

typedef struct {
    uint8_t opad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
    psa_algorithm_t alg;
    esp_sha_hash_operation_t esp_sha_ctx;
} esp_hmac_operation_t;

#endif /* ESP_SHA_DRIVER_ENABLED */

#ifdef __cplusplus
}
#endif
