/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "psa/crypto.h"
#include "psa_crypto_driver_esp_ds_contexts.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_DS_DRIVER_ENABLED)
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */
#endif /* ESP_DS_DRIVER_ENABLED */

#define PSA_CRYPTO_ESP_DS_DRIVER_LOCATION ((psa_key_location_t) 0x800002)


#define PSA_KEY_LIFETIME_ESP_DS \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
        PSA_KEY_PERSISTENCE_VOLATILE, \
        PSA_CRYPTO_ESP_DS_DRIVER_LOCATION)

#ifdef SOC_DIG_SIGN_SUPPORTED
psa_status_t esp_ds_opaque_sign_hash_start(
    esp_ds_opaque_sign_hash_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length);

psa_status_t esp_ds_opaque_sign_hash_complete(
    esp_ds_opaque_sign_hash_operation_t *operation,
    uint8_t *signature, size_t signature_size,
    size_t *signature_length);

psa_status_t esp_ds_opaque_sign_hash_abort(
    esp_ds_opaque_sign_hash_operation_t *operation);

psa_status_t esp_ds_opaque_signature_sign_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    uint8_t *signature,
    size_t signature_size,
    size_t *signature_length
);

psa_status_t esp_ds_opaque_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits);

size_t esp_ds_opaque_size_function(
    psa_key_type_t key_type,
    size_t key_bits);

void esp_ds_opaque_set_session_timeout(int timeout_ms);

psa_status_t esp_ds_opaque_asymmetric_decrypt(
    const psa_key_attributes_t *attributes, const uint8_t *key,
    size_t key_length, psa_algorithm_t alg, const uint8_t *input,
    size_t input_length, const uint8_t *salt, size_t salt_length,
    uint8_t *output, size_t output_size, size_t *output_length);
#endif /* SOC_DIG_SIGN_SUPPORTED */
#ifdef __cplusplus
}
#endif
