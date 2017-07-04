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
#include "CBasicTools.h"
#include "Config.h"
#include "app_Led.h"
static PT_LedDevManage g_ptLedDevHead=NULL;

static void LedInit();
static u8 LedConfig(void);
static u8 RedLedOnOpr();
static u8 RedLedOffOpr();
static u8 BlueLedOnOpr();
static u8 BlueLedOffOpr();
static u8 BlueLedPwmConfigOpr();

T_LedOpr const g_tLed ={
	.Init					=LedInit,
	.Config				=LedConfig,
	.RedLedOn			=RedLedOnOpr,
	.RedLedOff			=RedLedOffOpr,
	.BlueLedOn			=BlueLedOnOpr,
	.BlueLedOff			=BlueLedOffOpr,
	.BlueLedPwmConfig		=BlueLedPwmConfigOpr
};
/*****************************************************************************
-Fuction		: LedInit
-Description	: LedInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void LedInit()
{
	LedDevInit();
}
/*****************************************************************************
-Fuction		: LedConfig
-Description	: LedConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LedConfig(void)
{
	u8 ret=0;
	PT_LedDevManage ptTmp = g_ptLedDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->DevConfig();
			ptTmp = ptTmp->ptNext;
		}	
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RedLedOnOpr
-Description	: RedLedOnOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 RedLedOnOpr()
{
	u8 ret=FALSE;
	PT_LedDevManage ptTmp = g_ptLedDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"LedDevHead RedLedOnOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevLedOn(RedLed);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RedLedOffOpr
-Description	: RedLedOffOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 RedLedOffOpr()
{
	u8 ret=FALSE;
	PT_LedDevManage ptTmp = g_ptLedDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"LedDevHead RedLedOffOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevLedOff(RedLed);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: BlueLedOnOpr
-Description	: BlueLedOnOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 BlueLedOnOpr()
{
	u8 ret=FALSE;
	PT_LedDevManage ptTmp = g_ptLedDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"LedDevHead BlueLedOnOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevLedOn(BlueLed);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: BlueLedOffOpr
-Description	: BlueLedOffOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 BlueLedOffOpr()
{
	u8 ret=FALSE;
	PT_LedDevManage ptTmp = g_ptLedDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"LedDevHead BlueLedOffOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevLedOff(BlueLed);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: BlueLedPwmConfigOpr
-Description	: BlueLedPwmConfigOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 BlueLedPwmConfigOpr()
{
	u8 ret=FALSE;
	PT_LedDevManage ptTmp = g_ptLedDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"LedDevHead BlueLedOffOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevLedPwmConfig(BlueLed);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}

/*****************************************************************************
-Fuction		: RegisterLedDev
-Description	: RegisterLedDev
-Input			: i_ptLedDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterLedDev(PT_LedDevManage i_ptLedDev)
{
	PT_LedDevManage ptTmp;
	if (!g_ptLedDevHead)
	{
		g_ptLedDevHead   = i_ptLedDev;
		g_ptLedDevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptLedDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptLedDev;
		i_ptLedDev->ptNext = NULL;
	}
}

