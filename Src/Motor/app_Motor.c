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
#include "CBasicTools.h"
#include "Config.h"
#include "app_Motor.h"

static PT_MotorDevManage g_ptMotorDevHead=NULL;

static void MotorInit();
static u8 MotorConfig(void);
static u8 MotorOnOpr();
static u8 MotorOffOpr();


T_MotorOpr const g_tMotor ={
	.Init			=MotorInit,
	.Config		=MotorConfig,
	.MotorOn		=MotorOnOpr,
	.MotorOff		=MotorOffOpr,
};
/*****************************************************************************
-Fuction		: MotorInit
-Description	: MotorInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void MotorInit()
{
	MotorDevInit();
}
/*****************************************************************************
-Fuction		: MotorConfig
-Description	: MotorConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 MotorConfig(void)
{
	u8 ret=0;
	PT_MotorDevManage ptTmp = g_ptMotorDevHead;
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
-Fuction		: RedMotorOnOpr
-Description	: RedMotorOnOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 MotorOnOpr()
{
	u8 ret=FALSE;
	PT_MotorDevManage ptTmp = g_ptMotorDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"MotorDevHead MotorOnOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevMotorOn();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: MotorOnOpr
-Description	: MotorOnOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 MotorOffOpr()
{
	u8 ret=FALSE;
	PT_MotorDevManage ptTmp = g_ptMotorDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"MotorDevHead MotorOnOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevMotorOff();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RegisterMotorDev
-Description	: RegisterMotorDev
-Input			: i_ptMotorDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterMotorDev(PT_MotorDevManage i_ptMotorDev)
{
	PT_MotorDevManage ptTmp;
	if (!g_ptMotorDevHead)
	{
		g_ptMotorDevHead   = i_ptMotorDev;
		g_ptMotorDevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptMotorDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptMotorDev;
		i_ptMotorDev->ptNext = NULL;
	}
}


