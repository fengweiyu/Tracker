/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_UsbManage.c
* Description		: 	RTC application operation center
* Created			: 	2016.08.29.
* Author			: 	Yu Weifeng
* Function List 		: 	app_UsbManage
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "cBasicTools.h"
#include "app_UsbManage.h"
#include "Config.h"
#include <stdarg.h>
#include "main.h"

static PT_UsbManage g_ptUsbOprHead=NULL;
static PT_UsbManage g_ptUsbDefaultOpr=NULL;

static void UsbInit();
static u8 UsbOprConfig();
static u8 UsbOprDeConfig();
static u8 UsbOprVbusSetFallExti();
static u8 UsbSendDataBuf(u8 *i_buf,u16 i_Len);
static u8 UsbRecvDataBuf(u8 *i_buf,u16 i_Len);
static u8 UsbGetDataBuf(u8 *o_buf,u16 *o_Len);
static u8 UsbGetStateOpr(u8 *o_pucUsbState);
static u8 UsbDbgPrintf(const char *pcFormat, ...);
static u8 UsbDbgSend(u8 *i_buf,u16 i_Len);
static u8 GetUsbOpration(char *i_pcName,PT_UsbManage o_ptUsbOpr);

T_UsbOpr const g_tUsb={
	.Init				=UsbInit,
	.Config			=UsbOprConfig,
	.DeConfig			=UsbOprDeConfig,
	.VbusSetFallExti	=UsbOprVbusSetFallExti,
	.SendDataBuf		=UsbSendDataBuf,
	.RecvDataBuf		=UsbRecvDataBuf,
	.GetDataBuf		=UsbGetDataBuf,
	.DbgPrintf			=UsbDbgPrintf,
	.DbgSend			=UsbDbgSend,
	.GetUsbOpr		=GetUsbOpration,
	.GetUsbState		=UsbGetStateOpr,
};
/*****************************************************************************
-Fuction		: UsbInit
-Description	: UsbInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void UsbInit()
{
	UsbVirtualComInit();
}

/*****************************************************************************
-Fuction		: UsbOprConfig
-Description	: UsbOprConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 UsbOprConfig()
{
	u8 ret=FALSE;
	PT_UsbManage ptTmp = g_ptUsbOprHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->UsbConfig();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}

/*****************************************************************************
-Fuction		: UsbOprConfig
-Description	: UsbOprConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 UsbOprDeConfig()
{
	u8 ret=FALSE;
	PT_UsbManage ptTmp = g_ptUsbOprHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->UsbDeConfig();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: UsbOprVbusSetFallExti
-Description	: UsbOprVbusSetFallExti
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 UsbOprVbusSetFallExti()
{
	u8 ret=FALSE;
	PT_UsbManage ptTmp = g_ptUsbOprHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->UsbVbusSetFallExti();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: UsbSendDataBuf
-Description	: UsbSendDataBuf
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 UsbSendDataBuf(u8 *i_buf,u16 i_Len)
{
	u8 ret=FALSE;
	PT_UsbManage ptTmp = g_ptUsbOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"UsbOprHead UsbSendDataBuf Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->UsbSendData(i_buf,i_Len);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: UsbRecvDataBuf
-Description	: UsbRecvDataBuf
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 UsbRecvDataBuf(u8 *i_buf,u16 i_Len)
{
	u8 ret=FALSE;
	PT_UsbManage ptTmp = g_ptUsbOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"g_ptUsbOprHead UsbRecvDataBuf Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->UsbRecvData(i_buf,i_Len);
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: UsbRecvDataBuf
-Description	: UsbRecvDataBuf
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 UsbGetDataBuf(u8 *o_buf,u16 *o_Len)
{
	u8 ret=FALSE;
	PT_UsbManage ptTmp = g_ptUsbOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"g_ptUsbOprHead UsbGetDataBuf Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->UsbGetData(o_buf,o_Len);
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: UsbGetStateOpr
-Description	: UsbGetStateOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 UsbGetStateOpr(u8 *o_pucUsbState)
{
	u8 ret=FALSE;
	PT_UsbManage ptTmp = g_ptUsbOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"g_ptUsbOprHead UsbGetStateOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->UsbGetState(o_pucUsbState);
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}

/*****************************************************************************
-Fuction		: GetUsbOpration
-Description	: GetUsbOpration
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetUsbOpration(char *i_pcName,PT_UsbManage o_ptUsbOpr)
{
	u8 ret=FALSE;
	PT_UsbManage ptTmp = g_ptUsbOprHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"g_ptUsbOprHead GetUsbOpration Null\r\n");
		ret=FALSE;
	}
	else
	{		
		while (ptTmp)
		{
			if(0==strncmp(ptTmp->name,i_pcName,strnlen(ptTmp->name,MAX_NAME_SIZE)))
			{
				ret=TRUE;
				o_ptUsbOpr=ptTmp;
				break;
			}
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: UsbDbgPrintf
-Description	: UsbDbgPrintf
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 UsbDbgPrintf(const char *pcFormat, ...)
{
	u8 ret=0;
	u8 ucErr=0;
	static u8 strTmpBuf[280]={0};//没加锁所以打印会出现丢失打印数据的情况
	u8 *pcTmp;
	va_list tArg;
	u8 ucNum;
	u8 ucDebugLevel = DEFAULT_DBGLEVEL;

	OSSchedLock();
	memset(strTmpBuf,0,sizeof(strTmpBuf));
	/* 可变参数的处理, 抄自glibc的printf函数 */
	va_start (tArg, pcFormat);
	ucNum = vsnprintf (strTmpBuf, (sizeof(strTmpBuf)-1),pcFormat, tArg);//留一位放\0
	va_end (tArg);
	strTmpBuf[ucNum] = '\0';


	pcTmp = strTmpBuf;
	
	/* 根据打印级别决定是否打印 */
	if ((strTmpBuf[0] == '<') && (strTmpBuf[2] == '>'))
	{
		ucDebugLevel = strTmpBuf[1] - '0';
		if (ucDebugLevel >= 0 && ucDebugLevel <= 9)
		{
			pcTmp = strTmpBuf + 3;
		}
		else
		{
			ucDebugLevel = DEFAULT_DBGLEVEL;
		}
	}
	if (ucDebugLevel > SAVE_IN_FLASH_DBG_LEVEL_LIMIT)
	{
		ret=FALSE;
	}
	else
	{		
		SystemLogSaveInFlash(pcTmp,strnlen(pcTmp,sizeof(strTmpBuf)));
		ret=TRUE;
	}
	if (ucDebugLevel > DBG_LEVEL_LIMIT)
	{
		ret=FALSE;
	}
	else
	{
		OSFlagAccept(pstOsFlagForAppUse, VCP_DEBUG_DISABLE_FLAG, OS_FLAG_WAIT_CLR_ALL, &ucErr);	
		if(OS_ERR_NONE!=ucErr)
		{	
		}
		else
		{
			UsbSendDataBuf(pcTmp,strnlen(pcTmp,sizeof(strTmpBuf)));
		}
		ret=TRUE;
	}
	OSSchedUnlock();
	return ret;	
}
/*****************************************************************************
-Fuction		: UsbSendDataBuf
-Description	: UsbSendDataBuf
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 UsbDbgSend(u8 *i_buf,u16 i_Len)
{
	u8 ret=FALSE;
	u8 ucErr;
	OSFlagAccept(pstOsFlagForAppUse, VCP_DEBUG_DISABLE_FLAG, OS_FLAG_WAIT_CLR_ALL, &ucErr);	
	if(OS_ERR_NONE!=ucErr)
	{	
	}
	else
	{
		ret=UsbSendDataBuf(i_buf,i_Len);
	}
	return ret;
}

/*****************************************************************************
-Fuction		: RegisterUsb
-Description	: RegisterUsb
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterUsb(PT_UsbManage i_ptUsbOpr)
{
	PT_UsbManage ptTmp;
	if (!g_ptUsbOprHead)
	{
		g_ptUsbOprHead   = i_ptUsbOpr;
		g_ptUsbOprHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptUsbOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptUsbOpr;
		i_ptUsbOpr->ptNext = NULL;
	}
}

