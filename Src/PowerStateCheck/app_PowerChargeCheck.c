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
#include "CBasicTools.h"
#include "Config.h"
#include "app_PowerChargeCheck.h"
static PT_PowerChargeCheckDevManage g_ptPowerChargeCheckDevHead=NULL;

static void PowerChargeCheckInit();
static u8 PowerChargeCheckConfig(void);
static u8 SetFallEXTIOpr();
static u8 SetRiseEXTIOpr();
static u8 GetPowerChargeStateOpr(u8 *o_pucPowerChargeState);

T_PowerChargeCheckOpr  const g_tPowerChargeCheck ={
	.Init						=PowerChargeCheckInit,
	.Config					=PowerChargeCheckConfig,
	.GetPowerChargeState		=GetPowerChargeStateOpr,
	.SetRiseEXTI				=SetRiseEXTIOpr,
	.SetFallEXTI				=SetFallEXTIOpr,
};
/*****************************************************************************
-Fuction		: PowerChargeCheckInit
-Description	: PowerChargeCheckInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void PowerChargeCheckInit()
{
	PowerChargeCheckDevInit();	
}
/*****************************************************************************
-Fuction		: PowerChargeCheckConfig
-Description	: PowerChargeCheckConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 PowerChargeCheckConfig(void)
{
	u8 ret=0;
	PT_PowerChargeCheckDevManage ptTmp = g_ptPowerChargeCheckDevHead;
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
-Fuction		: GetPowerCheckStateOpr
-Description	: GetPowerCheckStateOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetPowerChargeStateOpr(u8 *o_pucPowerChargeState)
{
	u8 ret=FALSE;
	PT_PowerChargeCheckDevManage ptTmp = g_ptPowerChargeCheckDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetPowerChargeStateOpr  err\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->DevGetPowerChargeState(o_pucPowerChargeState);
			ptTmp = ptTmp->ptNext;
		}	
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetPowerCheckStateOpr
-Description	: GetPowerCheckStateOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SetFallEXTIOpr()
{
	u8 ret=FALSE;
	PT_PowerChargeCheckDevManage ptTmp = g_ptPowerChargeCheckDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetFallEXTIOpr  err\r\n");
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevSetFallEXTI();
			ptTmp = ptTmp->ptNext;
		}	
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetPowerCheckStateOpr
-Description	: GetPowerCheckStateOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SetRiseEXTIOpr()
{
	u8 ret=FALSE;
	PT_PowerChargeCheckDevManage ptTmp = g_ptPowerChargeCheckDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetRiseEXTIOpr  err\r\n");
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevSetRiseEXTI();
			ptTmp = ptTmp->ptNext;
		}	
	}
	return ret;
}

/*****************************************************************************
-Fuction		: RegisterPowerChargeCheckDev
-Description	: RegisterPowerChargeCheckDev
-Input			: i_ptPowerChargeCheckDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterPowerChargeCheckDev(PT_PowerChargeCheckDevManage i_ptPowerChargeCheckDev)
{
	PT_PowerChargeCheckDevManage ptTmp;
	if (!g_ptPowerChargeCheckDevHead)
	{
		g_ptPowerChargeCheckDevHead   = i_ptPowerChargeCheckDev;
		g_ptPowerChargeCheckDevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPowerChargeCheckDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptPowerChargeCheckDev;
		i_ptPowerChargeCheckDev->ptNext = NULL;
	}
}



