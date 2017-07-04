/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_Wifi.c
* Description		: 	app_Wifi application operation center
* Created			: 	2016.09.27.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_WIFI_H
#define _APP_WIFI_H
#include "cBasicTypeDefine.h"
#include "Dev_wifi.h"
#define Wifi_USART_BAUDRATE  115200
#define USART6_USART_RECV_SIZE   1

typedef struct GeneralWifiDevManage
{
	char *name;
	void (*GeneralWifiConfigDev)(void);
	u8 (*GeneralWifiSendDataDev)(u8 *i_buf,u16 i_Len);
	u8 (*GeneralWifiRecvDataDev)(void);
	void(*GeneralWifiPowerOnDev)(void);
	void(*GeneralWifiPowerOffDev)(void);
	struct GeneralWifiDevManage *ptNext;
}T_GeneralWifiDevManage,*PT_GeneralWifiDevManage;
typedef struct WifiOpr
{
	void (*Init)(void);
	void (*Config)(void);
	u8 (*WifiSendData)(u8 *i_buf,u16 i_Len);
	u8 (*WifiRecvData)(void);
	u8(*WifiPowerOn)(void);
	u8(*WifiPowerOff)(void);	
	u8 (*WifiSetScanApReq)(void);	
	u8 (*WifiReadApData)(WifiAccessPointSt *o_ptWifiAccessPoint);
}T_WifiOpr,*PT_WifiOpr;
typedef struct WifiAccessPoint
{
	WifiAccessPointSt tWifiApInfo;
	struct WifiAccessPoint *ptNext;
}T_WifiAccessPoint,*PT_WifiAccessPoint;

void RegisterGeneralWifiDev(PT_GeneralWifiDevManage i_ptGeneralWifiDev);
void GeneralWifiDevInit();

extern T_WifiOpr const g_tWifi; 
extern u8 USART6RecvDataBuffer[USART6_USART_RECV_SIZE];

#endif

