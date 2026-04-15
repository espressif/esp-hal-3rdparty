/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "esp_clk_tree.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_memory_utils.h"
#include "driver/ana_cmpr.h"
#include "esp_private/gpio.h"
#include "esp_private/io_mux.h"
#include "esp_private/esp_clk.h"
#include "ana_cmpr_private.h"

/* Global static object of the Analog Comparator unit */
static ana_cmpr_handle_t s_ana_cmpr[ANALOG_CMPR_LL_GET(INST_NUM)] = {
    [0 ...(ANALOG_CMPR_LL_GET(INST_NUM) - 1)] = NULL,
};

/* Global spin lock */
static portMUX_TYPE s_spinlock = portMUX_INITIALIZER_UNLOCKED;

void ana_cmpr_default_intr_handler(void *usr_data)
{
    bool need_yield = false;
    ana_cmpr_handle_t cmpr_handle = (ana_cmpr_handle_t)usr_data;
    ana_cmpr_cross_event_data_t evt_data = {.cross_type = ANA_CMPR_CROSS_ANY};
    /* Get and clear the interrupt status */
    uint32_t status = analog_cmpr_ll_get_intr_status(cmpr_handle->dev);
    analog_cmpr_ll_clear_intr(cmpr_handle->dev, status);

    /* Call the user callback function if it is specified and the corresponding event triggers*/
    ana_cmpr_cross_cb_t on_cross = cmpr_handle->cbs.on_cross;
    if (on_cross) {
        // some chip can distinguish the edge of the cross event
#if ANALOG_CMPR_LL_SUPPORT(EDGE_TYPE)
        for (int i = 0; i < ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM); i++) {
            evt_data.src_chan_id = i;
            if (status & ANALOG_CMPR_LL_POS_CROSS_INTR_MASK(cmpr_handle->unit_id, i)) {
                evt_data.cross_type = ANA_CMPR_CROSS_POS;
                need_yield |= on_cross(cmpr_handle, &evt_data, cmpr_handle->user_data);
            } else if (status & ANALOG_CMPR_LL_NEG_CROSS_INTR_MASK(cmpr_handle->unit_id, i)) {
                evt_data.cross_type = ANA_CMPR_CROSS_NEG;
                need_yield |= on_cross(cmpr_handle, &evt_data, cmpr_handle->user_data);
            }
        }
#else
        need_yield = on_cross(cmpr_handle, &evt_data, cmpr_handle->user_data);
#endif
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static void ana_cmpr_destroy_unit(ana_cmpr_handle_t cmpr)
{
    int unit_id = cmpr->unit_id;

    // Disable function clock first
    analog_cmpr_ll_enable_function_clock(unit_id, false);
    // Disable bus clock last
    analog_cmpr_ll_enable_bus_clock(unit_id, false);

#if CONFIG_PM_ENABLE
    if (cmpr->pm_lock) {
        esp_pm_lock_delete(cmpr->pm_lock);
    }
#endif
    if (cmpr->intr_handle) {
        esp_intr_free(cmpr->intr_handle);
    }
    free(cmpr);
}

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
static int _ana_cmpr_gpio_to_pad_id(ana_cmpr_handle_t cmpr, int gpio_num)
{
    int pad_id = -1;
    for (int i = 0; i < ANALOG_CMPR_LL_GET(PAD_NUM); i++) {
        if (ana_cmpr_periph[cmpr->unit_id].pad_gpios[i] == gpio_num) {
            pad_id = i;
            break;
        }
    }
    return pad_id;
}
#endif

static void _ana_cmpr_init_default_channels(ana_cmpr_handle_t cmpr, const ana_cmpr_config_t *config)
{
    int unit_id = cmpr->unit_id;
    cmpr->ref_chan.ref_src = config->ref_src;
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    // GPIO number of external reference channel is configurable
    // cmpr->ref_chan.gpio_num =
    cmpr->ref_chan.pad_id = _ana_cmpr_gpio_to_pad_id(cmpr, cmpr->ref_chan.gpio_num);
#else
    cmpr->ref_chan.gpio_num = ana_cmpr_periph[unit_id].ext_ref_gpio;
#endif

    cmpr->src_chans[0].chan_id = 0;
    cmpr->src_chans[0].cross_type = config->cross_type;
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    // GPIO number of source channel is configurable
    // cmpr->src_chans[0].gpio_num =
    cmpr->src_chans[0].pad_id = _ana_cmpr_gpio_to_pad_id(cmpr, cmpr->src_chans[0].gpio_num);
#else
    cmpr->src_chans[0].gpio_num = ana_cmpr_periph[unit_id].src_gpio;
#endif

    analog_cmpr_ll_set_ref_source(cmpr->dev, config->ref_src);

#if !ANALOG_CMPR_LL_SUPPORT(EDGE_TYPE)
    // set which cross type can trigger the interrupt
    analog_cmpr_ll_set_intr_cross_type(cmpr->dev, config->cross_type);
#endif  // !ANALOG_CMPR_LL_SUPPORT(EDGE_TYPE)
    // each source channel's cross type can contribute different mask to the unit's intr_mask, so set it here according to the config
    cmpr->intr_mask |= analog_cmpr_ll_get_intr_mask_by_type(unit_id, 0, config->cross_type);

    // setup the gpio pad for the source and reference signal
    gpio_config_as_analog(cmpr->src_chans[0].gpio_num);
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    analog_cmpr_ll_set_src_pad(cmpr->dev, 0, cmpr->src_chans[0].pad_id);
#endif
    if (config->ref_src == ANA_CMPR_REF_SRC_EXTERNAL) {
        gpio_config_as_analog(cmpr->ref_chan.gpio_num);
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
        analog_cmpr_ll_set_ext_ref_pad(cmpr->dev, cmpr->ref_chan.pad_id);
#endif
        ESP_LOGD(TAG, "unit %d: source0 signal from GPIO %d, reference signal from GPIO %d",
                 unit_id, cmpr->src_chans[0].gpio_num, cmpr->ref_chan.gpio_num);
    } else {
        ESP_LOGD(TAG, "unit %d: source0 signal from GPIO %d, reference signal from internal",
                 unit_id, cmpr->src_chans[0].gpio_num);
    }
}

esp_err_t ana_cmpr_new_unit(const ana_cmpr_config_t *config, ana_cmpr_handle_t *ret_cmpr)
{
    if (config == NULL || ret_cmpr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;
    ana_cmpr_handle_t ana_cmpr_hdl = NULL;
    int unit_id = config->unit;
    ESP_RETURN_ON_FALSE(unit_id >= 0 && unit_id < ANALOG_CMPR_LL_GET(INST_NUM), ESP_ERR_INVALID_ARG, TAG, "invalid unit id");
    ESP_RETURN_ON_FALSE(!s_ana_cmpr[unit_id], ESP_ERR_INVALID_STATE, TAG, "unit has been allocated already");
    if (config->intr_priority) {
        ESP_RETURN_ON_FALSE(1 << (config->intr_priority) & ANA_CMPR_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG,
                            TAG, "invalid interrupt priority:%d", config->intr_priority);
    }

    // analog comparator unit must be allocated from internal memory because it contains atomic variable
    ana_cmpr_hdl = heap_caps_calloc(1, sizeof(struct ana_cmpr_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(ana_cmpr_hdl, ESP_ERR_NO_MEM, TAG, "no memory for analog comparator object");

    /* Assign analog comparator unit */
    ana_cmpr_hdl->dev = ANALOG_CMPR_LL_GET_HW(unit_id);
    ana_cmpr_hdl->unit_id = unit_id;
    ana_cmpr_hdl->intr_priority = config->intr_priority;
    atomic_init(&ana_cmpr_hdl->fsm, ANA_CMPR_FSM_INIT);

    // Enable bus clock
    analog_cmpr_ll_enable_bus_clock(unit_id, true);
    // Reset register
    analog_cmpr_ll_reset_register(unit_id);
    // Reset core
    analog_cmpr_ll_reset_core(unit_id);

    // Set clock source (use default if not specified in config)
    ana_cmpr_clk_src_t clk_src = config->clk_src ? config->clk_src : ANA_CMPR_CLK_SRC_DEFAULT;
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    analog_cmpr_ll_set_clk_src(unit_id, clk_src);
    // Set clock divider to 1
    analog_cmpr_ll_set_clk_div(unit_id, 1);
    // Enable function clock
    analog_cmpr_ll_enable_function_clock(unit_id, true);
#else
    // Analog comparator located in the IO MUX module in older chips, so the clock source is shared with IO MUX.
    ESP_GOTO_ON_ERROR(io_mux_set_clock_source((soc_module_clk_t)clk_src), err, TAG, "clock source conflicts with other IOMUX consumers");
#endif
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &ana_cmpr_hdl->src_clk_freq_hz),
                      err, TAG, "get source clock frequency failed");

    // init the default source and reference channels according to the config
    _ana_cmpr_init_default_channels(ana_cmpr_hdl, config);

#if CONFIG_PM_ENABLE
    // Create PM lock, because the light sleep may disable the clock and power domain used by the analog comparator
    // TODO: IDF-12818
    ret  = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, ana_cmpr_periph[unit_id].module_name, &ana_cmpr_hdl->pm_lock);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "create NO_LIGHT_SLEEP lock failed");
#endif

    // different unit share the same interrupt register, so using a spin lock to protect it
    portENTER_CRITICAL(&s_spinlock);
    // disable the interrupt by default, and clear all pending status
    analog_cmpr_ll_enable_intr(ana_cmpr_hdl->dev, ANALOG_CMPR_LL_ALL_INTR_MASK(unit_id), false);
    analog_cmpr_ll_clear_intr(ana_cmpr_hdl->dev, ANALOG_CMPR_LL_ALL_INTR_MASK(unit_id));
    portEXIT_CRITICAL(&s_spinlock);

    // register the analog comparator unit to the global object array
    s_ana_cmpr[unit_id] = ana_cmpr_hdl;
    *ret_cmpr = ana_cmpr_hdl;
    return ESP_OK;

err:
    if (ana_cmpr_hdl) {
        ana_cmpr_destroy_unit(ana_cmpr_hdl);
    }
    return ret;
}

esp_err_t ana_cmpr_del_unit(ana_cmpr_handle_t cmpr)
{
    if (cmpr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    /* Search the global object array to check if the input handle is valid */
    int unit_id = -1;
    for (int i = 0; i < ANALOG_CMPR_LL_GET(INST_NUM); i++) {
        if (s_ana_cmpr[i] == cmpr) {
            unit_id = i;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(unit_id != -1, ESP_ERR_INVALID_ARG, TAG, "unregistered unit handle");
    ESP_RETURN_ON_FALSE(atomic_load(&cmpr->fsm) == ANA_CMPR_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "not in init state");

    ana_cmpr_destroy_unit(cmpr);
    // unregister it from the global object array
    s_ana_cmpr[unit_id] = NULL;
    ESP_LOGD(TAG, "unit %d deleted", (int)unit_id);

    return ESP_OK;
}

esp_err_t ana_cmpr_set_internal_reference(ana_cmpr_handle_t cmpr, const ana_cmpr_internal_ref_config_t *ref_cfg)
{
    if (cmpr == NULL || ref_cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    // external reference channel doesn't support it
    if (cmpr->ref_chan.ref_src != ANA_CMPR_REF_SRC_INTERNAL) {
        return ESP_ERR_NOT_ALLOWED;
    }

    // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
    portENTER_CRITICAL_SAFE(&s_spinlock);
    analog_cmpr_ll_set_internal_ref_voltage(cmpr->dev, ref_cfg->ref_volt);
    portEXIT_CRITICAL_SAFE(&s_spinlock);

    return ESP_OK;
}

esp_err_t ana_cmpr_set_debounce(ana_cmpr_handle_t cmpr, const ana_cmpr_debounce_config_t *dbc_cfg)
{
    if (cmpr == NULL || dbc_cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Transfer the time to clock cycles */
    uint32_t wait_cycle = dbc_cfg->wait_us * (cmpr->src_clk_freq_hz / 1000000);
    // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
    portENTER_CRITICAL_SAFE(&s_spinlock);
    analog_cmpr_ll_set_cross_debounce_cycle(cmpr->dev, wait_cycle);
    portEXIT_CRITICAL_SAFE(&s_spinlock);

    return ESP_OK;
}

esp_err_t ana_cmpr_register_event_callbacks(ana_cmpr_handle_t cmpr, const ana_cmpr_event_callbacks_t *cbs, void *user_data)
{
    if (cmpr == NULL || cbs == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_RETURN_ON_FALSE(atomic_load(&cmpr->fsm) == ANA_CMPR_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "not in init state");
#if CONFIG_ANA_CMPR_ISR_CACHE_SAFE
    if (cbs->on_cross) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_cross), ESP_ERR_INVALID_ARG, TAG, "on_cross is not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user_data is not in internal RAM");
    }
#endif

    // the interrupt service is lazy installed.
    if (!cmpr->intr_handle) {
        int intr_flags = ANA_CMPR_INTR_FLAG | ((cmpr->intr_priority > 0) ?  BIT(cmpr->intr_priority) : ESP_INTR_FLAG_LOWMED);
        ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(ana_cmpr_periph[cmpr->unit_id].intr_src, intr_flags,
                                                      (uint32_t)analog_cmpr_ll_get_intr_status_reg(cmpr->dev),
                                                      ANALOG_CMPR_LL_ALL_INTR_MASK(cmpr->unit_id), ana_cmpr_default_intr_handler,
                                                      cmpr, &cmpr->intr_handle),
                            TAG, "allocate interrupt failed");
    }

    /* Save the callback functions */
    memcpy(&(cmpr->cbs), cbs, sizeof(ana_cmpr_event_callbacks_t));
    cmpr->user_data = user_data;

    ESP_LOGV(TAG, "unit %d event callback registered", cmpr->unit_id);
    return ESP_OK;
}

esp_err_t ana_cmpr_enable(ana_cmpr_handle_t cmpr)
{
    if (cmpr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ana_cmpr_fsm_t expected_fsm = ANA_CMPR_FSM_INIT;
    if (atomic_compare_exchange_strong(&cmpr->fsm, &expected_fsm, ANA_CMPR_FSM_WAIT)) {
#if CONFIG_PM_ENABLE
        if (cmpr->pm_lock) {
            esp_pm_lock_acquire(cmpr->pm_lock);
        }
#endif

        // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
        portENTER_CRITICAL(&s_spinlock);
        analog_cmpr_ll_enable_intr(cmpr->dev, cmpr->intr_mask, true);
        analog_cmpr_ll_enable(cmpr->dev, true);
        portEXIT_CRITICAL(&s_spinlock);

        // switch the state machine to enable state
        atomic_store(&cmpr->fsm, ANA_CMPR_FSM_ENABLE);
        ESP_LOGD(TAG, "unit %d enabled", (int)cmpr->unit_id);
    } else {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "not in init state");
    }

    return ESP_OK;
}

esp_err_t ana_cmpr_disable(ana_cmpr_handle_t cmpr)
{
    if (cmpr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ana_cmpr_fsm_t expected_fsm = ANA_CMPR_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&cmpr->fsm, &expected_fsm, ANA_CMPR_FSM_WAIT)) {
        // the underlying register may be accessed by different threads at the same time, so use spin lock to protect it
        portENTER_CRITICAL(&s_spinlock);
        analog_cmpr_ll_enable_intr(cmpr->dev, cmpr->intr_mask, false);
        analog_cmpr_ll_enable(cmpr->dev, false);
        portEXIT_CRITICAL(&s_spinlock);

#if CONFIG_PM_ENABLE
        if (cmpr->pm_lock) {
            esp_pm_lock_release(cmpr->pm_lock);
        }
#endif

        // switch the state machine to init state
        atomic_store(&cmpr->fsm, ANA_CMPR_FSM_INIT);
        ESP_LOGD(TAG, "unit %d disabled", (int)cmpr->unit_id);
    } else {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "not enabled yet");
    }

    return ESP_OK;
}

#if CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void ana_cmpr_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
}
#endif

/////////////////////////////// Legacy API for backward compatibility, will be removed in the future ///////////////////
// These APIs are implemented based on the "legacy" ref and src channel objects in the analog comparator unit,
// which are designed for the old version driver that only support one ref and src channel and directly configured in the analog comparator unit.
// The legacy channels are still used in the new version driver for backward compatibility, but they are not recommended for new use
// because they have some limitations, such as source channel can only support one GPIO input.
// New APIs with more flexible channel configuration are provided in the new version driver,
// which are implemented based on the new ref and src channel objects and are recommended for new use.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
esp_err_t ana_cmpr_set_cross_type(ana_cmpr_handle_t cmpr, ana_cmpr_cross_type_t cross_type)
{
#if ANALOG_CMPR_LL_SUPPORT(EDGE_TYPE)
    /* Not support to set the cross type after initialized, because it relies on the interrupt types to distinguish the edge,
     * i.e. have to re-allocate the interrupt to change the cross type */
    (void)cmpr;
    (void)cross_type;
    return ESP_ERR_NOT_SUPPORTED;
#else
    if (cmpr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cross_type < ANA_CMPR_CROSS_DISABLE || cross_type > ANA_CMPR_CROSS_ANY) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL_SAFE(&s_spinlock);
    analog_cmpr_ll_set_intr_cross_type(cmpr->dev, cross_type);
    // each source channel's cross type can contribute different mask to the unit's intr_mask
    cmpr->intr_mask |= analog_cmpr_ll_get_intr_mask_by_type(cmpr->unit_id, 0, cross_type);
    portEXIT_CRITICAL_SAFE(&s_spinlock);

    return ESP_OK;
#endif
}

esp_err_t ana_cmpr_get_gpio(ana_cmpr_unit_t unit, ana_cmpr_channel_type_t chan_type, int *gpio_num)
{
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    (void)unit;
    (void)chan_type;
    (void)gpio_num;
    // the source channel and reference channel GPIO number are configurable in the new analog comparator IP.
    // we can't get the GPIO number from a fixed mapping like in the old version driver
    return ESP_ERR_NOT_SUPPORTED;
#else
    if (gpio_num == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_RETURN_ON_FALSE(unit >= 0 && unit < ANALOG_CMPR_LL_GET(INST_NUM), ESP_ERR_INVALID_ARG, TAG, "invalid unit id");

    /* Get the gpio number according to the channel type */
    switch (chan_type) {
    case ANA_CMPR_SOURCE_CHAN:
        *gpio_num = ana_cmpr_periph[unit].src_gpio;
        break;
    case ANA_CMPR_EXT_REF_CHAN:
        *gpio_num = ana_cmpr_periph[unit].ext_ref_gpio;
        break;
    default:
        ESP_LOGE(TAG, "invalid channel type");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
#endif
}
