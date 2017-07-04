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
static u8 SaveCommonPara(T_CommonPara i_CommonPara);
static u8 CommonParaInit();
static void CommonParaDeInit();
static u8 SetCommonPara(ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len);
static u8 GetCommonPara(ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);

static const char *g_strSoftwareVision="PT-810-20DC01-V1.0.9";
static const char *g_strHardwareVision="PT-810 C 20170426";
static T_CommonPara g_tCommonPara={0};
static T_ParameterManage g_tCommonParaOpr={
	.name="CommonPara",
	.ParaOprInit		=CommonParaInit,
	.ParaOprDeInit		=CommonParaDeInit,
	.SetParaOpr		=SetCommonPara,
	.GetParaOpr		=GetCommonPara
};
/*****************************************************************************
-Fuction		: RegisterCommonPara
-Description	: RegisterCommonPara
-Input			: 
-Output 		:   
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterCommonPara()
{
	RegisterParaOpr(&g_tCommonParaOpr);
}
/*****************************************************************************
-Fuction		: CommonParaInit
-Description	: CommonParaInit
-Input			: 
-Output 		:   
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
 static u8 CommonParaInit(T_AllSysPara * i_ptAllSysPara)
{
	u8 ret=FALSE;
	if(NULL==i_ptAllSysPara)
	{
	}
	else
	{
		OSSchedLock();
		memcpy(&g_tCommonPara,&i_ptAllSysPara->tCommonPara,sizeof(g_tCommonPara));
		OSSchedUnlock();
		ret=TRUE;
	}
	OSSchedLock();
	memset(g_tCommonPara.HardWareVision,0,sizeof(g_tCommonPara.HardWareVision));
	strncpy(g_tCommonPara.HardWareVision,g_strHardwareVision,HAEDWARE_VISION_LEN);
	memset(g_tCommonPara.SoftWareVision,0,sizeof(g_tCommonPara.SoftWareVision));
	strncpy(g_tCommonPara.SoftWareVision,g_strSoftwareVision,SOFTWARE_VISION_LEN);	
	OSSchedUnlock();
	return ret;
}
/*****************************************************************************
-Fuction		: CommonParaDeInit
-Description	: CommonParaDeInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void CommonParaDeInit()
{
	OSSchedLock();
	g_tCommonPara.ReportInterval=1800;
	g_tCommonPara.TrackerLanguage=CHINESE_LANGUAGE;
	memset(g_tCommonPara.HardWareVision,0,sizeof(g_tCommonPara.HardWareVision));
	strncpy(g_tCommonPara.HardWareVision,g_strHardwareVision,HAEDWARE_VISION_LEN);
	memset(g_tCommonPara.SoftWareVision,0,sizeof(g_tCommonPara.SoftWareVision));
	strncpy(g_tCommonPara.SoftWareVision,g_strSoftwareVision,SOFTWARE_VISION_LEN);
	OSSchedUnlock();
	SaveCommonPara(g_tCommonPara);
}

/*****************************************************************************
-Fuction		: SetCommonPara
-Description	: SetCommonPara
-Input			: i_SubName		i_Buf	i_Len
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SetCommonPara(ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len)
{
	u8 ret=FALSE;
	ParameterEnum SubCommonParaName=i_SubName;
	switch (SubCommonParaName)
	{
		case ReportInterval:
		{
			if((NULL==i_Buf)|| (i_Len>4))
			{
				ret=FALSE;
			}
			else
			{
				if(2==i_Len)
				{
					OSSchedLock();
					g_tCommonPara.ReportInterval=Lsb2U8ToU16(i_Buf);
					OSSchedUnlock();
					ret=TRUE;
				}
				else if(4==i_Len)
				{
					OSSchedLock();
					g_tCommonPara.ReportInterval=Lsb4U8ToU32(i_Buf);
					OSSchedUnlock();
					ret=TRUE;
				}
				else
				{
					ret=FALSE;
				}	
			}
			break;
		}		
		case TrackerLanguage:
		{
			if((NULL==i_Buf)|| (i_Len>1))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				g_tCommonPara.TrackerLanguage=i_Buf[0];
				OSSchedUnlock();
				ret=TRUE;			
			}
			break;
		}
		default :
		{
			DebugPrintf(ERR"CommonPara not found!\r\n");
			ret=FALSE;
			break;
		}
	}
	if(TRUE==ret)
	{
		ret&=SaveCommonPara(g_tCommonPara);
	}	
	else
	{
		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetCommonPara
-Description	: GetCommonPara
-Input			: i_SubName
-Output 		: o_buf  	o_Len
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetCommonPara(ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len)
{
	u8 ret=FALSE;
	ParameterEnum SubCommonParaName=i_SubName;
	switch (SubCommonParaName)
	{
		case ReportInterval:
		{
			OSSchedLock();
			*o_Len=4;
			LsbU32To4U8(o_Buf,g_tCommonPara.ReportInterval);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}		
		case TrackerLanguage:
		{
			OSSchedLock();
			*o_Len=1;
			o_Buf[0]=g_tCommonPara.TrackerLanguage;
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case HardWareVision:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tCommonPara.HardWareVision,HAEDWARE_VISION_LEN);
			memcpy(o_Buf,g_tCommonPara.HardWareVision,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case SoftWareVision:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tCommonPara.SoftWareVision,SOFTWARE_VISION_LEN);
			memcpy(o_Buf,g_tCommonPara.SoftWareVision,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		default :
		{
			DebugPrintf(ERR"CommonPara not found!\r\n");
			ret=FALSE;
			break;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SetCommonPara
-Description	: SetCommonPara
-Input			: i_CommonPara
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SaveCommonPara(T_CommonPara i_CommonPara)
{
	u8 ret=FALSE;
	OSSchedLock();
	memcpy(&g_tAllSysPara.tCommonPara,&i_CommonPara,sizeof(T_CommonPara));	
	OSSchedUnlock();
	SpiFlashStrctWrite(&g_tAllSysPara,
					sizeof(g_tAllSysPara),
					FLASH_ADDR_SYS_PARA_MAIN,
					FLASH_ADDR_SYS_PARA_BACK);
	ret=TRUE;
	return ret;

}


