/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 Motor.c
* Description		: 	Motor application operation center
* Created			: 	2016.09.23.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_Motor_H
#define _APP_Motor_H

#include "cBasicTypeDefine.h"


typedef struct MotorOpr
{
	void (*Init)(void);
	u8 (*Config)(void);
	u8 (*MotorOn)(void);
	u8 (*MotorOff)(void);
}T_MotorOpr,*PT_MotorOpr;


typedef struct MotorDevManage
{
	char *name;
	void (*DevConfig)(void);
	u8 (*DevMotorOn)();
	u8 (*DevMotorOff)();
	struct MotorDevManage *ptNext;
}T_MotorDevManage,*PT_MotorDevManage;

void RegisterMotorDev(PT_MotorDevManage i_ptMotorDev);
void MotorDevInit();

extern T_MotorOpr const g_tMotor;

#endif


