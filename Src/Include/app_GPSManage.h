/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_GPS.c
* Description		: 	GPS application operation center
* Created			: 	2016.08.29.
* Author			: 	Yu Weifeng
* Function List 		: 	RtcTimeInit
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_GPS_MANAGE_H
#define _APP_GPS_MANAGE_H

#include "cBasicTypeDefine.h"
#include "parseNmea.h"
#include "app_Gps.h"

#define GPS_LOCATE_TIME	60

typedef struct GpsInfoSaveInFlash
{
	LocationVectorInfo tGpsInfo;
	u32 dwCrcForFlash;//用于FLASH保存使用
}T_GpsInfoSaveInFlash,*PT_GpsInfoSaveInFlash;

typedef struct GpsManage
{
	char *name;
	void (*GpsOprConfig)(void);
	u8 (*GpsOprSendData)(u8 *i_buf,u16 i_Len);
	u8 (*GpsOprRecvData)(void);
	u8 (*GpsOprGetLocationInfo)(LocationVectorInfo *o_tLocationVectorInfo);
	void(*GpsOprClearBackupLocationInfo)(void);
	u8(*GpsOprPowerOn)(void);
	u8(*GpsOprPowerOff)(void);
	void(*GpsOprGetHistoryPos)(LocationVectorInfo *o_ptLocationVectorInfo);
	struct GpsManage *ptNext;
}T_GpsManage,*PT_GpsManage;

typedef struct GpsOpr
{
	void (*Init)(void);
	void (*Config)(void);
	u8 (*GpsSendData)(u8 *i_buf,u16 i_Len);
	u8 (*GpsRecvData)(void);
	u8(*GpsPowerOn)(void);
	u8(*GpsPowerOff)(void);	
	u8(*GpsClearBackupLocationInfo)(void);
	u8 (*GpsGetLocationInfo)(LocationVectorInfo *o_tLocationVectorInfo);	
	u8 (*GpsGetPosReq)(void);
	u8 (*GpsGetHistoryPos)(LocationVectorInfo *o_ptLocationVectorInfo);
	u8 (*GpsGetPos)(LocationVectorInfo *o_ptLocationVectorInfo);
}T_GpsOpr,*PT_GpsOpr;

extern T_GpsOpr const g_tGPS;
void RegisterGPS(PT_GpsManage i_ptGpsOpr);
void GeneralGpsInit();
void GpsProcessTask(void *pdata);

#endif
