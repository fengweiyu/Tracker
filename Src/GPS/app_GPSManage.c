/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_GpsManage.c
* Description		: 	Gps application operation center
* Created			: 	2016.08.29.
* Author			: 	Yu Weifeng
* Function List 		: 	Use Demo
						LocationVectorInfo tGpsLocationInfo={0};
						ret=g_tGPS.GpsGetPos(&tGpsLocationInfo);
						if(FALSE==ret)
						{
							DebugPrintf(ERR" err,UseHistoryPos\r\n");
							ret=g_tGPS.GpsGetHistoryPos(&tGpsLocationInfo);
						}
						else
						{
						}
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "cBasicTypeDefine.h"
#include "app_GpsManage.h"
#include "Config.h"
#include <stdarg.h>
#include "app_TimeManage.h"

static PT_GpsManage g_ptGpsOprHead=NULL;

static void GpsInit();
static void GpsConfig();
static u8 GpsSendDataBuf(u8 *i_buf,u16 i_Len);
static u8 GpsRecvDataBuf();
static u8 GpsGetLocationInfo(LocationVectorInfo *o_ptLocationVectorInfo);
static u8 GpsPowerOnOpr();
static u8 GpsPowerOffOpr();
static u8 GpsClearBackupLocationInfoOpr();
static u8 GpsGetPosReq();
static u8 GpsGetPos(LocationVectorInfo *o_ptLocationVectorInfo);
static u8 GpsGetHistoryPos(LocationVectorInfo *o_ptLocationVectorInfo);

static LocationVectorInfo g_tGpsLocationInfo={0};

T_GpsOpr const g_tGPS={
	.Init						=GpsInit,
	.Config					=GpsConfig,
	.GpsSendData				=GpsSendDataBuf,
	.GpsRecvData				=GpsRecvDataBuf,
	.GpsGetLocationInfo		=GpsGetLocationInfo,
	.GpsClearBackupLocationInfo =GpsClearBackupLocationInfoOpr,
	.GpsPowerOn				=GpsPowerOnOpr,
	.GpsPowerOff				=GpsPowerOffOpr,
	.GpsGetHistoryPos			=GpsGetHistoryPos,
	.GpsGetPos				=GpsGetPos,
};

/*****************************************************************************
-Fuction		: GpsProcessTask
-Description	: GpsProcessTask
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void GpsProcessTask(void *pdata)
{
	u8 ret=FALSE;
	u8 ucErr;
	u8 ucGpsLocateOkFlag=FALSE;
	u32 dwCurTime=0;
	u32 dwBkTime=0;	
	static LocationVectorInfo s_tGpsLocationInfo={0};
	while(1)
	{
		OSFlagPend(pstOsFlagForAppUse, GET_GPS_POSITION_FLAG,OS_FLAG_WAIT_SET_ALL, 0, &ucErr); 		
		if(OS_ERR_NONE!=ucErr)
		{	
		}
		else
		{
			memset(&s_tGpsLocationInfo,0,sizeof(LocationVectorInfo));
			dwBkTime=OSTimeGet();
			dwCurTime=OSTimeGet();		
			while((dwCurTime-dwBkTime)<(OS_TICKS_PER_SEC*GPS_LOCATE_TIME))
			{
				ret=GpsGetLocationInfo(&s_tGpsLocationInfo);
				GpsPowerOnOpr();//
				if(FALSE==ret)
				{
				}
				else
				{
					M_SetBit(ucGpsLocateOkFlag);
				}			
				dwCurTime=OSTimeGet();
			}
			GpsPowerOffOpr();//
			if(IS_FALSE_BIT(ucGpsLocateOkFlag))
			{
			}
			else
			{
				OSSchedLock();
				memcpy(&g_tGpsLocationInfo,&s_tGpsLocationInfo,sizeof(LocationVectorInfo));
				OSSchedUnlock();
				OSFlagPost(pstOsFlagForAppUse, GET_GPS_POSITION_OK_FLAG, OS_FLAG_SET, &ucErr);	
				M_ClrBit(ucGpsLocateOkFlag);
			}
			OSFlagPost(pstOsFlagForAppUse, GET_GPS_POSITION_FLAG, OS_FLAG_CLR, &ucErr);	
		}
	}
}

/*****************************************************************************
-Fuction		: GpsGetPos
-Description	: GpsGetPos
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GpsGetPos(LocationVectorInfo *o_ptLocationVectorInfo)
{
	u8 ret=FALSE;
	u8 ucErr;
	datetime_t tDataTime={0};
	if(FALSE==GpsGetPosReq())
	{
	}
	else
	{
		OSFlagPend(pstOsFlagForAppUse, GET_GPS_POSITION_OK_FLAG,OS_FLAG_WAIT_SET_ALL, ((GPS_LOCATE_TIME+1)*(OS_TICKS_PER_SEC)), &ucErr); 
		if(OS_ERR_NONE!=ucErr)
		{	
			DebugPrintf(ERR"GpsGetPos err:%d,UseHistoryPos\r\n",ucErr);
			GpsGetHistoryPos(o_ptLocationVectorInfo);
		}
		else
		{
			g_tTime.GetDateTime(&tDataTime);//在一分钟内的任意一个时间点都可能定位或不定位
			/*如果直接使用GPS时间，由于定位的时间点不确定
			导致每次取的定位时间点不一致(可能在一分钟内任意的时间点)
			从而导致固定上传间隔不准确,例如:上一次在一分钟开始定位后面不定位
			这一次在一分钟最后才定位，那么就间隔不准了，所以每次都
			直接获取RTC，因为GPS处理内部已经校准了RTC，认为RTC时间就是准确时间*/
			OSSchedLock();
			memcpy(o_ptLocationVectorInfo,&g_tGpsLocationInfo,sizeof(LocationVectorInfo));
			memcpy(&o_ptLocationVectorInfo->dataTime,&tDataTime,sizeof(datetime_t));
			OSSchedUnlock();
			OSFlagPost(pstOsFlagForAppUse, GET_GPS_POSITION_OK_FLAG, OS_FLAG_CLR, &ucErr);	
		}
		OSFlagPost(pstOsFlagForAppUse, GET_GPS_POSITION_BUSY_FLAG, OS_FLAG_CLR, &ucErr);	
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GpsGetPosReq
-Description	: GpsGetPosReq
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GpsGetPosReq()
{
	u8 ret=FALSE;
	u8 ucErr;
	OSFlagPend(pstOsFlagForAppUse, GET_GPS_POSITION_FLAG|GET_GPS_POSITION_BUSY_FLAG,OS_FLAG_WAIT_CLR_ALL,(GPS_LOCATE_TIME*(OS_TICKS_PER_SEC)),&ucErr); 
	if(OS_ERR_NONE!=ucErr)
	{	
		ret=FALSE;
		DebugPrintf(ERR"GpsGetPosReq err:%d\r\n",ucErr);
	}
	else
	{
		OSFlagPost(pstOsFlagForAppUse, GET_GPS_POSITION_FLAG|GET_GPS_POSITION_BUSY_FLAG, OS_FLAG_SET, &ucErr);	
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: GpsGetHistoryPos
-Description	: GpsGetHistoryPos
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GpsGetHistoryPos(LocationVectorInfo *o_ptLocationVectorInfo)
{
	u8 ret=FALSE;
	datetime_t tDataTime={0};
	PT_GpsManage ptTmp = g_ptGpsOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GpsGetBackupPos  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ptTmp->GpsOprGetHistoryPos(o_ptLocationVectorInfo);
			ptTmp = ptTmp->ptNext;
		}		
	}
	g_tTime.GetDateTime(&tDataTime);
	memcpy(&o_ptLocationVectorInfo->dataTime,&tDataTime,sizeof(datetime_t));
	return ret;
}

/*****************************************************************************
-Fuction		: RegisterRtcTimeDev
-Description	: RegisterRtcTimeDev
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GpsInit()
{
	GeneralGpsInit();
}

/*****************************************************************************
-Fuction		: RegisterRtcTimeDev
-Description	: RegisterRtcTimeDev
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GpsConfig()
{
	PT_GpsManage ptTmp = g_ptGpsOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GpsOprHead GpsConfig Null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->GpsOprConfig();
			ptTmp = ptTmp->ptNext;
		}		
	}
}
/*****************************************************************************
-Fuction		: GetRtcTime
-Description	: GetRtcTime
-Input			: o_ptDatetime
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GpsPowerOnOpr()
{
	u8 ret=FALSE;
	PT_GpsManage ptTmp = g_ptGpsOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GpsOprHead GpsPowerOn Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GpsOprPowerOn();
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GpsPowerOff
-Description	: GpsPowerOff
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GpsPowerOffOpr()
{
	u8 ret=FALSE;
	PT_GpsManage ptTmp = g_ptGpsOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GpsPowerOff  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GpsOprPowerOff();
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GpsClearBackupLocationInfoOpr
-Description	: GpsClearBackupLocationInfoOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GpsClearBackupLocationInfoOpr()
{
	u8 ret=FALSE;
	PT_GpsManage ptTmp = g_ptGpsOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GpsClearBackupLocationInfoOpr  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ptTmp->GpsOprClearBackupLocationInfo();
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}

/*****************************************************************************
-Fuction		: RegisterRtcTimeDev
-Description	: RegisterRtcTimeDev
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GpsSendDataBuf(u8 *i_buf,u16 i_Len)
{
	u8 ret=0;
	PT_GpsManage ptTmp = g_ptGpsOprHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GpsOprSendData(i_buf,i_Len);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetRtcTime
-Description	: GetRtcTime
-Input			: o_ptDatetime
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GpsRecvDataBuf()
{
	u8 ret;
	PT_GpsManage ptTmp = g_ptGpsOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GpsOprHead GpsRecvDataBuf Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GpsOprRecvData();
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetRtcTime
-Description	: GetRtcTime
-Input			: o_ptDatetime
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GpsGetLocationInfo(LocationVectorInfo *o_ptLocationVectorInfo)
{
	u8 ret;
	PT_GpsManage ptTmp = g_ptGpsOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GpsOprHead GpsGetPositionInfo Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GpsOprGetLocationInfo(o_ptLocationVectorInfo);
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}

/*****************************************************************************
-Fuction		: RegisterRtcTimeDev
-Description	: RegisterRtcTimeDev
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterGPS(PT_GpsManage i_ptGpsOpr)
{
	PT_GpsManage ptTmp;
	if (!g_ptGpsOprHead)
	{
		g_ptGpsOprHead   = i_ptGpsOpr;
		g_ptGpsOprHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptGpsOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptGpsOpr;
		i_ptGpsOpr->ptNext = NULL;
	}
}

