// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_MBEDTLS_H_
#define _ESP_MBEDTLS_H_

#ifdef __NuttX__

#ifndef MBEDTLS_ALLOW_PRIVATE_ACCESS
#define MBEDTLS_PRIVATE(member) private_##member
#else
#define MBEDTLS_PRIVATE(member) member
#endif

/* PSA Crypto API - Core Functions */
#  define psa_crypto_init                           esp_psa_crypto_init

/* PSA Crypto API - Key Attributes */
#  define psa_key_attributes_init                   esp_psa_key_attributes_init
#  define psa_set_key_id                            esp_psa_set_key_id
#  define mbedtls_set_key_owner_id                  esp_mbedtls_set_key_owner_id
#  define psa_set_key_lifetime                      esp_psa_set_key_lifetime
#  define psa_get_key_id                            esp_psa_get_key_id
#  define psa_get_key_lifetime                      esp_psa_get_key_lifetime
#  define psa_set_key_usage_flags                   esp_psa_set_key_usage_flags
#  define psa_get_key_usage_flags                   esp_psa_get_key_usage_flags
#  define psa_set_key_algorithm                     esp_psa_set_key_algorithm
#  define psa_get_key_algorithm                     esp_psa_get_key_algorithm
#  define psa_set_key_type                          esp_psa_set_key_type
#  define psa_set_key_bits                          esp_psa_set_key_bits
#  define psa_get_key_type                          esp_psa_get_key_type
#  define psa_get_key_bits                          esp_psa_get_key_bits
#  define psa_get_key_attributes                    esp_psa_get_key_attributes
#  define psa_reset_key_attributes                  esp_psa_reset_key_attributes

/* PSA Crypto API - Key Management */
#  define psa_purge_key                             esp_psa_purge_key
#  define psa_copy_key                              esp_psa_copy_key
#  define psa_destroy_key                           esp_psa_destroy_key
#  define psa_import_key                            esp_psa_import_key
#  define psa_export_key                            esp_psa_export_key
#  define psa_export_public_key                     esp_psa_export_public_key

/* PSA Crypto API - Hash Operations */
#  define psa_hash_compute                          esp_psa_hash_compute
#  define psa_hash_compare                          esp_psa_hash_compare
#  define psa_hash_operation_init                   esp_psa_hash_operation_init
#  define psa_hash_setup                            esp_psa_hash_setup
#  define psa_hash_update                           esp_psa_hash_update
#  define psa_hash_finish                           esp_psa_hash_finish
#  define psa_hash_verify                           esp_psa_hash_verify
#  define psa_hash_abort                            esp_psa_hash_abort
#  define psa_hash_clone                            esp_psa_hash_clone

/* PSA Crypto API - MAC Operations */
#  define psa_mac_compute                           esp_psa_mac_compute
#  define psa_mac_verify                            esp_psa_mac_verify
#  define psa_mac_operation_init                    esp_psa_mac_operation_init
#  define psa_mac_sign_setup                        esp_psa_mac_sign_setup
#  define psa_mac_verify_setup                      esp_psa_mac_verify_setup
#  define psa_mac_update                            esp_psa_mac_update
#  define psa_mac_sign_finish                       esp_psa_mac_sign_finish
#  define psa_mac_verify_finish                     esp_psa_mac_verify_finish
#  define psa_mac_abort                             esp_psa_mac_abort

/* PSA Crypto API - Cipher Operations */
#  define psa_cipher_encrypt                        esp_psa_cipher_encrypt
#  define psa_cipher_decrypt                        esp_psa_cipher_decrypt
#  define psa_cipher_operation_init                 esp_psa_cipher_operation_init
#  define psa_cipher_encrypt_setup                  esp_psa_cipher_encrypt_setup
#  define psa_cipher_decrypt_setup                  esp_psa_cipher_decrypt_setup
#  define psa_cipher_generate_iv                    esp_psa_cipher_generate_iv
#  define psa_cipher_set_iv                         esp_psa_cipher_set_iv
#  define psa_cipher_update                         esp_psa_cipher_update
#  define psa_cipher_finish                         esp_psa_cipher_finish
#  define psa_cipher_abort                          esp_psa_cipher_abort

/* PSA Crypto API - AEAD Operations */
#  define psa_aead_encrypt                          esp_psa_aead_encrypt
#  define psa_aead_decrypt                          esp_psa_aead_decrypt
#  define psa_aead_operation_init                   esp_psa_aead_operation_init
#  define psa_aead_encrypt_setup                    esp_psa_aead_encrypt_setup
#  define psa_aead_decrypt_setup                    esp_psa_aead_decrypt_setup
#  define psa_aead_generate_nonce                   esp_psa_aead_generate_nonce
#  define psa_aead_set_nonce                        esp_psa_aead_set_nonce
#  define psa_aead_set_lengths                      esp_psa_aead_set_lengths
#  define psa_aead_update_ad                        esp_psa_aead_update_ad
#  define psa_aead_update                           esp_psa_aead_update
#  define psa_aead_finish                           esp_psa_aead_finish
#  define psa_aead_verify                           esp_psa_aead_verify
#  define psa_aead_abort                            esp_psa_aead_abort

/* PSA Crypto API - Asymmetric Signature */
#  define psa_sign_message                          esp_psa_sign_message
#  define psa_verify_message                        esp_psa_verify_message
#  define psa_sign_hash                             esp_psa_sign_hash
#  define psa_verify_hash                           esp_psa_verify_hash

/* PSA Crypto API - Asymmetric Encryption */
#  define psa_asymmetric_encrypt                    esp_psa_asymmetric_encrypt
#  define psa_asymmetric_decrypt                    esp_psa_asymmetric_decrypt

/* PSA Crypto API - Key Derivation */
#  define psa_key_derivation_operation_init         esp_psa_key_derivation_operation_init
#  define psa_key_derivation_setup                  esp_psa_key_derivation_setup
#  define psa_key_derivation_get_capacity           esp_psa_key_derivation_get_capacity
#  define psa_key_derivation_set_capacity           esp_psa_key_derivation_set_capacity
#  define psa_key_derivation_input_bytes            esp_psa_key_derivation_input_bytes
#  define psa_key_derivation_input_integer          esp_psa_key_derivation_input_integer
#  define psa_key_derivation_input_key              esp_psa_key_derivation_input_key
#  define psa_key_derivation_key_agreement          esp_psa_key_derivation_key_agreement
#  define psa_key_derivation_output_bytes           esp_psa_key_derivation_output_bytes
#  define psa_key_derivation_output_key             esp_psa_key_derivation_output_key
#  define psa_key_derivation_output_key_custom      esp_psa_key_derivation_output_key_custom
#  define psa_key_derivation_verify_bytes           esp_psa_key_derivation_verify_bytes
#  define psa_key_derivation_verify_key             esp_psa_key_derivation_verify_key
#  define psa_key_derivation_abort                  esp_psa_key_derivation_abort

/* PSA Crypto API - Key Agreement */
#  define psa_raw_key_agreement                     esp_psa_raw_key_agreement
#  define psa_key_agreement                         esp_psa_key_agreement

/* PSA Crypto API - Random Generation */
#  define psa_generate_random                       esp_psa_generate_random

/* PSA Crypto API - Key Generation */
#  define psa_generate_key                          esp_psa_generate_key
#  define psa_generate_key_custom                   esp_psa_generate_key_custom

/* PSA Crypto API - Interruptible Operations */
#  define psa_interruptible_set_max_ops             esp_psa_interruptible_set_max_ops
#  define psa_interruptible_get_max_ops             esp_psa_interruptible_get_max_ops
#  define psa_sign_hash_get_num_ops                 esp_psa_sign_hash_get_num_ops
#  define psa_verify_hash_get_num_ops               esp_psa_verify_hash_get_num_ops
#  define psa_sign_hash_start                       esp_psa_sign_hash_start
#  define psa_sign_hash_complete                    esp_psa_sign_hash_complete
#  define psa_sign_hash_abort                       esp_psa_sign_hash_abort
#  define psa_verify_hash_start                     esp_psa_verify_hash_start
#  define psa_verify_hash_complete                  esp_psa_verify_hash_complete
#  define psa_verify_hash_abort                     esp_psa_verify_hash_abort

/* PSA Crypto API - Interruptible Key Agreement */
#  define psa_key_agreement_iop_get_num_ops         esp_psa_key_agreement_iop_get_num_ops
#  define psa_key_agreement_iop_setup               esp_psa_key_agreement_iop_setup
#  define psa_key_agreement_iop_complete            esp_psa_key_agreement_iop_complete
#  define psa_key_agreement_iop_abort               esp_psa_key_agreement_iop_abort

/* PSA Crypto API - Interruptible Key Generation */
#  define psa_generate_key_iop_get_num_ops          esp_psa_generate_key_iop_get_num_ops
#  define psa_generate_key_iop_setup                esp_psa_generate_key_iop_setup
#  define psa_generate_key_iop_complete             esp_psa_generate_key_iop_complete
#  define psa_generate_key_iop_abort                esp_psa_generate_key_iop_abort

/* PSA Crypto API - Interruptible Export Public Key */
#  define psa_export_public_key_iop_get_num_ops     esp_psa_export_public_key_iop_get_num_ops
#  define psa_export_public_key_iop_setup           esp_psa_export_public_key_iop_setup
#  define psa_export_public_key_iop_complete        esp_psa_export_public_key_iop_complete
#  define psa_export_public_key_iop_abort           esp_psa_export_public_key_iop_abort

/* PSA Crypto Extra - Key Attributes Extensions */
#  define psa_set_key_enrollment_algorithm          esp_psa_set_key_enrollment_algorithm
#  define psa_get_key_enrollment_algorithm          esp_psa_get_key_enrollment_algorithm

/* PSA Crypto Extra - Library Management */
#  define mbedtls_psa_crypto_free                   esp_mbedtls_psa_crypto_free
#  define mbedtls_psa_get_stats                     esp_mbedtls_psa_get_stats

/* PSA Crypto Extra - External RNG */
#  define mbedtls_psa_external_get_random           esp_mbedtls_psa_external_get_random

/* PSA Crypto Extra - Built-in Keys */
#  define psa_key_id_is_builtin                     esp_psa_key_id_is_builtin
#  define mbedtls_psa_platform_get_builtin_key      esp_mbedtls_psa_platform_get_builtin_key

/* PSA Crypto Extra - PAKE Cipher Suite */
#  define psa_pake_cipher_suite_init                esp_psa_pake_cipher_suite_init
#  define psa_pake_cs_get_algorithm                 esp_psa_pake_cs_get_algorithm
#  define psa_pake_cs_set_algorithm                 esp_psa_pake_cs_set_algorithm
#  define psa_pake_cs_get_primitive                 esp_psa_pake_cs_get_primitive
#  define psa_pake_cs_set_primitive                 esp_psa_pake_cs_set_primitive
#  define psa_pake_cs_get_family                    esp_psa_pake_cs_get_family
#  define psa_pake_cs_get_bits                      esp_psa_pake_cs_get_bits
#  define psa_pake_cs_get_key_confirmation          esp_psa_pake_cs_get_key_confirmation
#  define psa_pake_cs_set_key_confirmation          esp_psa_pake_cs_set_key_confirmation

/* PSA Crypto Extra - PAKE Operations */
#  define psa_pake_operation_init                   esp_psa_pake_operation_init
#  define psa_crypto_driver_pake_get_password_len   esp_psa_crypto_driver_pake_get_password_len
#  define psa_crypto_driver_pake_get_password       esp_psa_crypto_driver_pake_get_password
#  define psa_crypto_driver_pake_get_user_len       esp_psa_crypto_driver_pake_get_user_len
#  define psa_crypto_driver_pake_get_peer_len       esp_psa_crypto_driver_pake_get_peer_len
#  define psa_crypto_driver_pake_get_user           esp_psa_crypto_driver_pake_get_user
#  define psa_crypto_driver_pake_get_peer           esp_psa_crypto_driver_pake_get_peer
#  define psa_crypto_driver_pake_get_cipher_suite   esp_psa_crypto_driver_pake_get_cipher_suite
#  define psa_pake_setup                            esp_psa_pake_setup
#  define psa_pake_set_user                         esp_psa_pake_set_user
#  define psa_pake_set_peer                         esp_psa_pake_set_peer
#  define psa_pake_set_role                         esp_psa_pake_set_role
#  define psa_pake_set_context                      esp_psa_pake_set_context
#  define psa_pake_output                           esp_psa_pake_output
#  define psa_pake_input                            esp_psa_pake_input
#  define psa_pake_get_shared_key                   esp_psa_pake_get_shared_key
#  define psa_pake_abort                            esp_psa_pake_abort

/* MbedTLS Extensions */
#  define esp_mbedtls_md5_init                      esp_md5_init
#  define esp_mbedtls_md5_update                    esp_md5_update
#  define esp_mbedtls_md5_finish                    esp_md5_finish
#  define esp_mbedtls_md5_starts                    esp_md5_starts
#  define esp_mbedtls_md5_free                      esp_md5_free
#  define esp_mbedtls_md5_clone                     esp_md5_clone
#  define esp_mbedtls_internal_md5_process          esp_md5_process

#  define mbedtls_aes_crypt_cbc                     esp_mbedtls_aes_crypt_cbc
#  define mbedtls_aes_crypt_ctr                     esp_mbedtls_aes_crypt_ctr
#  define mbedtls_aes_crypt_ecb                     esp_mbedtls_aes_crypt_ecb
#  define mbedtls_aes_free                          esp_mbedtls_aes_free
#  define mbedtls_aes_init                          esp_mbedtls_aes_init
#  define mbedtls_aes_setkey_dec                    esp_mbedtls_aes_setkey_dec
#  define mbedtls_aes_setkey_enc                    esp_mbedtls_aes_setkey_enc
#  define mbedtls_asn1_write_algorithm_identifier   esp_mbedtls_asn1_write_algorithm_identifier
#  define mbedtls_asn1_write_len                    esp_mbedtls_asn1_write_len
#  define mbedtls_asn1_write_oid                    esp_mbedtls_asn1_write_oid
#  define mbedtls_asn1_write_tag                    esp_mbedtls_asn1_write_tag
#  define mbedtls_ccm_encrypt_and_tag               esp_mbedtls_ccm_encrypt_and_tag
#  define mbedtls_ccm_free                          esp_mbedtls_ccm_free
#  define mbedtls_ccm_init                          esp_mbedtls_ccm_init
#  define mbedtls_ccm_setkey                        esp_mbedtls_ccm_setkey
#  define mbedtls_ccm_star_auth_decrypt             esp_mbedtls_ccm_star_auth_decrypt
#  define mbedtls_cipher_cmac_finish                esp_mbedtls_cipher_cmac_finish
#  define mbedtls_cipher_cmac_starts                esp_mbedtls_cipher_cmac_starts
#  define mbedtls_cipher_cmac_update                esp_mbedtls_cipher_cmac_update
#  define mbedtls_cipher_finish                     esp_mbedtls_cipher_finish
#  define mbedtls_cipher_free                       esp_mbedtls_cipher_free
#  define mbedtls_cipher_info_from_type             esp_mbedtls_cipher_info_from_type
#  define mbedtls_cipher_init                       esp_mbedtls_cipher_init
#  define mbedtls_cipher_reset                      esp_mbedtls_cipher_reset
#  define mbedtls_cipher_set_iv                     esp_mbedtls_cipher_set_iv
#  define mbedtls_cipher_set_padding_mode           esp_mbedtls_cipher_set_padding_mode
#  define mbedtls_cipher_setkey                     esp_mbedtls_cipher_setkey
#  define mbedtls_cipher_setup                      esp_mbedtls_cipher_setup
#  define mbedtls_cipher_update                     esp_mbedtls_cipher_update
#  define mbedtls_ctr_drbg_free                     esp_mbedtls_ctr_drbg_free
#  define mbedtls_ctr_drbg_init                     esp_mbedtls_ctr_drbg_init
#  define mbedtls_ctr_drbg_random                   esp_mbedtls_ctr_drbg_random
#  define mbedtls_ctr_drbg_seed                     esp_mbedtls_ctr_drbg_seed
#  define mbedtls_des_crypt_ecb                     esp_mbedtls_des_crypt_ecb
#  define mbedtls_des_free                          esp_mbedtls_des_free
#  define mbedtls_des_init                          esp_mbedtls_des_init
#  define mbedtls_des_setkey_enc                    esp_mbedtls_des_setkey_enc
#  define mbedtls_ecdh_calc_secret                  esp_mbedtls_ecdh_calc_secret
#  define mbedtls_ecdh_free                         esp_mbedtls_ecdh_free
#  define mbedtls_ecdh_gen_public                   esp_mbedtls_ecdh_gen_public
#  define mbedtls_ecdh_get_params                   esp_mbedtls_ecdh_get_params
#  define mbedtls_ecdh_init                         esp_mbedtls_ecdh_init
#  define mbedtls_ecdsa_free                        esp_mbedtls_ecdsa_free
#  define mbedtls_ecdsa_from_keypair                esp_mbedtls_ecdsa_from_keypair
#  define mbedtls_ecdsa_init                        esp_mbedtls_ecdsa_init
#  define mbedtls_ecdsa_read_signature              esp_mbedtls_ecdsa_read_signature
#  define mbedtls_ecdsa_sign                        esp_mbedtls_ecdsa_sign
#  define mbedtls_ecdsa_verify                      esp_mbedtls_ecdsa_verify
#  define mbedtls_ecp_check_pubkey                  esp_mbedtls_ecp_check_pubkey
#  define mbedtls_ecp_copy                          esp_mbedtls_ecp_copy
#  define mbedtls_ecp_curve_info_from_grp_id        esp_mbedtls_ecp_curve_info_from_grp_id
#  define mbedtls_ecp_curve_info_from_name          esp_mbedtls_ecp_curve_info_from_name
#  define mbedtls_ecp_gen_key                       esp_mbedtls_ecp_gen_key
#  define mbedtls_ecp_get_type                      esp_mbedtls_ecp_get_type
#  define mbedtls_ecp_group_a_is_minus_3            esp_mbedtls_ecp_group_a_is_minus_3
#  define mbedtls_ecp_group_copy                    esp_mbedtls_ecp_group_copy
#  define mbedtls_ecp_group_free                    esp_mbedtls_ecp_group_free
#  define mbedtls_ecp_group_init                    esp_mbedtls_ecp_group_init
#  define mbedtls_ecp_group_load                    esp_mbedtls_ecp_group_load
#  define mbedtls_ecp_is_zero                       esp_mbedtls_ecp_is_zero
#  define mbedtls_ecp_mul                           esp_mbedtls_ecp_mul
#  define mbedtls_ecp_muladd                        esp_mbedtls_ecp_muladd
#  define mbedtls_ecp_point_cmp                     esp_mbedtls_ecp_point_cmp
#  define mbedtls_ecp_point_free                    esp_mbedtls_ecp_point_free
#  define mbedtls_ecp_point_init                    esp_mbedtls_ecp_point_init
#  define mbedtls_ecp_point_read_binary             esp_mbedtls_ecp_point_read_binary
#  define mbedtls_ecp_point_write_binary            esp_mbedtls_ecp_point_write_binary
#  define mbedtls_entropy_free                      esp_mbedtls_entropy_free
#  define mbedtls_entropy_func                      esp_mbedtls_entropy_func
#  define mbedtls_entropy_init                      esp_mbedtls_entropy_init
#  define mbedtls_hardware_poll                     esp_mbedtls_hardware_poll
#  define mbedtls_md_finish                         esp_mbedtls_md_finish
#  define mbedtls_md_free                           esp_mbedtls_md_free
#  define mbedtls_md_get_type                       esp_mbedtls_md_get_type
#  define mbedtls_md_hmac_finish                    esp_mbedtls_md_hmac_finish
#  define mbedtls_md_hmac_starts                    esp_mbedtls_md_hmac_starts
#  define mbedtls_md_hmac_update                    esp_mbedtls_md_hmac_update
#  define mbedtls_md_info_from_ctx                  esp_mbedtls_md_info_from_ctx
#  define mbedtls_md_info_from_type                 esp_mbedtls_md_info_from_type
#  define mbedtls_md_init                           esp_mbedtls_md_init
#  define mbedtls_md_setup                          esp_mbedtls_md_setup
#  define mbedtls_md_starts                         esp_mbedtls_md_starts
#  define mbedtls_md_update                         esp_mbedtls_md_update
#  define mbedtls_mpi_add_int                       esp_mbedtls_mpi_add_int
#  define mbedtls_mpi_add_mpi                       esp_mbedtls_mpi_add_mpi
#  define mbedtls_mpi_bitlen                        esp_mbedtls_mpi_bitlen
#  define mbedtls_mpi_cmp_int                       esp_mbedtls_mpi_cmp_int
#  define mbedtls_mpi_cmp_mpi                       esp_mbedtls_mpi_cmp_mpi
#  define mbedtls_mpi_copy                          esp_mbedtls_mpi_copy
#  define mbedtls_mpi_div_int                       esp_mbedtls_mpi_div_int
#  define mbedtls_mpi_div_mpi                       esp_mbedtls_mpi_div_mpi
#  define mbedtls_mpi_exp_mod                       esp_mbedtls_mpi_exp_mod
#  define mbedtls_mpi_free                          esp_mbedtls_mpi_free
#  define mbedtls_mpi_get_bit                       esp_mbedtls_mpi_get_bit
#  define mbedtls_mpi_init                          esp_mbedtls_mpi_init
#  define mbedtls_mpi_inv_mod                       esp_mbedtls_mpi_inv_mod
#  define mbedtls_mpi_lset                          esp_mbedtls_mpi_lset
#  define mbedtls_mpi_mod_mpi                       esp_mbedtls_mpi_mod_mpi
#  define mbedtls_mpi_mul_mpi                       esp_mbedtls_mpi_mul_mpi
#  define mbedtls_mpi_random                        esp_mbedtls_mpi_random
#  define mbedtls_mpi_read_binary                   esp_mbedtls_mpi_read_binary
#  define mbedtls_mpi_shift_r                       esp_mbedtls_mpi_shift_r
#  define mbedtls_mpi_size                          esp_mbedtls_mpi_size
#  define mbedtls_mpi_sub_int                       esp_mbedtls_mpi_sub_int
#  define mbedtls_mpi_sub_mpi                       esp_mbedtls_mpi_sub_mpi
#  define mbedtls_mpi_write_binary                  esp_mbedtls_mpi_write_binary
#  define mbedtls_mpi_write_string                  esp_mbedtls_mpi_write_string
#  define mbedtls_nist_kw_free                      esp_mbedtls_nist_kw_free
#  define mbedtls_nist_kw_init                      esp_mbedtls_nist_kw_init
#  define mbedtls_nist_kw_setkey                    esp_mbedtls_nist_kw_setkey
#  define mbedtls_nist_kw_unwrap                    esp_mbedtls_nist_kw_unwrap
#  define mbedtls_nist_kw_wrap                      esp_mbedtls_nist_kw_wrap
#  define mbedtls_oid_get_oid_by_ec_grp             esp_mbedtls_oid_get_oid_by_ec_grp
#  define mbedtls_oid_get_oid_by_pk_alg             esp_mbedtls_oid_get_oid_by_pk_alg
#  define mbedtls_pk_can_do                         esp_mbedtls_pk_can_do
#  define mbedtls_pk_check_pair                     esp_mbedtls_pk_check_pair
#  define mbedtls_pk_copy_from_psa                  esp_mbedtls_pk_copy_from_psa
#  define mbedtls_pk_ec                             esp_mbedtls_pk_ec
#  define mbedtls_pk_free                           esp_mbedtls_pk_free
#  define mbedtls_pk_get_bitlen                     esp_mbedtls_pk_get_bitlen
#  define mbedtls_pk_get_psa_attributes             esp_mbedtls_pk_get_psa_attributes
#  define mbedtls_pk_get_type                       esp_mbedtls_pk_get_type
#  define mbedtls_pk_import_into_psa                esp_mbedtls_pk_import_into_psa
#  define mbedtls_pk_info_from_type                 esp_mbedtls_pk_info_from_type
#  define mbedtls_pk_init                           esp_mbedtls_pk_init
#  define mbedtls_pk_parse_key                      esp_mbedtls_pk_parse_key
#  define mbedtls_pk_parse_public_key               esp_mbedtls_pk_parse_public_key
#  define mbedtls_pk_parse_subpubkey                esp_mbedtls_pk_parse_subpubkey
#  define mbedtls_pk_rsa                            esp_mbedtls_pk_rsa
#  define mbedtls_pk_setup                          esp_mbedtls_pk_setup
#  define mbedtls_pk_write_key_der                  esp_mbedtls_pk_write_key_der
#  define mbedtls_pk_write_pubkey                   esp_mbedtls_pk_write_pubkey
#  define mbedtls_pk_write_pubkey_der               esp_mbedtls_pk_write_pubkey_der
#  define mbedtls_pkcs5_pbkdf2_hmac_ext             esp_mbedtls_pkcs5_pbkdf2_hmac_ext
#  define mbedtls_pkcs5_pbkdf2_hmac                 esp_mbedtls_pkcs5_pbkdf2_hmac
#  define mbedtls_platform_zeroize                  esp_mbedtls_platform_zeroize
#  define mbedtls_psa_get_random                    esp_mbedtls_psa_get_random
#  define mbedtls_ecc_group_to_psa                  esp_mbedtls_ecc_group_to_psa
#  define mbedtls_ecc_group_from_psa                esp_mbedtls_ecc_group_from_psa
#  define mbedtls_md_psa_alg_from_type              esp_mbedtls_md_psa_alg_from_type
#  define mbedtls_md_type_from_psa_alg              esp_mbedtls_md_type_from_psa_alg
#  define mbedtls_ecdsa_raw_to_der                  esp_mbedtls_ecdsa_raw_to_der
#  define mbedtls_ecdsa_der_to_raw                  esp_mbedtls_ecdsa_der_to_raw
#  define mbedtls_rsa_copy                          esp_mbedtls_rsa_copy
#  define mbedtls_rsa_get_len                       esp_mbedtls_rsa_get_len
#  define mbedtls_rsa_pkcs1_encrypt                 esp_mbedtls_rsa_pkcs1_encrypt
#  define mbedtls_rsa_pkcs1_sign                    esp_mbedtls_rsa_pkcs1_sign
#  define mbedtls_rsa_public                        esp_mbedtls_rsa_public
#  define mbedtls_rsa_rsaes_pkcs1_v15_decrypt       esp_mbedtls_rsa_rsaes_pkcs1_v15_decrypt
#  define mbedtls_sha256_finish                     esp_mbedtls_sha256_finish
#  define mbedtls_sha256_free                       esp_mbedtls_sha256_free
#  define mbedtls_sha256_init                       esp_mbedtls_sha256_init
#  define mbedtls_sha256_starts                     esp_mbedtls_sha256_starts
#  define mbedtls_sha256_update                     esp_mbedtls_sha256_update
#  define mbedtls_sha512_finish                     esp_mbedtls_sha512_finish
#  define mbedtls_sha512_free                       esp_mbedtls_sha512_free
#  define mbedtls_sha512_init                       esp_mbedtls_sha512_init
#  define mbedtls_sha512_starts                     esp_mbedtls_sha512_starts
#  define mbedtls_sha512_update                     esp_mbedtls_sha512_update
#  define mbedtls_ssl_conf_authmode                 esp_mbedtls_ssl_conf_authmode
#  define mbedtls_ssl_conf_ca_chain                 esp_mbedtls_ssl_conf_ca_chain
#  define mbedtls_ssl_conf_cert_profile             esp_mbedtls_ssl_conf_cert_profile
#  define mbedtls_ssl_conf_ciphersuites             esp_mbedtls_ssl_conf_ciphersuites
#  define mbedtls_ssl_conf_own_cert                 esp_mbedtls_ssl_conf_own_cert
#  define mbedtls_ssl_conf_rng                      esp_mbedtls_ssl_conf_rng
#  define mbedtls_ssl_conf_sig_algs                 esp_mbedtls_ssl_conf_sig_algs
#  define mbedtls_ssl_config_defaults               esp_mbedtls_ssl_config_defaults
#  define mbedtls_ssl_config_free                   esp_mbedtls_ssl_config_free
#  define mbedtls_ssl_config_init                   esp_mbedtls_ssl_config_init
#  define mbedtls_ssl_free                          esp_mbedtls_ssl_free
#  define mbedtls_ssl_get_ciphersuite               esp_mbedtls_ssl_get_ciphersuite
#  define mbedtls_ssl_get_version                   esp_mbedtls_ssl_get_version
#  define mbedtls_ssl_handshake_step                esp_mbedtls_ssl_handshake_step
#  define mbedtls_ssl_init                          esp_mbedtls_ssl_init
#  define mbedtls_ssl_is_handshake_over             esp_mbedtls_ssl_is_handshake_over
#  define mbedtls_ssl_read                          esp_mbedtls_ssl_read
#  define mbedtls_ssl_session_reset                 esp_mbedtls_ssl_session_reset
#  define mbedtls_ssl_set_bio                       esp_mbedtls_ssl_set_bio
#  define mbedtls_ssl_set_export_keys_cb            esp_mbedtls_ssl_set_export_keys_cb
#  define mbedtls_ssl_set_hostname                  esp_mbedtls_ssl_set_hostname
#  define mbedtls_ssl_setup                         esp_mbedtls_ssl_setup
#  define mbedtls_ssl_tls_prf                       esp_mbedtls_ssl_tls_prf
#  define mbedtls_ssl_write                         esp_mbedtls_ssl_write
#  define mbedtls_ssl_conf_max_tls_version          esp_mbedtls_ssl_conf_max_tls_version
#  define mbedtls_timing_get_delay                  esp_mbedtls_timing_get_delay
#  define mbedtls_timing_get_timer                  esp_mbedtls_timing_get_timer
#  define mbedtls_timing_set_delay                  esp_mbedtls_timing_set_delay
#  define mbedtls_x509_crt_free                     esp_mbedtls_x509_crt_free
#  define mbedtls_x509_crt_init                     esp_mbedtls_x509_crt_init
#  define mbedtls_x509_crt_parse                    esp_mbedtls_x509_crt_parse
#  define mbedtls_x509_crt_verify                   esp_mbedtls_x509_crt_verify
#  define mbedtls_x509_crt_verify_info              esp_mbedtls_x509_crt_verify_info
#endif

#endif /* _ESP_MBEDTLS_H_ */
