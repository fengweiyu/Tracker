/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_RTC.c
* Description		: 	RTC application operation center
* Created			: 	2016.08.29.
* Author			: 	Yu Weifeng
* Function List 		: 	RtcDev
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_RTC_H
#define _APP_RTC_H
#include "cBasicTypeDefine.h"

typedef struct RtcTimeDevManage
{
	char *name;
	void (*DevConfig)(void);
	u8 (*SetDateTime)(datetime_t i_Datetime);
	u8 (*GetDateTime)(datetime_t *o_Datetime);
	u8 (*AlarmConfig)(datetime_t i_Datetime);
	void (*AlarmHandle)();
	u8 (*WakeUpConfig)(u32 i_dwSecondCount);
	void(*WakeUpHandle)();
	struct RtcTimeDevManage *ptNext;
}T_RtcTimeDevManage,*PT_RtcTimeDevManage;

void RegisterRtcTimeDev(PT_RtcTimeDevManage i_ptRtcTimeDev);
void RtcTimeDevInit();

#endif
