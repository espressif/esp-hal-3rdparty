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

#include "btm_ble_api.h"
#include "btc_gattc.h"
#include "btc_gatt_util.h"
#include "btc_manage.h"
#include "bta_gatt_api.h"
#include "bt_trace.h"
#include "esp_gattc_api.h"

#define BTC_GATTC_CB_TO_APP(_event, _param)    ((esp_profile_cb_t )btc_profile_cb_get(BTC_PID_GATTC))(_event, _param)

// TODO: to be finished, used in deep-copying data from lower layer
static void btc_gattc_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    tBTA_GATTC *p_dest_data = (tBTA_GATTC*) p_dest;
    tBTA_GATTC *p_src_data = (tBTA_GATTC*) p_src;

    if (!p_src_data || !p_dest_data)
       return;

    // Allocate buffer for request data if necessary
    switch (msg->act) {
    default:
        break;
    }
}

// TODO: to be finished, used to free deep-copied data allocated using btc_gattc_copy_req_data()
static void btc_gattc_free_req_data(btc_msg_t *msg)
{
    return;
}

static void btc_gattc_cback(tBTA_GATTC_EVT event, tBTA_GATTC *p_data)
{
	bt_status_t ret;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GATTC;
    msg.act = (uint8_t) event;
    LOG_ERROR("the gattc event = %x\n",event);	
    ret = btc_transfer_context(&msg, p_data, sizeof(tBTA_GATTC), btc_gattc_copy_req_data);

	if (ret)
		LOG_ERROR("%s transfer failed\n", __func__);
}

static void btc_gattc_app_register(btc_ble_gattc_args_t *arg)
{
    tBT_UUID app_uuid;
    app_uuid.len = 2;
    app_uuid.uu.uuid16 = arg->app_id;
    BTA_GATTC_AppRegister(&app_uuid, btc_gattc_cback);
}

static void btc_gattc_app_unregister(btc_ble_gattc_args_t *arg)
{
    BTA_GATTC_AppDeregister(arg->gatt_if);
}

static void btc_gattc_open(btc_ble_gattc_args_t *arg)
{
    tBTA_GATT_TRANSPORT transport = BTA_GATT_TRANSPORT_LE;
    BTA_GATTC_Open(arg->gatt_if, arg->remote_bda, arg->is_direct, transport);
}

static void btc_gattc_close(btc_ble_gattc_args_t *arg)
{
    // TODO; Review this call of BTA_API, check the usage of BTA_GATTC_CancelOpen
    BTA_GATTC_Close(arg->conn_id);
}

static void btc_gattc_cfg_mtu(btc_ble_gattc_args_t *arg)
{
    BTA_GATTC_ConfigureMTU (arg->conn_id, arg->mtu);
}

static void btc_gattc_search_service(btc_ble_gattc_args_t *arg)
{
    tBT_UUID *srvc_uuid = (tBT_UUID *)(&arg->uuid);
    BTA_GATTC_ServiceSearchRequest(arg->conn_id, srvc_uuid);
}

static void btc_gattc_get_first_char(btc_ble_gattc_args_t *arg)
{
    esp_gatt_id_t char_id;
    tBTA_GATT_STATUS status;
    tBTA_GATTC_CHAR_ID out_char_id;
    tBTA_GATT_CHAR_PROP out_char_prop;
	tBTA_GATT_SRVC_ID srvc_id;
    esp_ble_gattc_cb_param_t param;
	

    btc_to_bta_srvc_id(&srvc_id, &arg->service_id);
    status = BTA_GATTC_GetFirstChar(arg->conn_id, &srvc_id, NULL,
                                    &out_char_id, &out_char_prop);
    if (status == 0) {
	bta_to_btc_gatt_id(&char_id, &out_char_id.char_id);
    }

    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));
    param.get_char.conn_id = arg->conn_id;
    param.get_char.status = status;
    memcpy(&param.get_char.srvc_id, &arg->service_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&param.get_char.char_id, &char_id, sizeof(esp_gatt_id_t));
    param.get_char.char_prop = out_char_prop;
    BTC_GATTC_CB_TO_APP(ESP_GATTC_GET_CHAR_EVT, &param);
}

static void btc_gattc_get_next_char(btc_ble_gattc_args_t *arg)
{
    esp_gatt_id_t char_id;
    tBTA_GATT_STATUS status;
    tBTA_GATTC_CHAR_ID in_char_id;
    tBTA_GATTC_CHAR_ID out_char_id;
    tBTA_GATT_CHAR_PROP out_char_prop;
    esp_ble_gattc_cb_param_t param;
    btc_to_bta_srvc_id(&in_char_id.srvc_id, &arg->service_id);
    btc_to_bta_gatt_id(&in_char_id.char_id, &arg->char_id);

    status = BTA_GATTC_GetNextChar(arg->conn_id, &in_char_id, NULL,
                          &out_char_id, &out_char_prop);

    if (status == 0) {
		bta_to_btc_gatt_id(&char_id, &out_char_id.char_id);
    }
    
    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));
    param.get_char.conn_id = arg->conn_id;
    param.get_char.status = status;
    memcpy(&param.get_char.srvc_id, &arg->service_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&param.get_char.char_id, &char_id, sizeof(esp_gatt_id_t));
    param.get_char.char_prop = out_char_prop;
    BTC_GATTC_CB_TO_APP(ESP_GATTC_GET_CHAR_EVT, &param);
}

static void btc_gattc_get_first_descr(btc_ble_gattc_args_t *arg)
{
    esp_gatt_id_t descr_id;
    tBTA_GATT_STATUS status;
    tBTA_GATTC_CHAR_ID in_char_id;
    tBTA_GATTC_CHAR_DESCR_ID out_char_descr_id;
    esp_ble_gattc_cb_param_t param;
    
    btc_to_bta_srvc_id(&in_char_id.srvc_id, &arg->service_id);
    btc_to_bta_gatt_id(&in_char_id.char_id, &arg->char_id);
    
    status = BTA_GATTC_GetFirstCharDescr(arg->conn_id, &in_char_id, NULL,
					 &out_char_descr_id);

    if (status == 0) {
	bta_to_btc_gatt_id(&descr_id, &out_char_descr_id.descr_id);
    }

    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));
    param.get_descr.conn_id = arg->conn_id;
    param.get_descr.status = status;
    memcpy(&param.get_descr.srvc_id, &arg->service_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&param.get_descr.char_id, &arg->char_id, sizeof(esp_gatt_id_t));
    memcpy(&param.get_descr.descr_id, &descr_id, sizeof(esp_gatt_id_t));
    BTC_GATTC_CB_TO_APP(ESP_GATTC_GET_DESCR_EVT, &param);
}

static void btc_gattc_get_next_descr(btc_ble_gattc_args_t *arg)
{
    esp_gatt_id_t descr_id;
    tBTA_GATT_STATUS status;
    tBTA_GATTC_CHAR_DESCR_ID in_char_descr_id;
    tBTA_GATTC_CHAR_DESCR_ID out_char_descr_id;
    esp_ble_gattc_cb_param_t param;
    
    btc_to_bta_srvc_id(&in_char_descr_id.char_id.srvc_id, &arg->service_id);
    btc_to_bta_gatt_id(&in_char_descr_id.char_id.char_id, &arg->char_id);
    btc_to_bta_gatt_id(&in_char_descr_id.descr_id, &arg->descr_id);
    
    status = BTA_GATTC_GetNextCharDescr(arg->conn_id, &in_char_descr_id
					, NULL, &out_char_descr_id);
    if (status == 0) {
	bta_to_btc_gatt_id(&descr_id, &out_char_descr_id.descr_id);
    }

    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));
    param.get_descr.conn_id = arg->conn_id;
    param.get_descr.status = status;
    memcpy(&param.get_descr.srvc_id, &arg->service_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&param.get_descr.char_id, &arg->char_id, sizeof(esp_gatt_id_t));
    memcpy(&param.get_descr.descr_id, &descr_id, sizeof(esp_gatt_id_t));
    BTC_GATTC_CB_TO_APP(ESP_GATTC_GET_DESCR_EVT, &param);
}

static void btc_gattc_get_first_incl_service(btc_ble_gattc_args_t *arg)
{
    esp_gatt_srvc_id_t incl_srvc_id;
    tBTA_GATT_STATUS status;
    tBTA_GATT_SRVC_ID srvc_id;
    tBTA_GATTC_INCL_SVC_ID out_incl_svc_id;
    esp_ble_gattc_cb_param_t param;
    
    btc_to_bta_srvc_id(&srvc_id, &arg->service_id);

    status = BTA_GATTC_GetFirstIncludedService(arg->conn_id, &srvc_id,
					       NULL, &out_incl_svc_id);

    bta_to_btc_srvc_id(&incl_srvc_id, &out_incl_svc_id.incl_svc_id);

    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));
    param.get_incl_srvc.conn_id = arg->conn_id;
    param.get_incl_srvc.status = status;
    memcpy(&param.get_incl_srvc.srvc_id, &arg->service_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&param.get_incl_srvc.incl_srvc_id, &incl_srvc_id, sizeof(esp_gatt_srvc_id_t));
    BTC_GATTC_CB_TO_APP(ESP_GATTC_GET_INCL_SRVC_EVT, &param);
}

static void btc_gattc_get_next_incl_service(btc_ble_gattc_args_t *arg)
{
    esp_gatt_srvc_id_t incl_srvc_id;
    tBTA_GATT_STATUS status;
    tBTA_GATTC_INCL_SVC_ID in_incl_svc_id;
    tBTA_GATTC_INCL_SVC_ID out_incl_svc_id;
    esp_ble_gattc_cb_param_t param;
    
    btc_to_bta_srvc_id(&in_incl_svc_id.srvc_id, &arg->service_id);
    btc_to_bta_srvc_id(&in_incl_svc_id.incl_svc_id, &arg->start_service_id);

    status = BTA_GATTC_GetNextIncludedService(arg->conn_id, &in_incl_svc_id,
					      NULL, &out_incl_svc_id);

    bta_to_btc_srvc_id(&incl_srvc_id, &out_incl_svc_id.incl_svc_id);

    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));
    param.get_incl_srvc.conn_id = arg->conn_id;
    param.get_incl_srvc.status = status;
    memcpy(&param.get_incl_srvc.srvc_id, &arg->service_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&param.get_incl_srvc.incl_srvc_id, &incl_srvc_id, sizeof(esp_gatt_srvc_id_t));
    BTC_GATTC_CB_TO_APP(ESP_GATTC_GET_INCL_SRVC_EVT, &param);
}

static void btc_gattc_read_char(btc_ble_gattc_args_t *arg)
{
    tBTA_GATTC_CHAR_ID in_char_id;
    btc_to_bta_srvc_id(&in_char_id.srvc_id, &arg->service_id);
    btc_to_bta_gatt_id(&in_char_id.char_id, &arg->char_id);
    BTA_GATTC_ReadCharacteristic(arg->conn_id, &in_char_id, arg->auth_req);
}

static void btc_gattc_read_char_descr(btc_ble_gattc_args_t *arg)
{
    tBTA_GATTC_CHAR_DESCR_ID in_char_descr_id;
    btc_to_bta_srvc_id(&in_char_descr_id.char_id.srvc_id, &arg->service_id);
    btc_to_bta_gatt_id(&in_char_descr_id.char_id.char_id, &arg->char_id);
    btc_to_bta_gatt_id(&in_char_descr_id.descr_id, &arg->descr_id);
    
    BTA_GATTC_ReadCharDescr(arg->conn_id, &in_char_descr_id, arg->auth_req);
}

static void btc_gattc_write_char(btc_ble_gattc_args_t *arg)
{
    //TODO: check the write type
    tBTA_GATTC_CHAR_ID in_char_id;
    tBTA_GATTC_WRITE_TYPE write_type = BTA_GATTC_TYPE_WRITE;
 
    btc_to_bta_srvc_id(&in_char_id.srvc_id, &arg->service_id);
    btc_to_bta_gatt_id(&in_char_id.char_id, &arg->char_id);

    BTA_GATTC_WriteCharValue(arg->conn_id, &in_char_id,
                             write_type,
                             arg->value_len,
                             arg->value,
                             arg->auth_req);
}

static void btc_gattc_write_char_descr(btc_ble_gattc_args_t *arg)
{
    //TODO: check the write type
    tBTA_GATTC_CHAR_DESCR_ID in_char_descr_id;
    tBTA_GATT_UNFMT descr_val;
    tBTA_GATTC_WRITE_TYPE write_type = BTA_GATTC_TYPE_WRITE;
    btc_to_bta_srvc_id(&in_char_descr_id.char_id.srvc_id, &arg->service_id);
    btc_to_bta_gatt_id(&in_char_descr_id.char_id.char_id, &arg->char_id);
    btc_to_bta_gatt_id(&in_char_descr_id.descr_id, &arg->descr_id);

    descr_val.len = arg->value_len;
    descr_val.p_value = arg->value;

    BTA_GATTC_WriteCharDescr(arg->conn_id, &in_char_descr_id,
                             write_type, &descr_val,
                             arg->auth_req);
}

static void btc_gattc_prepare_write(btc_ble_gattc_args_t *arg)
{
    tBTA_GATTC_CHAR_ID in_char_id;
    btc_to_bta_srvc_id(&in_char_id.srvc_id, &arg->service_id);
    btc_to_bta_gatt_id(&in_char_id.char_id, &arg->char_id);

    BTA_GATTC_PrepareWrite(arg->conn_id, &in_char_id, arg->offset, arg->value_len,
			   arg->value, arg->auth_req);
}

static void btc_gattc_execute_wrtie(btc_ble_gattc_args_t *arg)
{
    BTA_GATTC_ExecuteWrite(arg->conn_id, arg->is_execute);
}

static void btc_gattc_reg_for_notify(btc_ble_gattc_args_t *arg)
{
    tBTA_GATT_STATUS status;
    tBTA_GATTC_CHAR_ID in_char_id;
    esp_ble_gattc_cb_param_t param;
    
    btc_to_bta_srvc_id(&in_char_id.srvc_id, &arg->service_id);
    btc_to_bta_gatt_id(&in_char_id.char_id, &arg->char_id);
            
    status = BTA_GATTC_RegisterForNotifications(arg->gatt_if,
                                                arg->remote_bda, &in_char_id);

    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));
    param.reg_for_notif.conn_id = arg->conn_id;
    param.reg_for_notif.status = status;
    param.reg_for_notif.registered = true;
    memcpy(&param.reg_for_notif.srvc_id, &arg->service_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&param.reg_for_notif.char_id, &arg->service_id, sizeof(esp_gatt_id_t));
    BTC_GATTC_CB_TO_APP(ESP_GATTC_REG_FOR_NOTIF_EVT, &param);
}

static void btc_gattc_unreg_for_notify(btc_ble_gattc_args_t *arg)
{
    tBTA_GATT_STATUS status;
    tBTA_GATTC_CHAR_ID in_char_id;
    esp_ble_gattc_cb_param_t param;
    
    btc_to_bta_srvc_id(&in_char_id.srvc_id, &arg->service_id);
    btc_to_bta_gatt_id(&in_char_id.char_id, &arg->char_id);
            
    status = BTA_GATTC_DeregisterForNotifications(arg->gatt_if,
                                                  arg->remote_bda, &in_char_id);
    
    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));
    param.reg_for_notif.conn_id = arg->conn_id;
    param.reg_for_notif.status = status;
    param.reg_for_notif.registered = false;
    memcpy(&param.reg_for_notif.srvc_id, &arg->service_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&param.reg_for_notif.char_id, &arg->service_id, sizeof(esp_gatt_id_t));
    BTC_GATTC_CB_TO_APP(ESP_GATTC_REG_FOR_NOTIF_EVT, &param);
}

void btc_gattc_call_handler(btc_msg_t *msg)
{
    btc_ble_gattc_args_t *arg = (btc_ble_gattc_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_GATTC_ACT_APP_REGISTER:
        btc_gattc_app_register(arg);
        break;
    case BTC_GATTC_ACT_APP_UNREGISTER:
        btc_gattc_app_unregister(arg);
        break;
    case BTC_GATTC_ACT_OPEN:
        btc_gattc_open(arg);
        break;
    case BTC_GATTC_ACT_CLOSE:
        btc_gattc_close(arg);
        break;
    case BTC_GATTC_ACT_CFG_MTU:
        btc_gattc_cfg_mtu(arg);
        break;
    case BTC_GATTC_ACT_SEARCH_SERVICE:
        btc_gattc_search_service(arg);
        break;
    case BTC_GATTC_ACT_GET_FIRST_CHAR:
        btc_gattc_get_first_char(arg);
        break;
    case BTC_GATTC_ACT_GET_NEXT_CHAR:
        btc_gattc_get_next_char(arg);
        break;
    case BTC_GATTC_ACT_GET_FIRST_DESCR:
        btc_gattc_get_first_descr(arg);
        break;
    case BTC_GATTC_ACT_GET_NEXT_DESCR:
        btc_gattc_get_next_descr(arg);
        break;
    case BTC_GATTC_ACT_GET_FIRST_INCL_SERVICE:
        btc_gattc_get_first_incl_service(arg);
        break;
    case BTC_GATTC_ACT_GET_NEXT_INCL_SERVICE:
        btc_gattc_get_next_incl_service(arg);
        break;
    case BTC_GATTC_ACT_READ_CHAR:
        btc_gattc_read_char(arg);
        break;
    case BTC_GATTC_ACT_READ_CHAR_DESCR:
        btc_gattc_read_char_descr(arg);
        break;
    case BTC_GATTC_ACT_WRITE_CHAR:
        btc_gattc_write_char(arg);
        break;
    case BTC_GATTC_ACT_WRITE_CHAR_DESCR:
        btc_gattc_write_char_descr(arg);
        break;
    case BTC_GATTC_ACT_PREPARE_WRITE:
		btc_gattc_prepare_write(arg);
		break;
    case BTC_GATTC_ACT_EXECUTE_WRITE:
        btc_gattc_execute_wrtie(arg);
        break;
    case BTC_GATTC_ACT_REG_FOR_NOTIFY:
        btc_gattc_reg_for_notify(arg);
        break;
    case BTC_GATTC_ACT_UNREG_FOR_NOTIFY:
        btc_gattc_unreg_for_notify(arg);
        break;
    default:
        LOG_ERROR("%s: Unhandled event (%d)!\n", __FUNCTION__, msg->act);
	break;
	
    }
}

void btc_gattc_cb_handler(btc_msg_t *msg)
{
    tBTA_GATTC *arg = (tBTA_GATTC *)(msg->arg);
    esp_ble_gattc_cb_param_t param;

    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));

	switch (msg->act) {
		case BTA_GATTC_REG_EVT: {
									tBTA_GATTC_REG *reg_oper = &arg->reg_oper;
									param.reg.status = reg_oper->status;
									param.reg.gatt_if = reg_oper->client_if;
									memcpy(&param.reg.uuid, &reg_oper->app_uuid, sizeof(esp_bt_uuid_t));
									BTC_GATTC_CB_TO_APP(ESP_GATTC_REG_EVT, &param);
									break;
								}
		case BTA_GATTC_DEREG_EVT: {
									  BTC_GATTC_CB_TO_APP(ESP_GATTC_UNREG_EVT, NULL);
									  break;
								  }
		case BTA_GATTC_READ_CHAR_EVT: {
										  set_read_value(&param, &arg->read);
										  BTC_GATTC_CB_TO_APP(ESP_GATTC_READ_CHAR_EVT, &param);
										  break;
									  }

		case BTA_GATTC_WRITE_CHAR_EVT:
		case BTA_GATTC_PREP_WRITE_EVT: {
										   tBTA_GATTC_WRITE *write = &arg->write;
										   uint32_t ret_evt = (msg->act == BTA_GATTC_WRITE_CHAR_EVT) ?
											   ESP_GATTC_WRITE_CHAR_EVT : ESP_GATTC_PREP_WRITE_EVT;
										   param.write.conn_id = write->conn_id;
										   param.write.status = write->status;
										   bta_to_btc_srvc_id(&param.write.srvc_id, &write->srvc_id);
										   bta_to_btc_gatt_id(&param.write.char_id, &write->char_id);
										   BTC_GATTC_CB_TO_APP(ret_evt, &param);
										   break;
									   }

		case BTA_GATTC_EXEC_EVT: {
									 tBTA_GATTC_EXEC_CMPL *exec_cmpl = &arg->exec_cmpl;
									 param.exec_cmpl.conn_id = exec_cmpl->conn_id;
									 param.exec_cmpl.status = exec_cmpl->status;
									 BTC_GATTC_CB_TO_APP(ESP_GATTC_EXEC_EVT, &param);
									 break;
								 }

		case BTA_GATTC_SEARCH_CMPL_EVT: {
											tBTA_GATTC_SEARCH_CMPL *search_cmpl = &arg->search_cmpl;
											param.search_cmpl.conn_id = search_cmpl->conn_id;
											param.search_cmpl.status = search_cmpl->status;
											BTC_GATTC_CB_TO_APP(ESP_GATTC_SEARCH_CMPL_EVT, &param);
											break;
										}
		case BTA_GATTC_SEARCH_RES_EVT: {
										   tBTA_GATTC_SRVC_RES *srvc_res = &arg->srvc_res;
										   param.search_res.conn_id = srvc_res->conn_id;
										   memcpy(&param.search_res.service_id, &srvc_res->service_uuid, sizeof(esp_gatt_srvc_id_t));
										   BTC_GATTC_CB_TO_APP(ESP_GATTC_SEARCH_RES_EVT, &param);
										   break;
									   }
		case BTA_GATTC_READ_DESCR_EVT: {
										   set_read_value(&param, &arg->read);
										   BTC_GATTC_CB_TO_APP(ESP_GATTC_READ_DESCR_EVT, &param);
										   break;
									   }
		case BTA_GATTC_WRITE_DESCR_EVT: {
											tBTA_GATTC_WRITE *write = &arg->write;
											param.write.conn_id = write->conn_id;
											param.write.status = write->status;
											bta_to_btc_srvc_id(&param.write.srvc_id, &write->srvc_id);
											bta_to_btc_gatt_id(&param.write.char_id, &write->char_id);
											bta_to_btc_gatt_id(&param.write.descr_id, &write->descr_type);
											BTC_GATTC_CB_TO_APP(ESP_GATTC_WRITE_DESCR_EVT, &param);
											break;
										}
		case BTA_GATTC_NOTIF_EVT: {
									  tBTA_GATTC_NOTIFY *notify = &arg->notify;
									  param.notify.conn_id = notify->conn_id;
									  memcpy(&param.notify.bda, &notify->bda, sizeof(esp_bd_addr_t));
									  bta_to_btc_srvc_id(&param.notify.srvc_id, &notify->char_id.srvc_id);
									  bta_to_btc_gatt_id(&param.notify.char_id, &notify->char_id.char_id);
									  bta_to_btc_gatt_id(&param.notify.descr_id, &notify->descr_type);
									  param.notify.is_notify = (notify->is_notify == TRUE) ? true : false;
									  param.notify.value_len = (notify->len > ESP_GATT_MAX_ATTR_LEN) ? \
															   ESP_GATT_MAX_ATTR_LEN : notify->len;
									  memcpy(&param.notify.value, notify->value, param.notify.value_len);
									  BTC_GATTC_CB_TO_APP(ESP_GATTC_NOTIF_EVT, &param);

									  if (notify->is_notify == FALSE) {
										  BTA_GATTC_SendIndConfirm(notify->conn_id, &notify->char_id);
									  }
									  break;
								  }
		case BTA_GATTC_OPEN_EVT: {
									 tBTA_GATTC_OPEN *open = &arg->open;
									 param.open.status = open->status;
									 param.open.conn_id = open->conn_id;
									 param.open.gatt_if = open->client_if;
									 memcpy(&param.open.remote_bda, &open->remote_bda, sizeof(esp_bd_addr_t));
									 param.open.mtu = open->mtu;
									 BTC_GATTC_CB_TO_APP(ESP_GATTC_OPEN_EVT, &param);
									 break;
								 }
		case BTA_GATTC_CLOSE_EVT: {
									  tBTA_GATTC_CLOSE *close = &arg->close;
									  param.close.status = close->status;
									  param.close.conn_id = close->conn_id;
									  param.close.gatt_if = close->client_if;
									  memcpy(&param.close.remote_bda, &close->remote_bda, sizeof(esp_bd_addr_t));
									  param.close.reason = close->reason;
									  BTC_GATTC_CB_TO_APP(ESP_GATTC_CLOSE_EVT, &param);
									  break;
								  }

		case BTA_GATTC_CFG_MTU_EVT: {
										tBTA_GATTC_CFG_MTU *cfg_mtu = &arg->cfg_mtu;
										param.cfg_mtu.conn_id = cfg_mtu->conn_id;
										param.cfg_mtu.status = cfg_mtu->status;
										param.cfg_mtu.mtu = cfg_mtu->mtu;
										BTC_GATTC_CB_TO_APP(ESP_GATTC_CFG_MTU_EVT, &param);
										break;
									}

		case BTA_GATTC_ACL_EVT: {
									BTC_GATTC_CB_TO_APP(ESP_GATTC_ACL_EVT, NULL);
									break;
								}
		case BTA_GATTC_CANCEL_OPEN_EVT: {
											BTC_GATTC_CB_TO_APP(ESP_GATTC_CANCEL_OPEN_EVT, NULL);
											break;
										}
		case BTA_GATTC_CONGEST_EVT: {
										tBTA_GATTC_CONGEST *congest = &arg->congest;
										param.congest.conn_id = congest->conn_id;
										param.congest.congested = (congest->congested == TRUE) ? true : false;
										BTC_GATTC_CB_TO_APP(ESP_GATTC_CONGEST_EVT, &param);
										break;
									}
		case BTA_GATTC_SRVC_CHG_EVT: {
										 memcpy(&param.srvc_chg.remote_bda, &arg->remote_bda, sizeof(esp_bd_addr_t));
										 BTC_GATTC_CB_TO_APP(ESP_GATTC_SRVC_CHG_EVT, &param);
										 break;
									 }
		default:
									 LOG_ERROR("%s: Unhandled event (%d)!", __FUNCTION__, msg->act);
									 break;
	}

    // free the deep-copied data
    btc_gattc_free_req_data(msg);
}
