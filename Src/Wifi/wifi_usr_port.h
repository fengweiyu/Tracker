/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: ESP8266任务程序
* 开发时间: 2016.05.04
* 软件版本: V1.0.3
************************************************************************************************************************
*/
#ifndef _WIFI_USR_PORT_H_
#define _WIFI_USR_PORT_H_

#include "esp8266.h"
#include "Myqueue.h"
#include "GsmDriverTypeDef.h"
#include "Ucos_ii.h"
#include "Dev_wifi.h"
#include "Bsp.h"

#define WIFI_RX_EVENT_TAB_SIZE  10
extern myevent*pstEveTabForWifiRx[WIFI_RX_EVENT_TAB_SIZE]; 
e_wifi_bool app_module_cmd_issue(WifiCmdId cmd_id, unsigned char param);

void app_wifi_cmd_fail_handle(WifiCmdId cmd_id);
void app_wifi_cmd_success_handle(WifiCmdId cmd_id);

void wifi_cmd_timeout_handle(void);

e_wifi_bool app_location_ap_get(WifiAccessPointSt *stWifiAccessPointInfo);
e_wifi_bool app_wifi_mode_get(unsigned char *mode);
e_wifi_bool app_wifi_macadr_get(unsigned char *mac_adr);
e_wifi_bool app_wifi_autoconn_get(unsigned char *stat);
e_wifi_bool app_wifi_version_get(unsigned char *version);

#endif

