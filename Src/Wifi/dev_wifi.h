/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: ESP8266应用程序
* 开发人员: winter han
* 开发时间: 2016.05.04
* 软件版本: V1.0.3
************************************************************************************************************************
*/
#ifndef _DEV_WIFI_H_
#define _DEV_WIFI_H_

#include "esp8266.h"

#define CMD_WITHOUT_DATA_LEN		0

typedef enum
{
	CMD_ID_NULL,
	CMD_ID_READY,
	CMD_ID_CONNECT,
	CMD_ID_VERSION,
	CMD_ID_RST,
	CMD_ID_MODE_GET,
	CMD_ID_MODE_SET,
	CMD_ID_MODE_SET_2_STA,
	CMD_ID_LAP,
	CMD_ID_JAP,
	CMD_ID_QAP,
	CMD_ID_AUTOCONN,
	CMD_ID_STAMAC,
	CMD_ID_NUM
}WifiCmdId;

typedef struct
{
	unsigned char ssid_len;
	unsigned char ssid[AT_WIFI_AP_NAME_LEN_MAX];
	unsigned char code_len;
	unsigned char code[AT_WIFI_AP_CODE_LEN_MAX];
}AP_JoinInfo;

typedef enum
{
	CMD_LIST_IDLE,
	CMD_LIST_WORK,
}CmdListStatus;

typedef enum
{
	ANTI_LOST_OFF,
	ANTI_LOST_ON
}AntiLostStatus;

typedef struct
{
	WifiCmdId wifi_cmd_id;
	AP_AutoConnStat autoconn_stat;
	WifiModuleMode mode;
	AntiLostStatus anti_lost_status;
	AP_JoinInfo AP_join_info;
	At_Resualt rev_flag[CMD_ID_NUM];
	CmdListStatus cmd_list_status;
}WifiCmdParam;

typedef struct{
	unsigned char write_offset;
	unsigned char read_offset;
	unsigned char cmd_queue[CMD_QUEUE_LEN_MAX];
}DataBuffParam;

typedef struct
{
	 unsigned char  u8Len;       //wifi热点数组数据总长度
	 AccessPointSt stAccessPointArray[AT_WIFI_AP_NUM];  //AccessPointSt[x]，扫描到的wifi热点数组，通常情况下需要至少3个热点信息
}WifiAccessPointSt;

WifiCmdParam *wifi_cmd_param_get(void);
/*返回TRUE标示指令执行成功,可以强制结束*/
typedef e_wifi_bool (*pWifiCmdRetHookType)(u8 *, u16) ; 
void app_wifi_init(void);

e_wifi_bool wifi_power_is_on(void);
void app_wifi_start(void);
void app_wifi_end(void);

void app_wifi_cmd_send(void);

void app_wifi_connection_establish(void);
void app_wifi_connection_flag_set(At_Resualt resualt);
At_Resualt app_wifi_connection_flag_get(void);
WifiCmdId app_wifi_cmd_id_get(void);
void app_wifi_cmd_id_clr(void);
void app_wifi_rev_flag_clr(void);
void app_wifi_rev_flag_set(At_Resualt resualt);
void app_wifi_rev_flag_get(At_Resualt *rev_flag);

e_wifi_bool app_module_mode_inquiry(unsigned char *mode);
e_wifi_bool app_module_version_inquiry(unsigned char *version);
e_wifi_bool app_module_autoconn_inquiry(unsigned char *stat);
e_wifi_bool app_module_macadr_inquiry(unsigned char *mac_adr);
e_wifi_bool app_wifi_ap_get(WifiAccessPointSt *stWifiAccessPointInfo);

void app_ap_join_info_fill(AP_JoinInfo *AP_join_info);
void app_anti_lost_status_fill(AntiLostStatus anti_lost_status);

void app_ap_test(void);

void cmd_queue_init(void);	//初始化命令队列
e_wifi_bool cmd_queue_has_new_member(void);//判断命令队列是否有新成员
e_wifi_bool cmd_queue_new_member_join(WifiCmdId cmd_id);//命令队列有新成员加入
e_wifi_bool cmd_queue_member_get_fifo(WifiCmdId *cmd_id);
	//按照先入先出的方式获取队列成员

#endif


