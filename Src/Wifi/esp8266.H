/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: ESP8266驱动程序
* 开发人员: winter han
* 开发时间: 2016.05.04
* 软件版本: V1.0.3
************************************************************************************************************************
*/
#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "wifi_usr_config.h"

typedef enum 
{
	WIFI_FALSE,
	WIFI_TRUE = !WIFI_FALSE
}e_wifi_bool;

#define AT_CMD_LEN_MAX			15
#define AT_WIFI_DATA_MAX			492//3072

#define AT_WIFI_VERSION_LEN		10
//#define AT_WIFI_AP_NUM				3
#define AT_WIFI_AP_DATA_LEN		100

#define AT_WIFI_AP_NAME_LEN_MAX	32
#define AT_WIFI_AP_CODE_LEN_MAX	64
#define AT_WIFI_AP_MAC_NUM		6

#define AT_CMD_STR_BUFFER_LEN		120

typedef enum
{
	AT_CMD_NULL=0,	
	AT_CMD_TEST,		//测试通信
	AT_CMD_RST,		//重启模块
	AT_CMD_GMR,		//查看版本
	AT_CMD_CWMODE,	//设置WIFI工作模式
						// 1-station;2-ap;3-station&ap
	AT_CMD_CWLAP,		//列出当前可用的AP
	AT_CMD_CWJAP,		//加入AP
	AT_CMD_CWQAP,	//退出与AP的连接
	AT_CMD_CWAUTOCONN,		//自动连接使能
	AT_CMD_CIPSTAMAC,		//中断MAC地址查询
	AT_CMD_NUM,
}AtCmdId;
#define AT_CMD_STR_NULL	NULL
#define AT_CMD_STR_TEST	"AT"
#define AT_CMD_STR_RST		"AT+RST"
#define AT_CMD_STR_GMR	"AT+GMR"
#define AT_CMD_STR_CWMODE	"AT+CWMODE"
#define AT_CMD_STR_CWLAP	"AT+CWLAP"
#define AT_CMD_STR_CWJAP	"AT+CWJAP"
#define AT_CMD_STR_CWQAP	"AT+CWQAP"
#define AT_CMD_STR_CWAUTOCONN	"AT+CWAUTOCONN"
#define AT_CMD_STR_CIPSTAMAC	"AT+CIPSTAMAC"

#define WIFI_POWER_ON_STR		"ready"


#define AT_CMD_ACK_CWLAP	"+CWLAP:"


typedef struct
{
	unsigned char *at_cmdName;
	unsigned char at_cmdLen;
	unsigned char( *cmddata_rev_test)(unsigned char *strdata);
	unsigned char( *cmddata_rev_query)(unsigned char *strdata);
	unsigned char( *cmddata_rev_execute)(unsigned char *strdata);
	unsigned char( *cmddata_rev_set)(unsigned char *strdata);
	
}AtCmdMessge_st;

typedef enum
{
	AT_FLAG_NULL=0,	
	AT_FLAG_OVER,		//结束
	AT_FLAG_OK,		//成功
	AT_FLAG_ERROR,	//错误
	AT_FLAG_READY,		//预备
	AT_FLAG_FAIL,		//失败
	AT_FLAG_BUSY,		//繁忙
	AT_FLAG_NUM
}AtFlagId;

typedef enum
{
	AT_RESUALT_NULL,	
	AT_RESUALT_FAIL,
	AT_RESUALT_SUCCESS,
	AT_RESUALT_WAIT
}At_Resualt;

typedef enum
{
	WIFI_MODE_STA = 1,	
	WIFI_MODE_AP,
	WIFI_MODE_APSTA,	
}WifiModuleMode;

typedef enum
{
	AP_AUTO_CLOSE,	
	AP_AUTO_OPEN
}AP_AutoConnStat;

typedef struct
{
	unsigned char *at_FlagName;
	unsigned char at_FlagLen;
}AtCommonFlag_st;

typedef struct 
{
	unsigned short int size;
	unsigned char buff[AT_WIFI_DATA_MAX];
}AtData_st;

typedef struct
{
	unsigned char At_data_len;
	unsigned char At_data[AT_CMD_STR_BUFFER_LEN];
}AtCmdData_st;

typedef struct
{
	AtCmdId 			AtCmd_Id;
	AtCmdData_st		AtCmd_Data;
}AtCmd_st;

typedef struct
{
	 unsigned char ssid[AT_WIFI_AP_NAME_LEN_MAX];      //AP名称,SSID是区分大小写的文本字符串，是一个最大长度不超过32个字符的字母数字字符（字母或数字）的顺序
	 unsigned char mac_address[AT_WIFI_AP_MAC_NUM];     //MAC地址,0x01 0x23 0x45 0x89 0xAB表示MAC地址为： 01:23:45:67:89:AB
	 signed char signal_strength;     //信号强度,单位：dBm
	 signed int age;        //检测wifi热点至请求定位时间,单位：毫秒
	 signed char channel;       //通道
	 signed char signal_to_noise_ratio;    //信号噪声比,单位：dB
}AccessPointSt;

typedef struct
{
	AP_AutoConnStat ap_autoconn_stat;
	WifiModuleMode wifi_module_mode;
	unsigned char wifi_module_mac[AT_WIFI_AP_MAC_NUM];
	unsigned char at_version[AT_WIFI_VERSION_LEN];
	AccessPointSt	ap_info[AT_WIFI_AP_NUM];
}AtWifiInfo_st;

typedef struct
{
	AP_AutoConnStat ap_autoconn_stat;
	WifiModuleMode wifi_module_mode;
	unsigned char	ap_num_had;
	unsigned char	ap_stren_id_min;
	signed char	ap_stren_val_min;
}AtApAssist_st;

void wifi_PowerOn(void);
void wifi_PowerOff(void);

void wifi_ClearAtData(AtData_st *atdata_rev);

void wifi_AtCmd_Send(AtCmd_st *atcmd_data);
At_Resualt wifi_AtData_Rev(AtData_st *atdata_rev);

e_wifi_bool wifi_module_mode_inquiry(unsigned char *mode);
e_wifi_bool wifi_module_autoconn_inquiry(unsigned char *stat);
e_wifi_bool wifi_module_version_inquiry(unsigned char *version);
e_wifi_bool wifi_module_macadr_inquiry(unsigned char *version);

unsigned char wifi_module_mode_preset(unsigned char mode);
unsigned char wifi_module_autoconn_preset(unsigned char stat);

#endif
