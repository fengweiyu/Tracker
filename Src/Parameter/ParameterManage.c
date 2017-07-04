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
#include "CBasicTools.h"
#include "ParameterManage.h"
#include "Flash.h"

static PT_ParameterManage g_tParameterManageHead=NULL;

static void RegisterAllNeedPara();
static u8 ParaInitOpr();
static void ParaDeInitOpr();
static void ParaDeInitExceptNetParaOpr();
static u8 SetParaOpr(char *i_Name,ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len);
static u8 GetParaOpr(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);

T_AllSysPara g_tAllSysPara={0};

T_ParameterOpr const g_tParameterOpr={
	.RegisterPara				=RegisterAllNeedPara,
	.ParaInit					=ParaInitOpr,
	.ParaDeInit				=ParaDeInitOpr,
	.ParaDeInitExceptNetPara	=ParaDeInitExceptNetParaOpr,
	.SetPara					=SetParaOpr,
	.GetPara					=GetParaOpr
};
/*****************************************************************************
-Fuction		: RegisterParaOpr
-Description	: RegisterParaOpr
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void RegisterAllNeedPara()
{
	RegisterSmsPara();
	RegisterAlramPara();
	RegisterCommonPara();
	RegisterNetPara();
}
/*****************************************************************************
-Fuction		: ParaInitOpr
-Description	: ParaInitOpr
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 ParaInitOpr()
{
	u8 ret=FALSE;
	PT_ParameterManage ptTmp = g_tParameterManageHead;
	if(FALSE==SpiFlashStrctRead(&g_tAllSysPara,
									sizeof(g_tAllSysPara),
									FLASH_ADDR_SYS_PARA_MAIN,
									FLASH_ADDR_SYS_PARA_BACK))

	{
		ParaDeInitOpr();
	}
	else
	{
		if(NULL==ptTmp)
		{
			DebugPrintf(ERR"ParameterManageHead Null\r\n");
		}
		else
		{
			ret=TRUE;
			while (ptTmp)
			{
				ret&=ptTmp->ParaOprInit(&g_tAllSysPara);
				ptTmp = ptTmp->ptNext;
			}		
		}
	}
	return ret;
}

/*****************************************************************************
-Fuction		: ParaDeInitOpr
-Description	: ParaDeInitOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void ParaDeInitOpr()
{
	PT_ParameterManage ptTmp = g_tParameterManageHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"ParameterManageHead ParaDeInitOpr Null\r\n");

	}
	else
	{
		while (ptTmp)
		{
			ptTmp->ParaOprDeInit();
			ptTmp = ptTmp->ptNext;
		}
	}
}
/*****************************************************************************
-Fuction		: ParaDeInitOpr
-Description	: ParaDeInitOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void ParaDeInitExceptNetParaOpr()
{
	PT_ParameterManage ptTmp = g_tParameterManageHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"ParameterManageHead ParaDeInitOpr Null\r\n");

	}
	else
	{
		while (ptTmp)
		{
			if(0==strncmp(ptTmp->name,"NetPara",strnlen(ptTmp->name,MAX_NAME_SIZE)))
			{
				DebugPrintf(CRIT"ExceptDeInitParaName:%s\r\n",ptTmp->name);
			}
			else
			{
				ptTmp->ParaOprDeInit();
			}
			ptTmp = ptTmp->ptNext;
		}
	}
}

/*****************************************************************************
-Fuction		: SetParaOpr
-Description	: SetParaOpr
-Input			: i_Name	i_SubName		i_Buf	i_Len
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SetParaOpr(char *i_Name,ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len)
{
	u8 ret=FALSE;
	PT_ParameterManage ptTmp = g_tParameterManageHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"ParameterManageHead SetParaOpr Null\r\n");
		ret=FALSE;
	}
	else
	{		
		while (ptTmp)
		{
			if(0==strncmp(ptTmp->name,i_Name,strnlen(ptTmp->name,MAX_NAME_SIZE)))
			{
				ret=ptTmp->SetParaOpr(i_SubName,i_Buf,i_Len);
				break;
			}
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}

/*****************************************************************************
-Fuction		: GetParaOpr
-Description	: GetParaOpr
-Input			: i_Name 	i_SubName
-Output 		: o_Buf		o_Len
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetParaOpr(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len)
{
	u8 ret=FALSE;
	PT_ParameterManage ptTmp = g_tParameterManageHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"ParameterManageHead GetParaOpr Null\r\n");
		ret=FALSE;
	}
	else
	{		
		while (ptTmp)
		{
			if(0==strncmp(ptTmp->name,i_Name,strnlen(ptTmp->name,MAX_NAME_SIZE)))
			{
				ret=ptTmp->GetParaOpr(i_SubName,o_Buf,o_Len);
				break;
			}
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RegisterParaOpr
-Description	: RegisterParaOpr
-Input			: i_ptParaOpr
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterParaOpr(PT_ParameterManage i_ptParaOpr)
{
	PT_ParameterManage ptTmp;
	if (!g_tParameterManageHead)
	{
		g_tParameterManageHead   = i_ptParaOpr;
		g_tParameterManageHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_tParameterManageHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptParaOpr;
		i_ptParaOpr->ptNext = NULL;
	}
}
