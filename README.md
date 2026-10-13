# `ESP-HAL-3RDPARTY`

Hardware Abstraction Layer (HAL) for Espressif SoCs, including Wi-Fi, used by NuttX.

The `master` branch is the one NuttX clones and builds. It continues the former [`release/master.c`](https://github.com/espressif/esp-hal-3rdparty/tree/release/master.c) branch. Contributors work directly on `master`. Automatic `sync/*` branches from that repository are not published here.

## Origin

The [`components`](./components/) folder contains code derived from ESP-IDF **6.1.0** (ESP-IDF `master`). That version is `ESP_IDF_VERSION` in [`components/esp_common/include/esp_idf_version.h`](components/esp_common/include/esp_idf_version.h).

NuttX imported that tree on 2026-04-28 ([`c26b7deea4a8ad9ce84f2448698b3b09f3ce203f`](../../../commit/c26b7deea4a8ad9ce84f2448698b3b09f3ce203f), tag `sync/master.c-nuttx-20260428`). The result was published as `release/master.c`, which is now `master`.

> **Note**
> Whenever code is brought from ESP-IDF, update the version recorded here, and keep `esp_idf_version.h` aligned with the imported sources. The next reader must be able to tell which ESP-IDF tree `components/` follows.

## How to Use this Repository

NuttX's build system clones this repository and builds Espressif SoCs from these sources. Clone with git submodules: a source archive leaves them out.

### Organization

#### `components`

The [`components`](./components/) folder contains the ESP-IDF-derived components and the small additions required to build them on NuttX. Some of these components are git submodules.

Keep `components/` as close as possible to the original ESP-IDF code. Leave the ESP-IDF logic in place.

- Implement OS-agnostic versions of OS-backed primitives (locks, timers, memory, critical sections, and similar) under [`nuttx`](./nuttx). The current layer is [`nuttx/include/platform/os.h`](nuttx/include/platform/os.h) and [`nuttx/src/platform/os.c`](nuttx/src/platform/os.c).
- When a change in `components/` is necessary, limit it to calling those primitives. Guard NuttX-only differences with `__NuttX__`.
- When a file must be completely reimplemented, add it under [`nuttx/src/components`](nuttx/src/components).

> **Note**
> Not all sources under `components` are required to build NuttX: it's feature-dependent. Please check which sources are built on NuttX when building, for instance, ESP32-S3's Wi-Fi: [nuttx/arch/xtensa/src/esp32s3/Wireless.mk](https://github.com/apache/nuttx/blob/master/arch/xtensa/src/esp32s3/Wireless.mk).

#### `nuttx`

The [`nuttx`](./nuttx) directory contains the following sub-directories:

##### `nuttx/include`

Includes headers that are used specifically to build NuttX that are not available in `components` (or that can't be directly added there), including API translations, macro (re)-definitions, and other necessary files that are meant to interface with sources and header files in `components`.

##### `nuttx/<chip>/include`

Similar to `nuttx/include`, but intended to be used specifically by a particular chip.

##### `nuttx/src/components`

NuttX reimplementations of component sources that cannot stay close to ESP-IDF. This is the place for a full replacement of a file from `components/`.

##### `nuttx/patches`

Provide patches to be applied in the source code when it isn't suitable to directly change it. Specifically, patches are used to change the source code of git submodules. The Mbed TLS is a submodule in the [`components/mbedtls/mbedtls`](./components/mbedtls/mbedtls) folder, and directly changing its source would require keeping a separate remote for Mbed TLS. Instead, patches are applied before building Mbed TLS.

> **Note**
> The path under `nuttx/patches` is related to the path where the patch is meant to be applied: [`nuttx/patches/components/mbedtls/mbedtls/0001-mbedtls_add_prefix.patch`](nuttx/patches/components/mbedtls/mbedtls/0001-mbedtls_add_prefix.patch), for instance, is meant to be applied at [`components/mbedtls/mbedtls/`](components/mbedtls/mbedtls/).

## Updating Code from ESP-IDF

When `components/` needs a newer ESP-IDF baseline, import that update as a **merge commit** on `master`.

On that merge commit:

1. Update the ESP-IDF version recorded in [Origin](#origin).
2. Keep the imported files under `components/` aligned with ESP-IDF. Restore only the edits that switch them over to the primitives in `nuttx/`.
3. Place a full reimplementation under `nuttx/src/components`.
4. Point git submodules at the imported hashes, and update patches under `nuttx/patches/` when they no longer apply. Recheck the Mbed TLS prefix patches as described in [Mbed TLS Symbol Collisions](#mbed-tls-symbol-collisions).

## Workarounds

This section explains the workarounds for using the components on NuttX.

### Mbed TLS Symbol Collisions

The ESP32 SoC family makes use of Mbed TLS to implement [wpa_supplicant crypto functions](components/wpa_supplicant/src/crypto). This same application may be present on 3rd party platforms. This is true for NuttX, for example. To provide complete userspace/kernel separation and to avoid problems regarding the Mbed TLS version, the ESP32 implementation builds a custom version of Mbed TLS. To avoid symbol collision if NuttX's Mbed TLS is used, functions and global variables with external linkage from the ESP32-custom Mbed TLS are prefixed.

This is done through patches that apply the prefix. See [`nuttx/patches/components/mbedtls/mbedtls`](nuttx/patches/components/mbedtls/mbedtls) for the patches applied to the Mbed TLS submodule used by the Wi-Fi driver.

Whenever code is brought from ESP-IDF, check whether these patches still apply. An import often moves the Mbed TLS submodule, and the prefix patches then fail at build time. Regenerate them before merging.

The symbol list is [`utils/ctags/mbedtls/tags`](utils/ctags/mbedtls/tags). [`utils/prefixer.sh`](utils/prefixer.sh) uses it to add the `esp_` prefix, and the result is saved as `0001-mbedtls_add_prefix.patch`. Refresh the tags file when the submodule changes. This needs [Universal Ctags](https://github.com/universal-ctags/ctags), which provides `ctags` and `readtags`.

From the repository root, with the submodule checked out at the new commit:

```bash
git -C components/mbedtls/mbedtls reset --hard
ctags -f utils/ctags/mbedtls/tags --kinds-c=fvp \
    components/mbedtls/mbedtls/library/*.c \
    components/mbedtls/mbedtls/library/*.h \
    components/mbedtls/mbedtls/include/mbedtls/*.h \
    components/mbedtls/mbedtls/tf-psa-crypto/core/*.c \
    components/mbedtls/mbedtls/tf-psa-crypto/core/*.h \
    components/mbedtls/mbedtls/tf-psa-crypto/drivers/builtin/src/*.c \
    components/mbedtls/mbedtls/tf-psa-crypto/drivers/builtin/src/*.h \
    components/mbedtls/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/*.h \
    components/mbedtls/mbedtls/tf-psa-crypto/include/psa/*.h
( cd utils && ./prefixer.sh ctags/mbedtls/tags ../components/mbedtls/mbedtls )
git -C components/mbedtls/mbedtls diff --full-index --binary \
    > nuttx/patches/components/mbedtls/mbedtls/0001-mbedtls_add_prefix.patch
git -C components/mbedtls/mbedtls reset --hard
```

Ctags records only the first branch of a `#if`. Names hidden by the preprocessor, and names introduced by macros, stay unprefixed. Those edits live in [`0002-mbedtls_add_prefix_to_macro.patch`](nuttx/patches/components/mbedtls/mbedtls/0002-mbedtls_add_prefix_to_macro.patch). Review that patch after regenerating `0001`, then commit the tags file and both patches.
