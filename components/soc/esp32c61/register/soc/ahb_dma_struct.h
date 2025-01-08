/**
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Interrupt Registers */
/** Type of in_int_raw_chn register
 *  RX channel n raw interrupt status register
 */
typedef union {
    struct {
        /** in_done_chn_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DONE_CHn_INT.
         */
        uint32_t in_done_chn_int_raw:1;
        /** in_suc_eof_chn_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_SUC_EOF_CHn_INT.
         */
        uint32_t in_suc_eof_chn_int_raw:1;
        /** in_err_eof_chn_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_ERR_EOF_CHn_INT.
         */
        uint32_t in_err_eof_chn_int_raw:1;
        /** in_dscr_err_chn_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DSCR_ERR_CHn_INT.
         */
        uint32_t in_dscr_err_chn_int_raw:1;
        /** in_dscr_empty_chn_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DSCR_EMPTY_CHn_INT.
         */
        uint32_t in_dscr_empty_chn_int_raw:1;
        /** infifo_ovf_chn_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of AHB_DMA_INFIFO_OVF_CHn_INT.
         */
        uint32_t infifo_ovf_chn_int_raw:1;
        /** infifo_udf_chn_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of AHB_DMA_INFIFO_UDF_CHn_INT.
         */
        uint32_t infifo_udf_chn_int_raw:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_in_int_raw_chn_reg_t;

/** Type of in_int_st_chn register
 *  RX channel n masked interrupt status register
 */
typedef union {
    struct {
        /** in_done_chn_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DONE_CHn_INT.
         */
        uint32_t in_done_chn_int_st:1;
        /** in_suc_eof_chn_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_SUC_EOF_CHn_INT.
         */
        uint32_t in_suc_eof_chn_int_st:1;
        /** in_err_eof_chn_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_ERR_EOF_CHn_INT.
         */
        uint32_t in_err_eof_chn_int_st:1;
        /** in_dscr_err_chn_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DSCR_ERR_CHn_INT.
         */
        uint32_t in_dscr_err_chn_int_st:1;
        /** in_dscr_empty_chn_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DSCR_EMPTY_CHn_INT.
         */
        uint32_t in_dscr_empty_chn_int_st:1;
        /** infifo_ovf_chn_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of AHB_DMA_INFIFO_OVF_CHn_INT.
         */
        uint32_t infifo_ovf_chn_int_st:1;
        /** infifo_udf_chn_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of AHB_DMA_INFIFO_UDF_CHn_INT.
         */
        uint32_t infifo_udf_chn_int_st:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_in_int_st_chn_reg_t;

/** Type of in_int_ena_chn register
 *  RX channel n interrupt enable register
 */
typedef union {
    struct {
        /** in_done_chn_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DONE_CHn_INT.
         */
        uint32_t in_done_chn_int_ena:1;
        /** in_suc_eof_chn_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_SUC_EOF_CHn_INT.
         */
        uint32_t in_suc_eof_chn_int_ena:1;
        /** in_err_eof_chn_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_ERR_EOF_CHn_INT.
         */
        uint32_t in_err_eof_chn_int_ena:1;
        /** in_dscr_err_chn_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DSCR_ERR_CHn_INT.
         */
        uint32_t in_dscr_err_chn_int_ena:1;
        /** in_dscr_empty_chn_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DSCR_EMPTY_CHn_INT.
         */
        uint32_t in_dscr_empty_chn_int_ena:1;
        /** infifo_ovf_chn_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable AHB_DMA_INFIFO_OVF_CHn_INT.
         */
        uint32_t infifo_ovf_chn_int_ena:1;
        /** infifo_udf_chn_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable AHB_DMA_INFIFO_UDF_CHn_INT.
         */
        uint32_t infifo_udf_chn_int_ena:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_in_int_ena_chn_reg_t;

/** Type of in_int_clr_chn register
 *  RX channel n interrupt clear register
 */
typedef union {
    struct {
        /** in_done_chn_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DONE_CHn_INT.
         */
        uint32_t in_done_chn_int_clr:1;
        /** in_suc_eof_chn_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_SUC_EOF_CHn_INT.
         */
        uint32_t in_suc_eof_chn_int_clr:1;
        /** in_err_eof_chn_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_ERR_EOF_CHn_INT.
         */
        uint32_t in_err_eof_chn_int_clr:1;
        /** in_dscr_err_chn_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DSCR_ERR_CHn_INT.
         */
        uint32_t in_dscr_err_chn_int_clr:1;
        /** in_dscr_empty_chn_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DSCR_EMPTY_CHn_INT.
         */
        uint32_t in_dscr_empty_chn_int_clr:1;
        /** infifo_ovf_chn_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear AHB_DMA_INFIFO_OVF_CHn_INT.
         */
        uint32_t infifo_ovf_chn_int_clr:1;
        /** infifo_udf_chn_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear AHB_DMA_INFIFO_UDF_CHn_INT.
         */
        uint32_t infifo_udf_chn_int_clr:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_in_int_clr_chn_reg_t;

/** Type of out_int_raw_chn register
 *  TX channel n raw interrupt status register
 */
typedef union {
    struct {
        /** out_done_chn_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_DONE_CHn_INT.
         */
        uint32_t out_done_chn_int_raw:1;
        /** out_eof_chn_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_EOF_CHn_INT.
         */
        uint32_t out_eof_chn_int_raw:1;
        /** out_dscr_err_chn_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_DSCR_ERR_CHn_INT.
         */
        uint32_t out_dscr_err_chn_int_raw:1;
        /** out_total_eof_chn_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_TOTAL_EOF_CHn_INT.
         */
        uint32_t out_total_eof_chn_int_raw:1;
        /** outfifo_ovf_chn_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUTFIFO_OVF_CHn_INT.
         */
        uint32_t outfifo_ovf_chn_int_raw:1;
        /** outfifo_udf_chn_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUTFIFO_UDF_CHn_INT.
         */
        uint32_t outfifo_udf_chn_int_raw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_out_int_raw_chn_reg_t;

/** Type of out_int_st_chn register
 *  TX channel n masked interrupt status register
 */
typedef union {
    struct {
        /** out_done_chn_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_DONE_CHn_INT.
         */
        uint32_t out_done_chn_int_st:1;
        /** out_eof_chn_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_EOF_CHn_INT.
         */
        uint32_t out_eof_chn_int_st:1;
        /** out_dscr_err_chn_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_DSCR_ERR_CHn_INT.
         */
        uint32_t out_dscr_err_chn_int_st:1;
        /** out_total_eof_chn_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_TOTAL_EOF_CHn_INT.
         */
        uint32_t out_total_eof_chn_int_st:1;
        /** outfifo_ovf_chn_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUTFIFO_OVF_CHn_INT.
         */
        uint32_t outfifo_ovf_chn_int_st:1;
        /** outfifo_udf_chn_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUTFIFO_UDF_CHn_INT.
         */
        uint32_t outfifo_udf_chn_int_st:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_out_int_st_chn_reg_t;

/** Type of out_int_ena_chn register
 *  TX channel n interrupt enable register
 */
typedef union {
    struct {
        /** out_done_chn_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_DONE_CHn_INT.
         */
        uint32_t out_done_chn_int_ena:1;
        /** out_eof_chn_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_EOF_CHn_INT.
         */
        uint32_t out_eof_chn_int_ena:1;
        /** out_dscr_err_chn_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_DSCR_ERR_CHn_INT.
         */
        uint32_t out_dscr_err_chn_int_ena:1;
        /** out_total_eof_chn_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_TOTAL_EOF_CHn_INT.
         */
        uint32_t out_total_eof_chn_int_ena:1;
        /** outfifo_ovf_chn_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable AHB_DMA_OUTFIFO_OVF_CHn_INT.
         */
        uint32_t outfifo_ovf_chn_int_ena:1;
        /** outfifo_udf_chn_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable AHB_DMA_OUTFIFO_UDF_CHn_INT.
         */
        uint32_t outfifo_udf_chn_int_ena:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_out_int_ena_chn_reg_t;

/** Type of out_int_clr_chn register
 *  TX channel n interrupt clear register
 */
typedef union {
    struct {
        /** out_done_chn_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_DONE_CHn_INT.
         */
        uint32_t out_done_chn_int_clr:1;
        /** out_eof_chn_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_EOF_CHn_INT.
         */
        uint32_t out_eof_chn_int_clr:1;
        /** out_dscr_err_chn_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_DSCR_ERR_CHn_INT.
         */
        uint32_t out_dscr_err_chn_int_clr:1;
        /** out_total_eof_chn_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_TOTAL_EOF_CHn_INT.
         */
        uint32_t out_total_eof_chn_int_clr:1;
        /** outfifo_ovf_chn_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear AHB_DMA_OUTFIFO_OVF_CHn_INT.
         */
        uint32_t outfifo_ovf_chn_int_clr:1;
        /** outfifo_udf_chn_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear AHB_DMA_OUTFIFO_UDF_CHn_INT.
         */
        uint32_t outfifo_udf_chn_int_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_out_int_clr_chn_reg_t;


/** Group: Configuration Registers */
/** Type of misc_conf register
 *  Miscellaneous register
 */
typedef union {
    struct {
        /** ahbm_rst_inter : R/W; bitpos: [0]; default: 0;
         *  Write 1 and then 0 to reset the internal AHB FSM.
         */
        uint32_t ahbm_rst_inter:1;
        uint32_t reserved_1:1;
        /** arb_pri_dis : R/W; bitpos: [2]; default: 0;
         *  Configures whether to disable the priority arbitration.
         *  0: Enable
         *  1: Disable
         */
        uint32_t arb_pri_dis:1;
        /** clk_en : R/W; bitpos: [3]; default: 0;
         *  Configures AHB DMA clock gating.
         *  0: Support clock only when the application writes registers
         *  1: Always force the clock on for registers
         */
        uint32_t clk_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_misc_conf_reg_t;

/** Type of in_conf0_chn register
 *  Configuration register 0 of RX channel n
 */
typedef union {
    struct {
        /** in_rst_chn : R/W; bitpos: [0]; default: 0;
         *  Write 1 and then 0 to reset RX channel n FSM and RX FIFO pointer.
         */
        uint32_t in_rst_chn:1;
        /** in_loop_test_chn : R/W; bitpos: [1]; default: 0;
         *  Configures the owner bit value for inlink write-back.
         */
        uint32_t in_loop_test_chn:1;
        /** indscr_burst_en_chn : R/W; bitpos: [2]; default: 0;
         *  Configures whether to enable INCR burst transfer for RX channel n to read
         *  descriptors.
         *  0: Disable
         *  1: Enable
         */
        uint32_t indscr_burst_en_chn:1;
        uint32_t reserved_3:1;
        /** mem_trans_en_chn : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable memory-to-memory data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t mem_trans_en_chn:1;
        /** in_etm_en_chn : R/W; bitpos: [5]; default: 0;
         *  Configures whether to enable ETM control for RX channeln.
         *  0: Disable
         *  1: Enable
         */
        uint32_t in_etm_en_chn:1;
        /** in_data_burst_mode_sel_chn : R/W; bitpos: [7:6]; default: 0;
         *  Configures maximum burst length for RX channeln.
         *  0: SINGLE
         *  1: INCR4
         *  2: INCR8
         *  3: Reserved
         */
        uint32_t in_data_burst_mode_sel_chn:2;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_conf0_chn_reg_t;

/** Type of in_conf1_chn register
 *  Configuration register 1 of RX channel n
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** in_check_owner_chn : R/W; bitpos: [12]; default: 0;
         *  Configures whether to enable owner bit check for RX channel n.
         *  0: Disable
         *  1: Enable
         */
        uint32_t in_check_owner_chn:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_in_conf1_chn_reg_t;

/** Type of in_pop_chn register
 *  Pop control register of RX channel n
 */
typedef union {
    struct {
        /** infifo_rdata_chn : RO; bitpos: [11:0]; default: 2048;
         *  Represents the data popped from AHB DMA RX FIFO.
         */
        uint32_t infifo_rdata_chn:12;
        /** infifo_pop_chn : WT; bitpos: [12]; default: 0;
         *  Configures whether to pop data from AHB DMA RX FIFO.
         *  0: Invalid. No effect
         *  1: Pop
         */
        uint32_t infifo_pop_chn:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_in_pop_chn_reg_t;

/** Type of in_link_chn register
 *  Linked list descriptor configuration and control register of RX channel n
 */
typedef union {
    struct {
        /** inlink_auto_ret_chn : R/W; bitpos: [0]; default: 1;
         *  Configures whether to return to the current receive descriptor's address when there
         *  are some errors in current receiving data.
         *  0: Not return
         *  1: Return
         */
        uint32_t inlink_auto_ret_chn:1;
        /** inlink_stop_chn : WT; bitpos: [1]; default: 0;
         *  Configures whether to stop RX channel n from receiving data.
         *  0: Invalid. No effect
         *  1: Stop
         */
        uint32_t inlink_stop_chn:1;
        /** inlink_start_chn : WT; bitpos: [2]; default: 0;
         *  Configures whether to enable RX channel n for data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t inlink_start_chn:1;
        /** inlink_restart_chn : WT; bitpos: [3]; default: 0;
         *  Configures whether to restart RX channel n for AHB DMA transfer.
         *  0: Invalid. No effect
         *  1: Restart
         */
        uint32_t inlink_restart_chn:1;
        /** inlink_park_chn : RO; bitpos: [4]; default: 1;
         *  Represents the status of the receive descriptor's FSM.
         *  0: Running
         *  1: Idle
         */
        uint32_t inlink_park_chn:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} ahb_dma_in_link_chn_reg_t;

/** Type of out_conf0_chn register
 *  Configuration register 0 of TX channel n
 */
typedef union {
    struct {
        /** out_rst_chn : R/W; bitpos: [0]; default: 0;
         *  Configures the reset state of TX channel n FSM and TX FIFO pointer.
         *  0: Release reset
         *  1: Reset
         */
        uint32_t out_rst_chn:1;
        /** out_loop_test_chn : R/W; bitpos: [1]; default: 0;
         *  Configures the owner bit value for outlink write-back.
         */
        uint32_t out_loop_test_chn:1;
        /** out_auto_wrback_chn : R/W; bitpos: [2]; default: 0;
         *  Configures whether to enable automatic outlink write-back when all the data in TX
         *  FIFO has been transmitted.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_auto_wrback_chn:1;
        /** out_eof_mode_chn : R/W; bitpos: [3]; default: 1;
         *  Configures when to generate EOF flag.
         *  0: EOF flag for TX channel n is generated when data to be transmitted has been
         *  pushed into FIFO in AHB DMA.
         *  1: EOF flag for TX channel n is generated when data to be transmitted has been
         *  popped from FIFO in AHB DMA.
         */
        uint32_t out_eof_mode_chn:1;
        /** outdscr_burst_en_chn : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable INCR burst transfer for TX channel n reading
         *  descriptors.
         *  0: Disable
         *  1: Enable
         */
        uint32_t outdscr_burst_en_chn:1;
        uint32_t reserved_5:1;
        /** out_etm_en_chn : R/W; bitpos: [6]; default: 0;
         *  Configures whether to enable ETM control for TX channel n.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_etm_en_chn:1;
        uint32_t reserved_7:1;
        /** out_data_burst_mode_sel_chn : R/W; bitpos: [9:8]; default: 0;
         *  Configures maximum burst length for TX channeln.
         *  0: SINGLE
         *  1: INCR4
         *  2: INCR8
         *  3: Reserved
         */
        uint32_t out_data_burst_mode_sel_chn:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} ahb_dma_out_conf0_chn_reg_t;

/** Type of out_conf1_chn register
 *  Configuration register 1 of TX channel n
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** out_check_owner_chn : R/W; bitpos: [12]; default: 0;
         *  Configures whether to enable owner bit check for TX channel n.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_check_owner_chn:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_out_conf1_chn_reg_t;

/** Type of out_push_chn register
 *  Push control register of TX channel n
 */
typedef union {
    struct {
        /** outfifo_wdata_chn : R/W; bitpos: [8:0]; default: 0;
         *  Represents the data that need to be pushed into AHB DMA TX FIFO.
         */
        uint32_t outfifo_wdata_chn:9;
        /** outfifo_push_chn : WT; bitpos: [9]; default: 0;
         *  Configures whether to push data into AHB DMA TX FIFO.
         *  0: Invalid. No effect
         *  1: Push
         */
        uint32_t outfifo_push_chn:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} ahb_dma_out_push_chn_reg_t;

/** Type of out_link_chn register
 *  Linked list descriptor configuration and control register of TX channel n
 */
typedef union {
    struct {
        /** outlink_stop_chn : WT; bitpos: [0]; default: 0;
         *  Configures whether to stop TX channel n from transmitting data.
         *  0: Invalid. No effect
         *  1: Stop
         */
        uint32_t outlink_stop_chn:1;
        /** outlink_start_chn : WT; bitpos: [1]; default: 0;
         *  Configures whether to enable TX channel n for data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t outlink_start_chn:1;
        /** outlink_restart_chn : WT; bitpos: [2]; default: 0;
         *  Configures whether to restart TX channel n for AHB DMA transfer.
         *  0: Invalid. No effect
         *  1: Restart
         */
        uint32_t outlink_restart_chn:1;
        /** outlink_park_chn : RO; bitpos: [3]; default: 1;
         *  Represents the status of the transmit descriptor's FSM.
         *  0: Running
         *  1: Idle
         */
        uint32_t outlink_park_chn:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_out_link_chn_reg_t;

/** Type of tx_ch_arb_weigh_chn register
 *  TX channel n arbitration weight configuration register
 */
typedef union {
    struct {
        /** tx_ch_arb_weigh_chn : R/W; bitpos: [3:0]; default: 0;
         *  Configures the weight (i.e the number of tokens) of TX channeln.
         *  Value range: 0 ~ 15.
         */
        uint32_t tx_ch_arb_weigh_chn:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_tx_ch_arb_weigh_chn_reg_t;

/** Type of tx_arb_weigh_opt_dir_chn register
 *  TX channel n weight arbitration optimization enable register
 */
typedef union {
    struct {
        /** tx_arb_weigh_opt_dir_chn : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable weight optimization for TX channel n.
         *  0: Disable
         *  1: Enable
         */
        uint32_t tx_arb_weigh_opt_dir_chn:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_tx_arb_weigh_opt_dir_chn_reg_t;

/** Type of rx_ch_arb_weigh_chn register
 *  RX channel n arbitration weight configuration register
 */
typedef union {
    struct {
        /** rx_ch_arb_weigh_chn : R/W; bitpos: [3:0]; default: 0;
         *  Configures the weight (i.e the number of tokens) of RX channeln.
         *  Value range: 0 ~ 15.
         */
        uint32_t rx_ch_arb_weigh_chn:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_rx_ch_arb_weigh_chn_reg_t;

/** Type of rx_arb_weigh_opt_dir_chn register
 *  RX channel n weight arbitration optimization enable register
 */
typedef union {
    struct {
        /** rx_arb_weigh_opt_dir_chn : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable weight optimization for RX channel n.
         *  0: Disable
         *  1: Enable
         */
        uint32_t rx_arb_weigh_opt_dir_chn:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_rx_arb_weigh_opt_dir_chn_reg_t;

/** Type of in_link_addr_chn register
 *  Linked list descriptor configuration register of RX channel n
 */
typedef union {
    struct {
        /** inlink_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  Represents the first receive descriptor's address.
         */
        uint32_t inlink_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_in_link_addr_chn_reg_t;

/** Type of out_link_addr_chn register
 *  Linked list descriptor configuration register of TX channel n
 */
typedef union {
    struct {
        /** outlink_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  Represents the first transmit descriptor's address.
         */
        uint32_t outlink_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_out_link_addr_chn_reg_t;

/** Type of intr_mem_start_addr register
 *  Accessible address space start address configuration register
 */
typedef union {
    struct {
        /** access_intr_mem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  Configures the start address of accessible address space.
         */
        uint32_t access_intr_mem_start_addr:32;
    };
    uint32_t val;
} ahb_dma_intr_mem_start_addr_reg_t;

/** Type of intr_mem_end_addr register
 *  Accessible address space end address configuration register
 */
typedef union {
    struct {
        /** access_intr_mem_end_addr : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the end address of accessible address space.
         */
        uint32_t access_intr_mem_end_addr:32;
    };
    uint32_t val;
} ahb_dma_intr_mem_end_addr_reg_t;

/** Type of arb_timeout_tx register
 *  TX arbitration timeout configuration register
 */
typedef union {
    struct {
        /** arb_timeout_tx : R/W; bitpos: [15:0]; default: 0;
         *  Configures the time slot for TX. Measurement unit: AHB bus clock cycle.
         */
        uint32_t arb_timeout_tx:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} ahb_dma_arb_timeout_tx_reg_t;

/** Type of arb_timeout_rx register
 *  RX arbitration timeout configuration register
 */
typedef union {
    struct {
        /** arb_timeout_rx : R/W; bitpos: [15:0]; default: 0;
         *  Configures the time slot for RX. Measurement unit: AHB bus clock cycle.
         */
        uint32_t arb_timeout_rx:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} ahb_dma_arb_timeout_rx_reg_t;

/** Type of weight_en_tx register
 *  TX weight arbitration enable register
 */
typedef union {
    struct {
        /** weight_en_tx : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable weight arbitration for TX.
         *  0: Disable
         *  1: Enable
         */
        uint32_t weight_en_tx:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_weight_en_tx_reg_t;

/** Type of weight_en_rx register
 *  RX weight arbitration enable register
 */
typedef union {
    struct {
        /** weight_en_rx : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable weight arbitration for RX.
         *  0: Disable
         *  1: Enable
         */
        uint32_t weight_en_rx:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_weight_en_rx_reg_t;


/** Group: Version Registers */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 36770448;
         *  Version control register.
         */
        uint32_t date:32;
    };
    uint32_t val;
} ahb_dma_date_reg_t;


/** Group: Status Registers */
/** Type of infifo_status_chn register
 *  RX channel n FIFO status
 */
typedef union {
    struct {
        /** infifo_full_chn : RO; bitpos: [0]; default: 1;
         *  Represents whether L1 RX FIFO is full.
         *  0: Not Full
         *  1: Full
         */
        uint32_t infifo_full_chn:1;
        /** infifo_empty_chn : RO; bitpos: [1]; default: 1;
         *  Represents whether L1 RX FIFO is empty.
         *  0: Not empty
         *  1: Empty
         */
        uint32_t infifo_empty_chn:1;
        uint32_t reserved_2:6;
        /** infifo_cnt_chn : RO; bitpos: [14:8]; default: 0;
         *  Represents the number of data bytes in L1 RX FIFO for RX channel n.
         */
        uint32_t infifo_cnt_chn:7;
        uint32_t reserved_15:8;
        /** in_remain_under_1b_chn : RO; bitpos: [23]; default: 1;
         *  Reserved.
         */
        uint32_t in_remain_under_1b_chn:1;
        /** in_remain_under_2b_chn : RO; bitpos: [24]; default: 1;
         *  Reserved.
         */
        uint32_t in_remain_under_2b_chn:1;
        /** in_remain_under_3b_chn : RO; bitpos: [25]; default: 1;
         *  Reserved.
         */
        uint32_t in_remain_under_3b_chn:1;
        /** in_remain_under_4b_chn : RO; bitpos: [26]; default: 1;
         *  Reserved.
         */
        uint32_t in_remain_under_4b_chn:1;
        /** in_buf_hungry_chn : RO; bitpos: [27]; default: 0;
         *  Reserved.
         */
        uint32_t in_buf_hungry_chn:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ahb_dma_infifo_status_chn_reg_t;

/** Type of in_state_chn register
 *  RX channel n status
 */
typedef union {
    struct {
        /** inlink_dscr_addr_chn : RO; bitpos: [17:0]; default: 0;
         *  Represents the lower 18 bits of the next receive descriptor address that is
         *  pre-read (but not processed yet). If the current receive descriptor is the last
         *  descriptor, then this field represents the address of the current receive
         *  descriptor.
         */
        uint32_t inlink_dscr_addr_chn:18;
        /** in_dscr_state_chn : RO; bitpos: [19:18]; default: 0;
         *  Reserved.
         */
        uint32_t in_dscr_state_chn:2;
        /** in_state_chn : RO; bitpos: [22:20]; default: 0;
         *  Reserved.
         */
        uint32_t in_state_chn:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} ahb_dma_in_state_chn_reg_t;

/** Type of in_suc_eof_des_addr_chn register
 *  Receive descriptor address when EOF occurs on RX channel n
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the receive descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_in_suc_eof_des_addr_chn_reg_t;

/** Type of in_err_eof_des_addr_chn register
 *  Receive descriptor address when errors occur of RX channel n
 */
typedef union {
    struct {
        /** in_err_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the receive descriptor when there are some errors in the
         *  currently received data.
         */
        uint32_t in_err_eof_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_in_err_eof_des_addr_chn_reg_t;

/** Type of in_dscr_chn register
 *  Address of the next receive descriptor pointed by the current pre-read receive
 *  descriptor on RX channel n
 */
typedef union {
    struct {
        /** inlink_dscr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the next receive descriptor x+1 pointed by the current
         *  receive descriptor that is pre-read.
         */
        uint32_t inlink_dscr_chn:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_chn_reg_t;

/** Type of in_dscr_bf0_chn register
 *  Address of the current pre-read receive descriptor on RX channel n
 */
typedef union {
    struct {
        /** inlink_dscr_bf0_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the current receive descriptor x that is pre-read.
         */
        uint32_t inlink_dscr_bf0_chn:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_bf0_chn_reg_t;

/** Type of in_dscr_bf1_chn register
 *  Address of the previous pre-read receive descriptor on RX channel n
 */
typedef union {
    struct {
        /** inlink_dscr_bf1_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the previous receive descriptor x-1 that is pre-read.
         */
        uint32_t inlink_dscr_bf1_chn:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_bf1_chn_reg_t;

/** Type of in_done_des_addr_chn register
 *  Address of the completed inlink descriptor on RX channel n
 */
typedef union {
    struct {
        /** in_done_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the inlink descriptor when this descriptor is completed.
         */
        uint32_t in_done_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_in_done_des_addr_chn_reg_t;

/** Type of outfifo_status_chn register
 *  TX channel n FIFO status
 */
typedef union {
    struct {
        /** outfifo_full_chn : RO; bitpos: [0]; default: 0;
         *  Represents whether L1 TX FIFO is full.
         *  0: Not Full
         *  1: Full
         */
        uint32_t outfifo_full_chn:1;
        /** outfifo_empty_chn : RO; bitpos: [1]; default: 1;
         *  Represents whether L1 TX FIFO is empty.
         *  0: Not empty
         *  1: Empty
         */
        uint32_t outfifo_empty_chn:1;
        uint32_t reserved_2:6;
        /** outfifo_cnt_chn : RO; bitpos: [14:8]; default: 0;
         *  Represents the number of data bytes in L1 TX FIFO for TX channel n.
         */
        uint32_t outfifo_cnt_chn:7;
        uint32_t reserved_15:8;
        /** out_remain_under_1b_chn : RO; bitpos: [23]; default: 1;
         *  Reserved.
         */
        uint32_t out_remain_under_1b_chn:1;
        /** out_remain_under_2b_chn : RO; bitpos: [24]; default: 1;
         *  Reserved.
         */
        uint32_t out_remain_under_2b_chn:1;
        /** out_remain_under_3b_chn : RO; bitpos: [25]; default: 1;
         *  Reserved.
         */
        uint32_t out_remain_under_3b_chn:1;
        /** out_remain_under_4b_chn : RO; bitpos: [26]; default: 1;
         *  Reserved.
         */
        uint32_t out_remain_under_4b_chn:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} ahb_dma_outfifo_status_chn_reg_t;

/** Type of out_state_chn register
 *  TX channel n status
 */
typedef union {
    struct {
        /** outlink_dscr_addr_chn : RO; bitpos: [17:0]; default: 0;
         *  Represents the lower 18 bits of the next transmit descriptor address that is
         *  pre-read (but not processed yet). If the current transmit descriptor is the last
         *  descriptor, then this field represents the address of the current transmit
         *  descriptor.
         */
        uint32_t outlink_dscr_addr_chn:18;
        /** out_dscr_state_chn : RO; bitpos: [19:18]; default: 0;
         *  Reserved.
         */
        uint32_t out_dscr_state_chn:2;
        /** out_state_chn : RO; bitpos: [22:20]; default: 0;
         *  Reserved.
         */
        uint32_t out_state_chn:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} ahb_dma_out_state_chn_reg_t;

/** Type of out_eof_des_addr_chn register
 *  Transmit descriptor address when EOF occurs on TX channel n
 */
typedef union {
    struct {
        /** out_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the transmit descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_out_eof_des_addr_chn_reg_t;

/** Type of out_eof_bfr_des_addr_chn register
 *  The last transmit descriptor address when EOF occurs on TX channel n
 */
typedef union {
    struct {
        /** out_eof_bfr_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the transmit descriptor before the last transmit
         *  descriptor.
         */
        uint32_t out_eof_bfr_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_out_eof_bfr_des_addr_chn_reg_t;

/** Type of out_dscr_chn register
 *  Address of the next transmit descriptor pointed by the current pre-read transmit
 *  descriptor on TX channel n
 */
typedef union {
    struct {
        /** outlink_dscr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the next transmit descriptor y+1 pointed by the current
         *  transmit descriptor that is pre-read.
         */
        uint32_t outlink_dscr_chn:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_chn_reg_t;

/** Type of out_dscr_bf0_chn register
 *  Address of the current pre-read transmit descriptor on TX channel n
 */
typedef union {
    struct {
        /** outlink_dscr_bf0_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the current transmit descriptor y that is pre-read.
         */
        uint32_t outlink_dscr_bf0_chn:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_bf0_chn_reg_t;

/** Type of out_dscr_bf1_chn register
 *  Address of the previous pre-read transmit descriptor on TX channel n
 */
typedef union {
    struct {
        /** outlink_dscr_bf1_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the previous transmit descriptor y-1 that is pre-read.
         */
        uint32_t outlink_dscr_bf1_chn:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_bf1_chn_reg_t;

/** Type of out_done_des_addr_chn register
 *  Address of the completed outlink descriptor on TX channel n
 */
typedef union {
    struct {
        /** out_done_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the outlink descriptor when this descriptor is completed.
         */
        uint32_t out_done_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_out_done_des_addr_chn_reg_t;


/** Group: Priority Registers */
/** Type of in_pri_chn register
 *  Priority register of RX channel n
 */
typedef union {
    struct {
        /** rx_pri_chn : R/W; bitpos: [3:0]; default: 0;
         *  Configures the priority of RX channel n. The larger the value, the higher the
         *  priority.
         *  Value range: 0  ~  5
         */
        uint32_t rx_pri_chn:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_in_pri_chn_reg_t;

/** Type of out_pri_chn register
 *  Priority register of TX channel n
 */
typedef union {
    struct {
        /** tx_pri_chn : R/W; bitpos: [3:0]; default: 0;
         *  Configures the priority of TX channel n. The larger the value, the higher the
         *  priority.
         *  Value range: 0  ~  5
         */
        uint32_t tx_pri_chn:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_out_pri_chn_reg_t;


/** Group: Peripheral Selection Registers */
/** Type of in_peri_sel_chn register
 *  Peripheral selection register of RX channel n
 */
typedef union {
    struct {
        /** peri_in_sel_chn : R/W; bitpos: [5:0]; default: 63;
         *  Configures the peripheral connected to RX channel n.
         *  0: Dummy
         *  1: GP-SPI
         *  2: Dummy
         *  3: I2S
         *  4 ~ 6: Dummy
         *  7: SHA
         *  8: ADC
         *  9 ~ 15: Dummy
         *  16 ~ 63: Invalid
         */
        uint32_t peri_in_sel_chn:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_in_peri_sel_chn_reg_t;

/** Type of out_peri_sel_chn register
 *  Peripheral selection register of TX channel n
 */
typedef union {
    struct {
        /** peri_out_sel_chn : R/W; bitpos: [5:0]; default: 63;
         *  Configures the peripheral connected to TX channel n.
         *  0: Dummy
         *  1: GP-SPI
         *  2: Dummy
         *  3: I2S
         *  4 ~ 6: Dummy
         *  7: SHA
         *  8: ADC
         *  9 ~ 15: Dummy
         *  16 ~ 63: Invalid
         */
        uint32_t peri_out_sel_chn:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_out_peri_sel_chn_reg_t;

typedef struct {
    volatile ahb_dma_in_int_raw_chn_reg_t raw;
    volatile ahb_dma_in_int_st_chn_reg_t st;
    volatile ahb_dma_in_int_ena_chn_reg_t ena;
    volatile ahb_dma_in_int_clr_chn_reg_t clr;
} ahb_dma_in_int_chn_reg_t;

typedef struct {
    volatile ahb_dma_out_int_raw_chn_reg_t raw;
    volatile ahb_dma_out_int_st_chn_reg_t st;
    volatile ahb_dma_out_int_ena_chn_reg_t ena;
    volatile ahb_dma_out_int_clr_chn_reg_t clr;
} ahb_dma_out_int_chn_reg_t;


typedef struct {
    volatile ahb_dma_in_conf0_chn_reg_t in_conf0;
    volatile ahb_dma_in_conf1_chn_reg_t in_conf1;
    volatile ahb_dma_infifo_status_chn_reg_t infifo_status;
    volatile ahb_dma_in_pop_chn_reg_t in_pop;
    volatile ahb_dma_in_link_chn_reg_t in_link;
    volatile ahb_dma_in_state_chn_reg_t in_state;
    volatile ahb_dma_in_suc_eof_des_addr_chn_reg_t in_suc_eof_des_addr;
    volatile ahb_dma_in_err_eof_des_addr_chn_reg_t in_err_eof_des_addr;
    volatile ahb_dma_in_dscr_chn_reg_t in_dscr;
    volatile ahb_dma_in_dscr_bf0_chn_reg_t in_dscr_bf0;
    volatile ahb_dma_in_dscr_bf1_chn_reg_t in_dscr_bf1;
    volatile ahb_dma_in_pri_chn_reg_t in_pri;
    volatile ahb_dma_in_peri_sel_chn_reg_t in_peri_sel;
} ahb_dma_in_chn_reg_t;

typedef struct {
    volatile ahb_dma_out_conf0_chn_reg_t out_conf0;
    volatile ahb_dma_out_conf1_chn_reg_t out_conf1;
    volatile ahb_dma_outfifo_status_chn_reg_t outfifo_status;
    volatile ahb_dma_out_push_chn_reg_t out_push;
    volatile ahb_dma_out_link_chn_reg_t out_link;
    volatile ahb_dma_out_state_chn_reg_t out_state;
    volatile ahb_dma_out_eof_des_addr_chn_reg_t out_eof_des_addr;
    volatile ahb_dma_out_eof_bfr_des_addr_chn_reg_t out_eof_bfr_des_addr;
    volatile ahb_dma_out_dscr_chn_reg_t out_dscr;
    volatile ahb_dma_out_dscr_bf0_chn_reg_t out_dscr_bf0;
    volatile ahb_dma_out_dscr_bf1_chn_reg_t out_dscr_bf1;
    volatile ahb_dma_out_pri_chn_reg_t out_pri;
    volatile ahb_dma_out_peri_sel_chn_reg_t out_peri_sel;
} ahb_dma_out_chn_reg_t;

typedef struct {
    volatile ahb_dma_in_chn_reg_t in;
    uint32_t reserved_0a4[3];
    volatile ahb_dma_in_done_des_addr_chn_reg_t in_done_des_addr_ch0;
    uint32_t reserved_0b4[7];
    volatile ahb_dma_out_chn_reg_t out;
    uint32_t reserved_104[3];
    volatile ahb_dma_out_done_des_addr_chn_reg_t out_done_des_addr_ch0;
    uint32_t reserved_114[7];
} ahb_dma_chn_reg_t;

typedef struct {
    uint32_t reserved[8];
    ahb_dma_tx_ch_arb_weigh_chn_reg_t ch_arb_weigh;
    ahb_dma_tx_arb_weigh_opt_dir_chn_reg_t arb_weigh_opt;
} ahb_dma_out_crc_arb_chn_reg_t;

typedef struct {
    uint32_t reserved[8];
    ahb_dma_rx_ch_arb_weigh_chn_reg_t ch_arb_weigh;
    ahb_dma_rx_arb_weigh_opt_dir_chn_reg_t arb_weigh_opt;
} ahb_dma_in_crc_arb_chn_reg_t;

typedef struct {
    volatile ahb_dma_in_int_chn_reg_t in_intr[2];
    uint32_t reserved_020[4];
    volatile ahb_dma_out_int_chn_reg_t out_intr[2];
    uint32_t reserved_050[5];
    volatile ahb_dma_misc_conf_reg_t misc_conf;
    volatile ahb_dma_date_reg_t date;
    uint32_t reserved_06c;
    volatile ahb_dma_chn_reg_t channel[2];
    uint32_t reserved_1d4[51];
    volatile ahb_dma_out_crc_arb_chn_reg_t out_crc_arb[2];
    uint32_t reserved_30c[10];
    volatile ahb_dma_in_crc_arb_chn_reg_t in_crc_arb[2];
    uint32_t reserved_384[10];
    volatile ahb_dma_in_link_addr_chn_reg_t in_link_addr[2];
    uint32_t reserved_3b4;
    volatile ahb_dma_out_link_addr_chn_reg_t out_link_addr[2];
    uint32_t reserved_3c0;
    volatile ahb_dma_intr_mem_start_addr_reg_t intr_mem_start_addr;
    volatile ahb_dma_intr_mem_end_addr_reg_t intr_mem_end_addr;
    volatile ahb_dma_arb_timeout_tx_reg_t arb_timeout_tx;
    volatile ahb_dma_arb_timeout_rx_reg_t arb_timeout_rx;
    volatile ahb_dma_weight_en_tx_reg_t weight_en_tx;
    volatile ahb_dma_weight_en_rx_reg_t weight_en_rx;
} ahb_dma_dev_t;

extern ahb_dma_dev_t AHB_DMA;

#ifndef __cplusplus
_Static_assert(sizeof(ahb_dma_dev_t) == 0x3dc, "Invalid size of ahb_dma_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
