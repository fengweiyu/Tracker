/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_RTC.c
* Description		: 	RTC application operation center
* Created			: 	2016.08.29.
* Author			: 	Yu Weifeng
* Function List 		: 	RtcTimeInit
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include"cBasicTools.h"
#include "app_UsbVirtualCom.h"
#include "app_UsbManage.h"
#include "Config.h"
static PT_UsbVirtualComDevManage g_ptUsbVirtualComDevHead=NULL;

static u8 UsbVirtualComConfig(void);
static u8 UsbVirtualComDeConfig(void);
static u8 VcpVbusSetFallExti(void);
static u8 SendData(u8 *i_buf,u16 i_Len);
static u8 RecvData(u8 *i_buf,u16 i_Len);
static u8 GetData(u8 *o_buf,u16 *o_Len);
static u8 GetUsbState(u8 *o_pucUsbState);

static T_UsbManage g_tUsbVirtualCom ={
	.name="UsbVirtualCom",
	.UsbConfig		=UsbVirtualComConfig,
	.UsbDeConfig		=UsbVirtualComDeConfig,
	.UsbVbusSetFallExti	=VcpVbusSetFallExti,
	.UsbSendData		=SendData,
	.UsbRecvData		=RecvData,
	.UsbGetData		=GetData,
	.UsbGetState		=GetUsbState
};
/*****************************************************************************
-Fuction		: RegisterRtcTimeDev
-Description	: RegisterRtcTimeDev
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void UsbVirtualComInit()
{
	UsbVirtualComDevInit();
	RegisterUsb(&g_tUsbVirtualCom);
}
/*****************************************************************************
-Fuction		: RegisterRtcTimeDev
-Description	: RegisterRtcTimeDev
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 UsbVirtualComConfig(void)
{
	u8 ret=0;
	PT_UsbVirtualComDevManage ptTmp = g_ptUsbVirtualComDevHead;
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
-Fuction		: VcpVbusSetFallExti
-Description	: VcpVbusSetFallExti
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 VcpVbusSetFallExti(void)
{
	u8 ret=0;
	PT_UsbVirtualComDevManage ptTmp = g_ptUsbVirtualComDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->DevVbusSetFallExti();
			ptTmp = ptTmp->ptNext;
		}	
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RegisterRtcTimeDev
-Description	: RegisterRtcTimeDev
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 UsbVirtualComDeConfig(void)
{
	u8 ret=0;
	PT_UsbVirtualComDevManage ptTmp = g_ptUsbVirtualComDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->DevDeConfig();
			ptTmp = ptTmp->ptNext;
		}	
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: RegisterRtcTimeDev
-Description	: RegisterRtcTimeDev
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SendData(u8 *i_buf,u16 i_Len)
{
	u8 ret=0;
	PT_UsbVirtualComDevManage ptTmp = g_ptUsbVirtualComDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"UsbVirtualComDevHead SendData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevSendData(i_buf,i_Len);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetRtcTime
-Description	: GetRtcTime
-Input			: o_ptDatetime
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 RecvData(u8 *i_buf,u16 i_Len)
{
	u8 ret=0;
	PT_UsbVirtualComDevManage ptTmp = g_ptUsbVirtualComDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"UsbVirtualComDevHead RecvData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevRecvData(i_buf,i_Len);
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetRtcTime
-Description	: GetRtcTime
-Input			: o_ptDatetime
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetData(u8 *o_buf,u16 *o_Len)
{
	u8 ret=FALSE;
	PT_UsbVirtualComDevManage ptTmp = g_ptUsbVirtualComDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"UsbVirtualComDevHead GetData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevGetData(o_buf,o_Len);
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetUsbState
-Description	: GetUsbState
-Input			: o_ptDatetime
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetUsbState(u8 *o_pucUsbState)
{
	u8 ret=FALSE;
	PT_UsbVirtualComDevManage ptTmp = g_ptUsbVirtualComDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"UsbVirtualComDevHead GetUsbState Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevGetUsbState(o_pucUsbState);
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}

/*****************************************************************************
-Fuction		: RegisterRtcTimeDev
-Description	: RegisterRtcTimeDev
-Input			: i_ptRtcTimeDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterUsbVirtualComDev(PT_UsbVirtualComDevManage i_ptUsbVirtualComDev)
{
	PT_UsbVirtualComDevManage ptTmp;
	if (!g_ptUsbVirtualComDevHead)
	{
		g_ptUsbVirtualComDevHead   = i_ptUsbVirtualComDev;
		g_ptUsbVirtualComDevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptUsbVirtualComDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptUsbVirtualComDev;
		i_ptUsbVirtualComDev->ptNext = NULL;
	}
}

