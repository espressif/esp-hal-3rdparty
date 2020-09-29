// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>

#include "btc_spp.h"
#include "btc/btc_manage.h"
#include "btc/btc_task.h"
#include "bta/bta_jv_api.h"
#include "common/bt_trace.h"
#include "osi/allocator.h"
#include "esp_spp_api.h"
#include "osi/list.h"
#include "freertos/ringbuf.h"
#include "osi/mutex.h"
#include <sys/errno.h>
#include <sys/lock.h>
#include <sys/fcntl.h>
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "stack/port_api.h"

#if (defined BTC_SPP_INCLUDED && BTC_SPP_INCLUDED == TRUE)

typedef struct {
    uint8_t serial;
    bool connected;
    uint8_t scn;
    uint8_t max_session;
    RingbufHandle_t ringbuf_read;
    RingbufHandle_t ringbuf_write;
    uint32_t id;
    uint32_t mtu;//unused
    uint32_t sdp_handle;
    uint32_t rfc_handle;
    uint32_t rfc_port_handle;
    int fd;
    uint8_t *write_data;
    esp_spp_role_t role;
    esp_spp_sec_t security;
    esp_bd_addr_t addr;
    list_t *list;
    list_t *incoming_list;
    uint8_t service_uuid[16];
    char service_name[ESP_SPP_SERVER_NAME_MAX + 1];
} spp_slot_t;

typedef struct {
    spp_slot_t *spp_slots[MAX_RFC_PORTS + 1];
    uint32_t spp_slot_id;
    esp_spp_mode_t spp_mode;
    osi_mutex_t spp_slot_mutex;
    esp_vfs_id_t spp_vfs_id;
} spp_local_param_t;

#if SPP_DYNAMIC_MEMORY == FALSE
static spp_local_param_t spp_local_param;
#else
static spp_local_param_t *spp_local_param_ptr;
#define spp_local_param (*spp_local_param_ptr)
#endif

static void spp_osi_free(void *p)
{
    osi_free(p);
}

#if SPP_DYNAMIC_MEMORY == FALSE
#define is_spp_init() (spp_local_param.spp_slot_mutex != NULL)
#else
#define is_spp_init() (&spp_local_param != NULL && spp_local_param.spp_slot_mutex != NULL)
#endif

static spp_slot_t *spp_malloc_slot(void)
{
    if (++spp_local_param.spp_slot_id == 0) {
        spp_local_param.spp_slot_id = 1;
    }
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        if (spp_local_param.spp_slots[i] == NULL) {
            spp_local_param.spp_slots[i] = (spp_slot_t *)osi_malloc(sizeof(spp_slot_t));
            if (!spp_local_param.spp_slots[i]) {
                return NULL;
            }
            spp_local_param.spp_slots[i]->id = spp_local_param.spp_slot_id;
            spp_local_param.spp_slots[i]->serial = i;
            spp_local_param.spp_slots[i]->sdp_handle = 0;
            spp_local_param.spp_slots[i]->rfc_handle = 0;
            spp_local_param.spp_slots[i]->rfc_port_handle = 0;
            spp_local_param.spp_slots[i]->connected = FALSE;
            spp_local_param.spp_slots[i]->write_data = NULL;
            spp_local_param.spp_slots[i]->list = list_new(spp_osi_free);
            spp_local_param.spp_slots[i]->incoming_list = list_new(spp_osi_free);
            if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
                if (esp_vfs_register_fd(spp_local_param.spp_vfs_id, &spp_local_param.spp_slots[i]->fd) != ESP_OK) {
                    osi_free(spp_local_param.spp_slots[i]);
                    return NULL;
                }
                spp_local_param.spp_slots[i]->ringbuf_read = xRingbufferCreate(ESP_SPP_RINGBUF_SIZE, RINGBUF_TYPE_BYTEBUF);
                spp_local_param.spp_slots[i]->ringbuf_write = xRingbufferCreate(ESP_SPP_RINGBUF_SIZE, RINGBUF_TYPE_BYTEBUF);
            }
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_id(uint32_t id)
{
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->id == id) {
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_handle(uint32_t handle)
{
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->rfc_handle == handle) {
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_fd(int fd)
{
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->fd == fd) {
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_scn(uint32_t scn)
{
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->scn == (uint8_t)scn) {
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static void spp_free_slot(spp_slot_t *slot)
{
    if (!slot) {
        return;
    }
    spp_local_param.spp_slots[slot->serial] = NULL;
    list_free(slot->list);
    if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
        (void) esp_vfs_unregister_fd(spp_local_param.spp_vfs_id, slot->fd);
        vRingbufferDelete(slot->ringbuf_read);
        vRingbufferDelete(slot->ringbuf_write);
    }
    list_free(slot->incoming_list);
    osi_free(slot);
}

static inline void btc_spp_cb_to_app(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    esp_spp_cb_t *btc_spp_cb = (esp_spp_cb_t *)btc_profile_cb_get(BTC_PID_SPP);
    if (btc_spp_cb) {
        btc_spp_cb(event, param);
    }
}

static void btc_create_server_fail_cb(void)
{
    esp_spp_cb_param_t param;
    param.start.status = ESP_SPP_FAILURE;
    btc_spp_cb_to_app(ESP_SPP_START_EVT, &param);
}

static void *btc_spp_rfcomm_inter_cb(tBTA_JV_EVT event, tBTA_JV *p_data, void *user_data)
{
    bt_status_t status;
    btc_msg_t msg;
    void *new_user_data = NULL;

    uint32_t id = (uintptr_t)user_data;
    spp_slot_t *slot, *slot_new;
    if (!spp_local_param.spp_slot_mutex) {
        BTC_TRACE_WARNING("%s SPP have been deinit, incoming events ignore!\n", __func__);
        return new_user_data;
    }
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    switch (event) {
    case BTA_JV_RFCOMM_START_EVT:
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            p_data->rfc_start.status = ESP_SPP_NO_CONNECTION;
            break;
        }
        slot->rfc_handle = p_data->rfc_start.handle;
        slot->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_start.handle);
        break;
    case BTA_JV_RFCOMM_SRV_OPEN_EVT:
        slot = p_data->rfc_srv_open.handle ? spp_find_slot_by_id(id) : spp_find_slot_by_scn((uint32_t)user_data);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            p_data->rfc_srv_open.status = ESP_SPP_NO_CONNECTION;
            break;
        }

        if (p_data->rfc_srv_open.handle) {
            new_user_data = (void *)(uintptr_t)slot->id;
            memcpy(slot->addr, p_data->rfc_srv_open.rem_bda, ESP_BD_ADDR_LEN);
            slot->connected = TRUE;
            slot->rfc_handle = p_data->rfc_srv_open.handle;
            slot->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_srv_open.handle);
            BTA_JvSetPmProfile(p_data->rfc_srv_open.handle, BTA_JV_PM_ALL, BTA_JV_CONN_OPEN);
        }

        if (p_data->rfc_srv_open.handle != p_data->rfc_srv_open.new_listen_handle) {
            slot_new = spp_malloc_slot();
            if (!slot_new) {
                BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
                p_data->rfc_srv_open.status = ESP_SPP_NO_RESOURCE;
                break;
            }
            new_user_data = (void *)(uintptr_t)slot_new->id;
            slot_new->security = slot->security;
            slot_new->role = slot->role;
            slot_new->scn = slot->scn;
            slot_new->max_session = slot->max_session;
            strcpy(slot_new->service_name, slot->service_name);
            slot_new->sdp_handle = slot->sdp_handle;
            slot_new->rfc_handle = p_data->rfc_srv_open.new_listen_handle;
            slot_new->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_srv_open.new_listen_handle);
        }
        break;
    case BTA_JV_RFCOMM_OPEN_EVT:
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            p_data->rfc_open.status = ESP_SPP_NO_CONNECTION;
            break;
        }
        slot->connected = TRUE;
        slot->rfc_handle = p_data->rfc_open.handle;
        slot->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_open.handle);
        BTA_JvSetPmProfile(p_data->rfc_open.handle, BTA_JV_PM_ID_1, BTA_JV_CONN_OPEN);
        break;
    case BTA_JV_RFCOMM_CLOSE_EVT:
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            p_data->rfc_close.status = ESP_SPP_NO_CONNECTION;
            break;
        }
        if (slot->connected && p_data->rfc_close.port_status != PORT_LOCAL_CLOSED) {
            BTA_JvRfcommClose(slot->rfc_handle, NULL, (void *)slot->id);
        }
        p_data->rfc_close.slot = slot;
        p_data->rfc_close.status = BTA_JV_SUCCESS;
        break;
    case BTA_JV_RFCOMM_DATA_IND_EVT:
        break;
    case BTA_JV_FREE_SCN_EVT:
        if (user_data) {
            id = ((tBTA_JV_FREE_SCN_USER_DATA *)user_data)->slot_id;
            slot = spp_find_slot_by_id(id);
            if (slot) {
                spp_free_slot(slot);
            } else {
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                p_data->free_scn.status = ESP_SPP_NO_CONNECTION;
            }
            osi_free(user_data);
        }
        break;
    default:
        break;
    }
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_SPP;
    msg.act = event;


    status = btc_transfer_context(&msg, p_data,
                                  sizeof(tBTA_JV), NULL);

    if (status != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed", __func__);
    }
    return new_user_data;
}

static void btc_spp_dm_inter_cb(tBTA_JV_EVT event, tBTA_JV *p_data, void *user_data)
{
    bt_status_t status;
    btc_msg_t msg;

    uint32_t id = (uintptr_t)user_data;
    spp_slot_t *slot;
    switch (event) {
    case BTA_JV_GET_SCN_EVT:
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            break;
        }
        if (p_data->scn == 0) {
            BTC_TRACE_ERROR("%s unable to get scn, start server fail!", __func__);
            btc_create_server_fail_cb();
            spp_free_slot(slot);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            break;
        }

        slot->scn = p_data->scn;
        BTA_JvCreateRecordByUser(slot->service_name, slot->scn, (void *)slot->id);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        break;
    case BTA_JV_CREATE_RECORD_EVT:
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            break;
        }
        if (p_data->create_rec.status == BTA_JV_SUCCESS) {
            slot->sdp_handle = p_data->create_rec.handle;
            BTA_JvRfcommStartServer(slot->security, slot->role, slot->scn,
                                    slot->max_session, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)slot->id);
        } else {
            BTC_TRACE_ERROR("%s unable to create record, start server fail!", __func__);
            btc_create_server_fail_cb();
            tBTA_JV_FREE_SCN_USER_DATA *user_data = osi_malloc(sizeof(tBTA_JV_FREE_SCN_USER_DATA));
            if (user_data) {
                user_data->server_status = BTA_JV_SERVER_START_FAILED;
                user_data->slot_id = slot->id;
            } else {
                BTC_TRACE_ERROR("%s unable to malloc user data!", __func__);
            }
            BTA_JvFreeChannel(slot->scn, BTA_JV_CONN_TYPE_RFCOMM,
                              (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)user_data);
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        break;
    default:
        msg.sig = BTC_SIG_API_CB;
        msg.pid = BTC_PID_SPP;
        msg.act = event;

        status = btc_transfer_context(&msg, p_data, sizeof(tBTA_JV), NULL);

        if (status != BT_STATUS_SUCCESS) {
            BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
        }
        break;
    }
}

static void btc_spp_init(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP has been initiated, shall uninit first!", __func__);
            ret = ESP_SPP_NEED_UNINIT;
            break;
        }

#if SPP_DYNAMIC_MEMORY == TRUE
        if ((spp_local_param_ptr = (spp_local_param_t *)osi_malloc(sizeof(spp_local_param_t))) == NULL) {
            BTC_TRACE_ERROR("%s malloc failed\n", __func__);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        memset((void *)spp_local_param_ptr, 0, sizeof(spp_local_param_t));
#endif

        if (osi_mutex_new(&spp_local_param.spp_slot_mutex) != 0) {
            BTC_TRACE_ERROR("%s osi_mutex_new failed\n", __func__);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        spp_local_param.spp_mode = arg->init.mode;
        spp_local_param.spp_slot_id = 0;
        BTA_JvEnable((tBTA_JV_DM_CBACK *)btc_spp_dm_inter_cb);
    } while (0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.init.status = ret;
        btc_spp_cb_to_app(ESP_SPP_INIT_EVT, &param);
    }
}

static void btc_spp_uninit(void)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP has not been initiated, shall init first!", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        // first, remove all connection
        for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
            if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->connected) {
                BTA_JvRfcommClose(spp_local_param.spp_slots[i]->rfc_handle, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb,
                                  (void *)spp_local_param.spp_slots[i]->id);
            }
        }
        // second, remove all server
        for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
            if (spp_local_param.spp_slots[i] != NULL && !spp_local_param.spp_slots[i]->connected) {
                if (spp_local_param.spp_slots[i]->sdp_handle > 0) {
                    BTA_JvDeleteRecord(spp_local_param.spp_slots[i]->sdp_handle);
                }

                if (spp_local_param.spp_slots[i]->rfc_handle > 0) {
                    BTA_JvRfcommStopServer(spp_local_param.spp_slots[i]->rfc_handle,
                                           (void *)spp_local_param.spp_slots[i]->id);
                }

                tBTA_JV_FREE_SCN_USER_DATA *user_data = osi_malloc(sizeof(tBTA_JV_FREE_SCN_USER_DATA));
                if (user_data) {
                    user_data->server_status = BTA_JV_SERVER_RUNNING;
                    user_data->slot_id = spp_local_param.spp_slots[i]->id;
                } else {
                    esp_spp_cb_param_t param;
                    BTC_TRACE_ERROR("%s unable to malloc user data!", __func__);
                    param.srv_stop.status = ESP_SPP_NO_RESOURCE;
                    btc_spp_cb_to_app(ESP_SPP_SRV_STOP_EVT, &param);
                }
                BTA_JvFreeChannel(spp_local_param.spp_slots[i]->scn, BTA_JV_CONN_TYPE_RFCOMM,
                                  (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)user_data);
            }
        }
        BTA_JvDisable((tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    } while(0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.uninit.status = ret;
        btc_spp_cb_to_app(ESP_SPP_UNINIT_EVT, &param);
    }
}

static void btc_spp_start_discovery(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        BTA_JvStartDiscovery(arg->start_discovery.bd_addr, arg->start_discovery.num_uuid, arg->start_discovery.p_uuid_list, NULL);
    } while (0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.disc_comp.status = ret;
        param.disc_comp.scn_num = 0xff;
        memset(param.disc_comp.scn, 0xff, ESP_SPP_MAX_SCN);
        btc_spp_cb_to_app(ESP_SPP_DISCOVERY_COMP_EVT, &param);
    }
}

static void btc_spp_connect(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        spp_slot_t *slot = spp_malloc_slot();
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        slot->security = arg->connect.sec_mask;
        slot->role = arg->connect.role;
        slot->scn = arg->connect.remote_scn;

        memcpy(slot->addr, arg->connect.peer_bd_addr, ESP_BD_ADDR_LEN);
        BTA_JvRfcommConnect(arg->connect.sec_mask, arg->connect.role, arg->connect.remote_scn,
                            arg->connect.peer_bd_addr, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)slot->id);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    } while (0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.open.status = ret;
        param.open.handle = 0;
        param.open.fd = -1;
        memset(param.open.rem_bda, 0, ESP_BD_ADDR_LEN);
        btc_spp_cb_to_app(ESP_SPP_OPEN_EVT, &param);
    }
}

static void btc_spp_disconnect(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        spp_slot_t *slot = spp_find_slot_by_handle(arg->disconnect.handle);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot! disconnect fail!", __func__);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            ret = ESP_SPP_NO_CONNECTION;
            break;
        }
        BTA_JvRfcommClose(arg->disconnect.handle, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)slot->id);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    } while(0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.close.status = ret;
        param.close.port_status = PORT_ERR_MAX;
        param.close.handle = 0;
        param.close.async = FALSE;
        btc_spp_cb_to_app(ESP_SPP_CLOSE_EVT, &param);
    }
}

static void btc_spp_start_srv(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        spp_slot_t *slot = spp_malloc_slot();
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        slot->security = arg->start_srv.sec_mask;
        slot->role = arg->start_srv.role;
        slot->scn = arg->start_srv.local_scn;
        slot->max_session = arg->start_srv.max_session;
        strcpy(slot->service_name, arg->start_srv.name);

        BTA_JvGetChannelId(BTA_JV_CONN_TYPE_RFCOMM, (void *)slot->id, arg->start_srv.local_scn);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    } while(0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.srv_open.status = ret;
        param.srv_open.handle = 0;
        param.srv_open.new_listen_handle = 0;
        param.srv_open.fd = -1;
        memset(param.srv_open.rem_bda, 0, ESP_BD_ADDR_LEN);
        btc_spp_cb_to_app(ESP_SPP_SRV_OPEN_EVT, &param);
    }
}

static void btc_spp_stop_srv(void) 
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        // first, remove all connection
        for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
            if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->connected) {
                BTA_JvRfcommClose(spp_local_param.spp_slots[i]->rfc_handle, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb,
                                  (void *)spp_local_param.spp_slots[i]->id);
            }
        }
        // second, remove all server
        for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
            if (spp_local_param.spp_slots[i] != NULL && !spp_local_param.spp_slots[i]->connected) {
                if (spp_local_param.spp_slots[i]->sdp_handle > 0) {
                    BTA_JvDeleteRecord(spp_local_param.spp_slots[i]->sdp_handle);
                }

                if (spp_local_param.spp_slots[i]->rfc_handle > 0) {
                    BTA_JvRfcommStopServer(spp_local_param.spp_slots[i]->rfc_handle,
                                           (void *)spp_local_param.spp_slots[i]->id);
                }

                tBTA_JV_FREE_SCN_USER_DATA *user_data = osi_malloc(sizeof(tBTA_JV_FREE_SCN_USER_DATA));
                if (user_data) {
                    user_data->server_status = BTA_JV_SERVER_RUNNING;
                    user_data->slot_id = spp_local_param.spp_slots[i]->id;
                } else {
                    esp_spp_cb_param_t param;
                    BTC_TRACE_ERROR("%s unable to malloc user data!", __func__);
                    param.srv_stop.status = ESP_SPP_NO_RESOURCE;
                    btc_spp_cb_to_app(ESP_SPP_SRV_STOP_EVT, &param);
                }
                BTA_JvFreeChannel(spp_local_param.spp_slots[i]->scn, BTA_JV_CONN_TYPE_RFCOMM,
                                  (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)user_data);
            }
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    } while(0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.srv_stop.status = ret;
        btc_spp_cb_to_app(ESP_SPP_SRV_STOP_EVT, &param);
    }
}

static void btc_spp_write(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        spp_slot_t *slot = spp_find_slot_by_handle(arg->write.handle);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            ret = ESP_SPP_NO_CONNECTION;
            break;
        }
        if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
            size_t item_size = 0;
            if (slot->write_data != NULL) {
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                return;
            }
            uint8_t *data = xRingbufferReceiveUpTo(slot->ringbuf_write, &item_size, 0, BTA_JV_DEF_RFC_MTU);
            if (item_size != 0) {
                slot->write_data = data;
                BTA_JvRfcommWrite(arg->write.handle, slot->id, item_size, data);
            }
        } else {
            list_append(slot->list, arg->write.p_data);
            BTA_JvRfcommWrite(arg->write.handle, slot->id, arg->write.len, arg->write.p_data);
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    } while (0);

    if (ret != ESP_SPP_SUCCESS && spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
        esp_spp_cb_param_t param;
        param.write.status = ret;
        param.write.handle = 0;
        param.write.len = -1;
        param.write.cong = false;
        btc_spp_cb_to_app(ESP_SPP_WRITE_EVT, &param);
    }
}


void btc_spp_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_spp_args_t *dst = (btc_spp_args_t *) p_dest;
    btc_spp_args_t *src = (btc_spp_args_t *) p_src;

    switch (msg->act) {
    case BTC_SPP_ACT_START_DISCOVERY:
        dst->start_discovery.p_uuid_list = (tSDP_UUID *)osi_malloc(src->start_discovery.num_uuid * sizeof(tSDP_UUID));
        if (dst->start_discovery.p_uuid_list) {
            memcpy(dst->start_discovery.p_uuid_list, src->start_discovery.p_uuid_list, src->start_discovery.num_uuid * sizeof(tSDP_UUID));
        } else if (src->start_discovery.num_uuid == 0) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        } else {
            BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
        }
        break;
    case BTC_SPP_ACT_WRITE:
        dst->write.p_data = (uint8_t *)osi_malloc(src->write.len);
        if (dst->write.p_data) {
            memcpy(dst->write.p_data, src->write.p_data, src->write.len);
        } else if (src->write.len == 0) {
            BTC_TRACE_DEBUG("%s %d no mem\n", __func__, msg->act);
        } else {
            BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
        }
        break;
    default:
        break;
    }
}

void btc_spp_arg_deep_free(btc_msg_t *msg)
{
    btc_spp_args_t *arg = (btc_spp_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_SPP_ACT_START_DISCOVERY:
        if (arg->start_discovery.p_uuid_list) {
            osi_free(arg->start_discovery.p_uuid_list);
        }
        break;
    default:
        break;
    }
}

void btc_spp_call_handler(btc_msg_t *msg)
{
    btc_spp_args_t *arg = (btc_spp_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_SPP_ACT_INIT:
        btc_spp_init(arg);
        break;
    case BTC_SPP_ACT_UNINIT:
        btc_spp_uninit();
        break;
    case BTC_SPP_ACT_START_DISCOVERY:
        btc_spp_start_discovery(arg);
        break;
    case BTC_SPP_ACT_CONNECT:
        btc_spp_connect(arg);
        break;
    case BTC_SPP_ACT_DISCONNECT:
        btc_spp_disconnect(arg);
        break;
    case BTC_SPP_ACT_START_SRV:
        btc_spp_start_srv(arg);
        break;
    case BTC_SPP_ACT_STOP_SRV:
        btc_spp_stop_srv();
        break;
    case BTC_SPP_ACT_WRITE:
        btc_spp_write(arg);
        break;
    default:
        BTC_TRACE_ERROR("%s: Unhandled event (%d)!\n", __FUNCTION__, msg->act);
        break;
    }
    btc_spp_arg_deep_free(msg);
}

void btc_spp_cb_handler(btc_msg_t *msg)
{
    esp_spp_cb_param_t param;
    tBTA_JV *p_data = (tBTA_JV *)msg->arg;
    spp_slot_t *slot;
    switch (msg->act) {
    case BTA_JV_ENABLE_EVT:
        param.init.status = p_data->status;
        btc_spp_cb_to_app(ESP_SPP_INIT_EVT, &param);
        break;
    case BTA_JV_DISCOVERY_COMP_EVT:
        param.disc_comp.status = p_data->disc_comp.status;
        param.disc_comp.scn_num = p_data->disc_comp.scn_num;
        memcpy(param.disc_comp.scn, p_data->disc_comp.scn, p_data->disc_comp.scn_num);
        btc_spp_cb_to_app(ESP_SPP_DISCOVERY_COMP_EVT, &param);
        break;
    case BTA_JV_RFCOMM_CL_INIT_EVT:
        param.cl_init.status = p_data->rfc_cl_init.status;
        param.cl_init.handle = p_data->rfc_cl_init.handle;
        param.cl_init.sec_id = p_data->rfc_cl_init.sec_id;
        param.cl_init.use_co = p_data->rfc_cl_init.use_co;
        btc_spp_cb_to_app(ESP_SPP_CL_INIT_EVT, &param);
        break;
    case BTA_JV_RFCOMM_OPEN_EVT:
        do {
            if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
                osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
                slot = spp_find_slot_by_handle(p_data->rfc_open.handle);
                if (!slot) {
                    BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                    param.open.status = ESP_SPP_NO_CONNECTION;
                    break;
                }
                param.open.fd = slot->fd;
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            }
            param.open.status = p_data->rfc_open.status;
        } while (0);
        param.open.handle = p_data->rfc_open.handle;
        memcpy(param.open.rem_bda, p_data->rfc_open.rem_bda, ESP_BD_ADDR_LEN);
        btc_spp_cb_to_app(ESP_SPP_OPEN_EVT, &param);
        break;
    case BTA_JV_RFCOMM_START_EVT:
        param.start.status = p_data->rfc_start.status;
        param.start.handle = p_data->rfc_start.handle;
        param.start.sec_id = p_data->rfc_start.sec_id;
        param.start.use_co = p_data->rfc_start.use_co;
        btc_spp_cb_to_app(ESP_SPP_START_EVT, &param);
        break;
    case BTA_JV_RFCOMM_SRV_OPEN_EVT:
        if (p_data->rfc_srv_open.handle) {
            do {
                if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
                    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
                    slot = spp_find_slot_by_handle(p_data->rfc_srv_open.handle);
                    if (!slot) {
                        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                        param.srv_open.status = ESP_SPP_NO_CONNECTION;
                        break;
                    }
                    param.srv_open.fd = slot->fd;
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                }
                param.srv_open.status = p_data->rfc_srv_open.status;
            } while (0);
            param.srv_open.handle = p_data->rfc_srv_open.handle;
            param.srv_open.new_listen_handle = p_data->rfc_srv_open.new_listen_handle;
            memcpy(param.srv_open.rem_bda, p_data->rfc_srv_open.rem_bda, ESP_BD_ADDR_LEN);
            btc_spp_cb_to_app(ESP_SPP_SRV_OPEN_EVT, &param);
        }
        break;
    case BTA_JV_RFCOMM_WRITE_EVT:
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = spp_find_slot_by_handle(p_data->rfc_write.handle);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        }
        if (spp_local_param.spp_mode == ESP_SPP_MODE_CB){
            param.write.status = slot ? p_data->rfc_write.status : ESP_SPP_NO_CONNECTION;
            param.write.handle = p_data->rfc_write.handle;
            param.write.len = p_data->rfc_write.len;
            param.write.cong = p_data->rfc_write.cong;
            btc_spp_cb_to_app(ESP_SPP_WRITE_EVT, &param);
            if (slot) {
                list_remove(slot->list, list_front(slot->list));
            }
        } else {
            if (slot) {
                if (p_data->rfc_write.status != BTA_JV_SUCCESS) {
                    if (slot->write_data != NULL) {
                        vRingbufferReturnItem(slot->ringbuf_write, slot->write_data);
                        slot->write_data = NULL;
                    }
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                    break;
                }
                if (p_data->rfc_write.cong == 0) {
                    if (slot->write_data != NULL) {
                        vRingbufferReturnItem(slot->ringbuf_write, slot->write_data);
                        slot->write_data = NULL;
                    }
                    size_t item_size = 0;
                    uint8_t *data = xRingbufferReceiveUpTo(slot->ringbuf_write, &item_size, 0, BTA_JV_DEF_RFC_MTU);
                    if (item_size != 0) {
                        slot->write_data = data;
                        BTA_JvRfcommWrite(slot->rfc_handle, slot->id, item_size, data);
                    }
                }
            }
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        break;
    case BTA_JV_RFCOMM_CLOSE_EVT:
        param.close.status = p_data->rfc_close.status;
        param.close.port_status = p_data->rfc_close.port_status;
        param.close.handle = p_data->rfc_close.handle;
        param.close.async = p_data->rfc_close.async;
        btc_spp_cb_to_app(ESP_SPP_CLOSE_EVT, &param);
        spp_free_slot((spp_slot_t *)p_data->rfc_close.slot);
        break;
    case BTA_JV_RFCOMM_CONG_EVT:
        if (spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
            param.cong.status = p_data->rfc_cong.status;
            param.cong.handle = p_data->rfc_cong.handle;
            param.cong.cong = p_data->rfc_cong.cong;
            btc_spp_cb_to_app(ESP_SPP_CONG_EVT, &param);
        } else {
            osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            slot = spp_find_slot_by_handle(p_data->rfc_cong.handle);
            if (!slot) {
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                break;
            }
            if (p_data->rfc_cong.cong == 0) {
                if (slot->write_data != NULL){
                    vRingbufferReturnItem(slot->ringbuf_write,slot->write_data);
                    slot->write_data = NULL;
                }
                size_t item_size = 0;
                uint8_t *data = xRingbufferReceiveUpTo(slot->ringbuf_write, &item_size, 0, BTA_JV_DEF_RFC_MTU);
                if (item_size != 0){
                    slot->write_data = data;
                    BTA_JvRfcommWrite(slot->rfc_handle, slot->id, item_size, data);
                }
            }
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        }
        break;
    case BTA_JV_RFCOMM_DATA_IND_EVT:
        do {
            uint8_t serial;
            BT_HDR *p_buf;
            UINT16 count = 0;

            osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            slot = spp_find_slot_by_handle(p_data->data_ind.handle);
            if (slot) {
                serial = slot->serial;
            }
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);

            if (!slot) {
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                break;
            }

            while (1) {
                // get incoming_data from slot incoming list
                osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
                if ((slot = spp_local_param.spp_slots[serial]) != NULL && slot->rfc_handle == p_data->data_ind.handle && !list_is_empty(slot->incoming_list)) {
                    p_buf = list_front(slot->incoming_list);
                    list_delete(slot->incoming_list, p_buf);
                } else {
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                    break;
                }
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);

                // invoke callback
                if (p_buf) {
                    count += 1;
                    param.data_ind.status = ESP_SPP_SUCCESS;
                    param.data_ind.handle = p_data->data_ind.handle;
                    param.data_ind.len = p_buf->len;
                    param.data_ind.data = p_buf->data + p_buf->offset;
                    btc_spp_cb_to_app(ESP_SPP_DATA_IND_EVT, &param);
                    BTC_TRACE_DEBUG("data cb to app: len %d\n", p_buf->len);
                    osi_free(p_buf);
                }
            }
            if (count != 0) {
                BTA_JvRfcommFlowControl(p_data->data_ind.handle, count);
                BTC_TRACE_DEBUG("%s give credits:%d\n", __func__, count);
            }
        } while (0);
        break;
    case BTA_JV_FREE_SCN_EVT:
        if (p_data->free_scn.server_status == BTA_JV_SERVER_RUNNING) {
            param.srv_stop.status = p_data->free_scn.status;
            btc_spp_cb_to_app(ESP_SPP_SRV_STOP_EVT, &param);
        }
        break;
    case BTA_JV_DISABLE_EVT:
        param.uninit.status = ESP_SPP_SUCCESS;
        BTA_JvFree();
        osi_mutex_free(&spp_local_param.spp_slot_mutex);
#if SPP_DYNAMIC_MEMORY == TRUE
        osi_free(spp_local_param_ptr);
        spp_local_param_ptr = NULL;
#endif
        btc_spp_cb_to_app(ESP_SPP_UNINIT_EVT, &param);
        break;
    default:
        BTC_TRACE_DEBUG("%s: Unhandled event (%d)!", __FUNCTION__, msg->act);
        break;
    }

}

int bta_co_rfc_data_incoming(void *user_data, BT_HDR *p_buf)
{
    int ret = 1;
    bt_status_t status;
    tBTA_JV p_data;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_SPP;
    msg.act = BTA_JV_RFCOMM_DATA_IND_EVT;

    uint32_t id = (uintptr_t)user_data;
    if (!is_spp_init()) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        return -1;
    }
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_find_slot_by_id(id);
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return -1;
    }
    p_data.data_ind.handle = slot->rfc_handle;
    p_data.data_ind.p_buf = NULL;

    if (spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
        bool incoming_list_empty = list_is_empty(slot->incoming_list);
        list_append(slot->incoming_list, p_buf);
        if (incoming_list_empty) {
            BTC_TRACE_DEBUG("%s data post! %d, %d", __func__, slot->rfc_handle, list_length(slot->incoming_list));
            status = btc_transfer_context(&msg, &p_data, sizeof(tBTA_JV), NULL);
            assert(status == BT_STATUS_SUCCESS);
        } else if (list_length(slot->incoming_list) > 2) {
            BTC_TRACE_ERROR("%s data post stop! %d %d", __func__, slot->rfc_handle, list_length(slot->incoming_list));
            ret = 0;
        }
    } else {
        list_append(slot->incoming_list, p_buf);
        if (list_length(slot->incoming_list) > 2) {
            BTC_TRACE_ERROR("%s data post stop! %d %d", __func__, slot->rfc_handle, list_length(slot->incoming_list));
            ret = 0;
        }
    }
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    return ret;
}

int bta_co_rfc_data_outgoing_size(void *user_data, int *size)
{
    return 1;
}

int bta_co_rfc_data_outgoing(void *user_data, uint8_t *buf, uint16_t size)
{
    return 1;
}


static ssize_t spp_vfs_write(int fd, const void * data, size_t size)
{
    if (!is_spp_init()) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        return -1;
    }
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return -1;
    }
    BaseType_t done = xRingbufferSend(slot->ringbuf_write, (void *)data, size, 0);
    esp_spp_write(slot->rfc_handle, 0, NULL);
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    if (done){
        return size;
    } else {
        return 0;
    }

}
static int spp_vfs_close(int fd)
{
    if (!is_spp_init()) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        return -1;
    }
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return -1;
    }
    esp_spp_disconnect(slot->rfc_handle);
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    return 0;
}

static ssize_t spp_vfs_read(int fd, void * dst, size_t size)
{
    if (!is_spp_init()) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        return -1;
    }
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return -1;
    }
    ssize_t item_size = 0;
    uint16_t count = 0;
    while (!list_is_empty(slot->incoming_list) && size > 0) {
        BT_HDR *p_buf = list_front(slot->incoming_list);
        if (p_buf->len <= size) {
            memcpy(dst, p_buf->data + p_buf->offset, p_buf->len);
            size -= p_buf->len;
            item_size += p_buf->len;
            dst += p_buf->len;
            list_remove(slot->incoming_list, p_buf);
            count++;
        } else {
            memcpy(dst, p_buf->data + p_buf->offset, size);
            item_size += size;
            p_buf->offset += size;
            p_buf->len -= size;
            size = 0;
        }
    }
    if (count > 0) {
        BTA_JvRfcommFlowControl(slot->rfc_handle, count);
        BTC_TRACE_DEBUG("%s give credits:%d\n", __func__, count);
    }
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    return item_size;
}

esp_err_t btc_spp_vfs_register(void)
{
    esp_vfs_t vfs = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = spp_vfs_write,
        .open = NULL,
        .fstat = NULL,
        .close = spp_vfs_close,
        .read = spp_vfs_read,
        .fcntl = NULL
    };

    // No FD range is registered here: spp_vfs_id is used to register/unregister
    // file descriptors
    if (esp_vfs_register_with_id(&vfs, NULL, &spp_local_param.spp_vfs_id) != ESP_OK) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#endif ///defined BTC_SPP_INCLUDED && BTC_SPP_INCLUDED == TRUE
