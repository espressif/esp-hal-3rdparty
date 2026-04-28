# esp-hal-3rdparty

HAL (Hardware Abstraction Layer) components - including Wi-Fi drivers - for Espressif chips intended to be used by 3rd Party Frameworks, like NuttX and Zephyr.

This repository provides automatically synchronized branches from ESP-IDF, updated by scripts according to specific rules.

Except for the synchronization scripts, no manual development occurs in this branch.

> **Warning**
> Please don't open any issues in this repository. It's intended to be used by NuttX or Zephyr.
> If you find anything you want to report, please report an issue on the respective OS repository.

## Sync Branches: `sync/[branch].[name]`

These are the automatically updated synchronization branches.

Where:

- `sync` indicates this is a branch maintained by the synchronization script
- `[branch]`: the ESP-IDF branch this branch is synchronized from, e.g., `master`, `release/v5.0`
- `[name]`: either a letter identifier (corresponding to a predefined component list), or a single component name for specific use cases

### Active Branches

- [`sync/master.c`](../../tree/sync/master.c):
    - Based on ESP-IDF `master` branch
    - Includes components defined at [`tools/sync_master.c`](../../tree/main/tools/sync_master.c.txt)

### Deprecated Branches

The following branches are deprecated and no longer updated:

- [`sync-1-release_v5.1`](../../tree/sync-1-release_v5.1):
    - Based on ESP-IDF `release/v5.1` branch
    - Includes components: `bootloader_support`, `efuse`, `esp_app_format`, `esp_common`,
    `esp_event`, `esp_hw_support`, `esp_phy`, `esp_rom`, `esp_system`, `esp_timer`, `esp_wifi`,
    `hal`, `log`, `mbedtls`, `newlib`, `riscv`, `soc`, `spi_flash`, `wpa_supplicant`, `xtensa`

- [`sync-2-release_v5.1`](../../tree/sync-2-release_v5.1):
    - Based on ESP-IDF `release/v5.1` branch
    - Includes components: `bootloader_support`, `bt`, `efuse`, `esp_app_format`, `esp_common`,
    `esp_event`, `esp_hw_support`, `esp_phy`, `esp_rom`, `esp_system`, `esp_timer`, `esp_wifi`,
    `hal`, `log`, `mbedtls`, `newlib`, `riscv`, `soc`, `spi_flash`, `wpa_supplicant`, `xtensa`

- [`sync-3-release_v5.1`](../../tree/sync-3-release_v5.1):
    - Based on ESP-IDF `release/v5.1` branch
    - Includes components: `bootloader`, `bootloader_support`, `bt`, `efuse`, `esp_adc`,
    `esp_app_format`, `esp_coex`, `esp_common`, `esp_event`, `esp_hw_support`, `esp_phy`,
    `esp_rom`, `esp_system`, `esp_timer`, `esp_wifi`, `hal`, `log`, `mbedtls`, `newlib`, `riscv`,
    `soc`, `spi_flash`, `wpa_supplicant`, `xtensa`

- [`sync/release_v5.1.b`](../../tree/sync/release_v5.1.b):
    - Based on ESP-IDF `release/v5.1` branch
    - Includes components: `bootloader`, `bootloader_support`, `bt`, `efuse`, `esp_adc`,
    `esp_app_format`, `esp_coex`, `esp_common`, `esp_event`, `esp_hw_support`, `esp_phy`,
    `esp_rom`, `esp_system`, `esp_timer`, `esp_wifi`, `hal`, `log`, `mbedtls`, `newlib`, `riscv`,
    `soc`, `spi_flash`, `wpa_supplicant`, `xtensa`

- [`sync/release_v5.1.c`](../../tree/sync/release_v5.1.c):
    - Based on ESP-IDF `release/v5.1` branch
    - Includes components: `bootloader`, `bootloader_support`, `bt`, `efuse`, `esp_adc`,
    `esp_app_format`, `esp_coex`, `esp_common`, `esp_event`, `esp_hw_support`, `esp_phy`,
    `esp_rom`, `esp_system`, `esp_timer`, `esp_wifi`, `hal`, `log`, `mbedtls`, `newlib`,
    `partition_table`, `riscv`, `soc`, `spi_flash`, `wpa_supplicant`, `xtensa`

- [`sync/release_v5.2.a`](../../tree/sync/release_v5.2.a):
    - Based on ESP-IDF `release/v5.2` branch
    - Includes components: `bootloader`, `bootloader_support`, `bt`, `efuse`, `esp_adc`,
    `esp_app_format`, `esp_coex`, `esp_common`, `esp_event`, `esp_hw_support`, `esp_phy`,
    `esp_rom`, `esp_system`, `esp_timer`, `esp_wifi`, `hal`, `log`, `mbedtls`, `newlib`,
    `partition_table`, `riscv`, `soc`, `spi_flash`, `wpa_supplicant`, `xtensa`

- [`sync/master.a`](../../tree/sync/master.a):
    - Based on ESP-IDF `master` branch
    - Includes components: `bootloader`, `bootloader_support`, `bt`, `efuse`, `esp_adc`,
      `esp_app_format`, `esp_coex`, `esp_common`, `esp_event`, `esp_hw_support`, `esp_mm`,
      `esp_phy`, `esp_pm`, `esp_psram`, `esp_rom`, `esp_system`, `esp_timer`, `esp_wifi`,
      `hal`, `heap`, `ieee802154`, `log`, `mbedtls`, `newlib`, `partition_table`, `riscv`,
      `soc`, `spi_flash`, `ulp`, `usb`, `wpa_supplicant`, `xtensa`

## Release Branches: `release/[branch]`

These are release branches intended to be used by 3rd Party Frameworks, such as NuttX. These branches include modifications made on top of a sync branch to enable compatibility with specific operating systems.

Where:
- `[branch]` refers to the synchronization branch (as listed above) from which this release branch is derived

### Active Branches

- [`release/master.c`](../../tree/release/master.c):
    - Based on [`sync/master.c`](../../tree/sync/master.c) branch
    - Currently used by NuttX, referenced by commit SHA

### Deprecated Branches

- [`release/v5.1`](../../tree/release/v5.1):
    - Based on [`sync-2-release_v5.1`](../../tree/sync-2-release_v5.1) branch
    - Deprecated: referenced by branch name on NuttX
- [`release/v5.1.a`](../../tree/release/v5.1.a):
    - Based on [`sync-2-release_v5.1`](../../tree/sync-2-release_v5.1) branch
    - Deprecated: referenced by commit SHA
- [`release/v5.1.b`](../../tree/release/v5.1.b):
    - Based on [`sync/release_v5.1.b`](../../tree/sync/release_v5.1.b) branch
    - Deprecated: referenced by commit SHA
- [`release/v5.1.c`](../../tree/release/v5.1.c):
    - Based on [`sync/release_v5.1.c`](../../tree/sync/release_v5.1.c) branch
    - Deprecated: referenced by commit SHA
- [`release/master.a`](../../tree/release/master.a):
    - Based on [`sync/master.a`](../../tree/sync/master.a) branch
    - Deprecated: referenced by commit SHA
- [`release/master.b`](../../tree/release/master.b):
    - Based on [`sync/master.b`](../../tree/sync/master.b) branch
    - Currently used by NuttX, referenced by commit SHA

## General Restrictions

1. **Sync branches don't have common ancestors**

   This may cause problems when you merge or cherry-pick from these branches.

2. **File list modifications are not easily supported for existing branches**

   The synchronization script converts ESP-IDF commits into new commits on the target branch. The history is preserved while files are filtered according to the specified component list.

   The generated commits will have the same SHA as long as the commit author, date, message, and change list remain identical.

   Any modification to the synchronization strategy of a sync branch (including modifying the component list) will change the SHA of the commits, preventing the generated branch from being merged (pushed) to the existing branch.

   When modifying the component list or any other aspect of the synchronization process, it's recommended to create a new sync branch.
