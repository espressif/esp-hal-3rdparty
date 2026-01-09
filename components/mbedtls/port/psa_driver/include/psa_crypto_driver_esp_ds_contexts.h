/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "psa/crypto.h"
#ifdef SOC_DIG_SIGN_SUPPORTED
#include "esp_ds.h"
#endif /* SOC_DIG_SIGN_SUPPORTED */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_DS_PADDING_PKCS_V15,
    ESP_DS_PADDING_PSS,
    ESP_DS_PADDING_OAEP,
} esp_ds_padding_t;

#ifdef SOC_DIG_SIGN_SUPPORTED
typedef struct {
    esp_ds_data_t *esp_ds_data;
    uint8_t efuse_key_id; /* efuse block id in which DS_KEY is stored e.g. 0,1*/
    uint16_t rsa_length_bits; /* length of RSA private key in bits e.g. 2048 */
} esp_ds_data_ctx_t;

typedef struct {
    const esp_ds_data_ctx_t *esp_ds_opaque_key; /*!< Pointer to the esp ds opaque key */
    psa_algorithm_t alg;             /*!< Algorithm used in the sign operation */
    uint32_t *sig_buffer;          /*!< Buffer to hold the signature */
    size_t sig_buffer_size;        /*!< Size of the signature buffer */
    esp_ds_context_t *esp_ds_ctx;    /*!< Pointer to the esp ds context */
    esp_ds_data_t esp_ds_data;
} esp_ds_opaque_sign_hash_operation_t;
#endif /* SOC_DIG_SIGN_SUPPORTED */

#ifdef __cplusplus
}
#endif
