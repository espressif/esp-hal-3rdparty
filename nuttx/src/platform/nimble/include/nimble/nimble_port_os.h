/*
 * SPDX-FileCopyrightText: 2015-2022 The Apache Software Foundation (ASF)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2019-2022 Espressif Systems (Shanghai) CO LTD
 */

#ifndef _NIMBLE_PORT_OS_H
#define _NIMBLE_PORT_OS_H

#include "nimble/nimble_npl.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t esp_nimble_enable(void *host_task);
esp_err_t esp_nimble_disable(void);

void nimble_port_os_init(void *host_task_fn);
void nimble_port_os_deinit(void);
void npl_os_funcs_init(void);
void npl_os_funcs_deinit(void);
int npl_os_mempool_init(void);
void npl_os_mempool_deinit(void);
struct npl_funcs_t *npl_os_funcs_get(void);
int npl_os_set_controller_npl_info(ble_npl_count_info_t *ctrl_npl_info);

#ifdef __cplusplus
}
#endif

#endif /* _NIMBLE_PORT_OS_H */
