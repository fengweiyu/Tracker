/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 Alarm.c
* Description		: 	Alarm  operation center
* Created			: 	2016.09.20.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "Alarm.h"
#include "ParameterManage.h"
#include "main.h"
static T_Alarm g_tAlarm={0};

static u8 SetAlarmState(T_Alarm i_tAlarm);
static u8 GetAlarmState(T_Alarm *o_tAlarm);
static void ResumeAlarmTask();
static u8 GetAlarmEffectiveState(T_Alarm *o_tAlarm);
static u8 RelieveAlarmState(T_Alarm i_tAlarm);

T_AlarmOpr const g_tAlarmOpr={
	.SetState			=SetAlarmState,
	.GetState			=GetAlarmState,
	.ResumeTask		=ResumeAlarmTask,
	.GetEffectiveState	=GetAlarmEffectiveState,
	.RelieveAlarmState	=RelieveAlarmState
};

/*****************************************************************************
-Fuction		: SetAlarmState
-Description	: SetAlarmState
-Input			: none
-Output 		: none
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/21	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SetAlarmState(T_Alarm i_tAlarm)
{
	u8 ret=FALSE;
	OSSchedLock();
	memcpy(&g_tAlarm,&i_tAlarm,sizeof(T_Alarm));
	OSSchedUnlock();
	ret=TRUE;
	return ret;
}
/*****************************************************************************
-Fuction		: ResumeAlarmTask
-Description	: ResumeAlarmTask
-Input			: none
-Output 		: none
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/21	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void ResumeAlarmTask()
{
	OSSemPost(ptOsSemUsedForAlarmTask);
}
/*****************************************************************************
-Fuction		: GetAlarmState
-Description	: GetAlarmState
-Input			: none
-Output 		: none
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/21	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetAlarmState(T_Alarm *o_tAlarm)
{
	u8 ret=FALSE;
	OSSchedLock();
	memcpy(o_tAlarm,&g_tAlarm,sizeof(T_Alarm));
	OSSchedUnlock();
	ret=TRUE;
	return ret;
}
/*****************************************************************************
-Fuction		: GetAlarmEffectiveState
-Description	: GetAlarmEffectiveState
-Input			: none
-Output 		: none
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/21	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetAlarmEffectiveState(T_Alarm *o_tAlarm)
{
	u8 ret=FALSE;
	T_Alarm tAlarm;
	u8 ucAlarmSwitchBuf;
	u16 wAlarmSwitchLen;
	GetAlarmState(&tAlarm);
	g_tParameterOpr.GetPara("AlarmPara",CutTrackerAlarmSwitch,&ucAlarmSwitchBuf,&wAlarmSwitchLen);
	if(0==ucAlarmSwitchBuf)
	{
		tAlarm.CutTrackerAlarm=0;
	}
	else
	{
	}
	g_tParameterOpr.GetPara("AlarmPara",DropTrackerAlarmSwitch,&ucAlarmSwitchBuf,&wAlarmSwitchLen);
	if(0==ucAlarmSwitchBuf)
	{
		tAlarm.DropTrackerAlarm=0;
	}
	else
	{
	}
	g_tParameterOpr.GetPara("AlarmPara",LowPowerAlarmSwitch,&ucAlarmSwitchBuf,&wAlarmSwitchLen);
	if(0==ucAlarmSwitchBuf)
	{
		tAlarm.LowPowerAlarm=0;
	}
	else
	{
	}
	if((0==tAlarm.CutTrackerAlarm)&&(0==tAlarm.DropTrackerAlarm)&&(0==tAlarm.LowPowerAlarm))
	{
		DebugPrintf("Do not have any Alarm!err \r\n");
		ret=FALSE;
	}
	else
	{
		memcpy(o_tAlarm,&tAlarm,sizeof(T_Alarm));
		ret=TRUE;
	}
        return ret;
}
/*****************************************************************************
-Fuction		: RelieveAlarmState
-Description	: RelieveAlarmState
-Input			: none
-Output 		: none
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/22	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 RelieveAlarmState(T_Alarm i_tAlarm)
{
	u8 ret=FALSE;
	OSSchedLock();
	if(1==i_tAlarm.CutTrackerAlarm)
	{
		g_tAlarm.CutTrackerAlarm=0;
	}
	else
	{
	}
	if(1==i_tAlarm.DropTrackerAlarm)
	{
		g_tAlarm.DropTrackerAlarm=0;
	}
	else
	{
	}
	if(1==i_tAlarm.LowPowerAlarm)
	{
		g_tAlarm.LowPowerAlarm=0;
	}
	else
	{
	}
	OSSchedUnlock();
	ret=TRUE;
	return ret;
}

