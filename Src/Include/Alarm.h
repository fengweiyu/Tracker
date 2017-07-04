/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 Alarm.h
* Description		: 	Alarm  operation center
* Created			: 	2016.09.20.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef	_ALARM_H
#define	_ALARM_H
#include "CBasicTools.h"
typedef struct Alarm
{
	u8 LowPowerAlarm;
	u8 CutTrackerAlarm;
	u8 DropTrackerAlarm;
}T_Alarm,*PT_Alarm;

typedef struct AlarmOpr
{
	u8 (*SetState)(T_Alarm i_tAlarm);	
	u8 (*GetState)(T_Alarm *o_tAlarm);
	void (*ResumeTask)();
	u8 (*GetEffectiveState)(T_Alarm *o_ptAlarm);
	u8 (*RelieveAlarmState)(T_Alarm i_tAlarm);
}T_AlarmOpr,*PT_AlarmOpr;
extern T_AlarmOpr const g_tAlarmOpr;
#endif
