/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 TrackerProtocol.c
* Description		: 	TrackerProtocol  operation center
* Created			: 	2016.09.08.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "TrackerProtocol.h"
#include "app_UsbManage.h"
#include "ParameterManage.h"
#include "main.h"
#include "app_Gsm.h"
#include "GsmDriverPort.h"
#include "GsmDriverTypeDef.h"
#include "Myqueue.h"
#include "Mymalloc.h"
#include "Flash.h"
#include "ProtocolManage.h"


static u8 NetDataSendIsOk(u8 *i_pBuf,u16 i_wLen);
static u8 NetDataSendOpr(u8 *i_buf,u16 i_Len);
static u8 TrackerProtocolGetDataSupport(u8 i_ucProtocolUser,u8 *o_buf,u16 *o_Len);
static u8 TrackerProtocolSendDataSupport(u8 i_ucProtocolUser,u8 *i_buf,u16 i_Len);
static u8 TrackerProtocolSetParaSupport(char *i_Name,ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len);
static u8 TrackerProtocolGetParaSupport(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);
static void TrackerProtocolParaDeInitSupport(u8 i_ucProtocolUser);
static u8 TrackerProtocolRelieveAlarmSupport(T_Alarm i_tAlarm);
static u8 TrackerProtocolGetImelSupport(u8 *o_buf,u16 *o_Len);

static T_TrackerProtocolSupport g_tTrackerProtocolSupport={
	.name="TrackerProtocolSupport",
	.GetDataSupport		=TrackerProtocolGetDataSupport,
	.SendDataSupport		=TrackerProtocolSendDataSupport,
	.SetParaSupport		=TrackerProtocolSetParaSupport,
	.GetParaSupport		=TrackerProtocolGetParaSupport,
	.ParaDeInitSupport		=TrackerProtocolParaDeInitSupport,
	.RelieveAlarmSupport	=TrackerProtocolRelieveAlarmSupport,
	.GetImelSupport		=TrackerProtocolGetImelSupport
};

/*****************************************************************************
-Fuction		: TrackerProtocolAnalyseData
-Description	: TrackerProtocolAnalyseData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08	V1.0.0		Yu Weifeng	Created
******************************************************************************/
void TrackerProtocolSupportInit()
{
	RegisterTrackerProtocolSupport(&g_tTrackerProtocolSupport);
}

/*****************************************************************************
-Fuction		: TrackerProtocolGetDataSupport
-Description	: TrackerProtocolGetDataSupport
把等待获取数据以及发送数据的方式改为优先串口的，
解决串口读取或设置参数，平台也会收到的问题
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/08   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 TrackerProtocolGetDataSupport(u8 i_ucProtocolUser,u8 *o_buf,u16 *o_Len)
{
	u8 ret=FALSE;
	u8 ucErr;
	if(PROTOCOL_USED_FOR_VCP==i_ucProtocolUser)
	{
		ret=TRUE;
		ret&=g_tUsb.GetDataBuf(o_buf,o_Len);
	}	
	else if(PROTOCOL_USED_FOR_GPRS==i_ucProtocolUser)
	{
		ret=g_tGsm.GsmReadGprsData(o_buf,o_Len);
	}
	else
	{
		DebugPrintf(ERR"GetDataSupportProtocolUserUnknow");
	}
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolSendDataSupport
-Description	: TrackerProtocolSendDataSupport
把等待获取数据以及发送数据的方式改为优先串口的，
解决串口读取或设置参数，平台也会收到的问题
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolSendDataSupport(u8 i_ucProtocolUser,u8 *i_buf,u16 i_Len)
{
	u8 ret=FALSE;
	u8 ucErr;	
	u8 ucCleanedSendBusyFlag=FALSE;
	u8 ucCount=0;
	if(PROTOCOL_USED_FOR_VCP==i_ucProtocolUser)
	{
		ret=TRUE;
		ret&=g_tUsb.SendDataBuf(i_buf,i_Len);
	}
	else if(PROTOCOL_USED_FOR_GPRS==i_ucProtocolUser)
	{
		ret=NetDataSendOpr(i_buf,i_Len);
		if(FALSE==ret)
		{
			DebugPrintf(ERR"NetDataSendOprFail\r\n");
		}
		else
		{
			ret=NetDataSendIsOk(i_buf,i_Len);
			if(FALSE==ret)
			{
				DebugPrintf(ERR"NetDataSendIsNotOk\r\n");
			}
			else
			{
			}
		}
	}
	else
	{
		DebugPrintf(ERR"SendDataSupportProtocolUserUnknow");
	}
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolSetParaSupport
-Description	: TrackerProtocolSetParaSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolSetParaSupport(char *i_Name,ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len)
{
	u8 ret=FALSE;
	ret=g_tParameterOpr.SetPara(i_Name,i_SubName,i_Buf,i_Len);
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolGetParaSupport
-Description	: TrackerProtocolGetParaSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolGetParaSupport(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len)
{
	u8 ret=FALSE;
	ret=g_tParameterOpr.GetPara(i_Name,i_SubName,o_Buf,o_Len);
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolParaDeInitSupport
-Description	: TrackerProtocolParaDeInitSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static void TrackerProtocolParaDeInitSupport(u8 i_ucProtocolUser)
{
	u8 ucErr;
	if(PROTOCOL_USED_FOR_VCP==i_ucProtocolUser)
	{
		DeleteAllLocationInfoInFlash();
		g_tParameterOpr.ParaDeInit();
		OSSemPost(ptOsSemUsedForSystemNeedReset);
	}
	else if(PROTOCOL_USED_FOR_GPRS==i_ucProtocolUser)
	{
		DeleteAllLocationInfoInFlash();
		g_tParameterOpr.ParaDeInitExceptNetPara();
	}
	else
	{
		DebugPrintf(ERR"ParaDeInitSupportProtocolUserUnknow");
	}

}
/*****************************************************************************
-Fuction		: TrackerProtocolRelieveAlarmSupport
-Description	: TrackerProtocolRelieveAlarmSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolRelieveAlarmSupport(T_Alarm i_tAlarm)
{
	u8 ret=FALSE;
	//ret=g_tAlarmOpr.RelieveAlarmState(i_tAlarm);
	ret=TRUE;
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolGetImelSupport 
-Description	: TrackerProtocolGetImelSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolGetImelSupport(u8 *o_buf,u16 *o_Len)
{
	u8 ret=FALSE;
	u16 wImelLen=TRACKER_PROTOCOL_IMEL_LEN;
	 *o_Len=wImelLen;
	ret=getGsmIMEI(o_buf,wImelLen);
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolGetImelSupport 
-Description	: TrackerProtocolGetImelSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 NetDataSendOpr(u8 *i_buf,u16 i_Len)
{
	u8 ret=FALSE;
	u8 ucErr;	
	myevent event_src;
	OSSemPost(ptOsSemUsedForResumeGsmTask);

	IWDG_Feed();
	OSSemPend(ptOsSemUsedForGprsDataSendIdle,(30*OS_TICKS_PER_SEC),&ucErr);
	if(OS_ERR_NONE!=ucErr)	
	{
		IWDG_Feed();//由于心跳执行不能大于30s否则看门狗复位所以分两次
		DebugPrintf(ERR"PendGsmTxIpDatFailButNotAbandonTx,err:%d\r\n",ucErr);
		OSSemPend(ptOsSemUsedForGprsDataSendIdle,(15*OS_TICKS_PER_SEC),&ucErr);
		if(OS_ERR_NONE!=ucErr)
		{
			IWDG_Feed();//由于心跳执行不能大于30s否则看门狗复位所以分两次
			DebugPrintf(ERR"PendGsmTxIpDatFailSendDataAbandon\r\n");//45sGSM网络数据内存应该释放了，后续优化为队列
			ret=FALSE;
		}
		else
		{
			IWDG_Feed();
			ret=TRUE;
		}	
	}
	else
	{
		IWDG_Feed();
		ret=TRUE;
	}
	if(FALSE==ret)
	{
	}
	else
	{
		event_src.size=i_Len;
		event_src.buff=mymalloc(event_src.size);
		if(NULL==event_src.buff)
		{
			DebugPrintf(ERR"stGsmIpDatTxBuf malloc err\r\n");
			ret=FALSE;
		}
		else
		{	
			memcpy(event_src.buff,i_buf,event_src.size);
			OSSchedLock();
			if(NULL == stGsmIpDatTxBuf.pu8Dat) {//防止内存泄漏，最好GSM库使用消息队列
				; 
			}
			else	{
				DebugPrintf(ERR"NetDataSendBufHaveDataErr,Len:%d\r\n",stGsmIpDatTxBuf.u16Len);
				myfree(stGsmIpDatTxBuf.pu8Dat, stGsmIpDatTxBuf.u16Len); 
				stGsmIpDatTxBuf.pu8Dat = NULL; 
			}
			stGsmIpDatTxBuf.pu8Dat=event_src.buff;
			stGsmIpDatTxBuf.enIpDatSrcId=IP_DAT_SRC_SCK_1;
			stGsmIpDatTxBuf.u16Len=event_src.size;
			OSSchedUnlock();

			OSSemPost(ptOsSemUsedForGprsDataSendReq);
			OSFlagPend(pstOsFlagForAppUse, GSM_TX_IP_DAT_BUSY_FLAG,OS_FLAG_WAIT_SET_ALL, (30*OS_TICKS_PER_SEC),&ucErr);//短信处理需要25s
			if(OS_ERR_NONE!=ucErr)//由于默认是30分钟大于30s的间隔,长时间等待保证长时间上传的上传数据可以上传上去
			{
				DebugPrintf(ERR"NetDataSendReq err:%d\r\n",ucErr);
				ret=FALSE;//GSM任务没来的及处理请求也返回失败
			}
			else
			{
				ret=TRUE;
			}
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: NetDataSendIsOk 
-Description	: 判断网络数据是否发送成功
-Input			: 网络数据，第一次发送不成功会重发
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 NetDataSendIsOk(u8 *i_pBuf,u16 i_wLen)
{
	u8 ret=FALSE;
	u8 ucErr;	
	u8 ucCount=0;

	for(ucCount=0;ucCount<8;ucCount++)
	{
		OSFlagPend(pstOsFlagForAppUse, GSM_TX_IP_DAT_BUSY_FLAG,OS_FLAG_WAIT_CLR_ALL, (5*OS_TICKS_PER_SEC),&ucErr);
		if(OS_ERR_NONE!=ucErr)//等待处理完
		{
			ret=FALSE;
			IWDG_Feed();//由于心跳执行不能大于30s否则看门狗复位
			DebugPrintf("GsmTxIpDatPendEndTimeOut:%d!\r\n",ucCount);
		}
		else
		{
			OSFlagAccept(pstOsFlagForAppUse, GSM_TX_IP_DAT_OK_FLAG,OS_FLAG_WAIT_SET_ALL, &ucErr); 
			if(OS_ERR_NONE != ucErr) 
			{
				ret=FALSE;
				if(0==ucCount)
				{
					DebugPrintf(ERR"GsmTxIpDatReqCleaned,SendAgain!\r\n");
					ret=NetDataSendOpr(i_pBuf,i_wLen);
					if(TRUE==ret)
					{
					}
					else
					{
						break;//失败不用等待成功标记
					}
				}
				else
				{
					DebugPrintf(ERR"GsmTxIpDatEndAndNotOk:%d!\r\n",ucCount);
					break;//失败不用等待成功标记
				}
			}
			else
			{
				ret=TRUE;
				OSFlagPost(pstOsFlagForAppUse,GSM_TX_IP_DAT_OK_FLAG,OS_FLAG_CLR,&ucErr);
				break;
			}
		}
	}
	
	return ret;
}

