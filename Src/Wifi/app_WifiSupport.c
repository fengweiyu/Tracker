/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_Wifi.c
* Description		: 	RTC application operation center
* Created			: 	2016.09.27.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include"cBasicTypeDefine.h"
#include "app_WifiSupport.h"
#include "Config.h"
#include "Ucos_ii.h"
#include "main.h"

static PT_GeneralWifiDevManage g_ptGeneralWifiDevHead=NULL;

static void GeneralWifiInit();
static void GeneralWifiConfig(void);
static u8 GeneralWifiPowerOn();
static u8 GeneralWifiPowerOff();
static u8 GeneralWifiSendData(u8 *i_buf,u16 i_Len);
static u8 GeneralWifiRecvData();
static u8 GeneralWifiReadAccessPointData(WifiAccessPointSt *o_ptWifiAccessPoint);
static u8 GeneralWifiSetScanApReq();


static T_WifiAccessPoint g_tWifiAccessPoint[2]={0};
static u8   g_ucWifiAccessPointWriteFlag=FALSE;
static u8   g_ucWifiAccessPointWriteFifoLen=0;

T_WifiOpr const g_tWifi = {
	.Init					=GeneralWifiInit,
	.Config				=GeneralWifiConfig,
	.WifiSendData			=GeneralWifiSendData,
	.WifiRecvData			=GeneralWifiRecvData,
	.WifiPowerOn			=GeneralWifiPowerOn,
	.WifiPowerOff			=GeneralWifiPowerOff,
	.WifiReadApData		=GeneralWifiReadAccessPointData,
	.WifiSetScanApReq		=GeneralWifiSetScanApReq,
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
static void GeneralWifiInit()
{
	GeneralWifiDevInit();
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
static void GeneralWifiConfig(void)
{
	PT_GeneralWifiDevManage ptTmp = g_ptGeneralWifiDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralWifiDevHead Null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->GeneralWifiConfigDev();
			ptTmp = ptTmp->ptNext;
		}		
	}
}
/*****************************************************************************
-Fuction		: GeneralWifiPowerOn
-Description	: GeneralWifiPowerOn
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralWifiPowerOn()
{
	u8 ret=0;
	PT_GeneralWifiDevManage ptTmp = g_ptGeneralWifiDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralWifiPowerOn  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ptTmp->GeneralWifiPowerOnDev();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralWifiPowerOff
-Description	: GeneralWifiPowerOff
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralWifiPowerOff()
{
	u8 ret=0;
	PT_GeneralWifiDevManage ptTmp = g_ptGeneralWifiDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralWifiPowerOff  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ptTmp->GeneralWifiPowerOffDev();
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
static u8 GeneralWifiSendData(u8 *i_buf,u16 i_Len)
{
	u8 ret=0;
	PT_GeneralWifiDevManage ptTmp = g_ptGeneralWifiDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralWifiSendData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GeneralWifiSendDataDev(i_buf,i_Len);
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
static u8 GeneralWifiRecvData()
{
	u8 ret;
	PT_GeneralWifiDevManage ptTmp = g_ptGeneralWifiDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralWifiRecvData  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GeneralWifiRecvDataDev();
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralWifiWriteAccessPointData
-Description	: GeneralWifiWriteAccessPointData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralWifiWriteAccessPointData(WifiAccessPointSt *i_ptWifiAccessPoint)
{
	u8 ret=FALSE;
	if(NULL==i_ptWifiAccessPoint)
	{
		DebugPrintf(ERR"GeneralWifiWriteAccessPointData err\r\n");
		ret=FALSE;
	}
	else 
	{
		OSSchedLock();	
		if(0==g_ucWifiAccessPointWriteFifoLen)
		{
			memset(&g_tWifiAccessPoint[0],0,sizeof(WifiAccessPointSt));
			memcpy(&g_tWifiAccessPoint[0],i_ptWifiAccessPoint,sizeof(WifiAccessPointSt));
			g_ucWifiAccessPointWriteFifoLen++;
		}
		else if(1==g_ucWifiAccessPointWriteFifoLen)
		{
			memset(&g_tWifiAccessPoint[1],0,sizeof(WifiAccessPointSt));
			memcpy(&g_tWifiAccessPoint[1],i_ptWifiAccessPoint,sizeof(WifiAccessPointSt));			
			g_ucWifiAccessPointWriteFifoLen++;
		}
		else//如果大于等于2，则覆盖0的，新的写入1
		{
			memset(&g_tWifiAccessPoint[0],0,sizeof(WifiAccessPointSt));
			memcpy(&g_tWifiAccessPoint[0],&g_tWifiAccessPoint[1],sizeof(WifiAccessPointSt));			
			memset(&g_tWifiAccessPoint[1],0,sizeof(WifiAccessPointSt));
			memcpy(&g_tWifiAccessPoint[1],i_ptWifiAccessPoint,sizeof(WifiAccessPointSt));			
		}
		OSSchedUnlock();
		
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralWifiReadGprsData
-Description	: GeneralWifiReadGprsData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/27   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralWifiReadAccessPointData(WifiAccessPointSt *o_ptWifiAccessPoint)
{
	u8 ret=FALSE;
	u8 ucWifiAccessPointWriteLen=0;	
	OSSchedLock();	
	ucWifiAccessPointWriteLen=g_ucWifiAccessPointWriteFifoLen;
	OSSchedUnlock();
	if(0==ucWifiAccessPointWriteLen)
	{
		DebugPrintf(ERR"WifiReadAccessPointData err,wifi fifo empty\r\n");
		OSSchedLock();	
		memset(o_ptWifiAccessPoint,0,sizeof(WifiAccessPointSt));//多任务的原因导致无法在外面初始化
		OSSchedUnlock();
		ret=FALSE;
	}
	else
	{
		OSSchedLock();
		if(0==g_ucWifiAccessPointWriteFifoLen)
		{
			DebugPrintf(ERR"WifiReadAccessPointData err,wifi fifo empty1\r\n");
			memset(o_ptWifiAccessPoint,0,sizeof(WifiAccessPointSt));//所以在这里修改
			ret=FALSE;
		}
		else if(1==g_ucWifiAccessPointWriteFifoLen)
		{
			memcpy(o_ptWifiAccessPoint,&g_tWifiAccessPoint[0],sizeof(WifiAccessPointSt));
			g_ucWifiAccessPointWriteFifoLen--;
		}
		else
		{
			memcpy(o_ptWifiAccessPoint,&g_tWifiAccessPoint[0],sizeof(WifiAccessPointSt));
			memset(&g_tWifiAccessPoint[0],0,sizeof(WifiAccessPointSt));
			memcpy(&g_tWifiAccessPoint[0],&g_tWifiAccessPoint[1],sizeof(WifiAccessPointSt));			
			memset(&g_tWifiAccessPoint[1],0,sizeof(WifiAccessPointSt));
			g_ucWifiAccessPointWriteFifoLen--;
		}
		OSSchedUnlock();
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralWifiWriteSmsData
-Description	: GeneralWifiWriteSmsData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/27   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void  GeneralWifiGetAccessPointDataHook(u8 ucScanOkFlg, WifiAccessPointSt *i_ptWifiAccessPointInfo)
{
	if(IS_FALSE_BIT(ucScanOkFlg))
	{
		DebugPrintf(ERR"GeneralWifiGetAccessPointDataHook Err!\r\n");
	}
	else 
	{
	    GeneralWifiWriteAccessPointData(i_ptWifiAccessPointInfo); 
	    OSSemPost(ptOsSemUsedForGetWifiApSuccess);
	}
}

/*****************************************************************************
-Fuction		: GeneralWifiReadSmsData
-Description	: GeneralWifiReadSmsData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/27   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralWifiSetScanApReq()
{
	u8 ret=FALSE;
	OSTaskResume(WIFI_TASK_PRIORITY);
	ret=WifiApiScanApReq(GeneralWifiGetAccessPointDataHook);
	if(FALSE==ret)
	{
		DebugPrintf(ERR"GeneralWifiSetScanApReq Err!\r\n");
	}
	else
	{
		DebugPrintf("GeneralWifiSetScanApReq ok!\r\n");
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RegisterGeneralWifiDev
-Description	: RegisterGeneralWifiDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterGeneralWifiDev(PT_GeneralWifiDevManage i_ptGeneralWifiDev)
{
	PT_GeneralWifiDevManage ptTmp;
	if (!g_ptGeneralWifiDevHead)
	{
		g_ptGeneralWifiDevHead   = i_ptGeneralWifiDev;
		g_ptGeneralWifiDevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptGeneralWifiDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptGeneralWifiDev;
		i_ptGeneralWifiDev->ptNext = NULL;
	}
}


