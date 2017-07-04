/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_Gsm.c
* Description		: 	RTC application operation center
* Created			: 	2016.09.27.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include"cBasicTypeDefine.h"
#include "app_Gsm.h"
#include "Config.h"
#include "TrackerProtocol.h"
#include "Ucos_ii.h"
#include "ParameterManage.h"
#include "main.h"
#include "app_gsm_Oper_Expand.h"

static PT_GeneralGsmDevManage g_ptGeneralGsmDevHead=NULL;

static void GeneralGsmInit();
static void GeneralGsmConfig(void);
static u8 GeneralGsmPowerOn();
static u8 GeneralGsmPowerOff();
static u8 GeneralGsmReset();
static u8 GeneralGsmSendData(u8 *i_buf,u16 i_Len);
static u8 GeneralGsmRecvData();
static u8 GeneralGsmWriteGprsData(u8 *i_pucDataBuf,u16 i_wDataLen);
static u8 GeneralGsmReadGprsData(u8 *o_pucDataBuf,u16 *o_wDataLen);
static u8 GeneralGsmWriteSmsData(sms_info_t *i_ptDataBuf,u16 i_wPDCS);
static u8 GeneralGsmReadSmsData(T_SmsInfo *o_ptSmsInfo);
static u8 GeneralGsmReadLbsLocationData(CellTowerInfoSt *o_ptCellTowerInfo);
static u8 GeneralGsmSetLbsLocationUpdateReq();
static u8 GeneralGsmCheckGsmState();
static u8 GeneralGsmWakeUp();
static u8 GeneralGsmEnterSleepMode();

static u8   g_ucGprsDataBuf[TRACKER_PROTOCOL_MAX_LEN]={0};
static u16 g_wGprsDataBufLen=TRACKER_PROTOCOL_MAX_LEN;
static T_SmsInfo g_tSmsData={0};
static CellTowerInfoSt g_tCellTowerInfo={0};
static u8   g_ucGprsDataWriteFlag=FALSE;
static u8   g_ucSmsDataWriteFlag=FALSE;
static u8   g_ucCellTowerInfoWriteFlag=FALSE;

T_GsmOpr const g_tGsm = {
	.Init							=GeneralGsmInit,
	.Config						=GeneralGsmConfig,
	.GsmSendData					=GeneralGsmSendData,
	.GsmRecvData					=GeneralGsmRecvData,
	.GsmReset					=GeneralGsmReset,
	.GsmPowerOn					=GeneralGsmPowerOn,
	.GsmPowerOff					=GeneralGsmPowerOff,
	.GsmWriteGprsData				=GeneralGsmWriteGprsData,
	.GsmReadGprsData				=GeneralGsmReadGprsData,
	.GsmWriteSmsData				=GeneralGsmWriteSmsData,
	.GsmReadSmsData				=GeneralGsmReadSmsData,
	.GsmSetLbsLocationUpdateReq	=GeneralGsmSetLbsLocationUpdateReq,
	.GsmReadLbsLocationData		=GeneralGsmReadLbsLocationData,
	.GsmCheckGsmState			=GeneralGsmCheckGsmState,
	.GsmWakeUp					=GeneralGsmWakeUp,
	.GsmEnterSleepMode			=GeneralGsmEnterSleepMode
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
static void GeneralGsmInit()
{
	GeneralGsmDevInit();
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
static void GeneralGsmConfig(void)
{
	PT_GeneralGsmDevManage ptTmp = g_ptGeneralGsmDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGsmDevHead Null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->GeneralGsmConfigDev();
			ptTmp = ptTmp->ptNext;
		}		
	}
}
/*****************************************************************************
-Fuction		: GeneralGsmPowerOn
-Description	: GeneralGsmPowerOn
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmPowerOn()
{
	u8 ret=0;
	PT_GeneralGsmDevManage ptTmp = g_ptGeneralGsmDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGsmPowerOn  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ptTmp->GeneralGsmPowerOnDev();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGsmPowerOff
-Description	: GeneralGsmPowerOff
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmPowerOff()
{
	u8 ret=0;
	PT_GeneralGsmDevManage ptTmp = g_ptGeneralGsmDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGsmPowerOff  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ptTmp->GeneralGsmPowerOffDev();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGsmWakeUp
-Description	: GeneralGsmWakeUp
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/12/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmWakeUp()
{
	u8 ret=0;
	PT_GeneralGsmDevManage ptTmp = g_ptGeneralGsmDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGsmPowerOn  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ptTmp->GeneralGsmWakeUpDev();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGsmEnterSleepMode
-Description	: GeneralGsmEnterSleepMode
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/12/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmEnterSleepMode()
{
	u8 ret=0;
	PT_GeneralGsmDevManage ptTmp = g_ptGeneralGsmDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGsmPowerOn  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ptTmp->GeneralGsmEnterSleepModeDev();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}

/*****************************************************************************
-Fuction		: GeneralGsmPowerOff
-Description	: GeneralGsmPowerOff
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmReset()
{
	u8 ret=0;
	PT_GeneralGsmDevManage ptTmp = g_ptGeneralGsmDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGsmPowerOff  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ptTmp->GeneralGsmResetDev();
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
static u8 GeneralGsmSendData(u8 *i_buf,u16 i_Len)
{
	u8 ret=0;
	PT_GeneralGsmDevManage ptTmp = g_ptGeneralGsmDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGsmSendData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GeneralGsmSendDataDev(i_buf,i_Len);
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
static u8 GeneralGsmRecvData()
{
	u8 ret;
	PT_GeneralGsmDevManage ptTmp = g_ptGeneralGsmDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGsmRecvData  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GeneralGsmRecvDataDev();
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGsmCheckGsmState
-Description	: GeneralGsmCheckGsmState
-Input			: o_ptDatetime
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmCheckGsmState()
{
	u8 ret;
	PT_GeneralGsmDevManage ptTmp = g_ptGeneralGsmDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"GeneralGsmCheckGsmState  Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GeneralGsmCheckGsmStateDev();
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}

/*****************************************************************************
-Fuction		: GeneralGsmWriteGprsData
-Description	: GeneralGsmWriteGprsData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/27   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmWriteGprsData(u8 *i_pucDataBuf,u16 i_wDataLen)
{
	u8 ret=FALSE;
	u8 ucDeviceID[DEVICE_ID_LEN]={0};	
	u16 wDevicIdLen=0;
	u8 ucTrackerProtocolLast=0;
	u8 ucTrackerProtocolSecondLast=0;
	u16 wGpsDataLen=0;
	u8 ucGprsDataWritedFlag=FALSE;
	static enum
	{
		GPRS_DAT_RX_IDLE,
		GPRS_DAT_RX_RXING,
		GPRS_DAT_RX_WAIT_END_1,
		GPRS_DAT_RX_WAIT_END_2
	}eGprsDataRx; 
	static u16 wPackageDataLen=0;
	
	OSSchedLock();
	ucGprsDataWritedFlag=g_ucGprsDataWriteFlag;//判断是否已经有数据了
	OSSchedUnlock();
	if((NULL==i_pucDataBuf)||IS_TRUE_BIT(ucGprsDataWritedFlag))//当正在读取数据时，不允许同时再写入数据
	{
		DebugPrintf(ERR"WriteGprsDataBusy Wait1s:%d\r\n",ucGprsDataWritedFlag);//防止数据混乱延时1s
		OSTimeDly(100);
	}
	else{
	}
	OSSchedLock();
	ucGprsDataWritedFlag=g_ucGprsDataWriteFlag;//判断是否已经有数据了
	OSSchedUnlock();
	if((NULL==i_pucDataBuf)||IS_TRUE_BIT(ucGprsDataWritedFlag))//当正在读取数据时，不允许同时再写入数据
	{
		DebugPrintf(ERR"WriteGprsData err:%d\r\n",ucGprsDataWritedFlag);//防止数据混乱
		ret=FALSE;
	}
	else
	{
		g_tParameterOpr.GetPara("SmsPara",DeviceID,ucDeviceID,&wDevicIdLen);
		if((TRACKER_PROTOCOL_FIRST != i_pucDataBuf[0])||
		    (TRACKER_PROTOCOL_SECOND!= i_pucDataBuf[1])||
		    (wPackageDataLen!=0)||
		    (0!=memcmp(ucDeviceID,&i_pucDataBuf[TRACKER_PROTOCOL_DEVICE_ID_OFFSET],sizeof(ucDeviceID)))) 
		{
		}
		else 
		{
			eGprsDataRx = GPRS_DAT_RX_IDLE; 
		}
		switch(eGprsDataRx) 
		{
			case GPRS_DAT_RX_IDLE: 
				if((TRACKER_PROTOCOL_FIRST != i_pucDataBuf[0])||
				    (TRACKER_PROTOCOL_SECOND != i_pucDataBuf[1])||
				    (0!=memcmp(ucDeviceID,&i_pucDataBuf[TRACKER_PROTOCOL_DEVICE_ID_OFFSET],sizeof(ucDeviceID)))) 
				{
					DebugPrintf(ERR"Start GprsDataRx Format err\r\n");
				}
				else	
				{
					wPackageDataLen=Lsb2U8ToU16(&i_pucDataBuf[2]);
					OSSchedLock();
					memset(g_ucGprsDataBuf,0,sizeof(g_ucGprsDataBuf));
					memcpy(g_ucGprsDataBuf,i_pucDataBuf,i_wDataLen);
					g_wGprsDataBufLen=i_wDataLen;
					ucTrackerProtocolLast=g_ucGprsDataBuf[g_wGprsDataBufLen-1];
					ucTrackerProtocolSecondLast=g_ucGprsDataBuf[g_wGprsDataBufLen-2];
					wGpsDataLen=g_wGprsDataBufLen;
					OSSchedUnlock();	
					if((wGpsDataLen==wPackageDataLen)&&
						(TRACKER_PROTOCOL_SECOND_LAST==ucTrackerProtocolSecondLast)&&
						(TRACKER_PROTOCOL_LAST==ucTrackerProtocolLast))
					{
						OSSchedLock();
						M_SetBit(g_ucGprsDataWriteFlag);
						OSSchedUnlock();	
						ret=TRUE;
						wPackageDataLen=0;
					}
					else if(wGpsDataLen<wPackageDataLen)
					{
						eGprsDataRx = GPRS_DAT_RX_RXING; 
					}
					else
					{
						DebugPrintf(ERR"Rx too long,len:%d,AllLen:%d\r\n",wGpsDataLen,wPackageDataLen);
						wPackageDataLen=0;
						ret=FALSE;
					}
				}
				break;
			
			case GPRS_DAT_RX_RXING:
				OSSchedLock();
				memcpy(&g_ucGprsDataBuf[g_wGprsDataBufLen],i_pucDataBuf,i_wDataLen);
				g_wGprsDataBufLen+=i_wDataLen;			
				ucTrackerProtocolLast=g_ucGprsDataBuf[g_wGprsDataBufLen-1];
				ucTrackerProtocolSecondLast=g_ucGprsDataBuf[g_wGprsDataBufLen-2];
				wGpsDataLen=g_wGprsDataBufLen;
				OSSchedUnlock();				
				if((wGpsDataLen==wPackageDataLen)&&
					(TRACKER_PROTOCOL_SECOND_LAST==ucTrackerProtocolSecondLast)&&
					(TRACKER_PROTOCOL_LAST==ucTrackerProtocolLast))
				{
					OSSchedLock();
					M_SetBit(g_ucGprsDataWriteFlag);
					OSSchedUnlock();				
					ret=TRUE;
					wPackageDataLen=0;
					eGprsDataRx = GPRS_DAT_RX_IDLE; 
				}
				else if(wGpsDataLen<wPackageDataLen)
				{
					eGprsDataRx = GPRS_DAT_RX_RXING; 
				}
				else
				{
					DebugPrintf(ERR"RXING GprsDataRx Format err,DataLen:%d,PackLen:%d\r\n",wGpsDataLen,wPackageDataLen);
					wPackageDataLen=0;
					ret=FALSE;
					eGprsDataRx = GPRS_DAT_RX_IDLE; 
				}
				break;
				
		}
	}
	if((FALSE==ret))
	{
	}
	else
	{
		OSSemPost(ptOsSemUsedForPlatformCmdAck);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGsmReadGprsData
-Description	: GeneralGsmReadGprsData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/27   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmReadGprsData(u8 *o_pucDataBuf,u16 *o_wDataLen)
{
	u8 ret=FALSE;
	u8 ucGprsDataWriteFlag=FALSE;
	OSSchedLock();
	ucGprsDataWriteFlag=g_ucGprsDataWriteFlag;
	OSSchedUnlock();
	if(IS_FALSE_BIT(ucGprsDataWriteFlag))
	{
		DebugPrintf(ERR"GeneralGsmReadGprsData err\r\n");
	}
	else
	{
		OSSchedLock();
		*o_wDataLen=g_wGprsDataBufLen;
		memcpy(o_pucDataBuf,g_ucGprsDataBuf,g_wGprsDataBufLen);
		M_ClrBit(g_ucGprsDataWriteFlag);
		OSSchedUnlock();
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGsmWriteSmsData
-Description	: GeneralGsmWriteSmsData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/27   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmWriteSmsData(sms_info_t *i_ptDataBuf,u16 i_wPDCS)
{
	u8 ret=FALSE;
	u8 ucSmsDataWriteFlag=FALSE;
	OSSchedLock();
	ucSmsDataWriteFlag=g_ucSmsDataWriteFlag;
	OSSchedUnlock();
	if((NULL==i_ptDataBuf)||(IS_TRUE_BIT(ucSmsDataWriteFlag)))//里面已经写入了数据则写不进去
	{
		DebugPrintf(ERR"GeneralGsmWriteSmsData err:%d\r\n",ucSmsDataWriteFlag);
	}
	else
	{
		OSSchedLock();
		memset(&g_tSmsData,0,sizeof(T_SmsInfo));
		memcpy(g_tSmsData.strPhoneNum,i_ptDataBuf->head.num,sizeof(i_ptDataBuf->head.num));
		memcpy(g_tSmsData.aucSmsContent,i_ptDataBuf->sms_info,i_ptDataBuf->head.len);
		g_tSmsData.wPDCS=i_wPDCS;
		g_tSmsData.wSmsContentLen=i_ptDataBuf->head.len;
		M_SetBit(g_ucSmsDataWriteFlag);
		OSSchedUnlock();
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: GeneralGsmReadSmsData
-Description	: GeneralGsmReadSmsData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/27   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmReadSmsData(T_SmsInfo *o_ptSmsInfo)
{
	u8 ret=FALSE;
	u8 ucSmsDataWriteFlag=FALSE;
	OSSchedLock();
	ucSmsDataWriteFlag=g_ucSmsDataWriteFlag;
	OSSchedUnlock();
	if(IS_FALSE_BIT(ucSmsDataWriteFlag))
	{
		DebugPrintf(ERR"GeneralGsmReadSmsData err\r\n");
	}
	else
	{
		OSSchedLock();
		memcpy(o_ptSmsInfo,&g_tSmsData,sizeof(T_SmsInfo));
		M_ClrBit(g_ucSmsDataWriteFlag);
		OSSchedUnlock();
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGsmWriteAccessPointData
-Description	: GeneralGsmWriteAccessPointData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmWriteLbsLocationData(CellTowerInfoSt *ptCellTowerInfo)
{
	u8 ret=FALSE;
	u8 u8i;
	if(NULL==ptCellTowerInfo)
	{
		DebugPrintf(ERR"GeneralGsmWriteLbsLocationData err\r\n");
		ret=FALSE;
	}
	else
	{
		OSSchedLock();	
		for(u8i = 0;u8i < MAX_GSM_CELL_TOWER_NUM;u8i++)
		{
			memset((u8 *)&g_tCellTowerInfo.stCellTower[u8i],0,sizeof(CellTowerSt));
			g_tCellTowerInfo.stCellTower[u8i].s32CellId = ptCellTowerInfo->stCellTower[u8i].s32CellId;
			g_tCellTowerInfo.stCellTower[u8i].u16LocAreaCode = ptCellTowerInfo->stCellTower[u8i].u16LocAreaCode;
			g_tCellTowerInfo.stCellTower[u8i].u16MobileCountryCode = ptCellTowerInfo->stCellTower[u8i].u16MobileCountryCode;
			g_tCellTowerInfo.stCellTower[u8i].s8MobileNetworkCode = ptCellTowerInfo->stCellTower[u8i].s8MobileNetworkCode;
			g_tCellTowerInfo.stCellTower[u8i].s32Age = ptCellTowerInfo->stCellTower[u8i].s32Age;
			g_tCellTowerInfo.stCellTower[u8i].s8SignalStrength = ptCellTowerInfo->stCellTower[u8i].s8SignalStrength;
			g_tCellTowerInfo.stCellTower[u8i].u16TimingAdvance = ptCellTowerInfo->stCellTower[u8i].u16TimingAdvance;
		}
		M_SetBit(g_ucCellTowerInfoWriteFlag);
		OSSchedUnlock();
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGsmReadGprsData
-Description	: GeneralGsmReadGprsData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/27   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmReadLbsLocationData(CellTowerInfoSt *o_ptCellTowerInfo)
{
	u8 ret=FALSE;
	u8 ucCellTowerInfoWriteFlag=FALSE;	
	OSSchedLock();	
	ucCellTowerInfoWriteFlag=g_ucCellTowerInfoWriteFlag;
	OSSchedUnlock();
	if(IS_FALSE_BIT(ucCellTowerInfoWriteFlag))
	{
		DebugPrintf(ERR"GeneralGsmReadLbsLocationData err,Lbs fifo empty\r\n");
		ret=FALSE;
	}
	else
	{
		OSSchedLock();
		memcpy(o_ptCellTowerInfo,&g_tCellTowerInfo,sizeof(CellTowerInfoSt));
		OSSchedUnlock();
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GeneralGsmWriteSmsData
-Description	: GeneralGsmWriteSmsData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/27   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void  GeneralGsmGetLbsLocationDataHook(CellTowerInfoSt * ptCellTowerInfo)
{
	GeneralGsmWriteLbsLocationData(ptCellTowerInfo); 
}

/*****************************************************************************
-Fuction		: GeneralGsmReadSmsData
-Description	: GeneralGsmReadSmsData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/27   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GeneralGsmSetLbsLocationUpdateReq()
{
	u8 ret=FALSE;
	ret=LbsLocationUpdateReq(GeneralGsmGetLbsLocationDataHook);
	return ret;
}

/*****************************************************************************
-Fuction		: RegisterGeneralGsmDev
-Description	: RegisterGeneralGsmDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterGeneralGsmDev(PT_GeneralGsmDevManage i_ptGeneralGsmDev)
{
	PT_GeneralGsmDevManage ptTmp;
	if (!g_ptGeneralGsmDevHead)
	{
		g_ptGeneralGsmDevHead   = i_ptGeneralGsmDev;
		g_ptGeneralGsmDevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptGeneralGsmDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptGeneralGsmDev;
		i_ptGeneralGsmDev->ptNext = NULL;
	}
}


