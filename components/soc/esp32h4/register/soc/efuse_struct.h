/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: program_data registers */
/** Type of pgm_datan register
 *  Represents pgm_datan
 */
typedef union {
    struct {
        /** pgm_data_n : R/W; bitpos: [31:0]; default: 0;
         *  Configures the nth 32-bit data to be programmed.
         */
        uint32_t pgm_data_n:32;
    };
    uint32_t val;
} efuse_pgm_datan_reg_t;

/** Type of pgm_check_valuen register
 *  Represents pgm_check_valuen
 */
typedef union {
    struct {
        /** pgm_rs_data_n : R/W; bitpos: [31:0]; default: 0;
         *  Configures the nth RS code to be programmed.
         */
        uint32_t pgm_rs_data_n:32;
    };
    uint32_t val;
} efuse_pgm_check_valuen_reg_t;


/** Group: block0 registers */
/** Type of rd_wr_dis0 register
 *  Represents rd_wr_dis
 */
typedef union {
    struct {
        /** wr_dis : RO; bitpos: [31:0]; default: 0;
         *  Represents whether programming of individual eFuse memory bit is disabled or
         *  enabled.
         *  1: Disabled
         *  0: Enabled
         */
        uint32_t wr_dis:32;
    };
    uint32_t val;
} efuse_rd_wr_dis0_reg_t;

/** Type of rd_repeat_data0 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** rd_dis : RO; bitpos: [6:0]; default: 0;
         *  Represents whether reading of individual eFuse block(block4~block10) is disabled or
         *  enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t rd_dis:7;
        /** dis_usb_jtag : RO; bitpos: [7]; default: 0;
         *  Represents whether the function of usb switch to jtag is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_usb_jtag:1;
        uint32_t reserved_8:1;
        /** dis_force_download : RO; bitpos: [9]; default: 0;
         *  Represents whether the function that forces chip into download mode is disabled or
         *  enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_force_download:1;
        /** spi_download_mspi_dis : RO; bitpos: [10]; default: 0;
         *  Represents whether SPI0 controller during boot_mode_download is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t spi_download_mspi_dis:1;
        /** dis_twai : RO; bitpos: [11]; default: 0;
         *  Represents whether TWAI function is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_twai:1;
        /** jtag_sel_enable : RO; bitpos: [12]; default: 0;
         *  Represents whether the selection between usb_to_jtag and pad_to_jtag through
         *  strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0
         *  is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t jtag_sel_enable:1;
        /** dis_pad_jtag : RO; bitpos: [13]; default: 0;
         *  Represents whether JTAG is disabled in the hard way(permanently).
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_pad_jtag:1;
        /** dis_download_manual_encrypt : RO; bitpos: [14]; default: 0;
         *  Represents whether flash encrypt function is disabled or enabled(except in SPI boot
         *  mode).
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_download_manual_encrypt:1;
        uint32_t reserved_15:3;
        /** pvt_glitch_en : RO; bitpos: [18]; default: 0;
         *  Represents whether to enable PVT power glitch monitor function.
         *  1:Enable.
         *  0:Disable
         */
        uint32_t pvt_glitch_en:1;
        uint32_t reserved_19:1;
        /** pvt_glitch_mode : RO; bitpos: [21:20]; default: 0;
         *  Use to configure glitch mode
         */
        uint32_t pvt_glitch_mode:2;
        /** dis_core1 : RO; bitpos: [22]; default: 0;
         *  Represents whether the CPU-Core1 is disabled.
         *  1: Disabled.
         *  0: Not disable.
         */
        uint32_t dis_core1:1;
        /** spi_boot_crypt_cnt : RO; bitpos: [25:23]; default: 0;
         *  Represents whether SPI boot encrypt/decrypt is disabled or enabled.
         *  Odd number of 1: enabled
         *  Even number of 1: disabled
         */
        uint32_t spi_boot_crypt_cnt:3;
        /** secure_boot_key_revoke0 : RO; bitpos: [26]; default: 0;
         *  Represents whether revoking first secure boot key is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t secure_boot_key_revoke0:1;
        /** secure_boot_key_revoke1 : RO; bitpos: [27]; default: 0;
         *  Represents whether revoking second secure boot key is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t secure_boot_key_revoke1:1;
        /** secure_boot_key_revoke2 : RO; bitpos: [28]; default: 0;
         *  Represents whether revoking third secure boot key is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t secure_boot_key_revoke2:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} efuse_rd_repeat_data0_reg_t;

/** Type of rd_repeat_data1 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** key_purpose_0 : RO; bitpos: [4:0]; default: 0;
         *  Represents the purpose of Key0.
         */
        uint32_t key_purpose_0:5;
        /** key_purpose_1 : RO; bitpos: [9:5]; default: 0;
         *  Represents the purpose of Key1.
         */
        uint32_t key_purpose_1:5;
        /** key_purpose_2 : RO; bitpos: [14:10]; default: 0;
         *  Represents the purpose of Key2.
         */
        uint32_t key_purpose_2:5;
        /** key_purpose_3 : RO; bitpos: [19:15]; default: 0;
         *  Represents the purpose of Key3.
         */
        uint32_t key_purpose_3:5;
        /** key_purpose_4 : RO; bitpos: [24:20]; default: 0;
         *  Represents the purpose of Key4.
         */
        uint32_t key_purpose_4:5;
        /** key_purpose_5 : RO; bitpos: [29:25]; default: 0;
         *  Represents the purpose of Key5.
         */
        uint32_t key_purpose_5:5;
        /** sec_dpa_level : RO; bitpos: [31:30]; default: 0;
         *  Represents the spa secure level by configuring the clock random divide mode.
         */
        uint32_t sec_dpa_level:2;
    };
    uint32_t val;
} efuse_rd_repeat_data1_reg_t;

/** Type of rd_repeat_data2 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** xts_dpa_pseudo_level : RO; bitpos: [1:0]; default: 0;
         *  Represents the pseudo round level of xts-aes anti-dpa attack.
         *  3: High.
         *  2: Moderate 1. Low
         *  0: Disabled
         */
        uint32_t xts_dpa_pseudo_level:2;
        /** xts_dpa_clk_enable : RO; bitpos: [2]; default: 0;
         *  Represents whether xts-aes anti-dpa attack clock is enabled.
         *  1. Enable.
         *  0: Disable.
         */
        uint32_t xts_dpa_clk_enable:1;
        /** ecc_force_const_time : RO; bitpos: [3]; default: 0;
         *  Represents whether to force ecc to use const-time calculation mode.
         *  1: Enable.
         *  0: Disable.
         */
        uint32_t ecc_force_const_time:1;
        /** ecdsa_p384_enable : RO; bitpos: [4]; default: 0;
         *  Represents if the chip supports ECDSA P384
         */
        uint32_t ecdsa_p384_enable:1;
        /** secure_boot_en : RO; bitpos: [5]; default: 0;
         *  Represents whether secure boot is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t secure_boot_en:1;
        /** secure_boot_aggressive_revoke : RO; bitpos: [6]; default: 0;
         *  Represents whether revoking aggressive secure boot is enabled or disabled.
         *  1: enabled.
         *  0: disabled
         */
        uint32_t secure_boot_aggressive_revoke:1;
        /** km_disable_deploy_mode : RO; bitpos: [11:7]; default: 0;
         *  Represents whether the new key deployment of key manager is disabled.
         *  Bit0: Represents whether the new ECDSA key deployment is disabled
         *  0: Enabled
         *  1: Disabled
         *  Bit1: Represents whether the new XTS-AES (flash and PSRAM) key deployment is
         *  disabled
         *  0: Enabled
         *  1: Disabled
         *  Bit2: Represents whether the new HMAC key deployment is disabled
         *  0: Enabled
         *  1: Disabled
         *  Bit3: Represents whether the new DS key deployment is disabled
         *  0: Enabled
         *  1: Disabled
         */
        uint32_t km_disable_deploy_mode:5;
        /** km_rnd_switch_cycle : RO; bitpos: [13:12]; default: 0;
         *  Represents the cycle at which the Key Manager switches random numbers.
         *  0: Controlled by the
         *  \hyperref[fielddesc:KEYMNGRNDSWITCHCYCLE]{KEYMNG\_RND\_SWITCH\_CYCLE} register. For
         *  more information, please refer to Chapter \ref{mod:keymng}
         *  \textit{\nameref{mod:keymng}}
         *  1: 8 Key Manager clock cycles
         *  2: 16 Key Manager clock cycles
         *  3: 32 Key Manager clock cycles
         */
        uint32_t km_rnd_switch_cycle:2;
        /** km_deploy_only_once : RO; bitpos: [18:14]; default: 0;
         *  Represents whether the corresponding key can be deployed only once.
         *  Bit0: Represents whether the ECDSA key can be deployed only once
         *  0: The key can be deployed multiple times
         *  1: The key can be deployed only once
         *  Bit1: Represents whether the XTS-AES (flash and PSRAM) key can be deployed only once
         *  0: The key can be deployed multiple times
         *  1: The key can be deployed only once
         *  Bit2: Represents whether the HMAC key can be deployed only once
         *  0: The key can be deployed multiple times
         *  1: The key can be deployed only once
         *  Bit3: Represents whether the DS key can be deployed only once
         *  0: The key can be deployed multiple times
         *  1: The key can be deployed only once
         */
        uint32_t km_deploy_only_once:5;
        /** force_use_key_manager_key : RO; bitpos: [23:19]; default: 0;
         *  Represents whether the corresponding key must come from Key Manager.
         *  Bit0: Represents whether the ECDSA key must come from Key Manager.
         *  0: The key does not need to come from Key Manager
         *  1: The key must come from Key Manager
         *  Bit1: Represents whether the XTS-AES (flash and PSRAM) key must come from Key
         *  Manager.
         *  0: The key does not need to come from Key Manager
         *  1: The key must come from Key Manager
         *  Bit2: Represents whether the HMAC key must come from Key Manager.
         *  0: The key does not need to come from Key Manager
         *  1: The key must come from Key Manager
         *  Bit3: Represents whether the DS key must come from Key Manager.
         *  0: The key does not need to come from Key Manager
         *  1: The key must come from Key Manager
         */
        uint32_t force_use_key_manager_key:5;
        /** force_disable_sw_init_key : RO; bitpos: [24]; default: 0;
         *  Represents whether to disable the use of the initialization key written by software
         *  and instead force use efuse\_init\_key.
         *  0: Enable
         *  1: Disable
         */
        uint32_t force_disable_sw_init_key:1;
        /** km_xts_key_length_256 : RO; bitpos: [25]; default: 0;
         *  Represents which key flash encryption uses.
         *  0: XTS-AES-256 key
         *  1: XTS-AES-128 key
         */
        uint32_t km_xts_key_length_256:1;
        /** lock_km_key : RO; bitpos: [26]; default: 0;
         *  Represents whether the keys in the Key Manager are locked after deployment.
         *  0: Not locked
         *  1: Locked
         */
        uint32_t lock_km_key:1;
        /** flash_tpuw : RO; bitpos: [29:27]; default: 0;
         *  Represents the flash waiting time after power-up, in unit of ms. When the value
         *  less than 15, the waiting time is the programmed value. Otherwise, the waiting time
         *  is 2 times the programmed value.
         */
        uint32_t flash_tpuw:3;
        uint32_t reserved_30:1;
        /** dis_download_mode : RO; bitpos: [31]; default: 0;
         *  Represents whether Download mode is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_download_mode:1;
    };
    uint32_t val;
} efuse_rd_repeat_data2_reg_t;

/** Type of rd_repeat_data3 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** dis_direct_boot : RO; bitpos: [0]; default: 0;
         *  Represents whether direct boot mode is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_direct_boot:1;
        /** dis_usb_serial_jtag_rom_print : RO; bitpos: [1]; default: 0;
         *  Represents whether print from USB-Serial-JTAG is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_usb_serial_jtag_rom_print:1;
        /** dis_usb_serial_jtag_download_mode : RO; bitpos: [2]; default: 0;
         *  Represents whether the USB-Serial-JTAG download function is disabled or enabled.
         *  1: Disable
         *  0: Enable
         */
        uint32_t dis_usb_serial_jtag_download_mode:1;
        /** enable_security_download : RO; bitpos: [3]; default: 0;
         *  Represents whether security download is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t enable_security_download:1;
        /** uart_print_control : RO; bitpos: [5:4]; default: 0;
         *  Represents the type of UART printing.
         *  00: force enable printing
         *  01: enable printing when GPIO8 is reset at low level
         *  10: enable printing when GPIO8 is reset at high level
         *  11: force disable printing
         */
        uint32_t uart_print_control:2;
        /** force_send_resume : RO; bitpos: [6]; default: 0;
         *  Represents whether ROM code is forced to send a resume command during SPI boot.
         *  1: forced
         *  0:not forced
         */
        uint32_t force_send_resume:1;
        /** secure_version : RO; bitpos: [22:7]; default: 0;
         *  Represents the version used by ESP-IDF anti-rollback feature.
         */
        uint32_t secure_version:16;
        /** huk_gen_state : RO; bitpos: [27:23]; default: 0;
         *  Represents whether the HUK generate mode is valid.
         *  Odd count of bits with a value of 1: Invalid
         *  Even count of bits with a value of 1: Valid
         */
        uint32_t huk_gen_state:5;
        /** flash_ldo_efuse_sel : RO; bitpos: [28]; default: 0;
         *  Represents whether to select efuse control flash ldo default voltage.
         *  1 : efuse 0 : strapping
         */
        uint32_t flash_ldo_efuse_sel:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} efuse_rd_repeat_data3_reg_t;

/** Type of rd_repeat_data4 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** usb_exchg_pins : RO; bitpos: [8]; default: 0;
         *  Represents whether the D+ and D- pins of USB_SERIAL_JTAG PHY is exchanged.
         *  1: exchanged
         *  0: not exchanged
         */
        uint32_t usb_exchg_pins:1;
        /** usb_otg_fs_exchg_pins : RO; bitpos: [9]; default: 0;
         *  Represents whether the D+ and D- pins of USB_OTG_FS PHY is exchanged.
         *  1: exchanged
         *  0: not exchanged
         */
        uint32_t usb_otg_fs_exchg_pins:1;
        /** usb_phy_sel : RO; bitpos: [10]; default: 0;
         *  Represents whether to exchange the USB_SERIAL_JTAG PHY with USB_OTG_FS PHY.
         *  1: exchanged.
         *  0: not exchanged.
         */
        uint32_t usb_phy_sel:1;
        /** soft_dis_jtag : RO; bitpos: [13:11]; default: 0;
         *  Represents whether JTAG is disabled in soft way.
         *  Odd number: disabled
         *  Even number: enabled
         */
        uint32_t soft_dis_jtag:3;
        /** io_ldo_adjust : RO; bitpos: [21:14]; default: 0;
         *  Represents configuration of IO LDO mode and voltage.
         */
        uint32_t io_ldo_adjust:8;
        /** io_ldo_1p8 : RO; bitpos: [22]; default: 0;
         *  Represents select IO LDO voltage to 1.8V or 3.3V.
         *  1: 1.8V
         *  0: 3.3V
         */
        uint32_t io_ldo_1p8:1;
        /** dcdc_ccm_en : RO; bitpos: [23]; default: 0;
         *  Represents whether change DCDC to CCM mode
         */
        uint32_t dcdc_ccm_en:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} efuse_rd_repeat_data4_reg_t;


/** Group: block1 registers */
/** Type of rd_mac_sys0 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** mac_0 : RO; bitpos: [31:0]; default: 0;
         *  Represents MAC address. Low 32-bit.
         */
        uint32_t mac_0:32;
    };
    uint32_t val;
} efuse_rd_mac_sys0_reg_t;

/** Type of rd_mac_sys1 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** mac_1 : RO; bitpos: [15:0]; default: 0;
         *  Represents MAC address. High 16-bit.
         */
        uint32_t mac_1:16;
        /** mac_ext : RO; bitpos: [31:16]; default: 0;
         *  Represents the extended bits of MAC address.
         */
        uint32_t mac_ext:16;
    };
    uint32_t val;
} efuse_rd_mac_sys1_reg_t;

/** Type of rd_mac_sys2 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** pvt_limit : RO; bitpos: [15:0]; default: 0;
         *  Power glitch monitor threthold.
         */
        uint32_t pvt_limit:16;
        /** pvt_cell_select : RO; bitpos: [22:16]; default: 0;
         *  Power glitch monitor PVT cell select.
         */
        uint32_t pvt_cell_select:7;
        /** pvt_pump_limit : RO; bitpos: [30:23]; default: 0;
         *  Use to configure voltage monitor limit for charge pump
         */
        uint32_t pvt_pump_limit:8;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} efuse_rd_mac_sys2_reg_t;

/** Type of rd_mac_sys3 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** pump_drv : RO; bitpos: [3:0]; default: 0;
         *  Use to configure charge pump voltage gain
         */
        uint32_t pump_drv:4;
        /** wdt_delay_sel : RO; bitpos: [5:4]; default: 0;
         *  Represents the threshold level of the RTC watchdog STG0 timeout.
         *  0: Original threshold configuration value of STG0 *2
         *  1: Original threshold configuration value of STG0 *4
         *  2: Original threshold configuration value of STG0 *8
         *  3: Original threshold configuration value of STG0 *16
         */
        uint32_t wdt_delay_sel:2;
        /** hys_en_pad : RO; bitpos: [6]; default: 0;
         *  Represents whether the hysteresis function of corresponding PAD is enabled.
         *  1: enabled
         *  0:disabled
         */
        uint32_t hys_en_pad:1;
        /** pvt_glitch_charge_reset : RO; bitpos: [7]; default: 0;
         *  Represents whether to trigger reset or charge pump when PVT power glitch happened.
         *  1:Trigger charge pump.
         *  0:Trigger reset
         */
        uint32_t pvt_glitch_charge_reset:1;
        uint32_t reserved_8:1;
        /** vdd_spi_ldo_adjust : RO; bitpos: [16:9]; default: 0;
         *  Represents configuration of FLASH LDO mode and voltage.
         */
        uint32_t vdd_spi_ldo_adjust:8;
        /** flash_ldo_power_sel : RO; bitpos: [17]; default: 0;
         *  Represents which flash ldo be select:
         *  1: FLASH LDO 1P2
         *  0 : FLASH LDO 1P8
         */
        uint32_t flash_ldo_power_sel:1;
        /** sys_data_part0_0 : RO; bitpos: [31:18]; default: 0;
         *  Represents the first 14-bit of zeroth part of system data.
         */
        uint32_t sys_data_part0_0:14;
    };
    uint32_t val;
} efuse_rd_mac_sys3_reg_t;

/** Type of rd_mac_sys4 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** sys_data_part0_1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the first 14-bit of zeroth part of system data.
         */
        uint32_t sys_data_part0_1:32;
    };
    uint32_t val;
} efuse_rd_mac_sys4_reg_t;

/** Type of rd_mac_sys5 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** sys_data_part0_2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the second 32-bit of zeroth part of system data.
         */
        uint32_t sys_data_part0_2:32;
    };
    uint32_t val;
} efuse_rd_mac_sys5_reg_t;


/** Group: block2 registers */
/** Type of rd_sys_part1_datan register
 *  Represents rd_sys_part1_datan
 */
typedef union {
    struct {
        /** sys_data_part1_n : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of first part of system data.
         */
        uint32_t sys_data_part1_n:32;
    };
    uint32_t val;
} efuse_rd_sys_part1_datan_reg_t;


/** Group: block3 registers */
/** Type of rd_usr_datan register
 *  Represents rd_usr_datan
 */
typedef union {
    struct {
        /** usr_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of block3 (user).
         */
        uint32_t usr_datan:32;
    };
    uint32_t val;
} efuse_rd_usr_datan_reg_t;


/** Group: block4 registers */
/** Type of rd_key0_datan register
 *  Represents rd_key0_datan
 */
typedef union {
    struct {
        /** key0_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key0.
         */
        uint32_t key0_datan:32;
    };
    uint32_t val;
} efuse_rd_key0_datan_reg_t;


/** Group: block5 registers */
/** Type of rd_key1_datan register
 *  Represents rd_key1_datan
 */
typedef union {
    struct {
        /** key1_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key1.
         */
        uint32_t key1_datan:32;
    };
    uint32_t val;
} efuse_rd_key1_datan_reg_t;


/** Group: block6 registers */
/** Type of rd_key2_datan register
 *  Represents rd_key2_datan
 */
typedef union {
    struct {
        /** key2_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key2.
         */
        uint32_t key2_datan:32;
    };
    uint32_t val;
} efuse_rd_key2_datan_reg_t;


/** Group: block7 registers */
/** Type of rd_key3_datan register
 *  Represents rd_key3_datan
 */
typedef union {
    struct {
        /** key3_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key3.
         */
        uint32_t key3_datan:32;
    };
    uint32_t val;
} efuse_rd_key3_datan_reg_t;


/** Group: block8 registers */
/** Type of rd_key4_datan register
 *  Represents rd_key4_datan
 */
typedef union {
    struct {
        /** key4_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key4.
         */
        uint32_t key4_datan:32;
    };
    uint32_t val;
} efuse_rd_key4_datan_reg_t;


/** Group: block9 registers */
/** Type of rd_key5_datan register
 *  Represents rd_key5_datan
 */
typedef union {
    struct {
        /** key5_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key5.
         */
        uint32_t key5_datan:32;
    };
    uint32_t val;
} efuse_rd_key5_datan_reg_t;


/** Group: block10 registers */
/** Type of rd_sys_part2_datan register
 *  Represents rd_sys_part2_datan
 */
typedef union {
    struct {
        /** sys_data_part2_n : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of second part of system data.
         */
        uint32_t sys_data_part2_n:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_datan_reg_t;


/** Group: block0 error report registers */
/** Type of rd_repeat_data_err0 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** rd_dis_err : RO; bitpos: [6:0]; default: 0;
         *  Represents the programming error of EFUSE_RD_DIS
         */
        uint32_t rd_dis_err:7;
        /** dis_usb_jtag_err : RO; bitpos: [7]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_JTAG
         */
        uint32_t dis_usb_jtag_err:1;
        uint32_t reserved_8:1;
        /** dis_force_download_err : RO; bitpos: [9]; default: 0;
         *  Represents the programming error of EFUSE_DIS_FORCE_DOWNLOAD
         */
        uint32_t dis_force_download_err:1;
        /** spi_download_mspi_dis_err : RO; bitpos: [10]; default: 0;
         *  Represents the programming error of EFUSE_SPI_DOWNLOAD_MSPI_DIS
         */
        uint32_t spi_download_mspi_dis_err:1;
        /** dis_twai_err : RO; bitpos: [11]; default: 0;
         *  Represents the programming error of EFUSE_DIS_TWAI
         */
        uint32_t dis_twai_err:1;
        /** jtag_sel_enable_err : RO; bitpos: [12]; default: 0;
         *  Represents the programming error of EFUSE_JTAG_SEL_ENABLE
         */
        uint32_t jtag_sel_enable_err:1;
        /** dis_pad_jtag_err : RO; bitpos: [13]; default: 0;
         *  Represents the programming error of EFUSE_DIS_PAD_JTAG
         */
        uint32_t dis_pad_jtag_err:1;
        /** dis_download_manual_encrypt_err : RO; bitpos: [14]; default: 0;
         *  Represents the programming error of EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT
         */
        uint32_t dis_download_manual_encrypt_err:1;
        uint32_t reserved_15:3;
        /** pvt_glitch_en_err : RO; bitpos: [18]; default: 0;
         *  Represents the programming error of EFUSE_PVT_GLITCH_EN
         */
        uint32_t pvt_glitch_en_err:1;
        uint32_t reserved_19:1;
        /** pvt_glitch_mode_err : RO; bitpos: [21:20]; default: 0;
         *  Represents the programming error of EFUSE_PVT_GLITCH_MODE
         */
        uint32_t pvt_glitch_mode_err:2;
        /** dis_core1_err : RO; bitpos: [22]; default: 0;
         *  Represents the programming error of EFUSE_DIS_CORE1
         */
        uint32_t dis_core1_err:1;
        /** spi_boot_crypt_cnt_err : RO; bitpos: [25:23]; default: 0;
         *  Represents the programming error of EFUSE_SPI_BOOT_CRYPT_CNT
         */
        uint32_t spi_boot_crypt_cnt_err:3;
        /** secure_boot_key_revoke0_err : RO; bitpos: [26]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE0
         */
        uint32_t secure_boot_key_revoke0_err:1;
        /** secure_boot_key_revoke1_err : RO; bitpos: [27]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE1
         */
        uint32_t secure_boot_key_revoke1_err:1;
        /** secure_boot_key_revoke2_err : RO; bitpos: [28]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE2
         */
        uint32_t secure_boot_key_revoke2_err:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} efuse_rd_repeat_data_err0_reg_t;

/** Type of rd_repeat_data_err1 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** key_purpose_0_err : RO; bitpos: [4:0]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_0
         */
        uint32_t key_purpose_0_err:5;
        /** key_purpose_1_err : RO; bitpos: [9:5]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_1
         */
        uint32_t key_purpose_1_err:5;
        /** key_purpose_2_err : RO; bitpos: [14:10]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_2
         */
        uint32_t key_purpose_2_err:5;
        /** key_purpose_3_err : RO; bitpos: [19:15]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_3
         */
        uint32_t key_purpose_3_err:5;
        /** key_purpose_4_err : RO; bitpos: [24:20]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_4
         */
        uint32_t key_purpose_4_err:5;
        /** key_purpose_5_err : RO; bitpos: [29:25]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_5
         */
        uint32_t key_purpose_5_err:5;
        /** sec_dpa_level_err : RO; bitpos: [31:30]; default: 0;
         *  Represents the programming error of EFUSE_SEC_DPA_LEVEL
         */
        uint32_t sec_dpa_level_err:2;
    };
    uint32_t val;
} efuse_rd_repeat_data_err1_reg_t;

/** Type of rd_repeat_data_err2 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** xts_dpa_pseudo_level_err : RO; bitpos: [1:0]; default: 0;
         *  Represents the programming error of EFUSE_XTS_DPA_PSEUDO_LEVEL
         */
        uint32_t xts_dpa_pseudo_level_err:2;
        /** xts_dpa_clk_enable_err : RO; bitpos: [2]; default: 0;
         *  Represents the programming error of EFUSE_XTS_DPA_CLK_ENABLE
         */
        uint32_t xts_dpa_clk_enable_err:1;
        /** ecc_force_const_time_err : RO; bitpos: [3]; default: 0;
         *  Represents the programming error of EFUSE_ECC_FORCE_CONST_TIME
         */
        uint32_t ecc_force_const_time_err:1;
        /** ecdsa_p384_enable_err : RO; bitpos: [4]; default: 0;
         *  Represents the programming error of EFUSE_ECDSA_P384_ENABLE
         */
        uint32_t ecdsa_p384_enable_err:1;
        /** secure_boot_en_err : RO; bitpos: [5]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_EN
         */
        uint32_t secure_boot_en_err:1;
        /** secure_boot_aggressive_revoke_err : RO; bitpos: [6]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE
         */
        uint32_t secure_boot_aggressive_revoke_err:1;
        /** km_disable_deploy_mode_err : RO; bitpos: [11:7]; default: 0;
         *  Represents the programming error of EFUSE_KM_DISABLE_DEPLOY_MODE
         */
        uint32_t km_disable_deploy_mode_err:5;
        /** km_rnd_switch_cycle_err : RO; bitpos: [13:12]; default: 0;
         *  Represents the programming error of EFUSE_KM_RND_SWITCH_CYCLE
         */
        uint32_t km_rnd_switch_cycle_err:2;
        /** km_deploy_only_once_err : RO; bitpos: [18:14]; default: 0;
         *  Represents the programming error of EFUSE_KM_DEPLOY_ONLY_ONCE
         */
        uint32_t km_deploy_only_once_err:5;
        /** force_use_key_manager_key_err : RO; bitpos: [23:19]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_USE_KEY_MANAGER_KEY
         */
        uint32_t force_use_key_manager_key_err:5;
        /** force_disable_sw_init_key_err : RO; bitpos: [24]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_DISABLE_SW_INIT_KEY
         */
        uint32_t force_disable_sw_init_key_err:1;
        /** km_xts_key_length_256_err : RO; bitpos: [25]; default: 0;
         *  Represents the programming error of EFUSE_KM_XTS_KEY_LENGTH_256
         */
        uint32_t km_xts_key_length_256_err:1;
        /** lock_km_key_err : RO; bitpos: [26]; default: 0;
         *  Represents the programming error of EFUSE_LOCK_KM_KEY
         */
        uint32_t lock_km_key_err:1;
        /** flash_tpuw_err : RO; bitpos: [29:27]; default: 0;
         *  Represents the programming error of EFUSE_FLASH_TPUW
         */
        uint32_t flash_tpuw_err:3;
        uint32_t reserved_30:1;
        /** dis_download_mode_err : RO; bitpos: [31]; default: 0;
         *  Represents the programming error of EFUSE_DIS_DOWNLOAD_MODE
         */
        uint32_t dis_download_mode_err:1;
    };
    uint32_t val;
} efuse_rd_repeat_data_err2_reg_t;

/** Type of rd_repeat_data_err3 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** dis_direct_boot_err : RO; bitpos: [0]; default: 0;
         *  Represents the programming error of EFUSE_DIS_DIRECT_BOOT
         */
        uint32_t dis_direct_boot_err:1;
        /** dis_usb_serial_jtag_rom_print_err : RO; bitpos: [1]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT
         */
        uint32_t dis_usb_serial_jtag_rom_print_err:1;
        /** dis_usb_serial_jtag_download_mode_err : RO; bitpos: [2]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE
         */
        uint32_t dis_usb_serial_jtag_download_mode_err:1;
        /** enable_security_download_err : RO; bitpos: [3]; default: 0;
         *  Represents the programming error of EFUSE_ENABLE_SECURITY_DOWNLOAD
         */
        uint32_t enable_security_download_err:1;
        /** uart_print_control_err : RO; bitpos: [5:4]; default: 0;
         *  Represents the programming error of EFUSE_UART_PRINT_CONTROL
         */
        uint32_t uart_print_control_err:2;
        /** force_send_resume_err : RO; bitpos: [6]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_SEND_RESUME
         */
        uint32_t force_send_resume_err:1;
        /** secure_version_err : RO; bitpos: [22:7]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_VERSION
         */
        uint32_t secure_version_err:16;
        /** huk_gen_state_err : RO; bitpos: [27:23]; default: 0;
         *  Represents the programming error of EFUSE_HUK_GEN_STATE
         */
        uint32_t huk_gen_state_err:5;
        /** flash_ldo_efuse_sel_err : RO; bitpos: [28]; default: 0;
         *  Represents the programming error of EFUSE_FLASH_LDO_EFUSE_SEL
         */
        uint32_t flash_ldo_efuse_sel_err:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} efuse_rd_repeat_data_err3_reg_t;

/** Type of rd_repeat_data_err4 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** usb_exchg_pins_err : RO; bitpos: [8]; default: 0;
         *  Represents the programming error of EFUSE_USB_EXCHG_PINS
         */
        uint32_t usb_exchg_pins_err:1;
        /** usb_otg_fs_exchg_pins_err : RO; bitpos: [9]; default: 0;
         *  Represents the programming error of EFUSE_USB_OTG_FS_EXCHG_PINS
         */
        uint32_t usb_otg_fs_exchg_pins_err:1;
        /** usb_phy_sel_err : RO; bitpos: [10]; default: 0;
         *  Represents the programming error of EFUSE_USB_PHY_SEL
         */
        uint32_t usb_phy_sel_err:1;
        /** soft_dis_jtag_err : RO; bitpos: [13:11]; default: 0;
         *  Represents the programming error of EFUSE_SOFT_DIS_JTAG
         */
        uint32_t soft_dis_jtag_err:3;
        /** io_ldo_adjust_err : RO; bitpos: [21:14]; default: 0;
         *  Represents the programming error of EFUSE_IO_LDO_ADJUST
         */
        uint32_t io_ldo_adjust_err:8;
        /** io_ldo_1p8_err : RO; bitpos: [22]; default: 0;
         *  Represents the programming error of EFUSE_IO_LDO_1P8
         */
        uint32_t io_ldo_1p8_err:1;
        /** dcdc_ccm_en_err : RO; bitpos: [23]; default: 0;
         *  Represents the programming error of EFUSE_DCDC_CCM_EN
         */
        uint32_t dcdc_ccm_en_err:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} efuse_rd_repeat_data_err4_reg_t;


/** Group: RS block error report registers */
/** Type of rd_rs_data_err0 register
 *  Represents rd_rs_data_err
 */
typedef union {
    struct {
        /** rd_mac_sys_err_num : RO; bitpos: [2:0]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_mac_sys
         */
        uint32_t rd_mac_sys_err_num:3;
        /** rd_mac_sys_fail : RO; bitpos: [3]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_mac_sys is reliable
         *  1: Means that programming rd_mac_sys failed and the number of error bytes is over 6.
         */
        uint32_t rd_mac_sys_fail:1;
        /** rd_sys_part1_data_err_num : RO; bitpos: [6:4]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_sys_part1_data
         */
        uint32_t rd_sys_part1_data_err_num:3;
        /** rd_sys_part1_data_fail : RO; bitpos: [7]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_sys_part1_data is reliable
         *  1: Means that programming rd_sys_part1_data failed and the number of error bytes is
         *  over 6.
         */
        uint32_t rd_sys_part1_data_fail:1;
        /** rd_usr_data_err_num : RO; bitpos: [10:8]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_usr_data
         */
        uint32_t rd_usr_data_err_num:3;
        /** rd_usr_data_fail : RO; bitpos: [11]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_usr_data is reliable
         *  1: Means that programming rd_usr_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_usr_data_fail:1;
        /** rd_key0_data_err_num : RO; bitpos: [14:12]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key0_data
         */
        uint32_t rd_key0_data_err_num:3;
        /** rd_key0_data_fail : RO; bitpos: [15]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key0_data is reliable
         *  1: Means that programming rd_key0_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key0_data_fail:1;
        /** rd_key1_data_err_num : RO; bitpos: [18:16]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key1_data
         */
        uint32_t rd_key1_data_err_num:3;
        /** rd_key1_data_fail : RO; bitpos: [19]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key1_data is reliable
         *  1: Means that programming rd_key1_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key1_data_fail:1;
        /** rd_key2_data_err_num : RO; bitpos: [22:20]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key2_data
         */
        uint32_t rd_key2_data_err_num:3;
        /** rd_key2_data_fail : RO; bitpos: [23]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key2_data is reliable
         *  1: Means that programming rd_key2_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key2_data_fail:1;
        /** rd_key3_data_err_num : RO; bitpos: [26:24]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key3_data
         */
        uint32_t rd_key3_data_err_num:3;
        /** rd_key3_data_fail : RO; bitpos: [27]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key3_data is reliable
         *  1: Means that programming rd_key3_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key3_data_fail:1;
        /** rd_key4_data_err_num : RO; bitpos: [30:28]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key4_data
         */
        uint32_t rd_key4_data_err_num:3;
        /** rd_key4_data_fail : RO; bitpos: [31]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key4_data is reliable
         *  1: Means that programming rd_key4_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key4_data_fail:1;
    };
    uint32_t val;
} efuse_rd_rs_data_err0_reg_t;

/** Type of rd_rs_data_err1 register
 *  Represents rd_rs_data_err
 */
typedef union {
    struct {
        /** rd_key5_data_err_num : RO; bitpos: [2:0]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key5_data
         */
        uint32_t rd_key5_data_err_num:3;
        /** rd_key5_data_fail : RO; bitpos: [3]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key5_data is reliable
         *  1: Means that programming rd_key5_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key5_data_fail:1;
        /** rd_sys_part2_data_err_num : RO; bitpos: [6:4]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_sys_part2_data
         */
        uint32_t rd_sys_part2_data_err_num:3;
        /** rd_sys_part2_data_fail : RO; bitpos: [7]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_sys_part2_data is reliable
         *  1: Means that programming rd_sys_part2_data failed and the number of error bytes is
         *  over 6.
         */
        uint32_t rd_sys_part2_data_fail:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} efuse_rd_rs_data_err1_reg_t;


/** Group: EFUSE Version Register */
/** Type of date register
 *  eFuse version register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38801528;
         *  Represents eFuse version. Date:2025-01-07 18:29:09,
         *  ScriptRev:4f86fc8055439e38ad20ae9516f64400592b3577
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} efuse_date_reg_t;


/** Group: EFUSE Clock Registers */
/** Type of clk register
 *  eFuse clcok configuration register.
 */
typedef union {
    struct {
        /** mem_force_pd : R/W; bitpos: [0]; default: 0;
         *  Configures whether to force power down eFuse SRAM.
         *  1: Force
         *  0: No effect
         */
        uint32_t mem_force_pd:1;
        /** mem_clk_force_on : R/W; bitpos: [1]; default: 1;
         *  Configures whether to force activate clock signal of eFuse SRAM.
         *  1: Force activate
         *  0: No effect
         */
        uint32_t mem_clk_force_on:1;
        /** mem_force_pu : R/W; bitpos: [2]; default: 0;
         *  Configures whether to force power up eFuse SRAM.
         *  1: Force
         *  0: No effect
         */
        uint32_t mem_force_pu:1;
        uint32_t reserved_3:13;
        /** clk_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether to force enable eFuse register configuration clock signal.
         *  1: Force
         *  0: The clock is enabled only during the reading and writing of registers
         */
        uint32_t clk_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} efuse_clk_reg_t;


/** Group: EFUSE Configure Registers */
/** Type of conf register
 *  eFuse operation mode configuration register
 */
typedef union {
    struct {
        /** op_code : R/W; bitpos: [15:0]; default: 0;
         *  Configures operation command type.
         *  0x5A5A: Program operation command
         *  0x5AA5: Read operation command
         *  Other values: No effect
         */
        uint32_t op_code:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} efuse_conf_reg_t;

/** Type of dac_conf register
 *  Controls the eFuse programming voltage.
 */
typedef union {
    struct {
        /** dac_clk_div : R/W; bitpos: [7:0]; default: 19;
         *  Configures the division factor of the rising clock of the programming voltage.
         */
        uint32_t dac_clk_div:8;
        /** dac_clk_pad_sel : R/W; bitpos: [8]; default: 0;
         *  Don't care.
         */
        uint32_t dac_clk_pad_sel:1;
        /** dac_num : R/W; bitpos: [16:9]; default: 255;
         *  Configures clock cycles for programming voltage to rise. Measurement unit: a clock
         *  cycle divided by EFUSE_DAC_CLK_DIV.
         */
        uint32_t dac_num:8;
        /** oe_clr : R/W; bitpos: [17]; default: 0;
         *  Configures whether to reduce the power supply of programming voltage.
         *  0: Not reduce
         *  1: Reduce
         */
        uint32_t oe_clr:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} efuse_dac_conf_reg_t;

/** Type of rd_tim_conf register
 *  Configures read timing parameters.
 */
typedef union {
    struct {
        /** thr_a : R/W; bitpos: [7:0]; default: 1;
         *  Configures the read hold time. Measurement unit: One cycle of the eFuse core clock.
         */
        uint32_t thr_a:8;
        /** trd : R/W; bitpos: [15:8]; default: 2;
         *  Configures the read time. Measurement unit: One cycle of the eFuse core clock.
         */
        uint32_t trd:8;
        /** tsur_a : R/W; bitpos: [23:16]; default: 1;
         *  Configures the read setup time. Measurement unit: One cycle of the eFuse core clock.
         */
        uint32_t tsur_a:8;
        /** read_init_num : R/W; bitpos: [31:24]; default: 18;
         *  Configures the waiting time of reading eFuse memory. Measurement unit: One cycle of
         *  the eFuse core clock.
         */
        uint32_t read_init_num:8;
    };
    uint32_t val;
} efuse_rd_tim_conf_reg_t;

/** Type of wr_tim_conf1 register
 *  Configurarion register 1 of eFuse programming timing parameters.
 */
typedef union {
    struct {
        /** tsup_a : R/W; bitpos: [7:0]; default: 1;
         *  Configures the programming setup time. Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t tsup_a:8;
        /** pwr_on_num : R/W; bitpos: [23:8]; default: 9831;
         *  Configures the power up time for VDDQ. Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t pwr_on_num:16;
        /** thp_a : R/W; bitpos: [31:24]; default: 1;
         *  Configures the programming hold time. Measurement unit: One cycle of the eFuse core
         *  clock.
         */
        uint32_t thp_a:8;
    };
    uint32_t val;
} efuse_wr_tim_conf1_reg_t;

/** Type of wr_tim_conf2 register
 *  Configurarion register 2 of eFuse programming timing parameters.
 */
typedef union {
    struct {
        /** pwr_off_num : R/W; bitpos: [15:0]; default: 320;
         *  Configures the power outage time for VDDQ. Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t pwr_off_num:16;
        /** tpgm : R/W; bitpos: [31:16]; default: 160;
         *  Configures the active programming time. Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t tpgm:16;
    };
    uint32_t val;
} efuse_wr_tim_conf2_reg_t;

/** Type of wr_tim_conf0_rs_bypass register
 *  Configurarion register0 of eFuse programming time parameters and rs bypass
 *  operation.
 */
typedef union {
    struct {
        /** bypass_rs_correction : R/W; bitpos: [0]; default: 0;
         *  Configures whether to bypass the Reed-Solomon (RS) correction step.
         *  0: Not bypass
         *  1: Bypass
         */
        uint32_t bypass_rs_correction:1;
        /** bypass_rs_blk_num : R/W; bitpos: [11:1]; default: 0;
         *  Configures which block number to bypass the Reed-Solomon (RS) correction step.
         */
        uint32_t bypass_rs_blk_num:11;
        /** update : WT; bitpos: [12]; default: 0;
         *  Configures whether to update multi-bit register signals.
         *  1: Update
         *  0: No effect
         */
        uint32_t update:1;
        /** tpgm_inactive : R/W; bitpos: [20:13]; default: 1;
         *  Configures the inactive programming time. Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t tpgm_inactive:8;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} efuse_wr_tim_conf0_rs_bypass_reg_t;


/** Group: EFUSE ECDSA Configure Registers */
/** Type of ecdsa register
 *  eFuse status register.
 */
typedef union {
    struct {
        /** cfg_ecdsa_p192_blk : R/W; bitpos: [3:0]; default: 0;
         *  Configures which block to use for ECDSA P192 key output.
         */
        uint32_t cfg_ecdsa_p192_blk:4;
        /** cfg_ecdsa_p256_blk : R/W; bitpos: [7:4]; default: 0;
         *  Configures which block to use for ECDSA P256 key output.
         */
        uint32_t cfg_ecdsa_p256_blk:4;
        /** cfg_ecdsa_p384_l_blk : R/W; bitpos: [11:8]; default: 0;
         *  Configures which block to use for ECDSA P384 key low part output.
         */
        uint32_t cfg_ecdsa_p384_l_blk:4;
        /** cfg_ecdsa_p384_h_blk : R/W; bitpos: [15:12]; default: 0;
         *  Configures which block to use for ECDSA P256 key high part output.
         */
        uint32_t cfg_ecdsa_p384_h_blk:4;
        /** cur_ecdsa_p192_blk : RO; bitpos: [19:16]; default: 0;
         *  Represents which block is used for ECDSA P192 key output.
         */
        uint32_t cur_ecdsa_p192_blk:4;
        /** cur_ecdsa_p256_blk : RO; bitpos: [23:20]; default: 0;
         *  Represents which block is used for ECDSA P256 key output.
         */
        uint32_t cur_ecdsa_p256_blk:4;
        /** cur_ecdsa_p384_l_blk : RO; bitpos: [27:24]; default: 0;
         *  Represents which block is used for ECDSA P384 key low part output.
         */
        uint32_t cur_ecdsa_p384_l_blk:4;
        /** cur_ecdsa_p384_h_blk : RO; bitpos: [31:28]; default: 0;
         *  Represents which block is used for ECDSA P384 key high part output.
         */
        uint32_t cur_ecdsa_p384_h_blk:4;
    };
    uint32_t val;
} efuse_ecdsa_reg_t;


/** Group: EFUSE Status Registers */
/** Type of status register
 *  eFuse status register.
 */
typedef union {
    struct {
        /** state : RO; bitpos: [3:0]; default: 0;
         *  Represents the state of the eFuse state machine.
         *  0: Reset state, the initial state after power-up
         *  1: Idle state
         *  Other values: Non-idle state
         */
        uint32_t state:4;
        uint32_t reserved_4:6;
        /** blk0_valid_bit_cnt : RO; bitpos: [19:10]; default: 0;
         *  Represents the number of block valid bit.
         */
        uint32_t blk0_valid_bit_cnt:10;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} efuse_status_reg_t;


/** Group: EFUSE Command Registers */
/** Type of cmd register
 *  eFuse command register.
 */
typedef union {
    struct {
        /** read_cmd : R/W/SC; bitpos: [0]; default: 0;
         *  Configures whether to send read commands.
         *  1: Send
         *  0: No effect
         */
        uint32_t read_cmd:1;
        /** pgm_cmd : R/W/SC; bitpos: [1]; default: 0;
         *  Configures whether to send programming commands.
         *  1: Send
         *  0: No effect
         */
        uint32_t pgm_cmd:1;
        /** blk_num : R/W; bitpos: [5:2]; default: 0;
         *  Configures the serial number of the block to be programmed. Value 0-10 corresponds
         *  to block number 0-10, respectively.
         */
        uint32_t blk_num:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} efuse_cmd_reg_t;


/** Group: Interrupt Registers */
/** Type of int_raw register
 *  eFuse raw interrupt register.
 */
typedef union {
    struct {
        /** read_done_int_raw : R/SS/WTC; bitpos: [0]; default: 0;
         *  The raw interrupt status of EFUSE_READ_DONE_INT.
         */
        uint32_t read_done_int_raw:1;
        /** pgm_done_int_raw : R/SS/WTC; bitpos: [1]; default: 0;
         *  The raw interrupt status of EFUSE_PGM_DONE_INT.
         */
        uint32_t pgm_done_int_raw:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_raw_reg_t;

/** Type of int_st register
 *  eFuse interrupt status register.
 */
typedef union {
    struct {
        /** read_done_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of EFUSE_READ_DONE_INT.
         */
        uint32_t read_done_int_st:1;
        /** pgm_done_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of EFUSE_PGM_DONE_INT.
         */
        uint32_t pgm_done_int_st:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_st_reg_t;

/** Type of int_ena register
 *  eFuse interrupt enable register.
 */
typedef union {
    struct {
        /** read_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable EFUSE_READ_DONE_INT.
         */
        uint32_t read_done_int_ena:1;
        /** pgm_done_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable EFUSE_PGM_DONE_INT.
         */
        uint32_t pgm_done_int_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_ena_reg_t;

/** Type of int_clr register
 *  eFuse interrupt clear register.
 */
typedef union {
    struct {
        /** read_done_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear EFUSE_READ_DONE_INT.
         */
        uint32_t read_done_int_clr:1;
        /** pgm_done_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear EFUSE_PGM_DONE_INT.
         */
        uint32_t pgm_done_int_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_clr_reg_t;


typedef struct {
    volatile efuse_pgm_datan_reg_t pgm_datan[8];
    volatile efuse_pgm_check_valuen_reg_t pgm_check_valuen[3];
    volatile efuse_rd_wr_dis0_reg_t rd_wr_dis0;
    volatile efuse_rd_repeat_data0_reg_t rd_repeat_data0;
    volatile efuse_rd_repeat_data1_reg_t rd_repeat_data1;
    volatile efuse_rd_repeat_data2_reg_t rd_repeat_data2;
    volatile efuse_rd_repeat_data3_reg_t rd_repeat_data3;
    volatile efuse_rd_repeat_data4_reg_t rd_repeat_data4;
    volatile efuse_rd_mac_sys0_reg_t rd_mac_sys0;
    volatile efuse_rd_mac_sys1_reg_t rd_mac_sys1;
    volatile efuse_rd_mac_sys2_reg_t rd_mac_sys2;
    volatile efuse_rd_mac_sys3_reg_t rd_mac_sys3;
    volatile efuse_rd_mac_sys4_reg_t rd_mac_sys4;
    volatile efuse_rd_mac_sys5_reg_t rd_mac_sys5;
    volatile efuse_rd_sys_part1_datan_reg_t rd_sys_part1_datan[8];
    volatile efuse_rd_usr_datan_reg_t rd_usr_datan[8];
    volatile efuse_rd_key0_datan_reg_t rd_key0_datan[8];
    volatile efuse_rd_key1_datan_reg_t rd_key1_datan[8];
    volatile efuse_rd_key2_datan_reg_t rd_key2_datan[8];
    volatile efuse_rd_key3_datan_reg_t rd_key3_datan[8];
    volatile efuse_rd_key4_datan_reg_t rd_key4_datan[8];
    volatile efuse_rd_key5_datan_reg_t rd_key5_datan[8];
    volatile efuse_rd_sys_part2_datan_reg_t rd_sys_part2_datan[8];
    volatile efuse_rd_repeat_data_err0_reg_t rd_repeat_data_err0;
    volatile efuse_rd_repeat_data_err1_reg_t rd_repeat_data_err1;
    volatile efuse_rd_repeat_data_err2_reg_t rd_repeat_data_err2;
    volatile efuse_rd_repeat_data_err3_reg_t rd_repeat_data_err3;
    volatile efuse_rd_repeat_data_err4_reg_t rd_repeat_data_err4;
    volatile efuse_rd_rs_data_err0_reg_t rd_rs_data_err0;
    volatile efuse_rd_rs_data_err1_reg_t rd_rs_data_err1;
    volatile efuse_date_reg_t date;
    uint32_t reserved_19c[11];
    volatile efuse_clk_reg_t clk;
    volatile efuse_conf_reg_t conf;
    volatile efuse_ecdsa_reg_t ecdsa;
    volatile efuse_status_reg_t status;
    volatile efuse_cmd_reg_t cmd;
    volatile efuse_int_raw_reg_t int_raw;
    volatile efuse_int_st_reg_t int_st;
    volatile efuse_int_ena_reg_t int_ena;
    volatile efuse_int_clr_reg_t int_clr;
    volatile efuse_dac_conf_reg_t dac_conf;
    volatile efuse_rd_tim_conf_reg_t rd_tim_conf;
    volatile efuse_wr_tim_conf1_reg_t wr_tim_conf1;
    volatile efuse_wr_tim_conf2_reg_t wr_tim_conf2;
    volatile efuse_wr_tim_conf0_rs_bypass_reg_t wr_tim_conf0_rs_bypass;
} efuse_dev_t;

extern efuse_dev_t EFUSE;

#ifndef __cplusplus
_Static_assert(sizeof(efuse_dev_t) == 0x200, "Invalid size of efuse_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
