/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 PowerChargeCheck.c
* Description		: 	PowerChargeCheck application operation center
* Created			: 	2016.09.26.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_POWER_CHARGE_CHECK_H
#define _APP_POWER_CHARGE_CHECK_H

#include "cBasicTypeDefine.h"

#define CHARGE_POWER_STATE			(0)
#define UN_CHARGE_POWER_STATE		(1)

#define FALL_EXTI_MODE					(0)
#define RISE_EXTI_MODE					(1)
typedef struct PowerChargeCheckDevManage
{
	char *name;
	void (*DevConfig)(void);
	u8 (*DevGetPowerChargeState)(u8 *o_pucPowerChargeState);
	u8 (*DevSetRiseEXTI)(void);
	u8 (*DevSetFallEXTI)(void);
	struct PowerChargeCheckDevManage *ptNext;
}T_PowerChargeCheckDevManage,*PT_PowerChargeCheckDevManage;

typedef struct PowerChargeCheckOpr
{
	void (*Init)(void);
	u8 (*Config)(void);
	u8 (*SetRiseEXTI)(void);
	u8 (*SetFallEXTI)(void);
	u8 (*GetPowerChargeState)(u8 *o_pucPowerChargeState);
}T_PowerChargeCheckOpr,*PT_PowerChargeCheckOpr;



void RegisterPowerChargeCheckDev(PT_PowerChargeCheckDevManage i_ptPowerChargeCheckDev);
void PowerChargeCheckDevInit();

extern T_PowerChargeCheckOpr const g_tPowerChargeCheck;

#endif



