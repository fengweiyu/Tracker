/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 LightDistanceSensor.c
* Description		: 	LightDistanceSensor application operation center
* Created			: 	2016.09.26.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "CBasicTools.h"
#include "Config.h"
#include "app_LightDistanceSensor.h"
static PT_LightDistanceSensorDevManage g_ptLightDistanceSensorDevHead=NULL;

static void LightDistanceSensorInit();
static u8 LightDistanceSensorConfig(void);
static u8 LightDistanceSensorSetAlarm(void);
static u8 LightDistanceSensorRelieveAlarm(void);
static u8 LightDistanceSensorSetNearFlagMode(void);
static u8 LightDistanceSensorSetFarFlagMode(void);
static u8 LightDistanceSensorReadData(u16* o_wLedIR,u16* o_wPS,u16* o_wALS);
static u8 LightDistanceSensorCheckDropState(void);

T_LightDistanceSensorOpr  const g_tLightDistanceSensor ={
	.Init						=LightDistanceSensorInit,
	.Config					=LightDistanceSensorConfig,
	.CheckDropState			=LightDistanceSensorCheckDropState,
	.GetSensorData			=LightDistanceSensorReadData
};
/*****************************************************************************
-Fuction		: LightDistanceSensorInit
-Description	: LightDistanceSensorInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void LightDistanceSensorInit()
{
	LightDistanceSensorDevInit();	
}
/*****************************************************************************
-Fuction		: LightDistanceSensorConfig
-Description	: LightDistanceSensorConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorConfig(void)
{
	u8 ret=0;
	PT_LightDistanceSensorDevManage ptTmp = g_ptLightDistanceSensorDevHead;
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
-Fuction		: LightDistanceSensorCheckDropState
-Description	: LightDistanceSensorCheckDropState
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorCheckDropState(void)
{
	u8 ret=FALSE;
	static u8 s_ucDropStateFlag=FALSE;
	if(IS_FALSE_BIT(s_ucDropStateFlag))
	{
		ret=LightDistanceSensorRelieveAlarm();
		ret&=LightDistanceSensorSetFarFlagMode();
		M_SetBit(s_ucDropStateFlag);
	}
	else
	{
		ret=LightDistanceSensorSetAlarm();
		ret&=LightDistanceSensorSetNearFlagMode();
		M_ClrBit(s_ucDropStateFlag);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: LightDistanceSensorReadData
-Description	: LightDistanceSensorReadData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorReadData(u16* o_wLedIR,u16* o_wPS,u16* o_wALS)
{
	u8 ret=FALSE;
	PT_LightDistanceSensorDevManage ptTmp = g_ptLightDistanceSensorDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
		DebugPrintf(ERR"LightDistanceSensorReadData  err\r\n");
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevReadData(o_wLedIR,o_wPS,o_wALS);
			ptTmp = ptTmp->ptNext;
		}	
	}
	return ret;
}

/*****************************************************************************
-Fuction		: LightDistanceSensorSetAlarm
-Description	: LightDistanceSensorSetAlarm
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorSetAlarm(void)
{
	u8 ret=0;
	PT_LightDistanceSensorDevManage ptTmp = g_ptLightDistanceSensorDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
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
-Fuction		: LightDistanceSensorRelieveAlarm
-Description	: LightDistanceSensorRelieveAlarm
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorRelieveAlarm(void)
{
	u8 ret=0;
	PT_LightDistanceSensorDevManage ptTmp = g_ptLightDistanceSensorDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
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
-Fuction		: LightDistanceSensorSetRiseExti
-Description	: LightDistanceSensorSetRiseExti
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorSetNearFlagMode(void)
{
	u8 ret=0;
	PT_LightDistanceSensorDevManage ptTmp = g_ptLightDistanceSensorDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevSetNearFlagMode();
			ptTmp = ptTmp->ptNext;
		}	
	}
	return ret;
}
/*****************************************************************************
-Fuction		: LightDistanceSensorSetFallExti
-Description	: LightDistanceSensorSetFallExti
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorSetFarFlagMode(void)
{
	u8 ret=0;
	PT_LightDistanceSensorDevManage ptTmp = g_ptLightDistanceSensorDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevSetFarFlagMode();
			ptTmp = ptTmp->ptNext;
		}	
	}
	return ret;
}

/*****************************************************************************
-Fuction		: RegisterLightDistanceSensorDev
-Description	: RegisterLightDistanceSensorDev
-Input			: i_ptLightDistanceSensorDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterLightDistanceSensorDev(PT_LightDistanceSensorDevManage i_ptLightDistanceSensorDev)
{
	PT_LightDistanceSensorDevManage ptTmp;
	if (!g_ptLightDistanceSensorDevHead)
	{
		g_ptLightDistanceSensorDevHead   = i_ptLightDistanceSensorDev;
		g_ptLightDistanceSensorDevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptLightDistanceSensorDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptLightDistanceSensorDev;
		i_ptLightDistanceSensorDev->ptNext = NULL;
	}
}




