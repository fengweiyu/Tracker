/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 TrackerProtocolManage.c
* Description		: 	TrackerProtocolManage application operation center
* Created			: 	2016.9.12.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "CBasicTools.h"
#include "ProtocolManage.h"
#include "Config.h"
static PT_ProtocolManage g_tProtocolManageHead=NULL;
static	 void ProtocolInitOpr();
static	 u8 ProtocolGetDataOpr(u8 i_ucProtocolUser,u8 *o_pucDataBuf,u16 *o_pwDataLen,u16 i_wDataBufMaxLen,const char *i_pcName);
static	 u8 ProtocolCheckDataOpr(u8 *i_pucDataBuf,u16 i_wDataLen,const char *i_pcName);
static	 u8 ProtocolAnalyseDataOpr(u8 i_ucProtocolUser,u8 *i_pucDataBuf,u16 i_wDataLen,T_HandleCmdAck *o_ptHandCmdAck,const char *i_pcName);
static u8 GetProtocolOpration(const char *i_pcName,PT_ProtocolManage *o_ptPtotocolOpr);

T_ProtocolOpr const g_tProtocol={
	.Init				=ProtocolInitOpr,
	.GetData			=ProtocolGetDataOpr,
	.CheckData		=ProtocolCheckDataOpr,
	.AnalyseData		=ProtocolAnalyseDataOpr,
	.GetProtocolOpr	=GetProtocolOpration
};

/*****************************************************************************
-Fuction		: RegisterTrackerProtocolSupport
-Description	: RegisterTrackerProtocolSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/08   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static	 void ProtocolInitOpr()
{
	TrackerProtocolInit();
	SmsProtocolInit();
}
/*****************************************************************************
-Fuction		: GetProtocolOpration
-Description	: GetProtocolOpration
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetProtocolOpration(const char *i_pcName,PT_ProtocolManage *o_ptPtotocolOpr)
{
	u8 ret=FALSE;
	PT_ProtocolManage ptTmp = g_tProtocolManageHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"g_tProtocolManageHead GetProtocolOpration Null\r\n");
		ret=FALSE;
	}
	else
	{		
		while (ptTmp)
		{
			if(0==strncmp(ptTmp->name,i_pcName,strnlen(ptTmp->name,MAX_NAME_SIZE)))
			{
				ret=TRUE;
				*o_ptPtotocolOpr=ptTmp;
				break;
			}
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}

/*****************************************************************************
-Fuction		: ProtocolGetDataOpr
-Description	: ProtocolGetDataOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/12   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static	 u8 ProtocolGetDataOpr(u8 i_ucProtocolUser,u8 *o_pucDataBuf,u16 *o_pwDataLen,u16 i_wDataBufMaxLen,const char *i_pcName)
{
	u8 ret=FALSE;
	PT_ProtocolManage ptTmp = g_tProtocolManageHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"ProtocolGetDataOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		while (ptTmp)
		{
			if(0==strncmp(ptTmp->name,i_pcName,strnlen(ptTmp->name,MAX_NAME_SIZE)))
			{
				ret=ptTmp->ProtocolGetData(i_ucProtocolUser,o_pucDataBuf,o_pwDataLen,i_wDataBufMaxLen);
				break;
			}
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;

}
/*****************************************************************************
-Fuction		: ProtocolCheckDataOpr
-Description	: ProtocolCheckDataOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/12   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static	 u8 ProtocolCheckDataOpr(u8 *i_pucDataBuf,u16 i_wDataLen,const char *i_pcName)
{
	u8 ret=FALSE;
	PT_ProtocolManage ptTmp = g_tProtocolManageHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"ProtocolCheckDataOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		while (ptTmp)
		{
			if(0==strncmp(ptTmp->name,i_pcName,strnlen(ptTmp->name,MAX_NAME_SIZE)))
			{
				ret=ptTmp->ProtocolCheckData(i_pucDataBuf,i_wDataLen);
				break;
			}
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;

}

/*****************************************************************************
-Fuction		: ProtocolAnalyseDataOpr
-Description	: ProtocolAnalyseDataOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/12   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static	 u8 ProtocolAnalyseDataOpr(u8 i_ucProtocolUser,u8 *i_pucDataBuf,u16 i_wDataLen,T_HandleCmdAck *o_ptHandCmdAck,const char *i_pcName)
{
	u8 ret=FALSE;
	PT_ProtocolManage ptTmp = g_tProtocolManageHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"ProtocolAnalyseDataOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			if(0==strncmp(ptTmp->name,i_pcName,strnlen(ptTmp->name,MAX_NAME_SIZE)))
			{
				ret=ptTmp->ProtocolAnalyseData(i_ucProtocolUser,i_pucDataBuf,i_wDataLen,o_ptHandCmdAck);
				break;
			}
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RegisterTrackerProtocolSupport
-Description	: RegisterTrackerProtocolSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/08   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterProtocol(PT_ProtocolManage i_ptProtocol)
{
	PT_ProtocolManage ptTmp;
	if (!g_tProtocolManageHead)
	{
		g_tProtocolManageHead   = i_ptProtocol;
		g_tProtocolManageHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_tProtocolManageHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptProtocol;
		i_ptProtocol->ptNext = NULL;
	}
}

