/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 ParameterManage.c
* Description		: 	ParameterManage application operation center
* Created			: 	2016.9.7.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "ParameterManage.h"
#include "Ucos_ii.h"
#include "Config.h"
#include "Flash.h"
static u8 AlramParaInit();
static void AlramParaDeInit();
static u8 SetAlramPara(ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len);
static u8 GetAlramPara(ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);
static u8 SaveAlramPara(T_AlarmPara i_AlramPara);

static T_AlarmPara g_tAlarmPara={0};
static T_ParameterManage g_tAlarmParaOpr={
	.name="AlarmPara",
	.ParaOprInit		=AlramParaInit,
	.ParaOprDeInit		=AlramParaDeInit,
	.SetParaOpr		=SetAlramPara,
	.GetParaOpr		=GetAlramPara
};
/*****************************************************************************
-Fuction		: RegisterAlramPara
-Description	: RegisterAlramPara
-Input			: 
-Output 		:   
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterAlramPara()
{
	RegisterParaOpr(&g_tAlarmParaOpr);
}
/*****************************************************************************
-Fuction		: AlramParaInit
-Description	: AlramParaInit
-Input			: 
-Output 		:   
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
 static u8 AlramParaInit(T_AllSysPara * i_ptAllSysPara)
{
	u8 ret=FALSE;
	if(NULL==i_ptAllSysPara)
	{
	}
	else
	{
		OSSchedLock();
		memcpy(&g_tAlarmPara,&i_ptAllSysPara->tAlarmPara,sizeof(g_tAlarmPara));
		OSSchedUnlock();
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: AlramParaDeInit
-Description	: AlramParaDeInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void AlramParaDeInit()
{
	OSSchedLock();
	g_tAlarmPara.CutTrackerAlarmSwitch=1;
	g_tAlarmPara.DropTrackerAlarmSwitch=1;
	g_tAlarmPara.LowPowerAlarmSwitch=1;
	g_tAlarmPara.SmsAlarmSwitch=0;
	OSSchedUnlock();
	
	SaveAlramPara(g_tAlarmPara);
}

/*****************************************************************************
-Fuction		: SetAlramPara
-Description	: SetAlramPara
-Input			: i_SubName		i_Buf	i_Len
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SetAlramPara(ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len)
{
	u8 ret=FALSE;
	ParameterEnum SubAlramParaName=i_SubName;
	switch (SubAlramParaName)
	{
		case CutTrackerAlarmSwitch:
		{
			if((NULL==i_Buf)|| (i_Len>1))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				g_tAlarmPara.CutTrackerAlarmSwitch=i_Buf[0];
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}		
		case DropTrackerAlarmSwitch:
		{
			if((NULL==i_Buf)|| (i_Len>1))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				g_tAlarmPara.DropTrackerAlarmSwitch=i_Buf[0];
				OSSchedUnlock();
				ret=TRUE;			
			}
			break;
		}
		case LowPowerAlarmSwitch:
		{
			if((NULL==i_Buf)|| (i_Len>1))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				g_tAlarmPara.LowPowerAlarmSwitch=i_Buf[0];
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}
		case SmsAlarmSwitch:
		{
			if((NULL==i_Buf)|| (i_Len>1))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				g_tAlarmPara.SmsAlarmSwitch=i_Buf[0];
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}
		default :
		{
			DebugPrintf(ERR"AlramPara not found!\r\n");
			ret=FALSE;
			break;
		}
	}
	if(TRUE==ret)
	{
		ret&=SaveAlramPara(g_tAlarmPara);
	}	
	else
	{
		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetAlramPara
-Description	: GetAlramPara
-Input			: i_SubName
-Output 		: o_buf  	o_Len
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetAlramPara(ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len)
{
	u8 ret=FALSE;
	ParameterEnum SubAlramParaName=i_SubName;
	switch (SubAlramParaName)
	{
		case CutTrackerAlarmSwitch:
		{
			OSSchedLock();
			*o_Len=1;
			o_Buf[0]=g_tAlarmPara.CutTrackerAlarmSwitch;
			OSSchedUnlock();
			ret=TRUE;
			break;
		}		
		case DropTrackerAlarmSwitch:
		{
			OSSchedLock();
			*o_Len=1;
			o_Buf[0]=g_tAlarmPara.DropTrackerAlarmSwitch;
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case LowPowerAlarmSwitch:
		{
			OSSchedLock();
			*o_Len=1;
			o_Buf[0]=g_tAlarmPara.LowPowerAlarmSwitch;
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case SmsAlarmSwitch:
		{
			OSSchedLock();
			*o_Len=1;
			o_Buf[0]=g_tAlarmPara.SmsAlarmSwitch;
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		default :
		{
			DebugPrintf(ERR"AlramPara not found!\r\n");
			ret=FALSE;
			break;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SetAlramPara
-Description	: SetAlramPara
-Input			: i_AlramPara
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SaveAlramPara(T_AlarmPara i_AlramPara)
{
	u8 ret=FALSE;
	OSSchedLock();
	memcpy(&g_tAllSysPara.tAlarmPara,&i_AlramPara,sizeof(T_AlarmPara));
	SpiFlashStrctWrite(&g_tAllSysPara,
					sizeof(g_tAllSysPara),
					FLASH_ADDR_SYS_PARA_MAIN,
					FLASH_ADDR_SYS_PARA_BACK);
	OSSchedUnlock();
	ret=TRUE;
	return ret;
}


