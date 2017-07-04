/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 Led.c
* Description		: 	Led application operation center
* Created			: 	2016.09.23.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_LED_H
#define _APP_LED_H

#include "cBasicTypeDefine.h"

typedef enum LedType
{
	RedLed,
	BlueLed,
}E_LedType;
typedef struct LedOpr
{
	void (*Init)(void);
	u8 (*Config)(void);
	u8 (*RedLedOn)(void);
	u8 (*RedLedOff)(void);
	u8 (*BlueLedOn)(void);
	u8 (*BlueLedOff)(void);	
	u8 (*BlueLedPwmConfig)(void);
}T_LedOpr,*PT_LedOpr;


typedef struct LedDevManage
{
	char *name;
	void (*DevConfig)(void);
	u8 (*DevLedOn)(E_LedType i_eLedType);
	u8 (*DevLedOff)(E_LedType i_eLedType);
	u8 (*DevLedPwmConfig)(E_LedType i_eLedType);
	struct LedDevManage *ptNext;
}T_LedDevManage,*PT_LedDevManage;

void RegisterLedDev(PT_LedDevManage i_ptLedDev);
void LedDevInit();

extern T_LedOpr const g_tLed;

#endif

