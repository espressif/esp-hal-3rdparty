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

#include "psa/crypto.h"
#include "psa_crypto_driver_esp_hmac_contexts.h"

psa_status_t esp_hmac_compute(const psa_key_attributes_t *attributes,
                                const uint8_t *key_buffer,
                                size_t key_buffer_size,
                                psa_algorithm_t alg,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *mac,
                                size_t mac_size,
                                size_t *mac_length);

psa_status_t esp_hmac_abort(esp_hmac_operation_t *esp_hmac_ctx);

psa_status_t esp_hmac_setup(esp_hmac_operation_t *esp_hmac_ctx,
                            const psa_key_attributes_t *attributes,
                            const uint8_t *key_buffer,
                            size_t key_buffer_size,
                            psa_algorithm_t alg);

psa_status_t esp_hmac_update(esp_hmac_operation_t *esp_hmac_ctx,
                            const uint8_t *data,
                            size_t data_length);

psa_status_t esp_hmac_finish(esp_hmac_operation_t *esp_hmac_ctx,
                            uint8_t *mac,
                            size_t mac_size,
                            size_t *mac_length);

psa_status_t esp_hmac_verify_finish(esp_hmac_operation_t *esp_hmac_ctx,
                                    const uint8_t *mac,
                                    size_t mac_length);

#endif /* ESP_SHA_DRIVER_ENABLED */

#ifdef __cplusplus
}
#endif
