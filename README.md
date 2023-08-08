# esp-hal-3rdparty

HAL (Hardware Abstraction Layer) components - including Wi-Fi drivers - for Espressif chips intended to be used by 3rd Party Frameworks, like NuttX and Zephyr.

This repository provides automatically synced branches from IDF, updated by scripts under given rules.

Except for the scripts, no manual development happens in this branch.

> **Warning**
> Please don't open any issues in this repository. It's intended to be used by NuttX or Zephyr.
> If you find anything you want to report, please report an issue on the OS repository.

## Branch naming convention

### sync/[branch].[name]

These are the automatically updated sync branches.

Where:

- `sync` means this is a branch maintained by the script.
- `[branch]`: the IDF branch the branch is synced from, e.g. `master`, `release/v5.0`
- `[name]`: either a letter (whose file list will be described somewhere), or a single component (in case someone needs it...)

#### Existing branches

- [`sync/release_v5.1.b`](../../tree/sync/release_v5.1.b):
    - Based on ESP-IDF `release/v5.1` branch.
    - Includes components: `bootloader`, `bootloader_support`, `bt`, `efuse`, `esp_adc`,
    `esp_app_format`, `esp_coex`, `esp_common`, `esp_event`, `esp_hw_support`, `esp_phy`,
    `esp_rom`, `esp_system`, `esp_timer`, `esp_wifi`, `hal`, `log`, `mbedtls`, `newlib`, `riscv`,
    `soc`, `spi_flash`, `wpa_supplicant`, `xtensa`.

## Depreacated branches

The following branches are deprecated (not updated anymore):

- [`sync-1-release_v5.1`](../../tree/sync-1-release_v5.1):
    - Based on ESP-IDF `release/v5.1` branch.
    - Includes components: `bootloader_support`, `efuse`, `esp_app_format`, `esp_common`,
    `esp_event`, `esp_hw_support`, `esp_phy`, `esp_rom`, `esp_system`, `esp_timer`, `esp_wifi`,
    `hal`, `log`, `mbedtls`, `newlib`, `riscv`, `soc`, `spi_flash`, `wpa_supplicant`, `xtensa`.

- [`sync-2-release_v5.1`](../../tree/sync-2-release_v5.1):
    - Based on ESP-IDF `release/v5.1` branch.
    - Includes components: `bootloader_support`, `bt`, `efuse`, `esp_app_format`, `esp_common`,
    `esp_event`, `esp_hw_support`, `esp_phy`, `esp_rom`, `esp_system`, `esp_timer`, `esp_wifi`,
    `hal`, `log`, `mbedtls`, `newlib`, `riscv`, `soc`, `spi_flash`, `wpa_supplicant`, `xtensa`.

- [`sync-3-release_v5.1`](../../tree/sync-3-release_v5.1):
    - Based on ESP-IDF `release/v5.1` branch.
    - Includes components: `bootloader`, `bootloader_support`, `bt`, `efuse`, `esp_adc`,
    `esp_app_format`, `esp_coex`, `esp_common`, `esp_event`, `esp_hw_support`, `esp_phy`,
    `esp_rom`, `esp_system`, `esp_timer`, `esp_wifi`, `hal`, `log`, `mbedtls`, `newlib`, `riscv`,
    `soc`, `spi_flash`, `wpa_supplicant`, `xtensa`.

#### Restrictions

1. Sync branches don't have common ancestors

   This may cause some problems when you merge or pick from these branches

2. Can't easily modify the file list of an existing branch

   The tool used by the sync script converts commits of IDF into new ones on the given branch. The history is kept while the files are filtered according to the given file list.

   The generated commits will have the same SHA as long as the commit author, date, message and change list are the same.

   Any modification to the script's strategy of a sync branch (including modifying the file list) will change the SHA of the commits, forbidding the generated branch from being merged (pushed) to the existing one.

   When we need to modify the file list or any other part of the commit, it's suggested to create a new sync branch.

### release/[branch]

These are release branches intended to be used by the 3rd Party Frameworks, like NuttX. These branches include modifications made on the top of a sync branch needed to enable it to be used by some OS.

Where:
- `branch` means the branch used by the sync branch this release is derived from.

- [`release/v5.1`](../../tree/release/v5.1):
    - Based on [`sync-2-release_v5.1`](../../tree/sync-2-release_v5.1) branch.
    - Deprecated: cloned by the branch name on NuttX.
- [`release/v5.1.a`](../../tree/release/v5.1.a):
    - Based on [`sync-2-release_v5.1`](../../tree/sync-2-release_v5.1) branch.
    - Currently used by NuttX, cloned by commit SHA.
