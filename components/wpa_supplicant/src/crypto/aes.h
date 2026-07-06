/*
 * AES functions
 * Copyright (c) 2003-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef AES_H
#define AES_H

#ifdef __NuttX__
#include "utils/common.h"
#define aes_encrypt_init esp_wpa_aes_encrypt_init
#define aes_encrypt esp_wpa_aes_encrypt
#define aes_encrypt_deinit esp_wpa_aes_encrypt_deinit
#define aes_decrypt_init esp_wpa_aes_decrypt_init
#define aes_decrypt esp_wpa_aes_decrypt
#define aes_decrypt_deinit esp_wpa_aes_decrypt_deinit
#endif

#define AES_BLOCK_SIZE 16

void * aes_encrypt_init(const u8 *key, size_t len);
int aes_encrypt(void *ctx, const u8 *plain, u8 *crypt);
void aes_encrypt_deinit(void *ctx);
void * aes_decrypt_init(const u8 *key, size_t len);
int aes_decrypt(void *ctx, const u8 *crypt, u8 *plain);
void aes_decrypt_deinit(void *ctx);

#endif /* AES_H */
