/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h> /* Required for NULL constant */
#include <stdint.h>
#include <stdbool.h>
#include "hal/dma2d_types.h"
#include "soc/dma2d_channel.h"
#include "soc/dma2d_struct.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/config.h"
#include "soc/soc.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/hp_system_struct.h"

#define DMA2D_LL_INST_NUM       1
#define DMA2D_LL_GET_HW(id)     (((id) == 0) ? (&DMA2D) : NULL)

#define DMA2D_LL_GET(_attr)     DMA2D_LL_ ## _attr

#define DMA2D_LL_TX_CHANS_PER_INST      4
#define DMA2D_LL_RX_CHANS_PER_INST      3

#define DMA2D_LL_RX_EVENT_MASK               (0x3FFF)
#define DMA2D_LL_TX_EVENT_MASK               (0x1FFF)

#define DMA2D_LL_EVENT_RX_DESC_TASK_OVF      (1<<13)
#define DMA2D_LL_EVENT_RX_FIFO_REORDER_UDF   (1<<12)
#define DMA2D_LL_EVENT_RX_FIFO_REORDER_OVF   (1<<11)
#define DMA2D_LL_EVENT_RX_DESC_EMPTY         (1<<10)
#define DMA2D_LL_EVENT_RX_FIFO_L3_UDF        (1<<9)
#define DMA2D_LL_EVENT_RX_FIFO_L3_OVF        (1<<8)
#define DMA2D_LL_EVENT_RX_FIFO_L2_UDF        (1<<7)
#define DMA2D_LL_EVENT_RX_FIFO_L2_OVF        (1<<6)
#define DMA2D_LL_EVENT_RX_FIFO_L1_UDF        (1<<5)
#define DMA2D_LL_EVENT_RX_FIFO_L1_OVF        (1<<4)
#define DMA2D_LL_EVENT_RX_DESC_ERROR         (1<<3)
#define DMA2D_LL_EVENT_RX_ERR_EOF            (1<<2)
#define DMA2D_LL_EVENT_RX_SUC_EOF            (1<<1)
#define DMA2D_LL_EVENT_RX_DONE               (1<<0)

#define DMA2D_LL_EVENT_TX_DESC_TASK_OVF      (1<<12)
#define DMA2D_LL_EVENT_TX_FIFO_REORDER_UDF   (1<<11)
#define DMA2D_LL_EVENT_TX_FIFO_REORDER_OVF   (1<<10)
#define DMA2D_LL_EVENT_TX_FIFO_L3_UDF        (1<<9)
#define DMA2D_LL_EVENT_TX_FIFO_L3_OVF        (1<<8)
#define DMA2D_LL_EVENT_TX_FIFO_L2_UDF        (1<<7)
#define DMA2D_LL_EVENT_TX_FIFO_L2_OVF        (1<<6)
#define DMA2D_LL_EVENT_TX_FIFO_L1_UDF        (1<<5)
#define DMA2D_LL_EVENT_TX_FIFO_L1_OVF        (1<<4)
#define DMA2D_LL_EVENT_TX_TOTAL_EOF          (1<<3)
#define DMA2D_LL_EVENT_TX_DESC_ERROR         (1<<2)
#define DMA2D_LL_EVENT_TX_EOF                (1<<1)
#define DMA2D_LL_EVENT_TX_DONE               (1<<0)

#define DMA2D_LL_TX_CHANNEL_SUPPORT_RO_MASK        (0U | BIT0)
#define DMA2D_LL_TX_CHANNEL_SUPPORT_CSC_MASK       (0U | BIT0 | BIT1 | BIT2 | BIT3)
#define DMA2D_LL_RX_CHANNEL_SUPPORT_RO_MASK        (0U | BIT0)
#define DMA2D_LL_RX_CHANNEL_SUPPORT_CSC_MASK       (0U | BIT0)

#define DMA2D_LL_TX_CHANNEL_PERIPH_M2M_AVAILABLE_ID_MASK   (0xF0)
#define DMA2D_LL_RX_CHANNEL_PERIPH_M2M_AVAILABLE_ID_MASK   (0xF8)
#define DMA2D_LL_CHANNEL_PERIPH_NO_CHOICE          (7)
#define DMA2D_LL_CHANNEL_PERIPH_SEL_BIT_WIDTH      (3)

#define DMA2D_LL_DESC_ALIGNMENT 8

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DMA2D_LL_MEM_LP_MODE_DEEP_SLEEP,    // memory will enter deep sleep during low power stage, keep memory data
    DMA2D_LL_MEM_LP_MODE_LIGHT_SLEEP,   // memory will enter light sleep during low power stage, keep memory data
    DMA2D_LL_MEM_LP_MODE_SHUT_DOWN,     // memory will be powered down during low power stage
    DMA2D_LL_MEM_LP_MODE_DISABLE,       // disable the low power stage
} dma2d_ll_mem_lp_mode_t;

///////////////////////////////////// Common /////////////////////////////////////////
/**
 * @brief Enable the bus clock for 2D-DMA module
 */
static inline void dma2d_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.dma2d_ctrl0.reg_dma2d_sys_clk_en = enable;
}

/**
 * @brief Reset the 2D-DMA module
 */
static inline void dma2d_ll_reset_register(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.dma2d_ctrl0.reg_dma2d_rst_en = 1;
    HP_SYS_CLKRST.dma2d_ctrl0.reg_dma2d_rst_en = 0;
}

/**
 * @brief Check if the bus clock is enabled for the DMA module
 */
__attribute__((always_inline))
static inline bool dma2d_ll_is_bus_clock_enabled(int group_id)
{
    (void) group_id;
    return HP_SYS_CLKRST.dma2d_ctrl0.reg_dma2d_sys_clk_en;
}

/**
 * @brief Force power on the DMA2D memory block, regardless of the outside PMU logic
 *
 * @param dev Peripheral instance address
 */
static inline void dma2d_ll_mem_force_power_on(dma2d_dev_t *dev)
{
    HP_SYSTEM.sys_dma2d_mem_lp_ctrl.sys_2ddma_mem_force_ctrl = 1;
    HP_SYSTEM.sys_dma2d_mem_lp_ctrl.sys_2ddma_mem_lp_en = 0;
}

/**
 * @brief Force the DMA2D memory block into low power mode, regardless of the outside PMU logic
 *
 * @param dev Peripheral instance address
 */
static inline void dma2d_ll_mem_force_low_power(dma2d_dev_t *dev)
{
    HP_SYSTEM.sys_dma2d_mem_lp_ctrl.sys_2ddma_mem_force_ctrl = 1;
    HP_SYSTEM.sys_dma2d_mem_lp_ctrl.sys_2ddma_mem_lp_en = 1;
}

/**
 * @brief Power control the DMA2D memory block by the outside PMU logic
 *
 * @param dev Peripheral instance address
 */
static inline void dma2d_ll_mem_power_by_pmu(dma2d_dev_t *dev)
{
    HP_SYSTEM.sys_dma2d_mem_lp_ctrl.sys_2ddma_mem_force_ctrl = 0;
    HP_SYSTEM.sys_dma2d_mem_lp_ctrl.sys_2ddma_mem_lp_en = 0;
}

/**
 * @brief Set low power mode for DMA2D memory block
 *
 * @param dev Peripheral instance address
 * @param mode DMA2D memory low power mode in low power stage
 */
static inline void dma2d_ll_mem_set_low_power_mode(dma2d_dev_t *dev, dma2d_ll_mem_lp_mode_t mode)
{
    HP_SYSTEM.sys_dma2d_mem_lp_ctrl.sys_2ddma_mem_lp_mode = mode;
}

/**
 * @brief Enable 2D-DMA module
 */
static inline void dma2d_ll_hw_enable(dma2d_dev_t *dev, bool enable)
{
    dev->rst_conf.clk_en = enable;
    dev->rst_conf.axim_rd_rst = 1;
    dev->rst_conf.axim_rd_rst = 0;
    dev->rst_conf.axim_wr_rst = 1;
    dev->rst_conf.axim_wr_rst = 0;
    dev->out_arb_config.val = 0;
    dev->in_arb_config.val = 0;
}

/**
 * @brief Helper function to get the register field value for different pixel bytes scramble order
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_get_scramble_order_sel(dma2d_scramble_order_t order)
{
    switch (order) {
    case DMA2D_SCRAMBLE_ORDER_BYTE2_1_0:
        return 0;
    case DMA2D_SCRAMBLE_ORDER_BYTE2_0_1:
        return 1;
    case DMA2D_SCRAMBLE_ORDER_BYTE1_0_2:
        return 2;
    case DMA2D_SCRAMBLE_ORDER_BYTE1_2_0:
        return 3;
    case DMA2D_SCRAMBLE_ORDER_BYTE0_2_1:
        return 4;
    case DMA2D_SCRAMBLE_ORDER_BYTE0_1_2:
        return 5;
    default:
        abort();
    }
}

/////////////////////////////////////// RX ///////////////////////////////////////////
__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_get_interrupt_status(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->in_channel[channel].in_int_st.val & DMA2D_LL_RX_EVENT_MASK;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_interrupt(dma2d_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->in_channel[channel].in_int_ena.val = dev->in_channel[channel].in_int_ena.val | (mask & DMA2D_LL_RX_EVENT_MASK);
    } else {
        dev->in_channel[channel].in_int_ena.val = dev->in_channel[channel].in_int_ena.val & ~(mask & DMA2D_LL_RX_EVENT_MASK);
    }
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_clear_interrupt_status(dma2d_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->in_channel[channel].in_int_clr.val = (mask & DMA2D_LL_RX_EVENT_MASK);
}

static inline volatile void *dma2d_ll_rx_get_interrupt_status_reg(dma2d_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->in_channel[channel].in_int_st);
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_owner_check(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->in_channel[channel].in_conf0.in_check_owner_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_page_bound_wrap(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->in_channel[channel].in_conf0.in_page_bound_en_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_set_data_burst_length(dma2d_dev_t *dev, uint32_t channel, uint32_t length)
{
    uint32_t sel;
    switch (length) {
    case 8:
        sel = 0;
        break;
    case 16:
        sel = 1;
        break;
    case 32:
        sel = 2;
        break;
    case 64:
        sel = 3;
        break;
    case 128:
        sel = 4;
        break;
    default:
        abort();
    }
    dev->in_channel[channel].in_conf0.in_mem_burst_length_chn = sel;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_descriptor_burst(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->in_channel[channel].in_conf0.indscr_burst_en_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_reset_channel(dma2d_dev_t *dev, uint32_t channel)
{
    dev->in_channel[channel].in_conf0.in_rst_chn = 1;
    dev->in_channel[channel].in_conf0.in_rst_chn = 0;
}

__attribute__((always_inline))
static inline bool dma2d_ll_rx_is_reset_avail(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->in_channel[channel].in_state.in_reset_avail_chn;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_abort(dma2d_dev_t *dev, uint32_t channel, bool disable)
{
    dev->in_channel[channel].in_conf0.in_cmd_disable_chn = disable;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_dscr_port(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->in_channel[channel].in_conf0.in_dscr_port_en_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_set_macro_block_size(dma2d_dev_t *dev, uint32_t channel, dma2d_macro_block_size_t size)
{
    uint32_t sel;
    switch (size) {
    case DMA2D_MACRO_BLOCK_SIZE_NONE:
        sel = 3;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_8_8:
        sel = 0;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_8_16:
        sel = 1;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_16_16:
        sel = 2;
        break;
    default:
        abort();
    }
    dev->in_channel[channel].in_conf0.in_macro_block_size_chn = sel;
}

__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_pop_data(dma2d_dev_t *dev, uint32_t channel)
{
    dev->in_channel[channel].in_pop.infifo_pop_chn = 1;
    return dev->in_channel[channel].in_pop.infifo_rdata_chn;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_set_desc_addr(dma2d_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->in_channel[channel].in_link_addr.inlink_addr_chn = addr;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_start(dma2d_dev_t *dev, uint32_t channel)
{
    dev->in_channel[channel].in_link_conf.inlink_start_chn = 1;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_stop(dma2d_dev_t *dev, uint32_t channel)
{
    dev->in_channel[channel].in_link_conf.inlink_stop_chn = 1;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_restart(dma2d_dev_t *dev, uint32_t channel)
{
    dev->in_channel[channel].in_link_conf.inlink_restart_chn = 1;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_set_auto_return_owner(dma2d_dev_t *dev, uint32_t channel, int owner)
{
    dev->in_channel[channel].in_link_conf.inlink_auto_ret_chn = owner;
}

__attribute__((always_inline))
static inline bool dma2d_ll_rx_is_desc_fsm_idle(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->in_channel[channel].in_link_conf.inlink_park_chn;
}

__attribute__((always_inline))
static inline bool dma2d_ll_rx_is_fsm_idle(dma2d_dev_t *dev, uint32_t channel)
{
    return (dev->in_channel[channel].in_state.in_state_chn == 0);
}

__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_get_success_eof_desc_addr(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->in_channel[channel].in_suc_eof_des_addr.val;
}

__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_get_error_eof_desc_addr(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->in_channel[channel].in_err_eof_des_addr.val;
}

__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_get_prefetched_desc_addr(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->in_channel[channel].in_dscr.val;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_connect_to_periph(dma2d_dev_t *dev, uint32_t channel, dma2d_trigger_peripheral_t periph, int periph_id)
{
    dev->in_channel[channel].in_peri_sel.in_peri_sel_chn = periph_id;
    dev->in_channel[channel].in_conf0.in_mem_trans_en_chn = (periph == DMA2D_TRIG_PERIPH_M2M);
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_disconnect_from_periph(dma2d_dev_t *dev, uint32_t channel)
{
    dev->in_channel[channel].in_peri_sel.in_peri_sel_chn = DMA2D_LL_CHANNEL_PERIPH_NO_CHOICE;
    dev->in_channel[channel].in_conf0.in_mem_trans_en_chn = false;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_reorder(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->in_channel[channel].in_conf0.in_reorder_en_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_configure_color_space_conv(dma2d_dev_t *dev, uint32_t channel, dma2d_csc_rx_option_t csc_sel)
{
    HAL_ASSERT(channel == 0);
    uint32_t input_sel = 7;
    bool proc_en = false;
    int (*table)[4] = NULL;
    uint32_t output_sel = 1;

    const int color_space_conv_param_yuv2rgb_bt601_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT601;
    const int color_space_conv_param_yuv2rgb_bt709_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT709;

    switch (csc_sel) {
    case DMA2D_CSC_RX_NONE:
        input_sel = 7;
        break;
    case DMA2D_CSC_RX_SCRAMBLE:
        input_sel = 1;
        proc_en = false;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV422_TO_YUV444:
    case DMA2D_CSC_RX_YUV420_TO_YUV444:
        input_sel = 0;
        proc_en = false;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV444_TO_YUV422:
        input_sel = 0;
        proc_en = false;
        output_sel = 2;
        break;
    case DMA2D_CSC_RX_YUV444_TO_YUV420:
    case DMA2D_CSC_RX_YUV422_TO_YUV420:
        input_sel = 0;
        proc_en = false;
        output_sel = 3;
        break;
    case DMA2D_CSC_RX_YUV420_TO_RGB888_601:
    case DMA2D_CSC_RX_YUV422_TO_RGB888_601:
        input_sel = 0;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV420_TO_RGB565_601:
    case DMA2D_CSC_RX_YUV422_TO_RGB565_601:
        input_sel = 0;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 0;
        break;
    case DMA2D_CSC_RX_YUV420_TO_RGB888_709:
    case DMA2D_CSC_RX_YUV422_TO_RGB888_709:
        input_sel = 0;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV420_TO_RGB565_709:
    case DMA2D_CSC_RX_YUV422_TO_RGB565_709:
        input_sel = 0;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 0;
        break;
    case DMA2D_CSC_RX_YUV444_TO_RGB888_601:
        input_sel = 2;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV444_TO_RGB565_601:
        input_sel = 2;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 0;
        break;
    case DMA2D_CSC_RX_YUV444_TO_RGB888_709:
        input_sel = 2;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV444_TO_RGB565_709:
        input_sel = 2;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 0;
        break;
    default:
        abort();
    }

    dev->in_channel[channel].in_color_convert.in_color_input_sel_chn = input_sel;
    dev->in_channel[channel].in_color_convert.in_color_3b_proc_en_chn = proc_en;
    dev->in_channel[channel].in_color_convert.in_color_output_sel_chn = output_sel;

    if (proc_en) {
        HAL_ASSERT(table);
        typeof(dev->in_channel[channel].in_color_param_group) color_param_group;

        color_param_group.param_h.a = table[0][0];
        color_param_group.param_h.b = table[0][1];
        color_param_group.param_h.c = table[0][2];
        color_param_group.param_h.d = table[0][3];

        color_param_group.param_m.a = table[1][0];
        color_param_group.param_m.b = table[1][1];
        color_param_group.param_m.c = table[1][2];
        color_param_group.param_m.d = table[1][3];

        color_param_group.param_l.a = table[2][0];
        color_param_group.param_l.b = table[2][1];
        color_param_group.param_l.c = table[2][2];
        color_param_group.param_l.d = table[2][3];

        dev->in_channel[channel].in_color_param_group.param_h.val[0] = color_param_group.param_h.val[0];
        dev->in_channel[channel].in_color_param_group.param_h.val[1] = color_param_group.param_h.val[1];
        dev->in_channel[channel].in_color_param_group.param_m.val[0] = color_param_group.param_m.val[0];
        dev->in_channel[channel].in_color_param_group.param_m.val[1] = color_param_group.param_m.val[1];
        dev->in_channel[channel].in_color_param_group.param_l.val[0] = color_param_group.param_l.val[0];
        dev->in_channel[channel].in_color_param_group.param_l.val[1] = color_param_group.param_l.val[1];
    }
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_set_csc_pre_scramble(dma2d_dev_t *dev, uint32_t channel, dma2d_scramble_order_t order)
{
    HAL_ASSERT(channel == 0);
    dev->in_channel[channel].in_scramble.in_scramble_sel_pre_chn = dma2d_ll_get_scramble_order_sel(order);
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_set_csc_post_scramble(dma2d_dev_t *dev, uint32_t channel, dma2d_scramble_order_t order)
{
    HAL_ASSERT(channel == 0);
    dev->in_channel[channel].in_scramble.in_scramble_sel_post_chn = dma2d_ll_get_scramble_order_sel(order);
}

static inline void dma2d_ll_rx_enable_arb_weight(dma2d_dev_t *dev, bool enable)
{
    dev->in_arb_config.in_weight_en = enable;
}

static inline void dma2d_ll_rx_set_arb_timeout(dma2d_dev_t *dev, uint32_t timeout_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->in_arb_config, in_arb_timeout_num, timeout_num);
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_set_arb_token_num(dma2d_dev_t *dev, uint32_t channel, uint32_t token_num)
{
    dev->in_channel[channel].in_arb.in_arb_token_num_chn = token_num;
}

__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_get_arb_token_num(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->in_channel[channel].in_arb.in_arb_token_num_chn;
}

__attribute__((always_inline))
static inline void dma2d_ll_rx_set_priority(dma2d_dev_t *dev, uint32_t channel, uint32_t priority)
{
    dev->in_channel[channel].in_arb.in_arb_priority_chn = priority;
}

/////////////////////////////////////// TX ///////////////////////////////////////////
__attribute__((always_inline))
static inline uint32_t dma2d_ll_tx_get_interrupt_status(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_int_st.val & DMA2D_LL_TX_EVENT_MASK;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_interrupt(dma2d_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->out_channel[channel].out_int_ena.val = dev->out_channel[channel].out_int_ena.val | (mask & DMA2D_LL_TX_EVENT_MASK);
    } else {
        dev->out_channel[channel].out_int_ena.val = dev->out_channel[channel].out_int_ena.val & ~(mask & DMA2D_LL_TX_EVENT_MASK);
    }
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_clear_interrupt_status(dma2d_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->out_channel[channel].out_int_clr.val = (mask & DMA2D_LL_TX_EVENT_MASK);
}

static inline volatile void *dma2d_ll_tx_get_interrupt_status_reg(dma2d_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->out_channel[channel].out_int_st);
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_owner_check(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_check_owner_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_eof_mode(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_eof_mode_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_auto_write_back(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_auto_wrback_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_page_bound_wrap(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_page_bound_en_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_set_data_burst_length(dma2d_dev_t *dev, uint32_t channel, uint32_t length)
{
    uint32_t sel;
    switch (length) {
    case 8:
        sel = 0;
        break;
    case 16:
        sel = 1;
        break;
    case 32:
        sel = 2;
        break;
    case 64:
        sel = 3;
        break;
    case 128:
        sel = 4;
        break;
    default:
        abort();
    }
    dev->out_channel[channel].out_conf0.out_mem_burst_length_chn = sel;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_descriptor_burst(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.outdscr_burst_en_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_reset_channel(dma2d_dev_t *dev, uint32_t channel)
{
    dev->out_channel[channel].out_conf0.out_rst_chn = 1;
    dev->out_channel[channel].out_conf0.out_rst_chn = 0;
}

__attribute__((always_inline))
static inline bool dma2d_ll_tx_is_reset_avail(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_state.out_reset_avail_chn;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_abort(dma2d_dev_t *dev, uint32_t channel, bool disable)
{
    dev->out_channel[channel].out_conf0.out_cmd_disable_chn = disable;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_dscr_port(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_dscr_port_en_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_set_dscr_port_block_size(dma2d_dev_t *dev, uint32_t channel, uint32_t blk_h, uint32_t blk_v)
{
    dev->out_channel[channel].out_dscr_port_blk.out_dscr_port_blk_h_chn = blk_h;
    dev->out_channel[channel].out_dscr_port_blk.out_dscr_port_blk_v_chn = blk_v;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_set_macro_block_size(dma2d_dev_t *dev, uint32_t channel, dma2d_macro_block_size_t size)
{
    uint32_t sel;
    switch (size) {
    case DMA2D_MACRO_BLOCK_SIZE_NONE:
        sel = 3;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_8_8:
        sel = 0;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_8_16:
        sel = 1;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_16_16:
        sel = 2;
        break;
    default:
        abort();
    }
    dev->out_channel[channel].out_conf0.out_macro_block_size_chn = sel;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_push_data(dma2d_dev_t *dev, uint32_t channel, uint32_t data)
{
    dev->out_channel[channel].out_push.outfifo_wdata_chn = data;
    dev->out_channel[channel].out_push.outfifo_push_chn = 1;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_set_desc_addr(dma2d_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->out_channel[channel].out_link_addr.outlink_addr_chn = addr;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_start(dma2d_dev_t *dev, uint32_t channel)
{
    dev->out_channel[channel].out_link_conf.outlink_start_chn = 1;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_stop(dma2d_dev_t *dev, uint32_t channel)
{
    dev->out_channel[channel].out_link_conf.outlink_stop_chn = 1;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_restart(dma2d_dev_t *dev, uint32_t channel)
{
    dev->out_channel[channel].out_link_conf.outlink_restart_chn = 1;
}

__attribute__((always_inline))
static inline bool dma2d_ll_tx_is_desc_fsm_idle(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_link_conf.outlink_park_chn;
}

__attribute__((always_inline))
static inline bool dma2d_ll_tx_is_fsm_idle(dma2d_dev_t *dev, uint32_t channel)
{
    return (dev->out_channel[channel].out_state.out_state_chn == 0);
}

__attribute__((always_inline))
static inline uint32_t dma2d_ll_tx_get_eof_desc_addr(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_eof_des_addr.val;
}

__attribute__((always_inline))
static inline uint32_t dma2d_ll_tx_get_prefetched_desc_addr(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_dscr.val;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_connect_to_periph(dma2d_dev_t *dev, uint32_t channel, dma2d_trigger_peripheral_t periph, int periph_id)
{
    (void)periph;
    dev->out_channel[channel].out_peri_sel.out_peri_sel_chn = periph_id;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_disconnect_from_periph(dma2d_dev_t *dev, uint32_t channel)
{
    dev->out_channel[channel].out_peri_sel.out_peri_sel_chn = DMA2D_LL_CHANNEL_PERIPH_NO_CHOICE;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_reorder(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_reorder_en_chn = enable;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_configure_color_space_conv(dma2d_dev_t *dev, uint32_t channel, dma2d_csc_tx_option_t csc_sel)
{
    uint32_t input_sel = 7;
    bool proc_en = false;
    int (*table)[4] = NULL;
    uint32_t output_sel = 2;

    const int color_space_conv_param_rgb2yuv_bt601_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_RGB2YUV_BT601;
    const int color_space_conv_param_rgb2yuv_bt709_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_RGB2YUV_BT709;
    const int color_space_conv_param_yuv2rgb_bt601_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT601;
    const int color_space_conv_param_yuv2rgb_bt709_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT709;

    switch (csc_sel) {
    case DMA2D_CSC_TX_NONE:
        input_sel = 7;
        break;
    case DMA2D_CSC_TX_SCRAMBLE:
        input_sel = 2;
        proc_en = false;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_RGB888_TO_RGB565:
        input_sel = 3;
        proc_en = false;
        output_sel = 0;
        break;
    case DMA2D_CSC_TX_RGB565_TO_RGB888:
        input_sel = 0;
        proc_en = false;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_RGB888_TO_YUV444_601:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_rgb2yuv_bt601_table;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_RGB888_TO_YUV444_709:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_rgb2yuv_bt709_table;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_RGB888_TO_YUV422_601:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_rgb2yuv_bt601_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_TX_RGB888_TO_YUV422_709:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_rgb2yuv_bt709_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_TX_YUV444_TO_RGB888_601:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_YUV444_TO_RGB888_709:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_YUV422_TO_RGB888_601:
        input_sel = 1;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_YUV422_TO_RGB888_709:
        input_sel = 1;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 2;
        break;
    default:
        abort();
    }

    dev->out_channel[channel].out_color_convert.out_color_input_sel_chn = input_sel;
    dev->out_channel[channel].out_color_convert.out_color_3b_proc_en_chn = proc_en;
    dev->out_channel[channel].out_color_convert.out_color_output_sel_chn = output_sel;
    if (proc_en) {
        HAL_ASSERT(table);
        typeof(dev->out_channel[channel].out_color_param_group) color_param_group;

        color_param_group.param_h.a = table[0][0];
        color_param_group.param_h.b = table[0][1];
        color_param_group.param_h.c = table[0][2];
        color_param_group.param_h.d = table[0][3];

        color_param_group.param_m.a = table[1][0];
        color_param_group.param_m.b = table[1][1];
        color_param_group.param_m.c = table[1][2];
        color_param_group.param_m.d = table[1][3];

        color_param_group.param_l.a = table[2][0];
        color_param_group.param_l.b = table[2][1];
        color_param_group.param_l.c = table[2][2];
        color_param_group.param_l.d = table[2][3];

        dev->out_channel[channel].out_color_param_group.param_h.val[0] = color_param_group.param_h.val[0];
        dev->out_channel[channel].out_color_param_group.param_h.val[1] = color_param_group.param_h.val[1];
        dev->out_channel[channel].out_color_param_group.param_m.val[0] = color_param_group.param_m.val[0];
        dev->out_channel[channel].out_color_param_group.param_m.val[1] = color_param_group.param_m.val[1];
        dev->out_channel[channel].out_color_param_group.param_l.val[0] = color_param_group.param_l.val[0];
        dev->out_channel[channel].out_color_param_group.param_l.val[1] = color_param_group.param_l.val[1];
    }
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_set_csc_pre_scramble(dma2d_dev_t *dev, uint32_t channel, dma2d_scramble_order_t order)
{
    dev->out_channel[channel].out_scramble.out_scramble_sel_pre_chn = dma2d_ll_get_scramble_order_sel(order);
}

static inline void dma2d_ll_tx_enable_arb_weight(dma2d_dev_t *dev, bool enable)
{
    dev->out_arb_config.out_weight_en = enable;
}

static inline void dma2d_ll_tx_set_arb_timeout(dma2d_dev_t *dev, uint32_t timeout_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->out_arb_config, out_arb_timeout_num, timeout_num);
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_set_arb_token_num(dma2d_dev_t *dev, uint32_t channel, uint32_t token_num)
{
    dev->out_channel[channel].out_arb.out_arb_token_num_chn = token_num;
}

__attribute__((always_inline))
static inline uint32_t dma2d_ll_tx_get_arb_token_num(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_arb.out_arb_token_num_chn;
}

__attribute__((always_inline))
static inline void dma2d_ll_tx_set_priority(dma2d_dev_t *dev, uint32_t channel, uint32_t priority)
{
    dev->out_channel[channel].out_arb.out_arb_priority_chn = priority;
}

#ifdef __cplusplus
}
#endif
