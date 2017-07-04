/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 SmsProtocol.c
* Description		: 	SmsProtocol  operation center
* Created			: 	2016.09.18.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "SmsProtocol.h"
#include "app_gsm_includes.h"
#include "ParameterManage.h"
#include "app_Gsm.h"
#include "app_gsm_Oper.h"
#include "Flash.h"

static void SmsProtocolParaDeInitSupport();
static u8 SmsProtocolGetDataSupport(T_SmsInfo *o_ptSmsInfo);
static u8 SmsProtocolSendAlarmDataSupport(u8 *i_buf,u16 i_Len);
static u8 SmsProtocolSetParaSupport(char *i_Name,ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len);
static u8 SmsProtocolGetParaSupport(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);
static u8 SmsProtocolGetImelSupport(u8 *o_buf,u16 *o_Len);
static u8 SmsProtocolSendDataSupport(u8 *i_PhoneNumBuf,u8 *i_buf,u16 i_Len);
static u8 SmsProtocolRestartSystemSupport();
static u8 SmsProtocolGetGpsLocateInfoSupport(LocationVectorInfo *o_ptGpsLocationInfo);
static u8 SmsProtocolLock();
static u8 SmsProtocolUnlock();
static u8 SmsProtocolTimeDelay(u16 i_wSecond,u16 i_wMilliSecond);
static u8 SmsProtocolGetGsmCsq(u8 *o_pucCsq);
s16 SmsProtocolPduEncode( u8  *pPdu, u8  *pSCA, u8  *pDA, u8  *pUD, s16 iUDL, s16 iDCS );

static T_SmsProtocolSupport g_tSmsProtocolSupport={
	.name="SmsProtocolSupport",
	.ParaDeInitSupport			=SmsProtocolParaDeInitSupport,
	.GetDataSupport			=SmsProtocolGetDataSupport,
	.SendDataSupport			=SmsProtocolSendDataSupport,
	.SetParaSupport			=SmsProtocolSetParaSupport,
	.GetParaSupport			=SmsProtocolGetParaSupport,
	.GetImelSupport			=SmsProtocolGetImelSupport,
	.SendAlarmDataSupport		=SmsProtocolSendAlarmDataSupport,
	.RestartSystemSupport		=SmsProtocolRestartSystemSupport,
	.GetGpsLocationInfoSupport	=SmsProtocolGetGpsLocateInfoSupport,
	.LockSupport				=SmsProtocolLock,
	.UnlockSupport			=SmsProtocolUnlock,
	.TimeDelaySupport			=SmsProtocolTimeDelay,
	.GetCsqSupport			=SmsProtocolGetGsmCsq,
	.PduEncodeSupport			=SmsProtocolPduEncode
};

/*****************************************************************************
-Fuction		: SmsProtocolAnalyseData
-Description	: SmsProtocolAnalyseData getGsmIMEI
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18	V1.0.0		Yu Weifeng	Created
******************************************************************************/
void SmsProtocolSupportInit()
{
	RegisterSmsProtocolSupport(&g_tSmsProtocolSupport);
}
/*****************************************************************************
-Fuction		: SmsProtocolParaDeInitSupport 
-Description	: SmsProtocolParaDeInitSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static void SmsProtocolParaDeInitSupport()
{
	DeleteAllLocationInfoInFlash();
	g_tParameterOpr.ParaDeInitExceptNetPara();
}

/*****************************************************************************
-Fuction		: SmsProtocolGetDataSupport
-Description	: SmsProtocolGetDataSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/18   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SmsProtocolGetDataSupport(T_SmsInfo *o_ptSmsInfo)
{
	u8 ret=FALSE;
	ret=g_tGsm.GsmReadSmsData(o_ptSmsInfo);
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolSendDataSupport
-Description	: SmsProtocolSendDataSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolSendAlarmDataSupport(u8 *i_buf,u16 i_Len)
{
	u8 ret=FALSE;
	ret=ATLib_gsm_CMGS(i_buf,i_Len);
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolGetDataSupport
-Description	: SmsProtocolGetDataSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/18   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void SmsSendDataAck(u8 i_ucResult)
{
	if(FALSE==i_ucResult)
	{
	}
	else
	{
		OSSemPost(ptOsSemUsedForSendSmsOK);
	}
}
/*****************************************************************************
-Fuction		: SmsProtocolSendAlarmDataSupport
-Description	: SmsProtocolSendAlarmDataSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolSendDataSupport(u8 *i_PhoneNumBuf,u8 *i_buf,u16 i_Len)
{
	u8 ret=FALSE;
	u8 ucErr;
	u8 ucCount=0;
	u8 ucGsmPhoneIdleFlag=FALSE;
	u8 ucGsmInitOkFlag=FALSE;
	u8 ucPhoneNumLen=0;
	SmsTxParaSt tSmsTxData={0};	
	for(ucCount=0;ucCount<130;ucCount++)//延时一分5,失败上层会重复三次调用
	{
		if(IS_FALSE_BIT(GsmPhoneIsIdle()))
		{
			OSTimeDly(50);//500ms查一次
		}
		else
		{
			M_SetBit(ucGsmPhoneIdleFlag);
			break;
		}
	}
	if(IS_FALSE_BIT(ucGsmPhoneIdleFlag))
	{
		ret=FALSE;
		DebugPrintf(ERR"SmsSend:GsmPhoneNotIdle!\r\n");
	}
	else
	{
		for(ucCount=0;ucCount<100;ucCount++)//延时50s,失败上层会重复三次调用
		{
			if(IS_FALSE_BIT(GsmModGsmIsOk()))
			{
				OSTimeDly(50);//500ms查一次
			}
			else
			{
				M_SetBit(ucGsmInitOkFlag);
				break;
			}
		}		
		if(IS_FALSE_BIT(ucGsmInitOkFlag))
		{
			ret=FALSE;
			DebugPrintf(ERR"SmsSend:GsmInitNotOk !\r\n");
		}
		else
		{
			ucPhoneNumLen=strnlen(i_PhoneNumBuf,GSM_PHONE_STR_LEN);
			tSmsTxData.stDstNum.u8Len=ucPhoneNumLen;
			memcpy(tSmsTxData.stDstNum.s8Num,i_PhoneNumBuf,ucPhoneNumLen);
			tSmsTxData.stSmsDat.pu8Dat=i_buf;
			tSmsTxData.stSmsDat.u16Len=i_Len;
			tSmsTxData.SmsTxEndHook=SmsSendDataAck;
			ret=GsmSmsTxReq(&tSmsTxData);//前面已经保证GSMok
			if(FALSE==ret)
			{
				DebugPrintf(ERR"SmsSend:GsmSmsTxReq err!\r\n");//一般是BUSY的错误			
				OSTimeDlyHMSM(0,0,20,0);//延时20s,失败上层会重复三次调用
			}
			else
			{
				DebugPrintf(ERR"SmsSend:GsmSmsTxReq ok!\r\n");					
				OSFlagPend(pstOsFlagForAppUse, GPRS_IS_OK_FLAG,OS_FLAG_WAIT_SET_ALL, 1, &ucErr);
				if(OS_ERR_NONE!=ucErr)//GPRS不ok，gsm是一直醒的
				{	
				}
				else
				{
					OSSemPost(ptOsSemUsedForResumeGsmTask);
				}
				OSSemPend(ptOsSemUsedForSendSmsOK,150*OS_TICKS_PER_SEC,&ucErr); 		
				if(OS_ERR_NONE!=ucErr)
				{
					ret=FALSE;
					DebugPrintf(ERR"SmsSend:PendSendSmsOkSem err:%d\r\n",ucErr);
				}
				else
				{
					ret=TRUE;
				}
			}
		}
	}	
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolSetParaSupport
-Description	: SmsProtocolSetParaSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolSetParaSupport(char *i_Name,ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len)
{
	u8 ret=FALSE;
	ret=g_tParameterOpr.SetPara(i_Name,i_SubName,i_Buf,i_Len);
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolGetParaSupport
-Description	: SmsProtocolGetParaSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolGetParaSupport(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len)
{
	u8 ret=FALSE;
	ret=g_tParameterOpr.GetPara(i_Name,i_SubName,o_Buf,o_Len);
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolGetImelSupport 
-Description	: SmsProtocolGetImelSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolGetImelSupport(u8 *o_buf,u16 *o_Len)
{
	u8 ret=FALSE;
	u16 wImelLen=GSM_IMEI_LEN;
	 *o_Len=wImelLen;
	ret=getGsmIMEI(o_buf,wImelLen);
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolGetImelSupport 
-Description	: SmsProtocolGetImelSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolRestartSystemSupport()
{
	u8 ret=FALSE;
	ret=TRUE;
	OSSemPost(ptOsSemUsedForSystemNeedReset);
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolReportInfoPack
-Description	: SmsProtocolReportInfoPack
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/21    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolGetGpsLocateInfoSupport(LocationVectorInfo *o_ptGpsLocationInfo)
{
	u8 ret=FALSE;	
	LocationVectorInfo tGpsLocationInfo={0};
	ret=g_tGPS.GpsGetPos(&tGpsLocationInfo);
	if(FALSE==ret)
	{
		DebugPrintf(ERR"SmsProtocolGetGpsPos err,UseHistoryPos\r\n");
		ret=g_tGPS.GpsGetHistoryPos(&tGpsLocationInfo);
	}
	else
	{
	}
	OSSchedLock();
	memcpy(o_ptGpsLocationInfo,&tGpsLocationInfo,sizeof(LocationVectorInfo));
	OSSchedUnlock();
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolGetImelSupport 
-Description	: SmsProtocolGetImelSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolLock()
{
	u8 ret=FALSE;
	ret=TRUE;
	OSSchedLock();
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolGetImelSupport 
-Description	: SmsProtocolGetImelSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolUnlock()
{
	u8 ret=FALSE;
	ret=TRUE;
	OSSchedUnlock();
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolGetImelSupport 
-Description	: SmsProtocolGetImelSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolTimeDelay(u16 i_wSecond,u16 i_wMilliSecond)
{
	u8 ret=FALSE;
	ret=TRUE;
	OSTimeDlyHMSM(0,0,i_wSecond,i_wMilliSecond);
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolGetImelSupport 
-Description	: SmsProtocolGetImelSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolGetGsmCsq(u8 *o_pucCsq)
{
	u8 ret=FALSE;
	ret=TRUE;
	u8 ucErr;
	OSFlagPend(pstOsFlagForAppUse, GPRS_IS_OK_FLAG,OS_FLAG_WAIT_SET_ALL, 1, &ucErr);
	if(OS_ERR_NONE!=ucErr)//GPRS不ok，gsm是一直醒的
	{	
	}
	else
	{
		OSSemPost(ptOsSemUsedForResumeGsmTask);//获取csq
	}
	OSTimeDly(100);
	*o_pucCsq=GsmGetCsq();
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolGetImelSupport 
-Description	: SmsProtocolGetImelSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
s16 SmsProtocolPduEncode( u8  *pPdu, u8  *pSCA, u8  *pDA, u8  *pUD, s16 iUDL, s16 iDCS )
{
	return PDUEncode( pPdu, pSCA, pDA, pUD, iUDL, iDCS );
}

