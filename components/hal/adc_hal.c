// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "hal/adc_hal.h"
#if !CONFIG_IDF_TARGET_ESP32C3
#include "hal/adc_hal_conf.h"
#endif

void adc_hal_init(void)
{
    // Set internal FSM wait time, fixed value.
    adc_ll_digi_set_fsm_time(SOC_ADC_FSM_RSTB_WAIT_DEFAULT, SOC_ADC_FSM_START_WAIT_DEFAULT,
                             SOC_ADC_FSM_STANDBY_WAIT_DEFAULT);
    adc_ll_set_sample_cycle(ADC_FSM_SAMPLE_CYCLE_DEFAULT);
    adc_hal_pwdet_set_cct(SOC_ADC_PWDET_CCT_DEFAULT);
    adc_ll_digi_output_invert(ADC_NUM_1, SOC_ADC_DIGI_DATA_INVERT_DEFAULT(ADC_NUM_1));
    adc_ll_digi_output_invert(ADC_NUM_2, SOC_ADC_DIGI_DATA_INVERT_DEFAULT(ADC_NUM_2));
    adc_ll_digi_set_clk_div(SOC_ADC_DIGI_SAR_CLK_DIV_DEFAULT);
}

void adc_hal_deinit(void)
{
    adc_ll_set_power_manage(ADC_POWER_SW_OFF);
}

int adc_hal_convert(adc_ll_num_t adc_n, int channel, int *value)
{
    adc_ll_rtc_enable_channel(adc_n, channel);
    adc_ll_rtc_start_convert(adc_n, channel);
    while (adc_ll_rtc_convert_is_done(adc_n) != true);
    *value = adc_ll_rtc_get_convert_value(adc_n);
    return (int)adc_ll_rtc_analysis_raw_data(adc_n, (uint16_t)(*value));
}

#if CONFIG_IDF_TARGET_ESP32C3
//This feature is currently supported on ESP32C3, will be supported on other chips soon
/*---------------------------------------------------------------
                    DMA setting
---------------------------------------------------------------*/
void adc_hal_digi_dma_multi_descriptor(adc_dma_hal_config_t *dma_config, uint8_t *data_buf, uint32_t size, uint32_t num)
{
    assert(((uint32_t)data_buf % 4) == 0);
    assert((size % 4) == 0);
    dma_descriptor_t *desc = dma_config->rx_desc;
    uint32_t n = 0;

    while (num--) {
        desc[n].dw0.size = size;
        desc[n].dw0.suc_eof = 0;
        desc[n].dw0.owner = 1;
        desc[n].buffer = data_buf;
        desc[n].next = &desc[n+1];
        data_buf += size;
        n++;
    }
    desc[n-1].next = NULL;
}

void adc_hal_digi_rxdma_start(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config)
{
    gdma_ll_rx_reset_channel(adc_dma_ctx->dev, dma_config->dma_chan);
    gdma_ll_rx_set_desc_addr(adc_dma_ctx->dev, dma_config->dma_chan, (uint32_t)dma_config->rx_desc);
    gdma_ll_rx_start(adc_dma_ctx->dev, dma_config->dma_chan);
}

void adc_hal_digi_rxdma_stop(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config)
{
    gdma_ll_rx_stop(adc_dma_ctx->dev, dma_config->dma_chan);
}

void adc_hal_digi_ena_intr(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config, uint32_t mask)
{
    gdma_ll_enable_interrupt(adc_dma_ctx->dev, dma_config->dma_chan, mask, true);
}

void adc_hal_digi_clr_intr(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config, uint32_t mask)
{
    gdma_ll_clear_interrupt_status(adc_dma_ctx->dev, dma_config->dma_chan, mask);
}

void adc_hal_digi_dis_intr(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config, uint32_t mask)
{
    gdma_ll_enable_interrupt(adc_dma_ctx->dev, dma_config->dma_chan, mask, false);
}

void adc_hal_digi_set_eof_num(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config, uint32_t num)
{
    adc_ll_digi_dma_set_eof_num(num);
}

void adc_hal_digi_start(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config)
{
    //Set to 1: the ADC data will be sent to the DMA
    adc_ll_digi_dma_enable();
    //enable sar adc timer
    adc_ll_digi_trigger_enable();
}

void adc_hal_digi_stop(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config)
{
    //Set to 0: the ADC data won't be sent to the DMA
    adc_ll_digi_dma_disable();
    //disable sar adc timer
    adc_ll_digi_trigger_disable();
}

void adc_hal_digi_init(adc_dma_hal_context_t *adc_dma_ctx, adc_dma_hal_config_t *dma_config)
{
    adc_dma_ctx->dev = &GDMA;
    gdma_ll_enable_clock(adc_dma_ctx->dev, true);
    gdma_ll_clear_interrupt_status(adc_dma_ctx->dev, dma_config->dma_chan, UINT32_MAX);
    gdma_ll_rx_connect_to_periph(adc_dma_ctx->dev, dma_config->dma_chan, GDMA_LL_TRIG_SRC_ADC_DAC);
}
#endif
