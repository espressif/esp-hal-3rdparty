/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_log.h"
#include "soc/spi_mem_reg.h"
#include "spi_flash_private.h"
#if CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/spi_timing_config.h"
#endif

static spi_timing_tuning_param_t s_flash_best_timing_tuning_config;
static spi_timing_tuning_param_t s_psram_best_timing_tuning_config;

static spi_timing_config_core_clock_t get_mspi_core_clock(void)
{
    return spi_timing_config_get_core_clock();
}

static uint32_t get_flash_clock_divider(void)
{
#if CONFIG_ESPTOOLPY_FLASHFREQ_20M
    return SPI_TIMING_CORE_CLOCK_MHZ / 20;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
    return SPI_TIMING_CORE_CLOCK_MHZ / 40;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
    return SPI_TIMING_CORE_CLOCK_MHZ / 80;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_120M
    return SPI_TIMING_CORE_CLOCK_MHZ / 120;
#else
    abort();
#endif
}

static uint32_t get_psram_clock_divider(void)
{
#if CONFIG_SPIRAM_SPEED_40M
    return SPI_TIMING_CORE_CLOCK_MHZ / 40;
#elif CONFIG_SPIRAM_SPEED_80M
    return SPI_TIMING_CORE_CLOCK_MHZ / 80;
#else
    //Will enter this branch only if PSRAM is not enable
    return 0;
#endif
}

#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
/**
 * Set timing tuning regs, in order to get successful sample points
 */
static void init_spi1_for_tuning(bool is_flash)
{
    //Get required core clock and module clock settings
    spi_timing_config_core_clock_t core_clock = get_mspi_core_clock();
    //Set SPI1 core clock. SPI0 and SPI1 share the register for core clock. So we only set SPI0 here.
    spi_timing_config_set_core_clock(0, core_clock);
    //Set SPI1 module clock as required
    if (is_flash) {
        uint32_t flash_div = get_flash_clock_divider();
        spi_timing_config_set_flash_clock(1, flash_div);
        //Power on HCLK
        REG_SET_BIT(SPI_MEM_TIMING_CALI_REG(0), SPI_MEM_TIMING_CLK_ENA);
    } else {
        //We use SPI1 Flash to tune PSRAM, PSRAM timing related regs do nothing on SPI1
        uint32_t psram_div = get_psram_clock_divider();
        spi_timing_config_set_flash_clock(1, psram_div);
        //Power on HCLK
        REG_SET_BIT(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(0), SPI_MEM_SPI_SMEM_TIMING_CLK_ENA);
    }
}

/**
 * We use different SPI1 timing tuning config to read data to see if current MSPI sampling is successful.
 * The sampling result will be stored in an array. In this array, successful item will be 1, failed item will be 0.
 */
static void sweep_for_success_sample_points(uint8_t *reference_data, const spi_timing_config_t *config, bool is_flash, uint8_t *out_array)
{
    uint32_t config_idx = 0;
    uint8_t read_data[SPI_TIMING_TEST_DATA_LEN] = {0};

    for (config_idx = 0; config_idx < config->available_config_num; config_idx++) {
        memset(read_data, 0, SPI_TIMING_TEST_DATA_LEN);
#if SPI_TIMING_FLASH_NEEDS_TUNING
        if (is_flash) {
            /**
             * 1. SPI_MEM_DINx_MODE(1), SPI_MEM_DINx_NUM(1) are meaningless
             *    SPI0 and SPI1 share the SPI_MEM_DINx_MODE(0), SPI_MEM_DINx_NUM(0) for FLASH timing tuning
             * 2. We use SPI1 to get the best Flash timing tuning (mode and num) config
             */
            spi_timing_config_flash_set_din_mode_num(0, config->tuning_config_table[config_idx].spi_din_mode, config->tuning_config_table[config_idx].spi_din_num);
            spi_timing_config_flash_set_extra_dummy(1, config->tuning_config_table[config_idx].extra_dummy_len);
            spi_timing_config_flash_read_data(1, read_data, SPI_TIMING_FLASH_TEST_DATA_ADDR, sizeof(read_data));
        }
#endif
#if SPI_TIMING_PSRAM_NEEDS_TUNING
        if (!is_flash) {
            /**
             * 1. SPI_MEM_SPI_SMEM_DINx_MODE(1), SPI_MEM_SPI_SMEM_DINx_NUM(1) are meaningless
             *    SPI0 and SPI1 share the SPI_MEM_SPI_SMEM_DINx_MODE(0), SPI_MEM_SPI_SMEM_DINx_NUM(0) for PSRAM timing tuning
             * 2. We use SPI1 to get the best PSRAM timing tuning (mode and num) config
             */
            spi_timing_config_psram_set_din_mode_num(0, config->tuning_config_table[config_idx].spi_din_mode, config->tuning_config_table[config_idx].spi_din_num);
            spi_timing_config_flash_set_extra_dummy(1, config->tuning_config_table[config_idx].extra_dummy_len);
            spi_timing_config_psram_read_data(1, read_data, SPI_TIMING_PSRAM_TEST_DATA_ADDR, SPI_TIMING_TEST_DATA_LEN);
        }
#endif
        if (memcmp(reference_data, read_data, sizeof(read_data)) == 0) {
            out_array[config_idx] = 1;
        }
    }
}

/**
 * Find consecutive successful sampling points.
 * e.g. array: {1, 1, 0, 0, 1, 1, 1, 0}
 * out_length: 3
 * outout_end_index: 6
 */
static void find_max_consecutive_success_points(uint8_t *array, uint32_t size, uint32_t *out_length, uint32_t *out_end_index)
{
    uint32_t max = 0;
    uint32_t match_num = 0;
    uint32_t i = 0;
    uint32_t end = 0;

    while (i < size) {
        if (array[i]) {
            match_num++;

        } else {
            if (match_num > max) {
                max = match_num;
                end = i - 1;
            }
            match_num = 0;
        }
        i++;
    }

    *out_length = match_num > max ? match_num : max;
    *out_end_index = match_num == size ? size : end;
}

static void select_best_tuning_config(spi_timing_config_t *config, uint32_t length, uint32_t end, bool is_flash)
{
    uint32_t best_point;
    if (length >= 3) {
        best_point = end - length / 2;
    } else {
        best_point = config->default_config_id;
    }

    if (is_flash) {
        s_flash_best_timing_tuning_config = config->tuning_config_table[best_point];
    } else {
        s_psram_best_timing_tuning_config = config->tuning_config_table[best_point];
    }
}

static void do_tuning(uint8_t *reference_data, spi_timing_config_t *timing_config, bool is_flash)
{
    /**
     * We use SPI1 to tune the FLASH timing:
     * 1. Get all SPI1 sampling results.
     * 2. Find the longest consecutive successful sampling points from the result above.
     * 3. The middle one will be the best sampling point.
     */
    uint32_t consecutive_length = 0;
    uint32_t last_success_point = 0;
    uint8_t sample_result[SPI_TIMING_CONFIG_NUM_DEFAULT] = {0};

    init_spi1_for_tuning(is_flash);
    sweep_for_success_sample_points(reference_data, timing_config, is_flash, sample_result);
    find_max_consecutive_success_points(sample_result, SPI_TIMING_CONFIG_NUM_DEFAULT, &consecutive_length, &last_success_point);
    select_best_tuning_config(timing_config, consecutive_length, last_success_point, is_flash);
}
#endif  //#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING


//------------------------------------------FLASH Timing Tuning----------------------------------------//
#if SPI_TIMING_FLASH_NEEDS_TUNING
static void get_flash_tuning_configs(spi_timing_config_t *config)
{
#if SPI_TIMING_FLASH_DTR_MODE
#define FLASH_MODE  DTR_MODE
#else //SPI_TIMING_FLASH_STR_MODE
#define FLASH_MODE  STR_MODE
#endif

#if CONFIG_ESPTOOLPY_FLASHFREQ_20M
    *config = SPI_TIMING_FLASH_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 20, FLASH_MODE);
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
    *config = SPI_TIMING_FLASH_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 40, FLASH_MODE);
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
    *config = SPI_TIMING_FLASH_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 80, FLASH_MODE);
#elif CONFIG_ESPTOOLPY_FLASHFREQ_120M
    *config = SPI_TIMING_FLASH_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 120, FLASH_MODE);
#endif

#undef FLASH_MODE
}

void spi_timing_flash_tuning(void)
{
    /**
     * set SPI01 related regs to 20mhz configuration, to get reference data from FLASH
     * see detailed comments in this function (`spi_timing_enter_mspi_low_speed_mode)
     */
    spi_timing_enter_mspi_low_speed_mode();

    //Disable the variable dummy mode when doing timing tuning
    CLEAR_PERI_REG_MASK(SPI_MEM_DDR_REG(1), SPI_MEM_SPI_FMEM_VAR_DUMMY);    //GD flash will read error in variable mode with 20MHz

    uint8_t reference_data[SPI_TIMING_TEST_DATA_LEN] = {0};
    spi_timing_config_flash_read_data(1, reference_data, SPI_TIMING_FLASH_TEST_DATA_ADDR, sizeof(reference_data));
    spi_timing_config_t timing_configs = {0};
    get_flash_tuning_configs(&timing_configs);

    do_tuning(reference_data, &timing_configs, true);
    spi_timing_enter_mspi_high_speed_mode();
}
#else
void spi_timing_flash_tuning(void)
{
    //Empty function for compatibility, therefore upper layer won't need to know that FLASH in which operation mode and frequency config needs to be tuned
}
#endif  //SPI_TIMING_FLASH_NEEDS_TUNING


//------------------------------------------PSRAM Timing Tuning----------------------------------------//
#if SPI_TIMING_PSRAM_NEEDS_TUNING
static void get_psram_tuning_configs(spi_timing_config_t *config)
{
#if SPI_TIMING_PSRAM_DTR_MODE
#define PSRAM_MODE  DTR_MODE
#else //SPI_TIMING_PSRAM_STR_MODE
#define PSRAM_MODE  STR_MODE
#endif

#if CONFIG_SPIRAM_SPEED_40M
    *config = SPI_TIMING_PSRAM_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 40, PSRAM_MODE);
#elif CONFIG_SPIRAM_SPEED_80M
    *config = SPI_TIMING_PSRAM_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 80, PSRAM_MODE);
#endif

#undef PSRAM_MODE
}

void spi_timing_psram_tuning(void)
{
    /**
     * set SPI01 related regs to 20mhz configuration, to write reference data to PSRAM
     * see detailed comments in this function (`spi_timing_enter_mspi_low_speed_mode)
     */
    spi_timing_enter_mspi_low_speed_mode();

    // write data into psram, used to do timing tuning test.
    uint8_t reference_data[SPI_TIMING_TEST_DATA_LEN];
    for (int i=0; i < SPI_TIMING_TEST_DATA_LEN/4; i++) {
        ((uint32_t *)reference_data)[i] = 0xa5ff005a;
    }
    spi_timing_config_psram_write_data(1, reference_data, SPI_TIMING_PSRAM_TEST_DATA_ADDR, SPI_TIMING_TEST_DATA_LEN);
    spi_timing_config_t timing_configs = {0};
    get_psram_tuning_configs(&timing_configs);

    //Disable the variable dummy mode when doing timing tuning
    CLEAR_PERI_REG_MASK(SPI_MEM_DDR_REG(1), SPI_MEM_SPI_FMEM_VAR_DUMMY);
    //Get required config, and set them to PSRAM related registers
    do_tuning(reference_data, &timing_configs, false);
    spi_timing_enter_mspi_high_speed_mode();
}

#else
void spi_timing_psram_tuning(void)
{
    //Empty function for compatibility, therefore upper layer won't need to know that FLASH in which operation mode and frequency config needs to be tuned
}
#endif  //SPI_TIMING_PSRAM_NEEDS_TUNING


//---------------------------------------------APIs to make SPI0 and SPI1 FLASH work for high/low freq-------------------------------//
static void clear_timing_tuning_regs(void)
{
    spi_timing_config_flash_set_din_mode_num(0, 0, 0);  //SPI0 and SPI1 share the registers for flash din mode and num setting, so we only set SPI0's reg
    spi_timing_config_flash_set_extra_dummy(0, 0);
    spi_timing_config_flash_set_extra_dummy(1, 0);
}

void spi_timing_enter_mspi_low_speed_mode(void)
{
    /**
     * Here we are going to slow the SPI1 frequency to 20Mhz, so we need to set SPI1 din_num and din_mode regs.
     *
     * Because SPI0 and SPI1 share the din_num and din_mode regs, so if we clear SPI1 din_num and din_mode to
     * 0, if the SPI0 flash module clock is still in high freq, it may not work correctly.
     *
     * Therefore, here we need to slow both the SPI0 and SPI1 and related timing tuning regs to 20Mhz configuration.
     */

    //Switch SPI1 and SPI0 clock as 20MHz, set its SPIMEM core clock as 80M and set clock division as 4
    spi_timing_config_set_core_clock(0, SPI_TIMING_CONFIG_CORE_CLOCK_80M);  //SPI0 and SPI1 share the register for core clock. So we only set SPI0 here.
    spi_timing_config_set_flash_clock(1, 4);
    spi_timing_config_set_flash_clock(0, 4);

    clear_timing_tuning_regs();
}

static void set_timing_tuning_regs_as_required(void)
{
    //SPI0 and SPI1 share the registers for flash din mode and num setting, so we only set SPI0's reg
    spi_timing_config_flash_set_din_mode_num(0, s_flash_best_timing_tuning_config.spi_din_mode, s_flash_best_timing_tuning_config.spi_din_num);
    spi_timing_config_flash_set_extra_dummy(0, s_flash_best_timing_tuning_config.extra_dummy_len);
    spi_timing_config_flash_set_extra_dummy(1, s_flash_best_timing_tuning_config.extra_dummy_len);

    spi_timing_config_psram_set_din_mode_num(0, s_psram_best_timing_tuning_config.spi_din_mode, s_psram_best_timing_tuning_config.spi_din_num);
    spi_timing_config_psram_set_extra_dummy(0, s_psram_best_timing_tuning_config.extra_dummy_len);
}

/**
 * Set SPI0 and SPI1 flash module clock, din_num, din_mode and extra dummy,
 * according to the configuration got from timing tuning function (`calculate_best_flash_tuning_config`).
 *
 * This function should always be called after `spi_timing_flash_tuning` or `calculate_best_flash_tuning_config`
 */
void spi_timing_enter_mspi_high_speed_mode(void)
{
    spi_timing_config_core_clock_t core_clock = get_mspi_core_clock();
    uint32_t flash_div = get_flash_clock_divider();
    uint32_t psram_div = get_psram_clock_divider();

    //Set SPI01 core clock
    spi_timing_config_set_core_clock(0, core_clock); //SPI0 and SPI1 share the register for core clock. So we only set SPI0 here.
    //Set FLASH module clock
    spi_timing_config_set_flash_clock(0, flash_div);
    spi_timing_config_set_flash_clock(1, flash_div);
    //Set PSRAM module clock
    spi_timing_config_set_psram_clock(0, psram_div);

    set_timing_tuning_regs_as_required();
}
