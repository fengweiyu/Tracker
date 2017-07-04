/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: ESP8266任务程序
* 开发时间: 2016.05.04
* 软件版本: V1.0.3
************************************************************************************************************************
*/
#ifndef _APP_WIFI_TASK_H_
#define _APP_WIFI_TASK_H_

#include "esp8266.h"
#include "dev_wifi.h"
#include "wifi_usr_port.h"
#include "main.h"
#include "Bsp.h"

#define WIFI_CONNET_INTERVAL  1
#define WIFI_TASK_CYCLE_MS	50

typedef enum
{
	WIFI_CMD_IDLE,
	WIFI_CMD_BUSY
}e_WifiCmdSendStatus;

typedef enum
{
	WIFI_PREPARE_NULL,
	WIFI_PREPARE_OK
}e_WifiPrepareStatus;

typedef enum
{
	WIFI_TIMER_CONNECT,
	WIFI_TIMER_WORK
}e_WifiTimerId;

typedef struct
{
	e_WifiTimerId id;
	e_wifi_bool validity;
	unsigned char cnt;
}st_WifCmdTimer;

typedef struct
{
	e_WifiCmdSendStatus wifi_cmd_send_status;
	e_WifiPrepareStatus wifi_prepare_status;
	st_WifCmdTimer wifi_cmd_timer;
}st_WifiTaskParam;

void app_wifiTask(void *pdata);
void wifi_cmd_timeout_cnt(void);
st_WifiTaskParam *wifi_task_param_get(void);

#endif

