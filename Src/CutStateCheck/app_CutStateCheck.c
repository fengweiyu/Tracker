/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 CutStateCheck.c
* Description		: 	CutStateCheck application operation center
* Created			: 	2016.09.26.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "CBasicTools.h"
#include "Config.h"
#include "app_CutStateCheck.h"

static PT_CutStateCheckDevManage g_ptCutStateCheckDevHead=NULL;

static void CutStateCheckInit();
static u8 CutStateCheckConfig(void);
static u8 CutStateCheckSetAlarmOpr();
static u8 CutStateCheckRelieveAlarmOpr();
static u8 CutStateCheckSetFallEXTIOpr();
static u8 CutStateCheckSetRiseEXTIOpr();
static u8 CheckCutStateOpr(void);

T_CutStateCheckOpr  const g_tCutStateCheck ={
	.Init						=CutStateCheckInit,
	.Config					=CutStateCheckConfig,
	.CheckCutState			=CheckCutStateOpr,
};
/*****************************************************************************
-Fuction		: CutStateCheckInit
-Description	: CutStateCheckInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void CutStateCheckInit()
{
	CutStateCheckDevInit();	
}
/*****************************************************************************
-Fuction		: CutStateCheckConfig
-Description	: CutStateCheckConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 CutStateCheckConfig(void)
{
	u8 ret=0;
	PT_CutStateCheckDevManage ptTmp = g_ptCutStateCheckDevHead;
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
-Fuction		: CheckCutStateOpr
-Description	: CheckCutStateOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 CheckCutStateOpr(void)
{
	u8 ret=FALSE;
	static u8 s_ucCutStateFlag=FALSE;
	if(IS_FALSE_BIT(s_ucCutStateFlag))
	{
		ret=CutStateCheckSetAlarmOpr();
		ret&=CutStateCheckSetRiseEXTIOpr();
		M_SetBit(s_ucCutStateFlag);
		DebugPrintf(ALERT"Tracker was be cutted!\r\n");
	}
	else
	{
		ret=CutStateCheckRelieveAlarmOpr();
		ret&=CutStateCheckSetFallEXTIOpr();
		M_ClrBit(s_ucCutStateFlag);		
		DebugPrintf(ALERT"Tracker was not cutted!\r\n");
	}
	return ret;
}

/*****************************************************************************
-Fuction		: CutStateCheckSetAlarmOpr
-Description	: CutStateCheckSetAlarmOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 CutStateCheckSetAlarmOpr()
{
	u8 ret=FALSE;
	PT_CutStateCheckDevManage ptTmp = g_ptCutStateCheckDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
		DebugPrintf(ERR"CutStateCheckSetAlarmOpr  err\r\n");
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevSetAlarm();
			ptTmp = ptTmp->ptNext;
		}	
	}
	return ret;
}
/*****************************************************************************
-Fuction		: CutStateCheckRelieveAlarmOpr
-Description	: CutStateCheckRelieveAlarmOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 CutStateCheckRelieveAlarmOpr()
{
	u8 ret=FALSE;
	PT_CutStateCheckDevManage ptTmp = g_ptCutStateCheckDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
		DebugPrintf(ERR"CutStateCheckRelieveAlarmOpr  err\r\n");
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevRelieveAlarm();
			ptTmp = ptTmp->ptNext;
		}	
	}
	return ret;
}

/*****************************************************************************
-Fuction		: CutStateCheckSetFallEXTIOpr
-Description	: CutStateCheckSetFallEXTIOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 CutStateCheckSetFallEXTIOpr()
{
	u8 ret=FALSE;
	PT_CutStateCheckDevManage ptTmp = g_ptCutStateCheckDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
		DebugPrintf(ERR"CutStateCheckSetFallEXTIOpr  err\r\n");
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
-Fuction		: CutStateCheckSetRiseEXTIOpr
-Description	: CutStateCheckSetRiseEXTIOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 CutStateCheckSetRiseEXTIOpr()
{
	u8 ret=FALSE;
	PT_CutStateCheckDevManage ptTmp = g_ptCutStateCheckDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
		DebugPrintf(ERR"CutStateCheckSetRiseEXTIOpr  err\r\n");
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
-Fuction		: RegisterCutStateCheckDev
-Description	: RegisterCutStateCheckDev
-Input			: i_ptCutStateCheckDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterCutStateCheckDev(PT_CutStateCheckDevManage i_ptCutStateCheckDev)
{
	PT_CutStateCheckDevManage ptTmp;
	if (!g_ptCutStateCheckDevHead)
	{
		g_ptCutStateCheckDevHead   = i_ptCutStateCheckDev;
		g_ptCutStateCheckDevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptCutStateCheckDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptCutStateCheckDev;
		i_ptCutStateCheckDev->ptNext = NULL;
	}
}




