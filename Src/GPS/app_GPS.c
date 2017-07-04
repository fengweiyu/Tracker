/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_Gps.c
* Description		: 	RTC application operation center
* Created			: 	2016.08.29.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include"cBasicTypeDefine.h"
#include "app_GPS.h"
#include "app_GPSManage.h"
#include "Config.h"
#include "MyQueue.h"
#include "Mymalloc.h"
#include "Bsp.h"
#include "Flash.h"
#include "app_TimeManage.h"

static PT_GeneralGpsDevManage g_ptGeneralGpsDevHead=NULL;
myevent*ptEveTabForGpsRx[GPS_RX_EVENT_TAB_SIZE]; 


static u8 GpsGetDevRxDatPend(u16 i_wPendTimeMs, u8 **o_p2ucDat, u16 *o_pwLen);
static void GeneralGpsLocateInfoSaveInFlash(void);
static void GeneralGpsLocateInfoReadFormFlash(void);
static u8 GeneralGpsSpeedExcursionHandle(LocationVectorInfo*i_ptCurrentLocation);
static u8 GeneralGpsLocateExcursionHandle(LocationVectorInfo *i_ptBackupLocation,LocationVectorInfo *i_ptCurrentLocation);
static void GeneralGpsConfig(void);
static u8 GeneralGpsSendData(u8 *i_buf,u16 i_Len);
static u8 GeneralGpsRecvData();
static u8 GeneralGpsGetData(u8 *o_buf,u16 *o_Len);
static u8 GeneralGpsGetLocationInfo(LocationVectorInfo *o_tLocationVectorInfo);
static u8 GeneralGpsPowerOn();
static u8 GeneralGpsPowerOff();
static void GeneralGpsLocateInfoClearInFlash(void);
static void GeneralGpsGetHistoryPos(LocationVectorInfo *o_ptCurrentLocation);

static T_GpsInfoSaveInFlash g_tBackupLocationInfo={0};
static T_GpsManage g_tGeneralGps = {
	.name="GeneralGps",
	.GpsOprConfig					=GeneralGpsConfig,
	.GpsOprSendData				=GeneralGpsSendData,
	.GpsOprRecvData				=GeneralGpsRecvData,
	.GpsOprGetLocationInfo			=GeneralGpsGetLocationInfo,
	.GpsOprClearBackupLocationInfo	=GeneralGpsLocateInfoClearInFlash,
	.GpsOprPowerOn				=GeneralGpsPowerOn,
	.GpsOprPowerOff				=GeneralGpsPowerOff,
	.GpsOprGetHistoryPos 			=GeneralGpsGetHistoryPos,
};
/*****************************************************************************
-Fuction		: GeneralGpsInit
-Description	: GeneralGpsInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void GeneralGpsInit()
{
	GeneralGpsDevInit();
	RegisterGPS(&g_tGeneralGps);
}
/*****************************************************************************
-Fuction		: GeneralGpsConfig
-Description	: GeneralGpsConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GeneralGpsConfig(void)
{
	PT_GeneralGpsDevManage ptTmp = g_ptGeneralGpsDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGpsDevHead Null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->GeneralGpsConfigDev();
			ptTmp = ptTmp->ptNext;
		}		
	}
	GeneralGpsLocateInfoReadFormFlash();//读取FLASH中保存的GPS数据
}
/*****************************************************************************
-Fuction		: GeneralGpsPowerOn
-Description	: GeneralGpsPowerOn
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGpsPowerOn()
{
	u8 ret=0;
	PT_GeneralGpsDevManage ptTmp = g_ptGeneralGpsDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGpsPowerOn  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ptTmp->GeneralGpsPowerOnDev();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGpsPowerOff
-Description	: GeneralGpsPowerOff
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGpsPowerOff()
{
	u8 ret=0;
	PT_GeneralGpsDevManage ptTmp = g_ptGeneralGpsDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGpsPowerOff  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ptTmp->GeneralGpsPowerOffDev();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}

/*****************************************************************************
-Fuction		: GeneralGpsSendData
-Description	: GeneralGpsSendData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGpsSendData(u8 *i_buf,u16 i_Len)
{
	u8 ret=0;
	PT_GeneralGpsDevManage ptTmp = g_ptGeneralGpsDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGpsDevHead  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GeneralGpsSendDataDev(i_buf,i_Len);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGpsRecvData
-Description	: GeneralGpsRecvData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGpsRecvData()
{
	u8 ret;
	PT_GeneralGpsDevManage ptTmp = g_ptGeneralGpsDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGpsDevHead  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GeneralGpsRecvDataDev();
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGpsGetData
-Description	: GeneralGpsGetData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGpsGetData(u8 *o_buf,u16 *o_Len)
{
	u8 ret;
	PT_GeneralGpsDevManage ptTmp = g_ptGeneralGpsDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGpsDevHead Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GeneralGpsGetDataDev(o_buf,o_Len);
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGpsGetLocationInfo
-Description	: GeneralGpsGetLocationInfo
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26    V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static u8 GeneralGpsGetLocationInfo(LocationVectorInfo *o_ptLocationVectorInfo)
{
	u8 ret=FALSE;
	u8 *pucGpsData;
	u16 wGpsDataLen;
	datetime_t tDataTime={0};	
	datetime_t tGpsDataTime={0};
	LocationVectorInfo tCurrentLocationInfo={0};	
	ret = GpsGetDevRxDatPend(GPS_RX_PEND_TIME_MS, &pucGpsData, &wGpsDataLen); 	
	if(FALSE==ret)
	{
		DebugPrintf(ERR"GeneralGpsPendtData err:%d \r\n",ret);
	}
	else
	{
		nmeaInit(&tCurrentLocationInfo);
		ret=nmeaAnalysis(pucGpsData,wGpsDataLen);
		if(FALSE==ret)
		{
		}
		else
		{
			if((tCurrentLocationInfo.locationCommonPara.fixStatus!=FIX_3D)||
				(LOC_INFO_INVALID==tCurrentLocationInfo.locationCommonPara.dataValidity))
			{
				ret=FALSE;
			}
			else
			{
				ret=GeneralGpsSpeedExcursionHandle(&tCurrentLocationInfo);
				if(FALSE==ret)
				{
				}
				else
				{
					OSSchedLock();
					ret=GeneralGpsLocateExcursionHandle(&g_tBackupLocationInfo.tGpsInfo,&tCurrentLocationInfo);
					if(FALSE==ret)
					{
					}
					else
					{
						memcpy(&g_tBackupLocationInfo.tGpsInfo,&tCurrentLocationInfo,sizeof(LocationVectorInfo));
						GeneralGpsLocateInfoSaveInFlash();
						g_tTime.GetDateTime(&tDataTime);
						memcpy(&tGpsDataTime,&tCurrentLocationInfo.dataTime,sizeof(datetime_t));
						if(abs(app_sys_dtime2mktime(tDataTime)-app_sys_dtime2mktime(tGpsDataTime))<5)
						{
						}
						else
						{
							g_tTime.SetDateTime(tGpsDataTime);
						}
					}					
					OSSchedUnlock();
				}
			}
		}
		myfree(pucGpsData,wGpsDataLen);
	}
	//没有定位时，使用历史数据,如果FLASH读取出来的不定位则清零	
	OSSchedLock();
	if(g_tBackupLocationInfo.tGpsInfo.locationCommonPara.fixStatus!=FIX_3D)
	{
		memset(&g_tBackupLocationInfo.tGpsInfo,0,sizeof(LocationVectorInfo));
	}
	else
	{
	}
	memcpy(o_ptLocationVectorInfo,&g_tBackupLocationInfo.tGpsInfo,sizeof(LocationVectorInfo));
	OSSchedUnlock();
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGpsLocateInfoSaveInFlash
-Description	: GeneralGpsLocateInfoSaveInFlash
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26    V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static void GeneralGpsLocateInfoSaveInFlash(void)
{
	SpiFlashStrctWrite(&g_tBackupLocationInfo, 
					sizeof(g_tBackupLocationInfo),
					FLASH_ADDR_GPS_INTF_MAIN,
					FLASH_ADDR_GPS_INTF_BACK);//函数里面有加锁
}
/*****************************************************************************
-Fuction		: GeneralGpsLocateInfoReadFormFlash
-Description	: GeneralGpsLocateInfoReadFormFlash
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26    V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static void GeneralGpsLocateInfoReadFormFlash(void)
{
	u8 ret=FALSE;
	ret=SpiFlashStrctRead(&g_tBackupLocationInfo, 
					    sizeof(g_tBackupLocationInfo), 
					    FLASH_ADDR_GPS_INTF_MAIN, 
					    FLASH_ADDR_GPS_INTF_BACK);//函数里面有加锁
	if(FALSE==ret)
	{
		DebugPrintf("GeneralGpsLocateInfoReadFormFlash err\r\n");
	}
	else
	{
	}
}
/*****************************************************************************
-Fuction		: GeneralGpsLocateInfoSaveInFlash
-Description	: GeneralGpsLocateInfoSaveInFlash
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26    V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static void GeneralGpsLocateInfoClearInFlash(void)
{
	OSSchedLock();
	memset(&g_tBackupLocationInfo,0,sizeof(g_tBackupLocationInfo));
	OSSchedUnlock();
	SpiFlashStrctWrite(&g_tBackupLocationInfo, 
					sizeof(g_tBackupLocationInfo),
					FLASH_ADDR_GPS_INTF_MAIN,
					FLASH_ADDR_GPS_INTF_BACK);//函数里面有加锁
}

/*****************************************************************************
-Fuction		: GeneralGpsGetHistoryPos
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26    V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static void GeneralGpsGetHistoryPos(LocationVectorInfo *o_ptCurrentLocation)
{
	OSSchedLock();
	memcpy(&o_ptCurrentLocation->positionInfo,&g_tBackupLocationInfo.tGpsInfo.positionInfo,sizeof(PositionInfo));
	OSSchedUnlock();

}

/*****************************************************************************
-Fuction		: GeneralGpsSpeedExcursionHandle
-Description	: 速度漂移处理函数
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26    V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static u8 GeneralGpsSpeedExcursionHandle(LocationVectorInfo *i_ptCurrentLocation)
{
	static float BackupSpeed= 0;
	float CurretnSpeed = i_ptCurrentLocation->pointInfo.speed;
	u8 ret=FALSE;
	if(CurretnSpeed > 180)	/* 180Km/h */
	{
		ret=FALSE;
	}
	else
	{
		/* 50Km/h */
		if(abs(CurretnSpeed - BackupSpeed) > 50)
		{
			BackupSpeed = CurretnSpeed;
			ret=FALSE;
		}
		else
		{
			BackupSpeed = CurretnSpeed;
			BackupSpeed = 0;
			ret=TRUE;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGpsLocateExcursionHandle
-Description	: 定位漂移处理函数
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26    V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static u8 GeneralGpsLocateExcursionHandle(LocationVectorInfo *i_ptBackupLocation,LocationVectorInfo *i_ptCurrentLocation)
{
	u8 ret=FALSE;
	u32 dwCurrentLatitude	=i_ptCurrentLocation->positionInfo.lat;
	u32 dwBackupLatitude = i_ptBackupLocation->positionInfo.lat;
	u32 dwCurrentLongitude =i_ptCurrentLocation->positionInfo.lon;
	u32 dwBackupLongitude = i_ptBackupLocation->positionInfo.lat;
	static u16 s_wLocationExcursionCount=0;
	/* 位置参数漂移处理*/
	if((abs(dwCurrentLatitude - dwBackupLatitude) > (5 * 60000))
	|| (abs(dwCurrentLongitude - dwBackupLongitude) > (5 * 60000)))  /* 漂移峰峰值= 5分(度数)约9.268km*/
	{
		if((++ s_wLocationExcursionCount) > 30)//连续30次出现漂移则正常澹(可能不定位造成的)
		{
			ret=TRUE;
		}
		else
		{
			ret=FALSE;
		}
	}
	else
	{
		if(s_wLocationExcursionCount > 2)	/* 防止迂回漂移*/
		{
			s_wLocationExcursionCount /= 2;
			ret=FALSE;
		}
		else
		{
			s_wLocationExcursionCount = 0;
			ret=TRUE;
		}
	}	
	return ret;
}


/*****************************************************************************
-Fuction		: GpsGetDevRxDatPend
-Description	: GpsGetDevRxDatPend
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09    V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static u8 GpsGetDevRxDatPend(u16 i_wPendTimeMs, u8 **o_p2ucDat, u16 *o_pwLen)
{
	u8 ret=FALSE; 
	myevent *ptEveTmp;
	u8 *pucTmp; 
	u16 wLenTmp; 
	myevent tEve;	
	
	ret=DevOsRxQReadPend(ptOsQForGpsRx, (myevent **)&ptEveTmp, i_wPendTimeMs); 
	if(FALSE==ret) 
	{; 
	}
	else 
	{
		pucTmp = ptEveTmp->buff; 
		wLenTmp = ptEveTmp->size - ST_MYEVENT_SIZE; 
		
		tEve.size = wLenTmp; 
		tEve.buff = mymalloc(tEve.size); 
		if(NULL==tEve.buff) 
		{
			ret=FALSE;
			DebugPrintf(ERR"gps  rd dat malloc err\r\n"); 
		}
		else 
		{
			memcpy(tEve.buff, pucTmp, tEve.size); 
			*o_p2ucDat = tEve.buff; 
			*o_pwLen = tEve.size; 
		}

		myfree((u8 *)ptEveTmp, ptEveTmp->size); 
	}
	
	return ret; 
}

/*****************************************************************************
-Fuction		: RegisterGeneralGpsDev
-Description	: RegisterGeneralGpsDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterGeneralGpsDev(PT_GeneralGpsDevManage i_ptGeneralGpsDev)
{
	PT_GeneralGpsDevManage ptTmp;
	if (!g_ptGeneralGpsDevHead)
	{
		g_ptGeneralGpsDevHead   = i_ptGeneralGpsDev;
		g_ptGeneralGpsDevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptGeneralGpsDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptGeneralGpsDev;
		i_ptGeneralGpsDev->ptNext = NULL;
	}
}

