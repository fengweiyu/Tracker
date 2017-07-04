/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_GPS.c
* Description		: 	app_GPS application operation center
* Created			: 	2016.09.02.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_GPS_H
#define _APP_GPS_H
#include "cBasicTypeDefine.h"
#include "Myqueue.h"
#include "main.h"
#define GPS_USART_BAUDRATE  			9600
#define GPS_USART_RECV_SIZE  			88
#define USART1_USART_RECV_SIZE   		1

#define GPS_RX_EVENT_TAB_SIZE 		(3)
#define GPS_RX_PEND_TIME_MS			(1000)
typedef struct GeneralGpsDevManage
{
	char *name;
	void (*GeneralGpsConfigDev)(void);
	u8 (*GeneralGpsSendDataDev)(u8 *i_buf,u16 i_Len);
	u8 (*GeneralGpsRecvDataDev)(void);
	u8 (*GeneralGpsGetDataDev)(u8 *o_buf,u16 *o_Len);
	void(*GeneralGpsPowerOnDev)(void);
	void(*GeneralGpsPowerOffDev)(void);
	struct GeneralGpsDevManage *ptNext;
}T_GeneralGpsDevManage,*PT_GeneralGpsDevManage;

void RegisterGeneralGpsDev(PT_GeneralGpsDevManage i_ptGeneralGpsDev);
void GeneralGpsDevInit();

extern myevent*ptEveTabForGpsRx[GPS_RX_EVENT_TAB_SIZE]; 
u8 GpsRecvDataDev(u8 i_ucGpsData);

#endif
