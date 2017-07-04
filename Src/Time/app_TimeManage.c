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
#include "cBasicTypeDefine.h"
#include "app_TimeManage.h"
#include "Config.h"
static PT_TimeManage g_ptTimeOprHead=NULL;

static void TimeInit();
static void TimeConfig();
static u8 SetTime(datetime_t i_Datetime);
static u8 GetTime(datetime_t *o_ptDatetime);
static u8 TimeAlarmConfig(datetime_t i_Datetime);
static void TimeAlarmHandle();
static u8 TimeWakeUpConfig(u32 i_dwSecondCount);
static void TimeWakeUpHandle();

T_TimeOpr const g_tTime={
	.TimeInit			=TimeInit,
	.TimeConfig		=TimeConfig,
	.SetDateTime		=SetTime,
	.GetDateTime		=GetTime,
	.AlarmConfig		=TimeAlarmConfig,
	.AlarmHandle		=TimeAlarmHandle,
	.WakeUpConfig	=TimeWakeUpConfig,
	.WakeUpHandle	=TimeWakeUpHandle,
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
static void TimeInit()
{
	RtcTimeInit();
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
static void TimeConfig()
{
	PT_TimeManage ptTmp = g_ptTimeOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"RTC TimeDevHead TimeConfig Null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->TimeConfig();
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
static u8 SetTime(datetime_t i_Datetime)
{
	u8 ret=0;
	PT_TimeManage ptTmp = g_ptTimeOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"RTC TimeDevHead SetTime Null\r\n");
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
static u8 GetTime(datetime_t *o_ptDatetime)
{
	u8 ret;
	PT_TimeManage ptTmp = g_ptTimeOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"RTC TimeDevHead GetTime Null\r\n");
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
-Fuction		: RegisterRtcTimeDev
-Description	: RegisterRtcTimeDev
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 TimeAlarmConfig(datetime_t i_Datetime)
{
	u8 ret;
	PT_TimeManage ptTmp = g_ptTimeOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"RTC TimeDevHead TimeAlarmConfig Null\r\n");
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
-Fuction		: TimeWakeUpConfig
-Description	: TimeWakeUpConfig
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 TimeWakeUpConfig(u32 i_dwSecondCount)
{
	u8 ret;
	PT_TimeManage ptTmp = g_ptTimeOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"RTC TimeDevHead TimeWakeUpConfig Null\r\n");
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
-Fuction		: TimeAlarmHandle
-Description	: TimeAlarmHandle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void TimeAlarmHandle()
{
	PT_TimeManage ptTmp = g_ptTimeOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"RTC TimeDevHead TimeAlarmHandle Null\r\n");
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
-Fuction		: TimeAlarmHandle
-Description	: TimeAlarmHandle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void TimeWakeUpHandle()
{
	PT_TimeManage ptTmp = g_ptTimeOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"RTC TimeDevHead TimeWakeUpHandle Null\r\n");
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
void RegisterTime(PT_TimeManage i_ptTimeOpr)
{
	PT_TimeManage ptTmp;
	if (!g_ptTimeOprHead)
	{
		g_ptTimeOprHead   = i_ptTimeOpr;
		g_ptTimeOprHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptTimeOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptTimeOpr;
		i_ptTimeOpr->ptNext = NULL;
	}
}

