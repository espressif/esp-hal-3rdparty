/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table 78dff63df528392f0f37f4880b83c6db
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.

static const esp_efuse_desc_t WR_DIS[] = {
    {EFUSE_BLK0, 0, 32}, 	 // [] Disable programming of individual eFuses,
};

static const esp_efuse_desc_t WR_DIS_RD_DIS[] = {
    {EFUSE_BLK0, 0, 1}, 	 // [] wr_dis of RD_DIS,
};

static const esp_efuse_desc_t WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 4, 1}, 	 // [] wr_dis of SPI_BOOT_CRYPT_CNT,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE0[] = {
    {EFUSE_BLK0, 5, 1}, 	 // [] wr_dis of SECURE_BOOT_KEY_REVOKE0,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE1[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of SECURE_BOOT_KEY_REVOKE1,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE2[] = {
    {EFUSE_BLK0, 7, 1}, 	 // [] wr_dis of SECURE_BOOT_KEY_REVOKE2,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_0[] = {
    {EFUSE_BLK0, 8, 1}, 	 // [WR_DIS.KEY0_PURPOSE] wr_dis of KEY_PURPOSE_0,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_1[] = {
    {EFUSE_BLK0, 9, 1}, 	 // [WR_DIS.KEY1_PURPOSE] wr_dis of KEY_PURPOSE_1,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_2[] = {
    {EFUSE_BLK0, 10, 1}, 	 // [WR_DIS.KEY2_PURPOSE] wr_dis of KEY_PURPOSE_2,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_3[] = {
    {EFUSE_BLK0, 11, 1}, 	 // [WR_DIS.KEY3_PURPOSE] wr_dis of KEY_PURPOSE_3,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_4[] = {
    {EFUSE_BLK0, 12, 1}, 	 // [WR_DIS.KEY4_PURPOSE] wr_dis of KEY_PURPOSE_4,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_5[] = {
    {EFUSE_BLK0, 13, 1}, 	 // [WR_DIS.KEY5_PURPOSE] wr_dis of KEY_PURPOSE_5,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 15, 1}, 	 // [] wr_dis of SECURE_BOOT_EN,
};

static const esp_efuse_desc_t WR_DIS_BLK1[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of BLOCK1,
};

static const esp_efuse_desc_t WR_DIS_MAC[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [WR_DIS.MAC_FACTORY] wr_dis of MAC,
};

static const esp_efuse_desc_t WR_DIS_MAC_EXT[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of MAC_EXT,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_SYS_DATA1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [WR_DIS.SYS_DATA_PART1] wr_dis of BLOCK_SYS_DATA1,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_USR_DATA[] = {
    {EFUSE_BLK0, 22, 1}, 	 // [WR_DIS.USER_DATA] wr_dis of BLOCK_USR_DATA,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY0[] = {
    {EFUSE_BLK0, 23, 1}, 	 // [WR_DIS.KEY0] wr_dis of BLOCK_KEY0,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY1[] = {
    {EFUSE_BLK0, 24, 1}, 	 // [WR_DIS.KEY1] wr_dis of BLOCK_KEY1,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY2[] = {
    {EFUSE_BLK0, 25, 1}, 	 // [WR_DIS.KEY2] wr_dis of BLOCK_KEY2,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY3[] = {
    {EFUSE_BLK0, 26, 1}, 	 // [WR_DIS.KEY3] wr_dis of BLOCK_KEY3,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY4[] = {
    {EFUSE_BLK0, 27, 1}, 	 // [WR_DIS.KEY4] wr_dis of BLOCK_KEY4,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY5[] = {
    {EFUSE_BLK0, 28, 1}, 	 // [WR_DIS.KEY5] wr_dis of BLOCK_KEY5,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_SYS_DATA2[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [WR_DIS.SYS_DATA_PART2] wr_dis of BLOCK_SYS_DATA2,
};

static const esp_efuse_desc_t RD_DIS[] = {
    {EFUSE_BLK0, 32, 7}, 	 // [] Disable reading from BlOCK4-10,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY0[] = {
    {EFUSE_BLK0, 32, 1}, 	 // [RD_DIS.KEY0] rd_dis of BLOCK_KEY0,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY1[] = {
    {EFUSE_BLK0, 33, 1}, 	 // [RD_DIS.KEY1] rd_dis of BLOCK_KEY1,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY2[] = {
    {EFUSE_BLK0, 34, 1}, 	 // [RD_DIS.KEY2] rd_dis of BLOCK_KEY2,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY3[] = {
    {EFUSE_BLK0, 35, 1}, 	 // [RD_DIS.KEY3] rd_dis of BLOCK_KEY3,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY4[] = {
    {EFUSE_BLK0, 36, 1}, 	 // [RD_DIS.KEY4] rd_dis of BLOCK_KEY4,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY5[] = {
    {EFUSE_BLK0, 37, 1}, 	 // [RD_DIS.KEY5] rd_dis of BLOCK_KEY5,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_SYS_DATA2[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [RD_DIS.SYS_DATA_PART2] rd_dis of BLOCK_SYS_DATA2,
};

static const esp_efuse_desc_t USB_DEVICE_EXCHG_PINS[] = {
    {EFUSE_BLK0, 39, 1}, 	 // [] Enable usb device exchange pins of D+ and D-,
};

static const esp_efuse_desc_t USB_OTG11_EXCHG_PINS[] = {
    {EFUSE_BLK0, 40, 1}, 	 // [] Enable usb otg11 exchange pins of D+ and D-,
};

static const esp_efuse_desc_t DIS_USB_JTAG[] = {
    {EFUSE_BLK0, 41, 1}, 	 // [] Represents whether the function of usb switch to jtag is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t POWERGLITCH_EN[] = {
    {EFUSE_BLK0, 42, 1}, 	 // [] Represents whether power glitch function is enabled. 1: enabled. 0: disabled,
};

static const esp_efuse_desc_t DIS_FORCE_DOWNLOAD[] = {
    {EFUSE_BLK0, 44, 1}, 	 // [] Represents whether the function that forces chip into download mode is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t SPI_DOWNLOAD_MSPI_DIS[] = {
    {EFUSE_BLK0, 45, 1}, 	 // [] Set this bit to disable accessing MSPI flash/MSPI ram by SYS AXI matrix during boot_mode_download,
};

static const esp_efuse_desc_t DIS_TWAI[] = {
    {EFUSE_BLK0, 46, 1}, 	 // [] Represents whether TWAI function is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t JTAG_SEL_ENABLE[] = {
    {EFUSE_BLK0, 47, 1}, 	 // [] Represents whether the selection between usb_to_jtag and pad_to_jtag through strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0 is enabled or disabled. 1: enabled. 0: disabled,
};

static const esp_efuse_desc_t SOFT_DIS_JTAG[] = {
    {EFUSE_BLK0, 48, 3}, 	 // [] Represents whether JTAG is disabled in soft way. Odd number: disabled. Even number: enabled,
};

static const esp_efuse_desc_t DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 51, 1}, 	 // [] Represents whether JTAG is disabled in the hard way(permanently). 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 52, 1}, 	 // [] Represents whether flash encrypt function is disabled or enabled(except in SPI boot mode). 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t USB_PHY_SEL[] = {
    {EFUSE_BLK0, 57, 1}, 	 // [] TBD,
};

static const esp_efuse_desc_t KM_HUK_GEN_STATE_LOW[] = {
    {EFUSE_BLK0, 58, 6}, 	 // [] Set this bit to control validation of HUK generate mode. Odd of 1 is invalid; even of 1 is valid,
};

static const esp_efuse_desc_t KM_HUK_GEN_STATE_HIGH[] = {
    {EFUSE_BLK0, 64, 3}, 	 // [] Set this bit to control validation of HUK generate mode. Odd of 1 is invalid; even of 1 is valid,
};

static const esp_efuse_desc_t KM_RND_SWITCH_CYCLE[] = {
    {EFUSE_BLK0, 67, 2}, 	 // [] Set bits to control key manager random number switch cycle. 0: control by register. 1: 8 km clk cycles. 2: 16 km cycles. 3: 32 km cycles,
};

static const esp_efuse_desc_t KM_DEPLOY_ONLY_ONCE[] = {
    {EFUSE_BLK0, 69, 4}, 	 // [] Set each bit to control whether corresponding key can only be deployed once. 1 is true; 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds,
};

static const esp_efuse_desc_t FORCE_USE_KEY_MANAGER_KEY[] = {
    {EFUSE_BLK0, 73, 4}, 	 // [] Set each bit to control whether corresponding key must come from key manager.. 1 is true; 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds,
};

static const esp_efuse_desc_t FORCE_DISABLE_SW_INIT_KEY[] = {
    {EFUSE_BLK0, 77, 1}, 	 // [] Set this bit to disable software written init key; and force use efuse_init_key,
};

static const esp_efuse_desc_t XTS_KEY_LENGTH_256[] = {
    {EFUSE_BLK0, 78, 1}, 	 // [] Set this bit to configure flash encryption use xts-128 key; else use xts-256 key,
};

static const esp_efuse_desc_t WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 80, 2}, 	 // [] Represents whether RTC watchdog timeout threshold is selected at startup. 1: selected. 0: not selected,
};

static const esp_efuse_desc_t SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 82, 3}, 	 // [] Enables flash encryption when 1 or 3 bits are set and disables otherwise {0: "Disable"; 1: "Enable"; 3: "Disable"; 7: "Enable"},
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE0[] = {
    {EFUSE_BLK0, 85, 1}, 	 // [] Revoke 1st secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE1[] = {
    {EFUSE_BLK0, 86, 1}, 	 // [] Revoke 2nd secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE2[] = {
    {EFUSE_BLK0, 87, 1}, 	 // [] Revoke 3rd secure boot key,
};

static const esp_efuse_desc_t KEY_PURPOSE_0[] = {
    {EFUSE_BLK0, 88, 4}, 	 // [KEY0_PURPOSE] Represents the purpose of Key0,
};

static const esp_efuse_desc_t KEY_PURPOSE_1[] = {
    {EFUSE_BLK0, 92, 4}, 	 // [KEY1_PURPOSE] Represents the purpose of Key1,
};

static const esp_efuse_desc_t KEY_PURPOSE_2[] = {
    {EFUSE_BLK0, 96, 4}, 	 // [KEY2_PURPOSE] Represents the purpose of Key2,
};

static const esp_efuse_desc_t KEY_PURPOSE_3[] = {
    {EFUSE_BLK0, 100, 4}, 	 // [KEY3_PURPOSE] Represents the purpose of Key3,
};

static const esp_efuse_desc_t KEY_PURPOSE_4[] = {
    {EFUSE_BLK0, 104, 4}, 	 // [KEY4_PURPOSE] Represents the purpose of Key4,
};

static const esp_efuse_desc_t KEY_PURPOSE_5[] = {
    {EFUSE_BLK0, 108, 4}, 	 // [KEY5_PURPOSE] Represents the purpose of Key5,
};

static const esp_efuse_desc_t SEC_DPA_LEVEL[] = {
    {EFUSE_BLK0, 112, 2}, 	 // [] Represents the spa secure level by configuring the clock random divide mode,
};

static const esp_efuse_desc_t ECDSA_ENABLE_SOFT_K[] = {
    {EFUSE_BLK0, 114, 1}, 	 // [] Represents whether hardware random number k is forced used in ESDCA. 1: force used. 0: not force used,
};

static const esp_efuse_desc_t CRYPT_DPA_ENABLE[] = {
    {EFUSE_BLK0, 115, 1}, 	 // [] Represents whether anti-dpa attack is enabled. 1:enabled. 0: disabled,
};

static const esp_efuse_desc_t SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 116, 1}, 	 // [] Represents whether secure boot is enabled or disabled. 1: enabled. 0: disabled,
};

static const esp_efuse_desc_t SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 117, 1}, 	 // [] Represents whether revoking aggressive secure boot is enabled or disabled. 1: enabled. 0: disabled,
};

static const esp_efuse_desc_t FLASH_TYPE[] = {
    {EFUSE_BLK0, 119, 1}, 	 // [] The type of interfaced flash. 0: four data lines; 1: eight data lines,
};

static const esp_efuse_desc_t FLASH_PAGE_SIZE[] = {
    {EFUSE_BLK0, 120, 2}, 	 // [] Set flash page size,
};

static const esp_efuse_desc_t FLASH_ECC_EN[] = {
    {EFUSE_BLK0, 122, 1}, 	 // [] Set this bit to enable ecc for flash boot,
};

static const esp_efuse_desc_t DIS_USB_OTG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 123, 1}, 	 // [] Set this bit to disable download via USB-OTG,
};

static const esp_efuse_desc_t FLASH_TPUW[] = {
    {EFUSE_BLK0, 124, 4}, 	 // [] Represents the flash waiting time after power-up; in unit of ms. When the value less than 15; the waiting time is the programmed value. Otherwise; the waiting time is 2 times the programmed value,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 128, 1}, 	 // [] Represents whether Download mode is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 129, 1}, 	 // [] Represents whether direct boot mode is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    {EFUSE_BLK0, 130, 1}, 	 // [] Represents whether print from USB-Serial-JTAG is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t LOCK_KM_KEY[] = {
    {EFUSE_BLK0, 131, 1}, 	 // [] TBD,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 132, 1}, 	 // [] Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 133, 1}, 	 // [] Represents whether security download is enabled or disabled. 1: enabled. 0: disabled,
};

static const esp_efuse_desc_t UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 134, 2}, 	 // [] Represents the type of UART printing. 00: force enable printing. 01: enable printing when GPIO8 is reset at low level. 10: enable printing when GPIO8 is reset at high level. 11: force disable printing,
};

static const esp_efuse_desc_t FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 136, 1}, 	 // [] Represents whether ROM code is forced to send a resume command during SPI boot. 1: forced. 0:not forced,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK0, 137, 16}, 	 // [] Represents the version used by ESP-IDF anti-rollback feature,
};

static const esp_efuse_desc_t SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    {EFUSE_BLK0, 153, 1}, 	 // [] Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t HYS_EN_PAD[] = {
    {EFUSE_BLK0, 154, 1}, 	 // [] Represents whether the hysteresis function of corresponding PAD is enabled. 1: enabled. 0:disabled,
};

static const esp_efuse_desc_t DCDC_VSET[] = {
    {EFUSE_BLK0, 155, 5}, 	 // [] Set the dcdc voltage default,
};

static const esp_efuse_desc_t PXA0_TIEH_SEL_0[] = {
    {EFUSE_BLK0, 160, 2}, 	 // [] TBD,
};

static const esp_efuse_desc_t PXA0_TIEH_SEL_1[] = {
    {EFUSE_BLK0, 162, 2}, 	 // [] TBD,
};

static const esp_efuse_desc_t PXA0_TIEH_SEL_2[] = {
    {EFUSE_BLK0, 164, 2}, 	 // [] TBD,
};

static const esp_efuse_desc_t PXA0_TIEH_SEL_3[] = {
    {EFUSE_BLK0, 166, 2}, 	 // [] TBD,
};

static const esp_efuse_desc_t KM_DISABLE_DEPLOY_MODE[] = {
    {EFUSE_BLK0, 168, 4}, 	 // [] TBD,
};

static const esp_efuse_desc_t HP_PWR_SRC_SEL[] = {
    {EFUSE_BLK0, 178, 1}, 	 // [] HP system power source select. 0:LDO. 1: DCDC,
};

static const esp_efuse_desc_t DCDC_VSET_EN[] = {
    {EFUSE_BLK0, 179, 1}, 	 // [] Select dcdc vset use efuse_dcdc_vset,
};

static const esp_efuse_desc_t DIS_WDT[] = {
    {EFUSE_BLK0, 180, 1}, 	 // [] Set this bit to disable watch dog,
};

static const esp_efuse_desc_t DIS_SWD[] = {
    {EFUSE_BLK0, 181, 1}, 	 // [] Set this bit to disable super-watchdog,
};

static const esp_efuse_desc_t MAC[] = {
    {EFUSE_BLK1, 40, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 32, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 24, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 16, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 8, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 0, 8}, 	 // [MAC_FACTORY] MAC address,
};

static const esp_efuse_desc_t MAC_EXT[] = {
    {EFUSE_BLK1, 48, 8}, 	 // [] Stores the extended bits of MAC address [0],
    {EFUSE_BLK1, 56, 8}, 	 // [] Stores the extended bits of MAC address [1],
};

static const esp_efuse_desc_t BLOCK_SYS_DATA1[] = {
    {EFUSE_BLK2, 0, 256}, 	 // [SYS_DATA_PART1] System data part 1,
};

static const esp_efuse_desc_t USER_DATA[] = {
    {EFUSE_BLK3, 0, 256}, 	 // [BLOCK_USR_DATA] User data,
};

static const esp_efuse_desc_t USER_DATA_MAC_CUSTOM[] = {
    {EFUSE_BLK3, 200, 48}, 	 // [MAC_CUSTOM CUSTOM_MAC] Custom MAC (TODO,
};

static const esp_efuse_desc_t KEY0[] = {
    {EFUSE_BLK4, 0, 256}, 	 // [BLOCK_KEY0] Key0 or user data,
};

static const esp_efuse_desc_t KEY1[] = {
    {EFUSE_BLK5, 0, 256}, 	 // [BLOCK_KEY1] Key1 or user data,
};

static const esp_efuse_desc_t KEY2[] = {
    {EFUSE_BLK6, 0, 256}, 	 // [BLOCK_KEY2] Key2 or user data,
};

static const esp_efuse_desc_t KEY3[] = {
    {EFUSE_BLK7, 0, 256}, 	 // [BLOCK_KEY3] Key3 or user data,
};

static const esp_efuse_desc_t KEY4[] = {
    {EFUSE_BLK8, 0, 256}, 	 // [BLOCK_KEY4] Key4 or user data,
};

static const esp_efuse_desc_t KEY5[] = {
    {EFUSE_BLK9, 0, 256}, 	 // [BLOCK_KEY5] Key5 or user data,
};

static const esp_efuse_desc_t SYS_DATA_PART2[] = {
    {EFUSE_BLK10, 0, 256}, 	 // [BLOCK_SYS_DATA2] System data part 2 (reserved),
};





const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[] = {
    &WR_DIS[0],    		// [] Disable programming of individual eFuses
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[] = {
    &WR_DIS_RD_DIS[0],    		// [] wr_dis of RD_DIS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    &WR_DIS_SPI_BOOT_CRYPT_CNT[0],    		// [] wr_dis of SPI_BOOT_CRYPT_CNT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE0[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE0[0],    		// [] wr_dis of SECURE_BOOT_KEY_REVOKE0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE1[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE1[0],    		// [] wr_dis of SECURE_BOOT_KEY_REVOKE1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE2[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE2[0],    		// [] wr_dis of SECURE_BOOT_KEY_REVOKE2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_0[] = {
    &WR_DIS_KEY_PURPOSE_0[0],    		// [WR_DIS.KEY0_PURPOSE] wr_dis of KEY_PURPOSE_0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_1[] = {
    &WR_DIS_KEY_PURPOSE_1[0],    		// [WR_DIS.KEY1_PURPOSE] wr_dis of KEY_PURPOSE_1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_2[] = {
    &WR_DIS_KEY_PURPOSE_2[0],    		// [WR_DIS.KEY2_PURPOSE] wr_dis of KEY_PURPOSE_2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_3[] = {
    &WR_DIS_KEY_PURPOSE_3[0],    		// [WR_DIS.KEY3_PURPOSE] wr_dis of KEY_PURPOSE_3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_4[] = {
    &WR_DIS_KEY_PURPOSE_4[0],    		// [WR_DIS.KEY4_PURPOSE] wr_dis of KEY_PURPOSE_4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_5[] = {
    &WR_DIS_KEY_PURPOSE_5[0],    		// [WR_DIS.KEY5_PURPOSE] wr_dis of KEY_PURPOSE_5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_EN[] = {
    &WR_DIS_SECURE_BOOT_EN[0],    		// [] wr_dis of SECURE_BOOT_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK1[] = {
    &WR_DIS_BLK1[0],    		// [] wr_dis of BLOCK1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC[] = {
    &WR_DIS_MAC[0],    		// [WR_DIS.MAC_FACTORY] wr_dis of MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC_EXT[] = {
    &WR_DIS_MAC_EXT[0],    		// [] wr_dis of MAC_EXT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_SYS_DATA1[] = {
    &WR_DIS_BLOCK_SYS_DATA1[0],    		// [WR_DIS.SYS_DATA_PART1] wr_dis of BLOCK_SYS_DATA1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_USR_DATA[] = {
    &WR_DIS_BLOCK_USR_DATA[0],    		// [WR_DIS.USER_DATA] wr_dis of BLOCK_USR_DATA
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY0[] = {
    &WR_DIS_BLOCK_KEY0[0],    		// [WR_DIS.KEY0] wr_dis of BLOCK_KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY1[] = {
    &WR_DIS_BLOCK_KEY1[0],    		// [WR_DIS.KEY1] wr_dis of BLOCK_KEY1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY2[] = {
    &WR_DIS_BLOCK_KEY2[0],    		// [WR_DIS.KEY2] wr_dis of BLOCK_KEY2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY3[] = {
    &WR_DIS_BLOCK_KEY3[0],    		// [WR_DIS.KEY3] wr_dis of BLOCK_KEY3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY4[] = {
    &WR_DIS_BLOCK_KEY4[0],    		// [WR_DIS.KEY4] wr_dis of BLOCK_KEY4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY5[] = {
    &WR_DIS_BLOCK_KEY5[0],    		// [WR_DIS.KEY5] wr_dis of BLOCK_KEY5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_SYS_DATA2[] = {
    &WR_DIS_BLOCK_SYS_DATA2[0],    		// [WR_DIS.SYS_DATA_PART2] wr_dis of BLOCK_SYS_DATA2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[] = {
    &RD_DIS[0],    		// [] Disable reading from BlOCK4-10
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY0[] = {
    &RD_DIS_BLOCK_KEY0[0],    		// [RD_DIS.KEY0] rd_dis of BLOCK_KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY1[] = {
    &RD_DIS_BLOCK_KEY1[0],    		// [RD_DIS.KEY1] rd_dis of BLOCK_KEY1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY2[] = {
    &RD_DIS_BLOCK_KEY2[0],    		// [RD_DIS.KEY2] rd_dis of BLOCK_KEY2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY3[] = {
    &RD_DIS_BLOCK_KEY3[0],    		// [RD_DIS.KEY3] rd_dis of BLOCK_KEY3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY4[] = {
    &RD_DIS_BLOCK_KEY4[0],    		// [RD_DIS.KEY4] rd_dis of BLOCK_KEY4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY5[] = {
    &RD_DIS_BLOCK_KEY5[0],    		// [RD_DIS.KEY5] rd_dis of BLOCK_KEY5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_SYS_DATA2[] = {
    &RD_DIS_BLOCK_SYS_DATA2[0],    		// [RD_DIS.SYS_DATA_PART2] rd_dis of BLOCK_SYS_DATA2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_DEVICE_EXCHG_PINS[] = {
    &USB_DEVICE_EXCHG_PINS[0],    		// [] Enable usb device exchange pins of D+ and D-
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_OTG11_EXCHG_PINS[] = {
    &USB_OTG11_EXCHG_PINS[0],    		// [] Enable usb otg11 exchange pins of D+ and D-
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_JTAG[] = {
    &DIS_USB_JTAG[0],    		// [] Represents whether the function of usb switch to jtag is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_POWERGLITCH_EN[] = {
    &POWERGLITCH_EN[0],    		// [] Represents whether power glitch function is enabled. 1: enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[] = {
    &DIS_FORCE_DOWNLOAD[0],    		// [] Represents whether the function that forces chip into download mode is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS[] = {
    &SPI_DOWNLOAD_MSPI_DIS[0],    		// [] Set this bit to disable accessing MSPI flash/MSPI ram by SYS AXI matrix during boot_mode_download
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_TWAI[] = {
    &DIS_TWAI[0],    		// [] Represents whether TWAI function is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_JTAG_SEL_ENABLE[] = {
    &JTAG_SEL_ENABLE[0],    		// [] Represents whether the selection between usb_to_jtag and pad_to_jtag through strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0 is enabled or disabled. 1: enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SOFT_DIS_JTAG[] = {
    &SOFT_DIS_JTAG[0],    		// [] Represents whether JTAG is disabled in soft way. Odd number: disabled. Even number: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[] = {
    &DIS_PAD_JTAG[0],    		// [] Represents whether JTAG is disabled in the hard way(permanently). 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// [] Represents whether flash encrypt function is disabled or enabled(except in SPI boot mode). 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_PHY_SEL[] = {
    &USB_PHY_SEL[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_HUK_GEN_STATE_LOW[] = {
    &KM_HUK_GEN_STATE_LOW[0],    		// [] Set this bit to control validation of HUK generate mode. Odd of 1 is invalid; even of 1 is valid
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_HUK_GEN_STATE_HIGH[] = {
    &KM_HUK_GEN_STATE_HIGH[0],    		// [] Set this bit to control validation of HUK generate mode. Odd of 1 is invalid; even of 1 is valid
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_RND_SWITCH_CYCLE[] = {
    &KM_RND_SWITCH_CYCLE[0],    		// [] Set bits to control key manager random number switch cycle. 0: control by register. 1: 8 km clk cycles. 2: 16 km cycles. 3: 32 km cycles
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_DEPLOY_ONLY_ONCE[] = {
    &KM_DEPLOY_ONLY_ONCE[0],    		// [] Set each bit to control whether corresponding key can only be deployed once. 1 is true; 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_USE_KEY_MANAGER_KEY[] = {
    &FORCE_USE_KEY_MANAGER_KEY[0],    		// [] Set each bit to control whether corresponding key must come from key manager.. 1 is true; 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_DISABLE_SW_INIT_KEY[] = {
    &FORCE_DISABLE_SW_INIT_KEY[0],    		// [] Set this bit to disable software written init key; and force use efuse_init_key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTS_KEY_LENGTH_256[] = {
    &XTS_KEY_LENGTH_256[0],    		// [] Set this bit to configure flash encryption use xts-128 key; else use xts-256 key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[] = {
    &WDT_DELAY_SEL[0],    		// [] Represents whether RTC watchdog timeout threshold is selected at startup. 1: selected. 0: not selected
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_CRYPT_CNT[] = {
    &SPI_BOOT_CRYPT_CNT[0],    		// [] Enables flash encryption when 1 or 3 bits are set and disables otherwise {0: "Disable"; 1: "Enable"; 3: "Disable"; 7: "Enable"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE0[] = {
    &SECURE_BOOT_KEY_REVOKE0[0],    		// [] Revoke 1st secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE1[] = {
    &SECURE_BOOT_KEY_REVOKE1[0],    		// [] Revoke 2nd secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE2[] = {
    &SECURE_BOOT_KEY_REVOKE2[0],    		// [] Revoke 3rd secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_0[] = {
    &KEY_PURPOSE_0[0],    		// [KEY0_PURPOSE] Represents the purpose of Key0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_1[] = {
    &KEY_PURPOSE_1[0],    		// [KEY1_PURPOSE] Represents the purpose of Key1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_2[] = {
    &KEY_PURPOSE_2[0],    		// [KEY2_PURPOSE] Represents the purpose of Key2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_3[] = {
    &KEY_PURPOSE_3[0],    		// [KEY3_PURPOSE] Represents the purpose of Key3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_4[] = {
    &KEY_PURPOSE_4[0],    		// [KEY4_PURPOSE] Represents the purpose of Key4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_5[] = {
    &KEY_PURPOSE_5[0],    		// [KEY5_PURPOSE] Represents the purpose of Key5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SEC_DPA_LEVEL[] = {
    &SEC_DPA_LEVEL[0],    		// [] Represents the spa secure level by configuring the clock random divide mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ECDSA_ENABLE_SOFT_K[] = {
    &ECDSA_ENABLE_SOFT_K[0],    		// [] Represents whether hardware random number k is forced used in ESDCA. 1: force used. 0: not force used
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CRYPT_DPA_ENABLE[] = {
    &CRYPT_DPA_ENABLE[0],    		// [] Represents whether anti-dpa attack is enabled. 1:enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[] = {
    &SECURE_BOOT_EN[0],    		// [] Represents whether secure boot is enabled or disabled. 1: enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    &SECURE_BOOT_AGGRESSIVE_REVOKE[0],    		// [] Represents whether revoking aggressive secure boot is enabled or disabled. 1: enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TYPE[] = {
    &FLASH_TYPE[0],    		// [] The type of interfaced flash. 0: four data lines; 1: eight data lines
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_PAGE_SIZE[] = {
    &FLASH_PAGE_SIZE[0],    		// [] Set flash page size
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_ECC_EN[] = {
    &FLASH_ECC_EN[0],    		// [] Set this bit to enable ecc for flash boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_OTG_DOWNLOAD_MODE[] = {
    &DIS_USB_OTG_DOWNLOAD_MODE[0],    		// [] Set this bit to disable download via USB-OTG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[] = {
    &FLASH_TPUW[0],    		// [] Represents the flash waiting time after power-up; in unit of ms. When the value less than 15; the waiting time is the programmed value. Otherwise; the waiting time is 2 times the programmed value
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[] = {
    &DIS_DOWNLOAD_MODE[0],    		// [] Represents whether Download mode is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[] = {
    &DIS_DIRECT_BOOT[0],    		// [] Represents whether direct boot mode is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    &DIS_USB_SERIAL_JTAG_ROM_PRINT[0],    		// [] Represents whether print from USB-Serial-JTAG is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LOCK_KM_KEY[] = {
    &LOCK_KM_KEY[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    &DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[0],    		// [] Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[] = {
    &ENABLE_SECURITY_DOWNLOAD[0],    		// [] Represents whether security download is enabled or disabled. 1: enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[] = {
    &UART_PRINT_CONTROL[0],    		// [] Represents the type of UART printing. 00: force enable printing. 01: enable printing when GPIO8 is reset at low level. 10: enable printing when GPIO8 is reset at high level. 11: force disable printing
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[] = {
    &FORCE_SEND_RESUME[0],    		// [] Represents whether ROM code is forced to send a resume command during SPI boot. 1: forced. 0:not forced
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// [] Represents the version used by ESP-IDF anti-rollback feature
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    &SECURE_BOOT_DISABLE_FAST_WAKE[0],    		// [] Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HYS_EN_PAD[] = {
    &HYS_EN_PAD[0],    		// [] Represents whether the hysteresis function of corresponding PAD is enabled. 1: enabled. 0:disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DCDC_VSET[] = {
    &DCDC_VSET[0],    		// [] Set the dcdc voltage default
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_0[] = {
    &PXA0_TIEH_SEL_0[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_1[] = {
    &PXA0_TIEH_SEL_1[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_2[] = {
    &PXA0_TIEH_SEL_2[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_3[] = {
    &PXA0_TIEH_SEL_3[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_DISABLE_DEPLOY_MODE[] = {
    &KM_DISABLE_DEPLOY_MODE[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HP_PWR_SRC_SEL[] = {
    &HP_PWR_SRC_SEL[0],    		// [] HP system power source select. 0:LDO. 1: DCDC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DCDC_VSET_EN[] = {
    &DCDC_VSET_EN[0],    		// [] Select dcdc vset use efuse_dcdc_vset
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_WDT[] = {
    &DIS_WDT[0],    		// [] Set this bit to disable watch dog
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_SWD[] = {
    &DIS_SWD[0],    		// [] Set this bit to disable super-watchdog
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_MAC[] = {
    &MAC[0],    		// [MAC_FACTORY] MAC address
    &MAC[1],    		// [MAC_FACTORY] MAC address
    &MAC[2],    		// [MAC_FACTORY] MAC address
    &MAC[3],    		// [MAC_FACTORY] MAC address
    &MAC[4],    		// [MAC_FACTORY] MAC address
    &MAC[5],    		// [MAC_FACTORY] MAC address
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_MAC_EXT[] = {
    &MAC_EXT[0],    		// [] Stores the extended bits of MAC address [0]
    &MAC_EXT[1],    		// [] Stores the extended bits of MAC address [1]
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLOCK_SYS_DATA1[] = {
    &BLOCK_SYS_DATA1[0],    		// [SYS_DATA_PART1] System data part 1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA[] = {
    &USER_DATA[0],    		// [BLOCK_USR_DATA] User data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA_MAC_CUSTOM[] = {
    &USER_DATA_MAC_CUSTOM[0],    		// [MAC_CUSTOM CUSTOM_MAC] Custom MAC (TODO
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0[] = {
    &KEY0[0],    		// [BLOCK_KEY0] Key0 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY1[] = {
    &KEY1[0],    		// [BLOCK_KEY1] Key1 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY2[] = {
    &KEY2[0],    		// [BLOCK_KEY2] Key2 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY3[] = {
    &KEY3[0],    		// [BLOCK_KEY3] Key3 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY4[] = {
    &KEY4[0],    		// [BLOCK_KEY4] Key4 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY5[] = {
    &KEY5[0],    		// [BLOCK_KEY5] Key5 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SYS_DATA_PART2[] = {
    &SYS_DATA_PART2[0],    		// [BLOCK_SYS_DATA2] System data part 2 (reserved)
    NULL
};
