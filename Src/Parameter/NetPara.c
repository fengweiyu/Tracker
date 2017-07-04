/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 NetPara.c
* Description		: 	NetPara application operation center
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
#include "Bsp.h"
#include "main.h"

static u8 SaveNetPara(T_NetPara i_NetPara);
static u8 NetParaInit();
static void NetParaDeInit();
static u8 SetNetPara(ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len);
static u8 GetNetPara(ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);

static T_NetPara g_tNetPara={0};
static T_ParameterManage g_tNetParaOpr={
	.name="NetPara",
	.ParaOprInit		=NetParaInit,
	.ParaOprDeInit		=NetParaDeInit,
	.SetParaOpr		=SetNetPara,
	.GetParaOpr		=GetNetPara
};
/*****************************************************************************
-Fuction		: RegisterNetPara
-Description	: RegisterNetPara
-Input			: 
-Output 		:   
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterNetPara()
{
	RegisterParaOpr(&g_tNetParaOpr);
}
/*****************************************************************************
-Fuction		: NetParaInit
-Description	: NetParaInit
-Input			: 
-Output 		:   
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
 static u8 NetParaInit(T_AllSysPara * i_ptAllSysPara)
{
	u8 ret=FALSE;
	if(NULL==i_ptAllSysPara)
	{
	}
	else
	{
		OSSchedLock();
		memcpy(&g_tNetPara,&i_ptAllSysPara->tNetPara,sizeof(g_tNetPara));
		OSSchedUnlock();
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: NetParaDeInit
-Description	: NetParaDeInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void NetParaDeInit()
{
	OSSchedLock();
	memset(g_tNetPara.Apn,0,sizeof(g_tNetPara.Apn));
	memset(&g_tNetPara.DailMode,0,sizeof(g_tNetPara.DailMode));
	memset(g_tNetPara.Domain,0,sizeof(g_tNetPara.Domain));
	memset(g_tNetPara.Ip,0,sizeof(g_tNetPara.Ip));
	memset(g_tNetPara.Pwd,0,sizeof(g_tNetPara.Pwd));
	memset(&g_tNetPara.Port,0,sizeof(g_tNetPara.Port));
	memset(g_tNetPara.UsrName,0,sizeof(g_tNetPara.UsrName));
	g_tNetPara.DailMode=DOMAIN_DIAL_MODE;
	g_tNetPara.Port=6088;
	strncpy(g_tNetPara.Ip,"54.179.149.239",IP_STR_LEN);
	strncpy(g_tNetPara.Domain,"www.livegpslocate.com",DOMAIN_STR_LEN);	
	OSSchedUnlock();
	
	SaveNetPara(g_tNetPara);
}

/*****************************************************************************
-Fuction		: SetNetPara
-Description	: SetNetPara
-Input			: i_SubName		i_Buf	i_Len
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SetNetPara(ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len)
{
	u8 ret=FALSE;
	ParameterEnum SubNetParaName=i_SubName;
	switch (SubNetParaName)
	{
		case Apn:
		{
			if((NULL==i_Buf)|| (i_Len>APN_STR_LEN))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(g_tNetPara.Apn,0,sizeof(g_tNetPara.Apn));
				memcpy(g_tNetPara.Apn,i_Buf,i_Len);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}		
		case DailMode:
		{
			if((NULL==i_Buf)|| (i_Len>sizeof(g_tNetPara.DailMode)))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(&g_tNetPara.DailMode,0,sizeof(g_tNetPara.DailMode));
				memcpy(&g_tNetPara.DailMode,i_Buf,i_Len);
				OSSchedUnlock();
				ret=TRUE;			
			}
			break;
		}
		case Domain:
		{
			if((NULL==i_Buf)|| (i_Len>DOMAIN_STR_LEN))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(g_tNetPara.Domain,0,sizeof(g_tNetPara.Domain));
				memcpy(g_tNetPara.Domain,i_Buf,i_Len);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}
		case Ip:
		{
			if((NULL==i_Buf)|| (i_Len>(IP_STR_LEN+1)))//由于协议过来正好是四个字节，所以这里加1
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(g_tNetPara.Ip,0,sizeof(g_tNetPara.Ip));
				memcpy(g_tNetPara.Ip,i_Buf,i_Len);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}
		case Port:
		{
			if((NULL==i_Buf)|| (i_Len>sizeof(g_tNetPara.Port)))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(&g_tNetPara.Port,0,sizeof(g_tNetPara.Port));
				g_tNetPara.Port=Lsb2U8ToU16(i_Buf);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}				
		case Pwd:
		{
			if((NULL==i_Buf)|| (i_Len>PASSWORD_STR_LEN))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(g_tNetPara.Pwd,0,sizeof(g_tNetPara.Pwd));
				memcpy(g_tNetPara.Pwd,i_Buf,i_Len);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}
		case UsrName:
		{
			if((NULL==i_Buf)|| (i_Len>USERNAME_STR_LEN))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(g_tNetPara.UsrName,0,sizeof(g_tNetPara.UsrName));
				memcpy(g_tNetPara.UsrName,i_Buf,i_Len);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}
		default :
		{
			DebugPrintf(ERR"NetPara not found!\r\n");
			ret=FALSE;
			break;
		}
	}
	if(TRUE==ret)
	{
		ret&=SaveNetPara(g_tNetPara);
	}	
	else
	{
		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetNetPara
-Description	: GetNetPara
-Input			: i_SubName
-Output 		: o_buf  	o_Len
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetNetPara(ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len)
{
	u8 ret=FALSE;
	ParameterEnum SubNetParaName=i_SubName;
	switch (SubNetParaName)
	{
		case Apn:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tNetPara.Apn,APN_STR_LEN);
			memcpy(o_Buf,g_tNetPara.Apn,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}		
		case DailMode:
		{
			OSSchedLock();
			*o_Len=sizeof(g_tNetPara.DailMode);
			memcpy(o_Buf,&g_tNetPara.DailMode,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case Domain:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tNetPara.Domain,DOMAIN_STR_LEN);
			memcpy(o_Buf,g_tNetPara.Domain,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case Ip:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tNetPara.Ip,IP_STR_LEN);
			memcpy(o_Buf,g_tNetPara.Ip,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case Port:
		{
			OSSchedLock();
			*o_Len=2;
			LsbU16To2U8(o_Buf,g_tNetPara.Port);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}				
		case Pwd:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tNetPara.Pwd,PASSWORD_STR_LEN);
			memcpy(o_Buf,g_tNetPara.Pwd,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case UsrName:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tNetPara.UsrName,USERNAME_STR_LEN);
			memcpy(o_Buf,g_tNetPara.UsrName,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		default :
		{
			DebugPrintf(ERR"NetPara not found!\r\n");
			ret=FALSE;
			break;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SetNetPara
-Description	: SetNetPara
-Input			: i_SubName		i_Buf	i_Len
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SaveNetPara(T_NetPara i_NetPara)
{
	u8 ret=FALSE;
	u8 ucErr=0;
	OSSchedLock();
	memcpy(&g_tAllSysPara.tNetPara,&i_NetPara,sizeof(T_NetPara));
	OSSchedUnlock();
	SpiFlashStrctWrite(&g_tAllSysPara,
					sizeof(g_tAllSysPara),
					FLASH_ADDR_SYS_PARA_MAIN,
					FLASH_ADDR_SYS_PARA_BACK);	
	ret=TRUE;
	return ret;
}



