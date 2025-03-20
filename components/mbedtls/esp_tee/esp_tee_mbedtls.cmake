idf_component_get_property(heap_dir heap COMPONENT_DIR)

set(priv_requires esp_hw_support hal soc)

set(include_dirs "${COMPONENT_DIR}/port/include"
                 "${COMPONENT_DIR}/mbedtls/include"
                 "${COMPONENT_DIR}/mbedtls/library")

# Supporting headers
list(APPEND include_dirs "${heap_dir}/include")

# Shared GDMA layer for TEE
set(srcs "${COMPONENT_DIR}/esp_tee/esp_tee_crypto_shared_gdma.c")

# AES-SHA implementation
list(APPEND srcs "${COMPONENT_DIR}/port/aes/dma/esp_aes.c"
                 "${COMPONENT_DIR}/port/aes/dma/esp_aes_dma_core.c")

list(APPEND srcs "${COMPONENT_DIR}/port/aes/esp_aes_common.c"
                 "${COMPONENT_DIR}/port/aes/esp_aes_gcm.c")

list(APPEND srcs "${COMPONENT_DIR}/port/sha/core/sha.c"
                 "${COMPONENT_DIR}/port/sha/esp_sha.c")

list(APPEND include_dirs "${COMPONENT_DIR}/port/aes/include"
                         "${COMPONENT_DIR}/port/aes/dma/include"
                         "${COMPONENT_DIR}/port/sha/core/include")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_REQUIRES "${priv_requires}")

# Only build mbedtls libraries
set(ENABLE_TESTING CACHE BOOL OFF)
set(ENABLE_PROGRAMS CACHE BOOL OFF)

# Use pre-generated source files in mbedtls repository
set(GEN_FILES CACHE BOOL OFF)

# Needed to for include_next includes to work from within mbedtls
include_directories("${COMPONENT_DIR}/port/include")

# Import mbedtls library targets
add_subdirectory(mbedtls)

set(mbedtls_targets mbedcrypto)

foreach(target ${mbedtls_targets})
    target_compile_definitions(${target} PUBLIC
                                    -DMBEDTLS_CONFIG_FILE="${COMPONENT_DIR}/esp_tee/esp_tee_mbedtls_config.h")
endforeach()

target_link_libraries(${COMPONENT_LIB} INTERFACE ${mbedtls_targets})

target_sources(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/sha/core/esp_sha1.c"
                                  "${COMPONENT_DIR}/port/sha/core/esp_sha256.c"
                                  "${COMPONENT_DIR}/port/sha/core/esp_sha512.c")
