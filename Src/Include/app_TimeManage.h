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
#ifndef _APP_TIME_MANAGE_H
#define _APP_TIME_MANAGE_H

#include "cBasicTypeDefine.h"


typedef struct TimeOpr
{
	void (*TimeInit)(void);
	void (*TimeConfig)(void);
	u8 (*SetDateTime)(datetime_t i_Datetime);
	u8 (*GetDateTime)(datetime_t *o_Datetime);
	u8 (*AlarmConfig)(datetime_t i_Datetime);
	void (*AlarmHandle)();
	u8 (*WakeUpConfig)(u32 i_dwSecondCount);
	void(*WakeUpHandle)();
}T_TimeOpr,*PT_TimeOpr;

typedef struct TimeManage
{
	char *name;
	void (*TimeConfig)(void);
	u8 (*SetDateTime)(datetime_t i_Datetime);
	u8 (*GetDateTime)(datetime_t *o_Datetime);
	u8 (*AlarmConfig)(datetime_t i_Datetime);
	void (*AlarmHandle)();
	u8 (*WakeUpConfig)(u32 i_dwSecondCount);
	void(*WakeUpHandle)();
	struct TimeManage *ptNext;
}T_TimeManage,*PT_TimeManage;

extern T_TimeOpr const g_tTime;
void RegisterTime(PT_TimeManage i_ptTimeOpr);
void RtcTimeInit();

#endif
