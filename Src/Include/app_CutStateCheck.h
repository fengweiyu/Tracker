/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 CutStateCheck.h
* Description		: 	CutStateCheck application operation center
* Created			: 	2016.10.20.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_CUT_STATE_CHECK_H
#define _APP_CUT_STATE_CHECK_H

#include "cBasicTypeDefine.h"

#define CUT_TRACKER_STATE			(0)
#define UN_CUT_TRACKER_STATE			(1)


typedef struct CutStateCheckDevManage
{
	char *name;
	void (*DevConfig)(void);
	u8 (*DevSetAlarm)();	
	u8 (*DevRelieveAlarm)();
	u8 (*DevSetRiseEXTI)(void);
	u8 (*DevSetFallEXTI)(void);
	struct CutStateCheckDevManage *ptNext;
}T_CutStateCheckDevManage,*PT_CutStateCheckDevManage;

typedef struct CutStateCheckOpr
{
	void (*Init)(void);
	u8 (*Config)(void);
	u8 (*CheckCutState)(void);
}T_CutStateCheckOpr,*PT_CutStateCheckOpr;



void RegisterCutStateCheckDev(PT_CutStateCheckDevManage i_ptCutStateCheckDev);
void CutStateCheckDevInit();

extern T_CutStateCheckOpr const g_tCutStateCheck;

#endif




