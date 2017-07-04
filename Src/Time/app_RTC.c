/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_RTC.c
* Description		: 	RTC application operation center
* Created			: 	2016.08.29.
* Author			: 	Yu Weifeng
* Function List 		: 	RtcTimeInit
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include"cBasicTypeDefine.h"
#include "app_RTC.h"
#include "app_TimeManage.h"
#include "Config.h"
static PT_RtcTimeDevManage g_ptRtcTimeDevHead=NULL;

static void RtcTimeConfig(void);
static u8 SetRtcTime(datetime_t i_Datetime);
static u8 GetRtcTime(datetime_t *o_ptDatetime);
static u8 RtcTimeAlarmConfig(datetime_t i_Datetime);
static void RtcTimeAlarmHandle();
static u8 RtcTimeWakeUpConfig(u32 i_dwSecondCount);
static void RtcTimeWakeUpHandle();
static T_TimeManage g_tRtcTime = {
	.name="RTC",
	.TimeConfig		=RtcTimeConfig,
	.SetDateTime		=SetRtcTime,
	.GetDateTime		=GetRtcTime,
	.AlarmConfig		=RtcTimeAlarmConfig,
	.AlarmHandle		=RtcTimeAlarmHandle,
	.WakeUpConfig	=RtcTimeWakeUpConfig,	
	.WakeUpHandle	=RtcTimeWakeUpHandle,
};
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
void RtcTimeInit()
{
	RtcTimeDevInit();
	RegisterTime(&g_tRtcTime);
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
static void RtcTimeConfig(void)
{
	PT_RtcTimeDevManage ptTmp = g_ptRtcTimeDevHead;
	if(NULL==ptTmp)
	{
		//DebugPrintf("RTC TimeDevHead Null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->DevConfig();
			ptTmp = ptTmp->ptNext;
		}		
	}
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
static u8 SetRtcTime(datetime_t i_Datetime)
{
	u8 ret=0;
	PT_RtcTimeDevManage ptTmp = g_ptRtcTimeDevHead;
	if(NULL==ptTmp)
	{
		//DebugPrintf("RTC TimeDevHead SetRtcTime Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->SetDateTime(i_Datetime);
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
static u8 GetRtcTime(datetime_t *o_ptDatetime)
{
	u8 ret;
	PT_RtcTimeDevManage ptTmp = g_ptRtcTimeDevHead;
	if(NULL==ptTmp)
	{
		//DebugPrintf("RTC TimeDevHead GetRtcTime Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GetDateTime(o_ptDatetime);
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RtcTimeAlarmConfig
-Description	: RtcTimeAlarmConfig
-Input			: i_Datetime
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 RtcTimeAlarmConfig(datetime_t i_Datetime)
{
	u8 ret;
	PT_RtcTimeDevManage ptTmp = g_ptRtcTimeDevHead;
	if(NULL==ptTmp)
	{
		//DebugPrintf("RTC TimeDevHead RtcTimeAlarmConfig Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->AlarmConfig(i_Datetime);
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RtcTimeAlarmConfig
-Description	: RtcTimeAlarmConfig
-Input			: i_Datetime
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 RtcTimeWakeUpConfig(u32 i_dwSecondCount)
{
	u8 ret;
	PT_RtcTimeDevManage ptTmp = g_ptRtcTimeDevHead;
	if(NULL==ptTmp)
	{
		//DebugPrintf("RTC TimeDevHead RtcTimeAlarmConfig Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->WakeUpConfig(i_dwSecondCount);
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}

/*****************************************************************************
-Fuction		: RtcTimeAlarmHandle
-Description	: RtcTimeAlarmHandle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void RtcTimeAlarmHandle()
{
	PT_RtcTimeDevManage ptTmp = g_ptRtcTimeDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"RTC TimeDevHead RtcTimeAlarmHandle Null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->AlarmHandle();
			ptTmp = ptTmp->ptNext;
		}		
	}
}
/*****************************************************************************
-Fuction		: RtcTimeAlarmHandle
-Description	: RtcTimeAlarmHandle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void RtcTimeWakeUpHandle()
{
	PT_RtcTimeDevManage ptTmp = g_ptRtcTimeDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"RTC TimeDevHead RtcTimeWakeUpHandle Null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->WakeUpHandle();
			ptTmp = ptTmp->ptNext;
		}		
	}
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
void RegisterRtcTimeDev(PT_RtcTimeDevManage i_ptRtcTimeDev)
{
	PT_RtcTimeDevManage ptTmp;
	if (!g_ptRtcTimeDevHead)
	{
		g_ptRtcTimeDevHead   = i_ptRtcTimeDev;
		g_ptRtcTimeDevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptRtcTimeDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptRtcTimeDev;
		i_ptRtcTimeDev->ptNext = NULL;
	}
}

