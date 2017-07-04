/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 TrackerProtocol.c
* Description		: 	TrackerProtocol  operation center
* Created			: 	2016.09.12.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "cBasicTools.h"
#include "TrackerProtocol.h"
#include "Config.h"
#include "ParameterManage.h"
#include "Crc.h"
#include "Ucos_ii.h"
#include "app_GpsManage.h"
#include "app_Gsm_Oper.h"
#include "main.h"
#include "app_PowerStateCheck.h"
#include "app_Gsm.h"
#include "app_WifiSupport.h"
#include "app_TimeManage.h"
#include "Bsp.h"
#include "Crc.h"
#include "Flash.h"

static PT_TrackerProtocolSupport g_ptTrackerProtocolSupportHead=NULL;
static u8 hookTrackerRtcTimeUpdate(u32 i_dwNewRtcTime);
static void hookTrackerDeInit(u8 i_ucProtocolUser);
static u8 hookTrackerGetData(u8 i_ucProtocolUser,u8 *o_ucBuf, u16 *o_wLen);
static u8 hookTrackerSendData(u8 i_ucProtocolUser,u8 *i_ucBuf, u16 i_wLen);
static u8 hookTrackerSetParameter(char *i_Name,ParameterEnum i_SubName,u8 *i_ucBuf,u16 i_wLen);
static u8 hookTrackerGetParameter(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);
static u8 hookTrackerRelieveAlarm(T_Alarm i_tAlarm);
static u8 hookTrackerGetIMEL(u8 *o_ucBuf,u16 *o_wLen);

static u8 GetEmergencyContactNum(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetDeviceID(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetIMEL(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetSimCardNum(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetReportInterval(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetDailMode(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetDomain(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetIp(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetPort(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetApn(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetUsrName(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetPassword(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetCenterNum(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetSmsSecretKey(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetTrackerLanguage(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetLowPowerAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetSmsAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetDropTrackerAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetCutTrackerAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetSoftWareVision(u8 *o_ucBuf,u16 *o_wLen);
static u8 GetHardWareVision(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetEmergencyContactNum(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetDeviceID(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetSimCardNum(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetReportInterval(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetDailMode(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetDomain(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetIp(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetPort(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetApn(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetUsrName(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetPassword(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetCenterNum(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetSmsSecretKey(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetTrackerLanguage(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetLowPowerAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetSmsAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetDropTrackerAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetCutTrackerAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetSoftWareVision(u8 *o_ucBuf,u16 *o_wLen);
static u8 SetHardWareVision(u8 *o_ucBuf,u16 *o_wLen);

static u8 TrackerLoginAck(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 TrackerHeartBeatAck(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 TrackerCallTheRoll(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 TrackerAlarmAck(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 TrackerReportAck(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 TrackerReset(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 TrackerDefaultInit(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 TrackerParaSet(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 TrackerParaRead(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 TrackerUpdateRequest(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 TrackerUpdatePackage(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 TrackerReadSystemLog(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);

static u8 TrackerProtocolGetLocationInfo(WifiAccessPointSt *o_ptWifiApInfo,LocationVectorInfo *o_ptLocationInfo);
static u8 TrackerProtocolUpdateGprsPara();
static u8 TrackerProtocolWifiApNumIsEnough(WifiAccessPointSt *i_ptWifiApInfo);
static u8 TrackerProtocolPackTrackerInfo(WifiAccessPointSt *i_ptWifiApInfo,LocationVectorInfo *i_ptLocationInfo,u8 *o_ucBuf,u16 *o_wBufLen);
static u8 TrackerProtocolPackWifiInfo(WifiAccessPointSt *i_ptWifiApInfo,u8 *o_ucBuf,u16 *o_wBufLen);
static u8 TrackerProtocolPackCellTowerInfo(u8 *o_ucBuf,u16 *o_wBufLen);
static u8 TrackerProtocolPackAlarmState(T_Alarm i_tAlarm,u16 i_wLen,u8 *o_ucBuf);
static u8 TrackerProtocolGetData(u8 i_ucProtocolUser,u8 *o_pucDataBuf,u16 *o_pwDataLen,u16 i_wDataBufMaxLen);
static u8 TrackerProtocolCheckData(u8 *i_pucDataBuf,u16 i_wDataLen);
static u8 TrackerProtocolAnalyseData(u8 i_ucProtocolUser,u8 *i_pucDataBuf,u16 i_wDataLen,T_HandleCmdAck *o_ptHandCmdAck);
static void TrackerProtocolPackData(u16 i_wCmdID,u8 *i_Buf,u16 i_wLen,u8 *o_Buf,u16 *o_pwLen);
static u8 TrackerProtocolSendData(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen);
static u8 TrackerProtocolReportAlarm(T_Alarm i_tAlarm,u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen);
static u8 TrackerProtocolLogin(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen);
static u8 TrackerProtocolHeartBeat(u8 *o_pucDataSendBuf,u16 *o_wSendLen);
static u8 TrackerProtocolLogout(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen);
static u8 TrackerProtocolReport(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen);

//static u16 g_wTrackerGetDataBufLen=TRACKER_PROTOCOL_MAX_LEN;
//static u8   g_TrackerGetDataBuf[TRACKER_PROTOCOL_MAX_LEN]={0};
//static u8   g_TrackerReadDataBuf[(TRACKER_PROTOCOL_MAX_LEN-TRACKER_PROTOCOL_BASE_LEN)]={0};
//static u8   g_TrackerSendDataBuf[TRACKER_PROTOCOL_MAX_LEN]={0};
static u8   g_TrackerGetSmsBuf[TRACKER_PROTOCOL_MAX_RECV_SMS_COUNT][TRACKER_PROTOCOL_MAX_SMS_LEN]={0};
static u8   g_TrackerAlarmStateBuf[TRACKER_PROTOCOL_ALARM_STATE_LEN]={0};
static u8   g_wTrackerAlarmStateLen=TRACKER_PROTOCOL_ALARM_STATE_LEN;
//static u16   g_wTrackerLocationInfoLen=TRACKER_PROTOCOL_LOCATION_INFO_SAVE_MAX_LEN;

static u8   g_ucTrackerCheckGetDataFlag=FALSE;
//static u8   g_ucTrackerSendDataFlag=FALSE;
static u8   g_ucTrackerReportedAlarmFlag=FALSE;

static T_SoftwarePackInfo g_tSoftwarePackInfo={0};
static T_ProtocolManage g_tTrackerProtocol={
	.name="TrackerProtocol",
	.ProtocolGetData		=TrackerProtocolGetData,
	.ProtocolCheckData		=TrackerProtocolCheckData,
	.ProtocolAnalyseData	=TrackerProtocolAnalyseData,
	.ProtocolReportAlarm	=TrackerProtocolReportAlarm,
	.ProtocolLogin			=TrackerProtocolLogin,
	.ProtocolHeartBeat		=TrackerProtocolHeartBeat,
	.ProtocolLogout		=TrackerProtocolLogout,
	.ProtocolReport		=TrackerProtocolReport
};

static T_TrackerProtocolHandleMenu g_tTrackerProtocolHandleMenu[]={
	{0x8001,TrackerLoginAck},
	{0x8003,TrackerHeartBeatAck},
	{0x4101,TrackerCallTheRoll},
	{0x8203,TrackerAlarmAck},	
	{0x8206,TrackerReportAck},
	{0x4581,TrackerReset},
	{0x4582,TrackerDefaultInit},
	{0x5101,TrackerParaSet},
	{0x5201,TrackerParaRead},
	{0x4122,TrackerUpdateRequest},
	{0x4123,TrackerUpdatePackage},
	{0x4602,TrackerReadSystemLog},
};
#define ProtocolHandleMenuNum  (sizeof(g_tTrackerProtocolHandleMenu) / sizeof(T_TrackerProtocolHandleMenu))

static T_TrackerProtocolSetHandleMenu g_tTrackerProtocolSetHandleMenu[]={
	{0x1001,SetEmergencyContactNum},
	{0x2001,SetDeviceID},
	{0x2003,SetSimCardNum},
	{0x4001,SetReportInterval},
	{0x5008,SetCenterNum},
	{0x6001,SetSmsSecretKey},
	{0x6002,SetTrackerLanguage},
	{0x9006,SetLowPowerAlarmSwitch},
	{0x900A,SetSmsAlarmSwitch},
	{0x900B,SetDropTrackerAlarmSwitch},
	{0x900C,SetCutTrackerAlarmSwitch},
	{0xA001,SetSoftWareVision},
	{0xA002,SetHardWareVision},	
	/*不能随意改动上下的位置，同时如果改动了或者增加或
	减少某一项，则调用该表的地方的判断网络参数位置
	也要随之改变*/
	{0x5001,SetDailMode},//13 网络参数放最后便于判断当前设置参数里是否有网络参数
	{0x5002,SetDomain},
	{0x5003,SetIp},
	{0x5004,SetPort},
	{0x5005,SetApn},
	{0x5006,SetUsrName},
	{0x5007,SetPassword},
};
#define ProtocolSetHandleMenuNum	(sizeof(g_tTrackerProtocolSetHandleMenu) / sizeof(T_TrackerProtocolSetHandleMenu)) 

static T_TrackerProtocolGetHandleMenu g_tTrackerProtocolGetHandleMenu[]={
	{0x1001,GetEmergencyContactNum},
	{0x2001,GetDeviceID},
	{0x2002,GetIMEL},
	{0x2003,GetSimCardNum},
	{0x4001,GetReportInterval},
	{0x5001,GetDailMode},
	{0x5002,GetDomain},
	{0x5003,GetIp},
	{0x5004,GetPort},
	{0x5005,GetApn},
	{0x5006,GetUsrName},
	{0x5007,GetPassword},
	{0x5008,GetCenterNum},
	{0x6001,GetSmsSecretKey},
	{0x6002,GetTrackerLanguage},
	{0x9006,GetLowPowerAlarmSwitch},
	{0x900A,GetSmsAlarmSwitch},
	{0x900B,GetDropTrackerAlarmSwitch},
	{0x900C,GetCutTrackerAlarmSwitch},
	{0xA001,GetSoftWareVision},
	{0xA002,GetHardWareVision}
};
#define  ProtocolGetHandleMenuNum   (sizeof(g_tTrackerProtocolGetHandleMenu) / sizeof(T_TrackerProtocolGetHandleMenu)) 

/*****************************************************************************
-Fuction		: TrackerProtocolInit
-Description	: TrackerProtocolInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
void TrackerProtocolInit()
{
	TrackerProtocolSupportInit();
	RegisterProtocol(&g_tTrackerProtocol);
}
/*****************************************************************************
-Fuction		: TrackProtocolGetData
-Description	: TrackProtocolGetData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/08   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 TrackerProtocolGetData(u8 i_ucProtocolUser,u8 *o_pucDataBuf,u16 *o_pwDataLen,u16 i_wDataBufMaxLen)
{
	u8 ret=FALSE;
	memset(o_pucDataBuf,0,i_wDataBufMaxLen);
	ret=hookTrackerGetData(i_ucProtocolUser,o_pucDataBuf,o_pwDataLen);
	if(*o_pwDataLen>i_wDataBufMaxLen)
	{
		DebugPrintf(ERR"TrackerProtocolGetDataTooLong,Len:%d,MaxLen:%d\r\n",*o_pwDataLen,i_wDataBufMaxLen);
		ret=FALSE;
	}
	else
	{
		ret&=TRUE;
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: TrackProtocolGetData
-Description	: TrackProtocolGetData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/08   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 TrackerProtocolCheckData(u8 *i_pucDataBuf,u16 i_wDataLen)
{
	u8 ret=FALSE;
	u8 *pGetDataBuf=i_pucDataBuf;
	u16 wGetDataBufLen=i_wDataLen;
	u16 wCrcCheck=0;
	u16 wDataLenCheck=0;
	
	OSSchedLock();
	if((TRACKER_PROTOCOL_FIRST!=pGetDataBuf[0])||(TRACKER_PROTOCOL_SECOND!=pGetDataBuf[1])||
	    (TRACKER_PROTOCOL_SECOND_LAST!=pGetDataBuf[wGetDataBufLen-2])||
	    (TRACKER_PROTOCOL_LAST!=pGetDataBuf[wGetDataBufLen-1])||
	    (wGetDataBufLen<TRACKER_PROTOCOL_BASE_LEN))
	{
		ret=FALSE;
		M_ClrBit(g_ucTrackerCheckGetDataFlag);
		memset(pGetDataBuf,0,sizeof(pGetDataBuf));
		DebugPrintf(ERR"TrackerProtocolCheckData err!");
	}
	else
	{
		/*协议下来的类型数组都是小端的，需要反一下*/
		wDataLenCheck=(u16)(pGetDataBuf[3])<<8|(pGetDataBuf[2]);
		wCrcCheck=(u16)(pGetDataBuf[wGetDataBufLen-3]<<8)|(pGetDataBuf[wGetDataBufLen-4]);
		if((wCrcCheck!=CRC_MakeCrc(pGetDataBuf,wGetDataBufLen-4))||
		    (wDataLenCheck>TRACKER_PROTOCOL_MAX_LEN))
		{
			ret=FALSE;
			M_ClrBit(g_ucTrackerCheckGetDataFlag);
			memset(pGetDataBuf,0,sizeof(pGetDataBuf));
			DebugPrintf(ERR"Tracker Protocol CRC||DataLen check err!\r\n");
		}
		else
		{
			M_SetBit(g_ucTrackerCheckGetDataFlag);
			ret=TRUE;
		}
	}	
	OSSchedUnlock();
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolAnalyseData
-Description	: TrackerProtocolAnalyseData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolAnalyseData(u8 i_ucProtocolUser,u8 *i_pucDataBuf,u16 i_wDataLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	s32 i;
	u16 Cmd=0;
	u8 *pAnalyseDataBuf=&i_pucDataBuf[TRACKER_PROTOCOL_DATA_OFFSET];//由于有的处理时间长，所以处理里面进行保护
	u16 wAnalyseDataBufLen=i_wDataLen-TRACKER_PROTOCOL_BASE_LEN;
	Cmd=(u16)(i_pucDataBuf[TRACKER_PROTOCOL_CMD_OFFSET])<<8|(i_pucDataBuf[TRACKER_PROTOCOL_CMD_OFFSET+1]);
	if(IS_FALSE_BIT(g_ucTrackerCheckGetDataFlag))
	{
		ret=FALSE;
		DebugPrintf(ERR"Is not Check Data ok \r\n");
	}
	else
	{
		for(i=0;i<ProtocolHandleMenuNum;i++)
		{
			if(Cmd!=g_tTrackerProtocolHandleMenu[i].Cmd)
			{
				
			}
			else
			{
				ret=g_tTrackerProtocolHandleMenu[i].ProtocolHandle(i_ucProtocolUser,pAnalyseDataBuf,wAnalyseDataBufLen,o_ptHandCmdAck);
				break;
			}

		}
		if(FALSE==ret)
		{
			DebugPrintf(ERR"TrackerProtocolAnalyseDataErr!\r\n");
		}
		else{
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolPackData
-Description	: TrackerProtocolPackData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static void TrackerProtocolPackData(u16 i_wCmdID,u8 *i_Buf,u16 i_wLen,u8 *o_Buf,u16 *o_pwLen)
{
	u8 ret=FALSE;
	u8 ucErr;
	u16 wPackLen=0;
	u16 wCRC=0;
	u8 CRCBuf[2];
	u8 PackLen[2];
	u8 CmdID[2];
        u16 DeviceIdLen=0;
	u8 DeviceId[DEVICE_ID_LEN]={0};
	u8 *pSendDataBuf=o_Buf;
	u16 SendDataBufLen=0;

	OSSchedLock();
	//memset(g_TrackerSendDataBuf,0,sizeof(g_TrackerSendDataBuf));
	*pSendDataBuf=TRACKER_PROTOCOL_FIRST;
	pSendDataBuf+=1;
	SendDataBufLen+=1;
	
	*pSendDataBuf=TRACKER_PROTOCOL_SECOND;
	pSendDataBuf+=1;
	SendDataBufLen+=1;
	
	wPackLen=i_wLen+TRACKER_PROTOCOL_BASE_LEN;
	LsbU16To2U8(PackLen,wPackLen);
	memcpy(pSendDataBuf,PackLen,2);
	pSendDataBuf+=2;
	SendDataBufLen+=2;
	
	*pSendDataBuf=TRACKER_PROTOCOL_VERSION;
	pSendDataBuf+=1;
	SendDataBufLen+=1;
	
	ret=hookTrackerGetParameter("SmsPara",DeviceID,DeviceId,&DeviceIdLen);
	memcpy(pSendDataBuf,DeviceId,DeviceIdLen);
	pSendDataBuf+=DeviceIdLen;
	SendDataBufLen+=DeviceIdLen;
	
	MsbU16To2U8(CmdID,i_wCmdID);
	memcpy(pSendDataBuf,CmdID,2);
	pSendDataBuf+=2;
	SendDataBufLen+=2;
	
	memcpy(pSendDataBuf,i_Buf,i_wLen);
	pSendDataBuf+=i_wLen;
	SendDataBufLen+=i_wLen;
	
	wCRC=CRC_MakeCrc(o_Buf,SendDataBufLen);
	LsbU16To2U8(CRCBuf,wCRC);
	memcpy(pSendDataBuf,CRCBuf,2);
	pSendDataBuf+=2;
	SendDataBufLen+=2;
	
	*pSendDataBuf=TRACKER_PROTOCOL_SECOND_LAST;
	pSendDataBuf+=1;
	SendDataBufLen+=1;
	*pSendDataBuf=TRACKER_PROTOCOL_LAST;
	pSendDataBuf+=1;
	SendDataBufLen+=1;
	
	//M_SetBit(g_ucTrackerSendDataFlag);
	
	OSSchedUnlock();
	//memset(i_Buf,0,i_wLen);//这里不需要清零，使用前已经清零，如果清零后面无法使用
	//ret&=TrackerProtocolSendData(i_ucProtocolUser,g_TrackerSendDataBuf,SendDataBufLen);
	*o_pwLen=SendDataBufLen;

}
/*****************************************************************************
-Fuction		: TrackerProtocolSendData
-Description	: TrackerProtocolSendData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolSendData(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen)
{
	u8 ret=FALSE;
	
	ret=hookTrackerSendData(i_ucProtocolUser,i_ucBuf,i_wLen);		
	
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolReportAlarm
-Description	: TrackerProtocolReportAlarm
-Input			:  
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/21    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolReportAlarm(T_Alarm i_tAlarm,u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen)
{
	u8 ret=FALSE;
	u8 *pReportAlarmBuf=i_pucDataBuf;
	u16 wReportAlarmBufLen=0;
	u8 ucErr=0;
	static WifiAccessPointSt s_tWifiApInfo={0};//函数内部会拷贝不需要初始化
	static LocationVectorInfo s_tGpsLocationInfo={0};
	 OSFlagPend(pstOsFlagForAppUse, GPRS_IS_OK_FLAG,OS_FLAG_WAIT_SET_ALL, (OS_TICKS_PER_SEC/50), &ucErr);
	 if(OS_ERR_NONE!=ucErr)
	 {
		 ret=FALSE;
		 DebugPrintf(ERR"Gprs Is not Ok now \r\n");
	 }
	 else
	 {
		ret=TrackerProtocolPackAlarmState(i_tAlarm,g_wTrackerAlarmStateLen,g_TrackerAlarmStateBuf);
		if(FALSE==ret)
		{
			DebugPrintf(ERR"TrackerProtocolReportAlarm err\r\n");
		}
		else
		{
			OSSchedLock();
			M_SetBit(g_ucTrackerReportedAlarmFlag); 		
			memset(&s_tGpsLocationInfo,0,sizeof(LocationVectorInfo));
			OSSchedUnlock();
			TrackerProtocolGetLocationInfo(&s_tWifiApInfo,&s_tGpsLocationInfo);
			OSSchedLock();
			memset(i_pucDataBuf,0,i_wDataLen);
			TrackerProtocolPackTrackerInfo(&s_tWifiApInfo,&s_tGpsLocationInfo,pReportAlarmBuf,&wReportAlarmBufLen);
			pReportAlarmBuf+=wReportAlarmBufLen;
			OSSchedUnlock();

			if(wReportAlarmBufLen>i_wDataLen)
			{
				DebugPrintf(ERR"TrackerProtocolReportAlarmLenErr,Len:%d,MaxLen:%d\r\n",wReportAlarmBufLen,i_wDataLen);
				ret=FALSE;
			}
			else
			{
				TrackerProtocolPackData(TRACKER_PROTOCOL_REPORT_ALARM_CMD,i_pucDataBuf,wReportAlarmBufLen,o_pucDataSendBuf,o_wSendLen);
				ret=TrackerProtocolSendData(PROTOCOL_USED_FOR_GPRS,o_pucDataSendBuf,*o_wSendLen);
			}
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolLogin
-Description	: TrackerProtocolLogin
-Input			:  
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolLogin(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen)
{
	u8 ret=FALSE;
	u8 *pLoginDataBuf=i_pucDataBuf;
	u16 wLoginDataBufLen=0;
	u16 wTempLen=0;
	u16 wLoginLocationInfoLen=0;
	u8 ucErr=0;
	static u8 ucIsNotFirstLoginPlatformFlag=FALSE;
	static WifiAccessPointSt s_tWifiApInfo={0};
	static LocationVectorInfo s_tGpsLocationInfo={0};
	if(FALSE==ucIsNotFirstLoginPlatformFlag)
	{
		TrackerProtocolUpdateGprsPara();
		OSTimeDlyHMSM(0,0,50,0);
		DebugPrintf(CRIT"SystemStartUpdateGprsPara:%d\r\n",ucIsNotFirstLoginPlatformFlag);
		M_SetBit(ucIsNotFirstLoginPlatformFlag);
	}
	else{
	}
	OSFlagPend(pstOsFlagForAppUse, GPRS_IS_OK_FLAG,OS_FLAG_WAIT_SET_ALL, OS_TICKS_PER_SEC, &ucErr);//因汇报获取位置信息需要8秒，flash打印2秒这里凑成30秒
	if(OS_ERR_NONE!=ucErr)
	{
		DebugPrintf("OSFlagPend GprsOk err:%d\r\n",ucErr);//减少打印到FALSH减少擦写flash
	}
	else
	{		
		OSSchedLock();
		memset(&s_tGpsLocationInfo,0,sizeof(LocationVectorInfo));
		OSSchedUnlock();
		TrackerProtocolGetLocationInfo(&s_tWifiApInfo,&s_tGpsLocationInfo);
		
		OSSchedLock();
		memset(i_pucDataBuf,0,i_wDataLen);
		TrackerProtocolPackTrackerInfo(&s_tWifiApInfo,&s_tGpsLocationInfo,pLoginDataBuf,&wLoginLocationInfoLen);
		wLoginDataBufLen+=wLoginLocationInfoLen;
		pLoginDataBuf+=wLoginLocationInfoLen;
		
		ret=hookTrackerGetParameter("CommonPara",SoftWareVision,pLoginDataBuf,&wTempLen);
		if(FALSE==ret)
		{
			DebugPrintf(ERR"hookTrackerGetParameter err0\r\n");
		}
		pLoginDataBuf+=wTempLen;
		wLoginDataBufLen+=wTempLen;
		*pLoginDataBuf++='\0';
		wLoginDataBufLen+=1;
		ret=hookTrackerGetParameter("CommonPara",HardWareVision,pLoginDataBuf,&wTempLen);
		if(FALSE==ret)
		{
			DebugPrintf(ERR"hookTrackerGetParameter err0\r\n");
		}
		pLoginDataBuf+=wTempLen;
		wLoginDataBufLen+=wTempLen;
		*pLoginDataBuf++='\0';
		wLoginDataBufLen+=1;
		ret=hookTrackerGetIMEL(pLoginDataBuf,&wTempLen);
		if(FALSE==ret)
		{
			DebugPrintf(ERR"hookTrackerGetIMEL err\r\n");
		}
		else
		{
			pLoginDataBuf+=wTempLen;
			wLoginDataBufLen+=wTempLen;
		}	
		OSSchedUnlock();
		
		if(wLoginDataBufLen>i_wDataLen)
		{
			DebugPrintf(ERR"TrackerProtocolLogin err,Len:%d,MaxLen:%d\r\n",wLoginDataBufLen,i_wDataLen);
			ret=FALSE;
		}
		else
		{
			TrackerProtocolPackData(TRACKER_PROTOCOL_LOGIN_CMD,i_pucDataBuf,wLoginDataBufLen,o_pucDataSendBuf,o_wSendLen);
			ret=TrackerProtocolSendData(PROTOCOL_USED_FOR_GPRS,o_pucDataSendBuf,*o_wSendLen);
			if(FALSE==ret)
			{
				DebugPrintf(ERR"LoginPack&SendFail\r\n");
			}
			else
			{
				OSSemPend(ptOsSemUsedForLoginPlatformOk,(30*OS_TICKS_PER_SEC),&ucErr);
				if(OS_ERR_NONE!=ucErr)
				{
					DebugPrintf(ERR"OSSemPend LoginPlatformOk err:%d\r\n",ucErr);			
					ret=FALSE;
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
-Fuction		: TrackerProtocolProtocolLogout
-Description	: TrackerProtocolProtocolLogout 
-Input			:  
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/30    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolHeartBeat(u8 *o_pucDataSendBuf,u16 *o_wSendLen)
{
	u8 ret=FALSE;
	u8 ucErr=0;
	u8 ucLogoutDataBuf=0;
	u16 wLogoutDataBufLen=0;
	 OSFlagPend(pstOsFlagForAppUse, GPRS_IS_OK_FLAG,OS_FLAG_WAIT_SET_ALL, (OS_TICKS_PER_SEC/50), &ucErr);
	 if(OS_ERR_NONE!=ucErr)
	{
		ret=FALSE;
		DebugPrintf(ERR"Gprs Is not Ok now, HeartBeat abandon\r\n");
	}
	else
	{
		TrackerProtocolPackData(TRACKER_PROTOCOL_HEAET_BEAT_CMD,&ucLogoutDataBuf,wLogoutDataBufLen,o_pucDataSendBuf,o_wSendLen);
		ret=TrackerProtocolSendData(PROTOCOL_USED_FOR_GPRS,o_pucDataSendBuf,*o_wSendLen);
	}	
	return ret;
}

/*****************************************************************************
-Fuction		: TrackerProtocolProtocolLogout
-Description	: TrackerProtocolProtocolLogout 
-Input			:  
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/30    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolLogout(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen)
{
	u8 ret=FALSE;
	u8 *pLogoutBuf=i_pucDataBuf;
	u16 wLogoutDataBufLen=0;
	u8 ucErr=0;
	static WifiAccessPointSt s_tWifiApInfo={0};
	static LocationVectorInfo s_tGpsLocationInfo={0};
	 OSFlagPend(pstOsFlagForAppUse, GPRS_IS_OK_FLAG,OS_FLAG_WAIT_SET_ALL, (OS_TICKS_PER_SEC/50), &ucErr);
	 if(OS_ERR_NONE!=ucErr)
	{
		ret=FALSE;
		DebugPrintf(ERR"Gprs Is not Ok now \r\n");
	}
	else
	{
		
		OSSchedLock();
		memset(&s_tGpsLocationInfo,0,sizeof(LocationVectorInfo));
		OSSchedUnlock();
		TrackerProtocolGetLocationInfo(&s_tWifiApInfo,&s_tGpsLocationInfo);
		
		OSSchedLock();
		memset(i_pucDataBuf,0,i_wDataLen);
		TrackerProtocolPackTrackerInfo(&s_tWifiApInfo,&s_tGpsLocationInfo,pLogoutBuf,&wLogoutDataBufLen);
		pLogoutBuf+=wLogoutDataBufLen;
		OSSchedUnlock();
		
		if(wLogoutDataBufLen>i_wDataLen)
		{
			DebugPrintf(ERR"TrackerProtocolLogoutLenErr,Len:%d,MaxLen:%d\r\n",wLogoutDataBufLen,i_wDataLen);
			ret=FALSE;
		}
		else
		{
			TrackerProtocolPackData(TRACKER_PROTOCOL_LOGOUT_CMD,i_pucDataBuf,wLogoutDataBufLen,o_pucDataSendBuf,o_wSendLen);
			ret=TrackerProtocolSendData(PROTOCOL_USED_FOR_GPRS,o_pucDataSendBuf,*o_wSendLen);
		}
	}
	return ret;
}

/*****************************************************************************
-Fuction		: TrackerProtocolProtocolReport
-Description	: TrackerProtocolProtocolReport
-Input			:  
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/30    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolReport(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen)
{
	u8 ret=FALSE;
	u8 *pReportDataBuf=i_pucDataBuf;
	u16 wReportDataBufLen=0;
	u16 wTrackerLocationInfoLen=0;	
	u8 ucLocationInfoLen=0;
	u8 ucErr=0;
	u8 ucSaveInFlashDataNum=0;
	u8 ucCount=0;
	u8 ucSendDataOkFlag=FALSE;
	u8 ucReadLocationInfoLenBuf[TRACKER_PROTOCOL_HISTORY_LOCATION_INFO_PACK_NUM]={0};
	static WifiAccessPointSt s_tWifiApInfo={0};
	static LocationVectorInfo s_tGpsLocationInfo={0};
	 
	 OSSchedLock();
	 memset(&s_tGpsLocationInfo,0,sizeof(LocationVectorInfo));
	 OSSchedUnlock();
	TrackerProtocolGetLocationInfo(&s_tWifiApInfo,&s_tGpsLocationInfo);
	
	OSSchedLock();//不加可能会丢数据
	memset(i_pucDataBuf,0,i_wDataLen);
	*pReportDataBuf++=0x00;
	*pReportDataBuf++=0x01;
	TrackerProtocolPackTrackerInfo(&s_tWifiApInfo,&s_tGpsLocationInfo,pReportDataBuf,&wReportDataBufLen);
	pReportDataBuf+=wReportDataBufLen;
	wReportDataBufLen+=2;//前面的两个长度
	ucLocationInfoLen=i_pucDataBuf[34];
	OSSchedUnlock();
	
	OSFlagAccept(pstOsFlagForAppUse, GPRS_IS_OK_FLAG,OS_FLAG_WAIT_SET_ALL,&ucErr);
	if(OS_ERR_NONE!=ucErr)
	{	
		wReportDataBufLen-=2;
		DebugPrintf(ERR"GprsNotOk!,ReportInfoSaved,Len%dInfoLen%d \r\n",wReportDataBufLen,ucLocationInfoLen);		
		OSSchedLock();
		LocationInfoSaveInFlash(&i_pucDataBuf[2],wReportDataBufLen);//只保存位置信息	
		OSSchedUnlock();
		M_ClrBit(ucSendDataOkFlag);	
		ret=FALSE;
	}
	else
	{
		OSFlagAccept(pstOsFlagForAppUse, LOGIN_PLATFORM_OK_FLAG,OS_FLAG_WAIT_SET_ALL,&ucErr);	
		if(OS_ERR_NONE!=ucErr)
		{
			wReportDataBufLen-=2;
			DebugPrintf(ERR"NotLoginOkNow,ReportInfoWillBeSaved,InfoLen:%d \r\n",wReportDataBufLen);
			OSSchedLock();
			LocationInfoSaveInFlash(&i_pucDataBuf[2],wReportDataBufLen);//只保存位置信息	
			OSSchedUnlock();
			M_ClrBit(ucSendDataOkFlag); 
			ret=FALSE;
		}
		else
		{
			TrackerProtocolPackData(TRACKER_PROTOCOL_REPORT_CMD,i_pucDataBuf,wReportDataBufLen,o_pucDataSendBuf,o_wSendLen);
			ret=TrackerProtocolSendData(PROTOCOL_USED_FOR_GPRS,o_pucDataSendBuf,*o_wSendLen);
			if(FALSE==ret)
			{
				wReportDataBufLen-=2;
				DebugPrintf(ERR"TrackerProtocolPack&SendData err,ReportInfoWillBeSaved:%d\r\n",wReportDataBufLen);			
				OSSchedLock();
				LocationInfoSaveInFlash(&i_pucDataBuf[2],wReportDataBufLen);//只保存位置信息			
				OSSchedUnlock();
				M_ClrBit(ucSendDataOkFlag);
			}
			else
			{
				M_SetBit(ucSendDataOkFlag);
			}		
		}
	}
	if(FALSE==ucSendDataOkFlag)
	{
	}
	else
	{
		OSSchedLock();
		memset(i_pucDataBuf,0,i_wDataLen);
		pReportDataBuf=&i_pucDataBuf[2];//留两个位置放前面的两个数据
		wReportDataBufLen=0;		
		ucErr=LocationInfoReadFromFlash(TRACKER_PROTOCOL_HISTORY_LOCATION_INFO_PACK_NUM,
			                                                    pReportDataBuf,&wReportDataBufLen,ucReadLocationInfoLenBuf,&ucSaveInFlashDataNum);		
		OSSchedUnlock();
		
		if((FALSE==ucErr)||(0==ucSaveInFlashDataNum))
		{
		}
		else
		{	
			OSSchedLock();
			i_pucDataBuf[0]=0x01;//有历史定位数据
			i_pucDataBuf[1]=ucSaveInFlashDataNum;//有多少包
			wReportDataBufLen+=2;//前面两个数据的长度
			OSSchedUnlock();
			
			TrackerProtocolPackData(TRACKER_PROTOCOL_REPORT_CMD,i_pucDataBuf,wReportDataBufLen,o_pucDataSendBuf,o_wSendLen);
			ret=TrackerProtocolSendData(PROTOCOL_USED_FOR_GPRS,o_pucDataSendBuf,*o_wSendLen);
			if(FALSE==ret)
			{
				DebugPrintf(ERR"ReportTrackerProtocolPack&SendData er,HistoryReportInfoWillBeSaved\r\n");
				wTrackerLocationInfoLen=0;				
				OSSchedLock();
				for(ucCount=0;ucCount<ucSaveInFlashDataNum;ucCount++)
				{
					LocationInfoSaveInFlash(&i_pucDataBuf[2+wTrackerLocationInfoLen],ucReadLocationInfoLenBuf[ucCount]);//只保存位置信息
					wTrackerLocationInfoLen+=ucReadLocationInfoLenBuf[ucCount];
				}
				OSSchedUnlock();
			}
			else
			{
			}			
		}			
		M_ClrBit(ucSendDataOkFlag);
	}
	return ret;
}
/*******************追踪器协议操作支持函数***********************/

/*****************************************************************************
-Fuction		: TrackerProtocolGetLocationInfo
-Description	: TrackerProtocolGetLocationInfo
-Input			:   
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/11/01    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolGetLocationInfo(WifiAccessPointSt *o_ptWifiApInfo,LocationVectorInfo *o_ptLocationInfo)
{
	u8 ret=FALSE;
	u8 ucErr;
	u8 ucCount=0;
	LocationVectorInfo tGpsLocationInfo={0};
		
	g_tGsm.GsmSetLbsLocationUpdateReq();//这里发送请求，打包位置信息函数里获取该数据
	OSFlagAccept(pstOsFlagForAppUse, GPRS_IS_OK_FLAG,OS_FLAG_WAIT_SET_ALL, &ucErr);
	if(OS_ERR_NONE!=ucErr)//GPRS不ok，gsm是一直醒的
	{	
	}
	else
	{
		OSSemPost(ptOsSemUsedForResumeGsmTask);
	}
	for(ucCount=0;ucCount<10;ucCount++)
	{
		ret=g_tWifi.WifiSetScanApReq();//提前时间打开，不然没那么快获得信息 
		if(FALSE==ret)
		{
		}
		else
		{
			break;
		}
		OSTimeDly(100);
	}
	OSSemPend(ptOsSemUsedForGetWifiApSuccess,8*OS_TICKS_PER_SEC,&ucErr);	
	if(OS_ERR_NONE!=ucErr)
	{
		DebugPrintf(ERR"PendGetWifiApSuccess err:%d\r\n",ucErr);
	}
	else{
	}
	ret=g_tGPS.GpsGetHistoryPos(&tGpsLocationInfo);
	if(NULL==o_ptWifiApInfo)
	{
		DebugPrintf(ERR"GetLocationInfo o_ptWifiApInfo null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=g_tWifi.WifiReadApData(o_ptWifiApInfo);
		ret&=TrackerProtocolWifiApNumIsEnough(o_ptWifiApInfo);
	}
	if(FALSE==ret)
	{
		ret=g_tGPS.GpsGetPos(&tGpsLocationInfo);
	}	
	else
	{
	}
	OSSchedLock();
	memcpy(o_ptLocationInfo,&tGpsLocationInfo,sizeof(LocationVectorInfo));
	OSSchedUnlock();
	return ret;
}

/*****************************************************************************
-Fuction		: TrackerProtocolWifiApNumIsEnough
-Description	: TrackerProtocolWifiApNumIsEnough 
-Input			:   
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolWifiApNumIsEnough(WifiAccessPointSt *i_ptWifiApInfo)
{
	u8 ret=FALSE;
	u8 ucTemp=0;
	u16 wWifiLen=0;
	for(ucTemp= 0;ucTemp < TRACKER_PROTOCOL_MAX_ACCESS_POINT_NUM;ucTemp++)
	{
		OSSchedLock();
		wWifiLen=strnlen(i_ptWifiApInfo->stAccessPointArray[ucTemp].ssid,AT_WIFI_AP_NAME_LEN_MAX);
		OSSchedUnlock();
		if(IS_FALSE_BIT(wWifiLen))
		{
			break;
		}
	}		
	if(ucTemp<TRACKER_PROTOCOL_ENOUGH_AP_NUM)
	{
		ret=FALSE;
		DebugPrintf(ERR"wifi info is not enough!\n");
	}
	else
	{
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: TrackerProtocolUpdateGprsPara
-Description	: TrackerProtocolUpdateGprsPara
-Input			:   
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolUpdateGprsPara()
{
	u8 ret=FALSE;
	static GsmParaSt tGsmPara={0};
	u16 wParaLen;
	u8 ucDailMode;
	u8  aucPort[2];
	memset(&tGsmPara,0,sizeof(GsmParaSt));
	hookTrackerGetParameter("NetPara",Apn,tGsmPara.GsmLinkPara.u8Apn,&wParaLen);	
	hookTrackerGetParameter("NetPara",UsrName,tGsmPara.GsmLinkPara.u8UserName,&wParaLen);
	hookTrackerGetParameter("NetPara",Pwd,tGsmPara.GsmLinkPara.u8PassWord,&wParaLen);
	tGsmPara.GsmGprsPara.GprsCommMode=GPRS_COMM_MODE_TCP;	
	hookTrackerGetParameter("NetPara",DailMode,&ucDailMode,&wParaLen);	
	if(DOMAIN_DIAL_MODE==ucDailMode)
	{
		hookTrackerGetParameter("NetPara",Domain,tGsmPara.GsmGprsPara.u8IpDomain,&wParaLen);	
	}
	else
	{
		hookTrackerGetParameter("NetPara",Ip,tGsmPara.GsmGprsPara.u8IpDomain,&wParaLen);	
	}
	hookTrackerGetParameter("NetPara",Port,aucPort,&wParaLen);	
	tGsmPara.GsmGprsPara.u16CommPort=Lsb2U8ToU16(aucPort);
	ret=GsmDriverInit(&tGsmPara);
	return ret;
}

/*****************************************************************************
-Fuction		: TrackerProtocolPackTrackerInfo
-Description	: TrackerProtocolPackTrackerInfo 
-Input			:   
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/28    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolPackTrackerInfo(WifiAccessPointSt *i_ptWifiApInfo,LocationVectorInfo *i_ptLocationInfo,u8 *o_ucBuf,u16 *o_wBufLen)
{
	u8 ret=FALSE;
	u8 ucTemp=0;
	u16 wTemp=0;
	u8 ucTempBuf[4];
	u8 ucSpeedTempBuf[2];
	T_Alarm tAlarm={0};
	u8 *pucTemp=o_ucBuf;
	u8 *pucAddtionalInfoSwitch;
	u8 AddtioalInfoFlag=FALSE;
	E_PowerCheckStateType ePowerCheckState;
	*pucTemp++=i_ptLocationInfo->dataTime.date.day;
	*pucTemp++=i_ptLocationInfo->dataTime.date.month;
	*pucTemp++=i_ptLocationInfo->dataTime.date.year;
	*pucTemp++=i_ptLocationInfo->dataTime.time.hour;
	*pucTemp++=i_ptLocationInfo->dataTime.time.minute;
	*pucTemp++=i_ptLocationInfo->dataTime.time.second;
	i_ptLocationInfo->positionInfo.lat=(u32)(i_ptLocationInfo->positionInfo.lat*3.6+0.5);
	LsbU32To4U8(ucTempBuf,i_ptLocationInfo->positionInfo.lat);
	memcpy(pucTemp,ucTempBuf,4);
	pucTemp+=4;
	i_ptLocationInfo->positionInfo.lon=(u32)(i_ptLocationInfo->positionInfo.lon*3.6+0.5);
	LsbU32To4U8(ucTempBuf,i_ptLocationInfo->positionInfo.lon);
	memcpy(pucTemp,ucTempBuf,4);
	pucTemp+=4;	
	wTemp=(u16)((i_ptLocationInfo->pointInfo.speed*1000)/36);
	LsbU16To2U8(ucSpeedTempBuf,wTemp);
	memcpy(pucTemp,ucSpeedTempBuf,2);
	pucTemp+=2;	
	wTemp=(u16)i_ptLocationInfo->pointInfo.course;
	if(wTemp>=3600)
	{
		DebugPrintf(ERR"Gps Direction err%d",wTemp);
		wTemp=3590;
	}
	LsbU16To2U8(ucSpeedTempBuf,wTemp);
	memcpy(pucTemp,ucSpeedTempBuf,2);
	pucTemp+=2;	
	ucTemp=0;
	if(LONGITUDE_EAST==i_ptLocationInfo->positionInfo.lonRegion)
	{
		ucTemp|=1;
	}
	else
	{
	}
	if(LATITUDE_NORTH==i_ptLocationInfo->positionInfo.latRegion)
	{
		ucTemp|=2;
	}
	switch(i_ptLocationInfo->locationCommonPara.fixStatus)
	{
		case FIX_INVALID:
			break;
		case FIX_2D:
			ucTemp |= 1 << 2;
			break;
		case FIX_3D:
			ucTemp |= 3 << 2;
			break;
		default:
			break;
	}
	if(i_ptLocationInfo->locationCommonPara.satelliteNum>15)//四位最大只能表示15
	{
		i_ptLocationInfo->locationCommonPara.satelliteNum=15;
	}
	else
	{
	}
	ucTemp|=i_ptLocationInfo->locationCommonPara.satelliteNum<<4;
	*pucTemp++=ucTemp;
	g_tAlarmOpr.GetEffectiveState(&tAlarm);	

	wTemp=TRACKER_PROTOCOL_ALARM_STATE_LEN;
	TrackerProtocolPackAlarmState(tAlarm,wTemp,ucTempBuf);
	memcpy(pucTemp,ucTempBuf,TRACKER_PROTOCOL_ALARM_STATE_LEN);
	pucTemp+=TRACKER_PROTOCOL_ALARM_STATE_LEN;
	g_tPowerStateCheck.GetPowerCheckState(&ePowerCheckState,&ucTemp);
	*pucTemp++=ucTemp;
	memset(pucTemp, 0x00,6);
	pucTemp += 6; 
	
	pucAddtionalInfoSwitch=pucTemp;
	*pucAddtionalInfoSwitch=0x01;//附加信息
	pucTemp++;
	ret=TrackerProtocolPackWifiInfo(i_ptWifiApInfo,pucTemp,&wTemp);
	if(FALSE==ret)
	{
		DebugPrintf(ERR"TrackerProtocolPackWifiInfo err \r\n");
	}
	else
	{
		pucTemp+=wTemp;
		M_SetBit(AddtioalInfoFlag);
	}
	ret=TrackerProtocolPackCellTowerInfo(pucTemp,&wTemp);
	if(FALSE==ret)
	{
		DebugPrintf(ERR"TrackerProtocolPackCellTowerInfo err \r\n");
	}
	else
	{
		pucTemp+=wTemp;
		M_SetBit(AddtioalInfoFlag);
	}
	if(IS_FALSE_BIT(AddtioalInfoFlag))
	{
		*pucAddtionalInfoSwitch=0x00;
	}
	else
	{
		*pucAddtionalInfoSwitch=0x01;
	}
	memset(pucTemp, 0x00,4);//基站信息
	pucTemp += 4; 
	*o_wBufLen=(pucTemp-o_ucBuf);


	ret=TRUE;
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolPackWifiInfo
-Description	: TrackerProtocolPackWifiInfo 
-Input			:   
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/01    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolPackWifiInfo(WifiAccessPointSt *i_ptWifiApInfo,u8 *o_ucBuf,u16 *o_wBufLen)
{
	u8 ret=FALSE;
	u8 ucTemp=0;
	u16 wWifiInfoLen=0;
	u8 *pucWifiInfo=o_ucBuf;
	u8 *pucWifiInfoLen;
	*pucWifiInfo++=0x01;
	pucWifiInfoLen=pucWifiInfo;
	pucWifiInfo++;
	for(ucTemp= 0;ucTemp < TRACKER_PROTOCOL_MAX_ACCESS_POINT_NUM;ucTemp++)
	{
		if(IS_FALSE_BIT(strnlen(i_ptWifiApInfo->stAccessPointArray[ucTemp].ssid,AT_WIFI_AP_NAME_LEN_MAX)))
		{
			break;
		}
		strncpy(pucWifiInfo,i_ptWifiApInfo->stAccessPointArray[ucTemp].ssid,AT_WIFI_AP_NAME_LEN_MAX);
		pucWifiInfo += strnlen(i_ptWifiApInfo->stAccessPointArray[ucTemp].ssid,AT_WIFI_AP_NAME_LEN_MAX);
		*pucWifiInfo++ = 0x00;
		wWifiInfoLen += strnlen(i_ptWifiApInfo->stAccessPointArray[ucTemp].ssid,AT_WIFI_AP_NAME_LEN_MAX) + 1;
	
		memcpy(pucWifiInfo,i_ptWifiApInfo->stAccessPointArray[ucTemp].mac_address,AT_WIFI_AP_MAC_NUM);
		pucWifiInfo += AT_WIFI_AP_MAC_NUM;
		*pucWifiInfo++ = i_ptWifiApInfo->stAccessPointArray[ucTemp].signal_strength;
		memcpy(pucWifiInfo,(u8 *)&i_ptWifiApInfo->stAccessPointArray[ucTemp].age,4);
		pucWifiInfo += 4;
		*pucWifiInfo++ = i_ptWifiApInfo->stAccessPointArray[ucTemp].channel;
		*pucWifiInfo++ = i_ptWifiApInfo->stAccessPointArray[ucTemp].signal_to_noise_ratio;
		wWifiInfoLen += AT_WIFI_AP_MAC_NUM + 7;
	}		
	if(IS_FALSE_BIT(ucTemp))
	{
		ret=FALSE;
		DebugPrintf(ERR"not rev wifi info!\n");
	}
	else
	{
		*pucWifiInfoLen=wWifiInfoLen;
		 *o_wBufLen=(pucWifiInfo-o_ucBuf);
		 ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolPackCellTowerInfo
-Description	: TrackerProtocolPackCellTowerInfo 
-Input			:   
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/01    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolPackCellTowerInfo(u8 *o_ucBuf,u16 *o_wBufLen)
{	
	u8 ret=FALSE;
	u8 ucTemp=0;
	u16 wCellTowerInfoLen=0;
	u8 *pucCellTowerInfo=o_ucBuf;
	u8 *pucCellTowerLen;
	CellTowerInfoSt tCellTowerInfo={0};
	ret=g_tGsm.GsmReadLbsLocationData(&tCellTowerInfo);
	if(FALSE==ret)
	{
		DebugPrintf(ERR"Protocol Read CellTowerInfo Fail\r\n");
	}
	else
	{
		*pucCellTowerInfo++=0x02;
		pucCellTowerLen=pucCellTowerInfo;
		pucCellTowerInfo++;
		for(ucTemp = 0;ucTemp < MAX_GSM_CELL_TOWER_NUM;ucTemp++)
		{
			if( IS_FALSE_BIT(tCellTowerInfo.stCellTower[ucTemp].s32CellId) || 
			     IS_FALSE_BIT(tCellTowerInfo.stCellTower[ucTemp].u16LocAreaCode) ||
			     IS_FALSE_BIT(tCellTowerInfo.stCellTower[ucTemp].u16MobileCountryCode) )
			{
				break;
			}			
			if((0xFFFFFFFF == tCellTowerInfo.stCellTower[ucTemp].s32CellId) || 
			    (0xFFFF == tCellTowerInfo.stCellTower[ucTemp].u16LocAreaCode) || 
			    (0xFFFF == tCellTowerInfo.stCellTower[ucTemp].u16MobileCountryCode) || 
			    (0xFF == tCellTowerInfo.stCellTower[ucTemp].s8MobileNetworkCode) )
			{
				break;
			}
			
			memcpy(pucCellTowerInfo,(u8 *)&tCellTowerInfo.stCellTower[ucTemp].s32CellId,sizeof(tCellTowerInfo.stCellTower[ucTemp].s32CellId));
			pucCellTowerInfo += sizeof(tCellTowerInfo.stCellTower[ucTemp].s32CellId);
			memcpy(pucCellTowerInfo,(u8 *)&tCellTowerInfo.stCellTower[ucTemp].u16LocAreaCode,sizeof(tCellTowerInfo.stCellTower[ucTemp].u16LocAreaCode));
			pucCellTowerInfo += sizeof(tCellTowerInfo.stCellTower[ucTemp].u16LocAreaCode);
			memcpy(pucCellTowerInfo,(u8 *)&tCellTowerInfo.stCellTower[ucTemp].u16MobileCountryCode,sizeof(tCellTowerInfo.stCellTower[ucTemp].u16MobileCountryCode));
			pucCellTowerInfo += sizeof(tCellTowerInfo.stCellTower[ucTemp].u16MobileCountryCode);
			memcpy(pucCellTowerInfo,(u8 *)&tCellTowerInfo.stCellTower[ucTemp].s8MobileNetworkCode,sizeof(tCellTowerInfo.stCellTower[ucTemp].s8MobileNetworkCode));
			pucCellTowerInfo += sizeof(tCellTowerInfo.stCellTower[ucTemp].s8MobileNetworkCode);
			memcpy(pucCellTowerInfo,(u8 *)&tCellTowerInfo.stCellTower[ucTemp].s32Age,sizeof(tCellTowerInfo.stCellTower[ucTemp].s32Age));
			pucCellTowerInfo += sizeof(tCellTowerInfo.stCellTower[ucTemp].s32Age);
			memcpy(pucCellTowerInfo,(u8 *)&tCellTowerInfo.stCellTower[ucTemp].s8SignalStrength,sizeof(tCellTowerInfo.stCellTower[ucTemp].s8SignalStrength));
			pucCellTowerInfo += sizeof(tCellTowerInfo.stCellTower[ucTemp].s8SignalStrength);
			memcpy(pucCellTowerInfo,(u8 *)&tCellTowerInfo.stCellTower[ucTemp].u16TimingAdvance,sizeof(tCellTowerInfo.stCellTower[ucTemp].u16TimingAdvance));
			pucCellTowerInfo += sizeof(tCellTowerInfo.stCellTower[ucTemp].u16TimingAdvance);
		
			wCellTowerInfoLen += TRACKER_PROTOCOL_CELL_TOWER_INFO_LEN;
		}
		if(IS_FALSE_BIT(ucTemp))
		{
			ret=FALSE;
			DebugPrintf(ERR"not rev CellTower info!\n");
		}
		else
		{
			*pucCellTowerLen=wCellTowerInfoLen;
			 *o_wBufLen=(pucCellTowerInfo-o_ucBuf);
		}
	}
	return ret;
}

/*****************************************************************************
-Fuction		: TrackerProtocolPackAlarmState
-Description	: TrackerProtocolPackAlarmState
-Input			:   
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/21    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolPackAlarmState(T_Alarm i_tAlarm,u16 i_wLen,u8 *o_ucBuf)
{
	u8 ret=FALSE;	
	if((NULL==o_ucBuf)||(TRACKER_PROTOCOL_ALARM_STATE_LEN!=i_wLen))
	{
		DebugPrintf(ERR"TrackerProtocolPackAlarmState err \r\n");
		ret=FALSE;
	}
	else
	{
		memset(o_ucBuf,0,i_wLen);
		if(1==i_tAlarm.CutTrackerAlarm)
		{
			o_ucBuf[0]|=TRACKER_PROTOCOL_CUT_TRACKER_STATE;
		}
		else
		{
		}
		if(1==i_tAlarm.DropTrackerAlarm)
		{
			o_ucBuf[0]|=TRACKER_PROTOCOL_DROP_TRACKER_STATE;
		}
		else
		{
		}
		if(1==i_tAlarm.LowPowerAlarm)
		{
			o_ucBuf[2]|=TRACKER_PROTOCOL_LOW_POWER_STATE;
		}
		else
		{
		}
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolPackAlarmState
-Description	: TrackerProtocolPackAlarmState
-Input			:   
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/21    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolParserAlarmState(u8 *i_ucBuf,u16 i_wLen,T_Alarm *o_tAlarm)
{
	u8 ret=FALSE;	
	if((NULL==i_ucBuf)||(TRACKER_PROTOCOL_ALARM_STATE_LEN!=i_wLen))
	{
		DebugPrintf(ERR"TrackerProtocolParserAlarmState err \r\n");
		ret=FALSE;
	}
	else
	{
		o_tAlarm->CutTrackerAlarm=i_ucBuf[0]&TRACKER_PROTOCOL_CUT_TRACKER_STATE;
		o_tAlarm->DropTrackerAlarm=i_ucBuf[0]&TRACKER_PROTOCOL_DROP_TRACKER_STATE;
		o_tAlarm->LowPowerAlarm=i_ucBuf[2]&TRACKER_PROTOCOL_LOW_POWER_STATE;		
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: TrackerProtocolGetAlarmState
-Description	: TrackerProtocolGetAlarmState
-Input			:  
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/21    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolGetAlarmState(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	if(IS_FALSE_BIT(g_ucTrackerReportedAlarmFlag))
	{
		DebugPrintf(ERR"TrackerProtocolGetAlarmState err\r\n");
	}
	else
	{
		*o_wLen=g_wTrackerAlarmStateLen;
		memcpy(o_ucBuf,g_TrackerAlarmStateBuf,*o_wLen);
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: TrackerProtocolSoftwarePackCrcCheck
-Description	: TrackerProtocolSoftwarePackCrcCheck
-Input			:  
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/21    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolSoftwarePackCrcCheck(u8* i_pDataBuf,u16 i_wDataLen,u8 i_ucLastPackFlg)
{
	u8 ret = FALSE;
	u32 i = 0;
	static u16 s_wRecvUpdatePackSize=0;
	
	for(i = 0;i < i_wDataLen;i++)
	{
		g_tSoftwarePackInfo.CurrentCrc= GetFcs(g_tSoftwarePackInfo.CurrentCrc,i_pDataBuf[i]);
		s_wRecvUpdatePackSize++;
		if(s_wRecvUpdatePackSize >= 0x8000)//当接收到的一包完整的32K数据后再比较
		{
			g_tSoftwarePackInfo.CurrentCrc ^= FCS_START;
			if(memcmp((u8 *)&g_tSoftwarePackInfo.Crc[g_tSoftwarePackInfo.CrcIndex],(u8 *)&g_tSoftwarePackInfo.CurrentCrc,2) != 0)
			{
				s_wRecvUpdatePackSize = 0;
				ret = FALSE;				
				DebugPrintf(ERR"err:Update Package RecvCRC=%d\r\n",g_tSoftwarePackInfo.Crc[g_tSoftwarePackInfo.CrcIndex]);
				DebugPrintf(ERR"err:Update Package CurCRC=%d\r\n",g_tSoftwarePackInfo.CurrentCrc);
			}
			else
			{
				g_tSoftwarePackInfo.CrcIndex++;
				g_tSoftwarePackInfo.CurrentCrc = FCS_START;
				s_wRecvUpdatePackSize = 0;
				ret=TRUE;
			}
		}
		else
		{
			ret=TRUE;
		}
	}
	if(FALSE==ret)
	{
	}
	else
	{
		if(TRUE==i_ucLastPackFlg)
		{		
			g_tSoftwarePackInfo.CurrentCrc ^= FCS_START;
			if (s_wRecvUpdatePackSize != 0)
			{			
				if(memcmp((u8 *)&g_tSoftwarePackInfo.Crc[g_tSoftwarePackInfo.CrcIndex],(u8 *)&g_tSoftwarePackInfo.CurrentCrc,2) != 0)
				{
					s_wRecvUpdatePackSize = 0;
					DebugPrintf(ERR"LastPackerr:Update Package RecvCRC=%d\r\n",g_tSoftwarePackInfo.Crc[g_tSoftwarePackInfo.CrcIndex]);
					DebugPrintf(ERR"LastPackerr:Update Package CurCRC=%d\r\n",g_tSoftwarePackInfo.CurrentCrc);
					g_tSoftwarePackInfo.CurrentCrc = FCS_START;
					ret = FALSE;
				}
				else
				{
					ret=TRUE;
				}
			}
			else
			{
				ret=TRUE;
			}
		}
		else
		{
			ret=TRUE;
		}
	}
	
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerProtocolUpdatePackageAck
-Description	: TrackerProtocolUpdatePackageAck
-Input			:  
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/21    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolUpdatePackageAck(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,u8 i_ucUpdateResult,u16 i_wAckCmd,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	u8 *pucPackAckBuf=o_ptHandCmdAck->pucDataBuf;
	u16 wUpateAckBufLen=0;
	if(i_wLen<4)
	{
		DebugPrintf(ERR"UpdateRecvDataLenErr:%#x\r\n",i_wLen);
	}
	else
	{
		OSSchedLock();
		memset(o_ptHandCmdAck->pucDataBuf,0,o_ptHandCmdAck->wMaxDataLen);
		wUpateAckBufLen=0;
		
		memcpy(pucPackAckBuf,i_ucBuf,4);//升级服务ID
		pucPackAckBuf+=4;
		wUpateAckBufLen+=4;
		
		if(i_wAckCmd==TRACKER_PROTOCOL_UPDATE_PACK_ACK)
		{
			*pucPackAckBuf = i_ucUpdateResult;
			pucPackAckBuf++;
			wUpateAckBufLen++;
			memcpy(pucPackAckBuf,(u8 *)&g_tSoftwarePackInfo.CurrentRecvPackageNum,2);				
			pucPackAckBuf+=2;
			wUpateAckBufLen+=2; 				
		}
		else if(i_wAckCmd==TRACKER_PROTOCOL_UPDATE_REQUEST_ACK)
		{
			memcpy(pucPackAckBuf,g_tSoftwarePackInfo.SN,TRACKER_PROTOCOL_SOFTWARE_SN_LEN);
			pucPackAckBuf+=TRACKER_PROTOCOL_SOFTWARE_SN_LEN;
			wUpateAckBufLen+=TRACKER_PROTOCOL_SOFTWARE_SN_LEN;
			*pucPackAckBuf++=i_ucUpdateResult;
			wUpateAckBufLen++;	
		}
		else
		{
			DebugPrintf(ERR"UnkownUpdateAckCmd:%#x\r\n",i_wAckCmd);
		}
		
		OSSchedUnlock();
		TrackerProtocolPackData(i_wAckCmd,o_ptHandCmdAck->pucDataBuf,wUpateAckBufLen,o_ptHandCmdAck->pucDataSendBuf,&o_ptHandCmdAck->wSendLen);
		ret=TrackerProtocolSendData(i_ucProtocolUser,o_ptHandCmdAck->pucDataSendBuf,o_ptHandCmdAck->wSendLen);
	}
}
/**********************追踪器协议处理函数**************************/

/*****************************************************************************
-Fuction		: TrackerProtocolSendData
-Description	: TrackerProtocolSendData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerLoginAck(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	u32 PlatformRtcTime;
	if(4!=i_wLen)
	{
		ret=FALSE;
		DebugPrintf(ERR"Login Ack Format err \r\n");
	}
	if(4==i_wLen)
	{
		PlatformRtcTime=Lsb4U8ToU32(i_ucBuf);
		//ret=TRUE;
		ret=hookTrackerRtcTimeUpdate(PlatformRtcTime);
		
		//OSSemPost(ptOsSemUsedForFixedTimeReportTask);
		OSSemPost(ptOsSemUsedForLoginPlatformOk);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerHeartBeatAck
-Description	: TrackerHeartBeatAck
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerHeartBeatAck(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	DebugPrintf("HeartBeat ok \r\n");
	OSSemPost(ptOsSemUsedForPlatformCommunicateHeartbeatOk);	
	ret=TRUE;
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerCallTheRoll
-Description	: TrackerCallTheRoll
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerCallTheRoll(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	u8 *pCallTheRollDataBuf=o_ptHandCmdAck->pucDataBuf;
	u16 wCallTheRollDataBufLen=0;
	u8 ucErr=0;
	static WifiAccessPointSt s_tWifiApInfo={0};
	static LocationVectorInfo s_tGpsLocationInfo={0};
	 OSFlagPend(pstOsFlagForAppUse, GPRS_IS_OK_FLAG,OS_FLAG_WAIT_SET_ALL, (OS_TICKS_PER_SEC/50), &ucErr);
	 if(OS_ERR_NONE!=ucErr)
	{
		ret=FALSE;
		DebugPrintf(ERR"Gprs Is not Ok now \r\n");
	}
	else
	{	
		
		OSSchedLock();
		memset(&s_tGpsLocationInfo,0,sizeof(LocationVectorInfo));
		OSSchedUnlock();
		TrackerProtocolGetLocationInfo(&s_tWifiApInfo,&s_tGpsLocationInfo);
		
		OSSchedLock();
		memset(o_ptHandCmdAck->pucDataBuf,0,o_ptHandCmdAck->wMaxDataLen);
		TrackerProtocolPackTrackerInfo(&s_tWifiApInfo,&s_tGpsLocationInfo,pCallTheRollDataBuf,&wCallTheRollDataBufLen);
		pCallTheRollDataBuf+=wCallTheRollDataBufLen;		
		OSSchedUnlock();
		o_ptHandCmdAck->wAckCmd=TRACKER_PROTOCOL_CALL_THE_ROLL_ACK;
		TrackerProtocolPackData(o_ptHandCmdAck->wAckCmd,o_ptHandCmdAck->pucDataBuf,wCallTheRollDataBufLen,o_ptHandCmdAck->pucDataSendBuf,&o_ptHandCmdAck->wSendLen);		
		ret=TrackerProtocolSendData(i_ucProtocolUser,o_ptHandCmdAck->pucDataSendBuf,o_ptHandCmdAck->wSendLen);
	}
	return ret;
}

/*****************************************************************************
-Fuction		: TrackerAlarmAck
-Description	: TrackerAlarmAck  g_TrackerGetSmsBuf
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerAlarmAck(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	u8 ucSmsNum=i_ucBuf[0];
	u8 ucSmsLen;
	u8 *pucAlarmAck=i_ucBuf;
	u8 ucBufPos=0;
	u8 aucAlarmStateBuf[4];
	u16 wAlarmStateBufLen;
	T_Alarm tAlarm;
	s32 i;
	if(i_wLen<TRACKER_PROTOCOL_MIN_ALARM_ACK_LEN)
	{
		DebugPrintf(ERR"TrackerAlarmAck err \r\n");
		ret=FALSE;
	}
	else
	{
		pucAlarmAck++;
		//if(0==ucSmsNum)//由于有时没有透传短信，所以不用判断
		{
		//	DebugPrintf("TrackerAlarmAck no sms\r\n");
		}
		//else
		{
			ucBufPos=0;
			for(i=0;i<ucSmsNum;i++)
			{
				ucSmsLen=*pucAlarmAck++;
				memcpy(g_TrackerGetSmsBuf[ucBufPos],pucAlarmAck,ucSmsLen);
				ucBufPos++;
				if(ucBufPos>=TRACKER_PROTOCOL_MAX_RECV_SMS_COUNT)
				{
					ucBufPos=0;
				}
				pucAlarmAck+=ucSmsLen;
			}
			ret=TrackerProtocolGetAlarmState(aucAlarmStateBuf,&wAlarmStateBufLen);
			if(FALSE==ret)
			{
				DebugPrintf(ERR"TrackerAlarmAck get alarm state err\r\n");
			}
			else
			{
				if(0!=memcmp(pucAlarmAck,aucAlarmStateBuf,wAlarmStateBufLen))
				{
					ret=FALSE;
				}
				else
				{
					wAlarmStateBufLen=TRACKER_PROTOCOL_ALARM_STATE_LEN;
					ret=TrackerProtocolParserAlarmState(pucAlarmAck,wAlarmStateBufLen,&tAlarm);
					ret&=hookTrackerRelieveAlarm(tAlarm);
					if(FALSE==ret)
					{
					}
					else
					{
						//OSSemPost(ptOsSemUsedForPlatformReceivedAlarm);						
					}
				}
			}
		}	
		//由于收到平台确认不解除报警，同时为防止报警恢复还报警的现象
		ret=TRUE;
		OSSemPost(ptOsSemUsedForPlatformReceivedAlarm); 					
	}
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerReportAck
-Description	: TrackerReportAck
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerReportAck(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	//OSSemPost(ptOsSemUsedForFixedTimeReportInfoReceived);现在的平台没有回应的
	ret=TRUE;
	return ret;
}

/*****************************************************************************
-Fuction		: TrackerReset
-Description	: TrackerReset
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerReset(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=TRUE;
	u8 ucResetAck=1;
	u16 wResetAckLen=1;
	
	o_ptHandCmdAck->wAckCmd=TRACKER_PROTOCOL_RESET_ACK;
	TrackerProtocolPackData(o_ptHandCmdAck->wAckCmd,&ucResetAck,wResetAckLen,o_ptHandCmdAck->pucDataSendBuf,&o_ptHandCmdAck->wSendLen);
	ret=TrackerProtocolSendData(i_ucProtocolUser,o_ptHandCmdAck->pucDataSendBuf,o_ptHandCmdAck->wSendLen);

	OSSemPost(ptOsSemUsedForSystemNeedReset);
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerDefaultInit
-Description	: TrackerDefaultInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerDefaultInit(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=TRUE;
	u8 ucDefaultInitBuf=1;
	u16 wDefaultInitBufLen=1;
	hookTrackerDeInit(i_ucProtocolUser);
	
	o_ptHandCmdAck->wAckCmd=TRACKER_PROTOCOL_DEFAULT_INIT_ACK;
	TrackerProtocolPackData(o_ptHandCmdAck->wAckCmd,&ucDefaultInitBuf,wDefaultInitBufLen,o_ptHandCmdAck->pucDataSendBuf,&o_ptHandCmdAck->wSendLen);
	ret=TrackerProtocolSendData(i_ucProtocolUser,o_ptHandCmdAck->pucDataSendBuf,o_ptHandCmdAck->wSendLen);

	return ret;
}
/*****************************************************************************
-Fuction		: TrackerParaSet	
-Description	: TrackerParaSet
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerParaSet(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	u16 wSubCmd;
	u8 SubCmd[2];
	u8 *SubCmdDataBuf=i_ucBuf;
	u8 *SubDataBuf;
	u8 ucHaveNetParaFlag=FALSE;
	u16 wSubDataLen=i_wLen;	
	u8 SetDataLenBuf[2]={0};
	u8 ucTagLen=i_ucBuf[(TRACKER_PROTOCOL_SUBCMD_OFFSET-1)];
	u16 wProcessedSubDataBufLen=0;
	
	u16 SetSuccessBufLen=0;
	//u8 SetSuccessBuf[(2*ProtocolSetHandleMenuNum)+TRACKER_PROTOCOL_CMD_SEQ_LEN+TRACKER_PROTOCOL_CMD_SUCCESS_LEN];
	u8 *SetSuccessBuf=o_ptHandCmdAck->pucDataBuf;
	u8 SetCmdSuccessCount=0;		
	s32 i;
	if(i_wLen<TRACKER_PROTOCOL_TLV_SET_BASE_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"TrackerParaSet Format err \r\n");
	}
	else
	{			
		OSSchedLock();
		memset(o_ptHandCmdAck->pucDataBuf,0,o_ptHandCmdAck->wMaxDataLen);
		memcpy(SetSuccessBuf,i_ucBuf,TRACKER_PROTOCOL_CMD_SEQ_LEN);//记录命令序列号
		SubCmdDataBuf=i_ucBuf+TRACKER_PROTOCOL_SUBCMD_OFFSET;//指向子命令
		wSubDataLen=i_wLen-TRACKER_PROTOCOL_SUBCMD_OFFSET;//不包含命令字的协议长度减去序列号以及子命令个数的长度	
		OSSchedUnlock();//下面不能加锁，因为使用了信号量
		/*当TAG个数解析完以及处理过的子命令数据大于等于下发的子命令数据
		的异常时则退出循环*/
		for(ucTagLen=0;((ucTagLen<i_ucBuf[(TRACKER_PROTOCOL_SUBCMD_OFFSET-1)])&&
		       (wProcessedSubDataBufLen<wSubDataLen));ucTagLen++)
		{
			SubCmdDataBuf+=wProcessedSubDataBufLen;//指向子命令
			memcpy(SubCmd,SubCmdDataBuf,2);
			SubDataBuf=SubCmdDataBuf+TRACKER_PROTOCOL_SUBCMD_LEN;
			wSubCmd=Lsb2U8ToU16(SubCmd);
			for(i=0;i<ProtocolSetHandleMenuNum;i++)
			{
				if(wSubCmd!=g_tTrackerProtocolSetHandleMenu[i].Cmd)
				{
				}
				else
				{
					if(i>=13)//说明当前有网络参数,如果处理函数表改动了，这个也要改
					{
						M_SetBit(ucHaveNetParaFlag);
					}
					else
					{
					}
					ret=g_tTrackerProtocolSetHandleMenu[i].ProtocolHandle(SubDataBuf,&wProcessedSubDataBufLen);	
					if(TRUE==ret)
					{
						memcpy(&SetSuccessBuf[TRACKER_PROTOCOL_CMD_SEQ_LEN+
							      TRACKER_PROTOCOL_CMD_SUCCESS_LEN+SetCmdSuccessCount],SubCmd,2);
						SetCmdSuccessCount++;
					}
					else
					{
						DebugPrintf(ERR"SetPara Fail CmdID:%x\r\n",wSubCmd);
					}
					break;
				}
			}				
			memcpy(SetDataLenBuf,SubDataBuf,TRACKER_PROTOCOL_SUBCMD_DATA_LEN);
			wProcessedSubDataBufLen=Lsb2U8ToU16(SetDataLenBuf);//当前命令下的数据长度
			wProcessedSubDataBufLen+=(TRACKER_PROTOCOL_SUBCMD_LEN+TRACKER_PROTOCOL_SUBCMD_DATA_LEN);//命令和数据长度的长度
		}
		SetSuccessBuf[TRACKER_PROTOCOL_CMD_SEQ_LEN+TRACKER_PROTOCOL_CMD_SUCCESS_LEN-1]=SetCmdSuccessCount;
		SetSuccessBufLen=TRACKER_PROTOCOL_CMD_SEQ_LEN+TRACKER_PROTOCOL_CMD_SUCCESS_LEN+(2*SetCmdSuccessCount);

		o_ptHandCmdAck->wAckCmd=TRACKER_PROTOCOL_SET_CMD_ACK;
		TrackerProtocolPackData(o_ptHandCmdAck->wAckCmd,SetSuccessBuf,SetSuccessBufLen,o_ptHandCmdAck->pucDataSendBuf,&o_ptHandCmdAck->wSendLen);
		ret=TrackerProtocolSendData(i_ucProtocolUser,o_ptHandCmdAck->pucDataSendBuf,o_ptHandCmdAck->wSendLen);

		if(IS_FALSE_BIT(ucHaveNetParaFlag))
		{
		}
		else
		{
			OSSemPost(ptOsSemUsedForSystemNeedReset);
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerParaRead
-Description	: TrackerParaRead
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerParaRead(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	u16 wSubDataLen=0;
	u16 wSubCmd;
	u8 SubCmd[2];
	u8 *pSubCmdDataBuf=i_ucBuf;		
	u8 *pGetParaBuf=o_ptHandCmdAck->pucDataBuf;
	u16 GetParaBufAllLen=0;
	u16 GetParaBufLen=0;
	u8 ucTagLen=i_ucBuf[(TRACKER_PROTOCOL_SUBCMD_OFFSET-1)];
	u8 ReadSuccessCount=0;
	u8 *pReadSuccessCount;//为省内存，使用指针记录BUF位置
	u8 *pReadSuccess;	
	u8 ReadFailCount=0;
	u8 *pReadFailCount;
	u8 *pReadFail;	
	s32 i;
	if(i_wLen<TRACKER_PROTOCOL_TLV_GET_BASE_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"TrackerParaGet Format err \r\n");
	}
	else
	{	
		pSubCmdDataBuf=i_ucBuf+TRACKER_PROTOCOL_SUBCMD_OFFSET;
		wSubDataLen=i_wLen-TRACKER_PROTOCOL_SUBCMD_OFFSET;
		
		OSSchedLock();
		memset(o_ptHandCmdAck->pucDataBuf,0,o_ptHandCmdAck->wMaxDataLen);
		memcpy(pGetParaBuf,i_ucBuf,TRACKER_PROTOCOL_CMD_SEQ_LEN);
		pGetParaBuf+=TRACKER_PROTOCOL_CMD_SEQ_LEN;
		*pGetParaBuf++=0x01;//总响应包数
		*pGetParaBuf++=0x00;//当前响应包数
		pReadFailCount=pGetParaBuf;//记录读取失败个数存储位置
		pGetParaBuf++;
		pReadFail=pGetParaBuf;//记录读取失败存取数组位置
		
		pGetParaBuf+=(2*ucTagLen);//临时把要读取的长度作为失败的长度
		pReadSuccessCount=pGetParaBuf;//记录读取成功个数存储位置
		pGetParaBuf++;//指向要存放读取数据的位置
		for(ucTagLen=0;ucTagLen<i_ucBuf[(TRACKER_PROTOCOL_SUBCMD_OFFSET-1)];ucTagLen++)
		{
			memcpy(SubCmd,pSubCmdDataBuf,2);
			wSubCmd=Lsb2U8ToU16(SubCmd);
			pSubCmdDataBuf+=2;
			for(i=0;i<ProtocolGetHandleMenuNum;i++)
			{
				if(wSubCmd!=g_tTrackerProtocolGetHandleMenu[i].Cmd)
				{
				}
				else
				{
					pReadSuccess=pGetParaBuf;//记录读取成功命令字的位置，长度2
					pGetParaBuf+=2;
					ret=g_tTrackerProtocolGetHandleMenu[i].ProtocolHandle(pGetParaBuf,&GetParaBufLen);	
					if(TRUE==ret)
					{
						memcpy(pReadSuccess,SubCmd,2);//存储成功的命令字
						ReadSuccessCount++;
						pGetParaBuf+=GetParaBufLen;//指向下一个要读取的数组
						GetParaBufAllLen+=(GetParaBufLen+2);//记录读取成功的数据长度
						GetParaBufLen=0;
					}
					else
					{
						memcpy(pReadFail,SubCmd,2);//存储失败的命令字
						pReadFail+=2;//长度2
						ReadFailCount++;
						pGetParaBuf-=2;//如果失败，记录成功的命令字还是原来位置
						DebugPrintf(ERR"ReadPara Fail CmdID:%x\r\n",wSubCmd);
					}
					break;
				}
			}
		}
		*pReadFailCount=ReadFailCount;//存储失败的个数
		*pReadSuccessCount=ReadSuccessCount;
		/*失败数组是临时取的最大的，确定后就用实际的覆盖*/
		memcpy(pReadFail,pReadSuccessCount,(GetParaBufAllLen+1));
		/*TLV数据长度+成功个数长度+失败数组+失败个数长度
		+响应包号与总包数+流水号*/
		GetParaBufAllLen=GetParaBufAllLen+1+(2*ReadFailCount)+1+2+TRACKER_PROTOCOL_CMD_SEQ_LEN;
		/*覆盖了前面后，对后面的偏移内容清零*/
		pGetParaBuf=o_ptHandCmdAck->pucDataBuf+GetParaBufAllLen;
		memset(pGetParaBuf,0,(TRACKER_PROTOCOL_MAX_LEN-TRACKER_PROTOCOL_BASE_LEN-
			GetParaBufAllLen));		
		OSSchedUnlock();
		
		o_ptHandCmdAck->wAckCmd=TRACKER_PROTOCOL_GET_CMD_ACK;
		TrackerProtocolPackData(o_ptHandCmdAck->wAckCmd,o_ptHandCmdAck->pucDataBuf,GetParaBufAllLen,o_ptHandCmdAck->pucDataSendBuf,&o_ptHandCmdAck->wSendLen);		
		ret=TrackerProtocolSendData(i_ucProtocolUser,o_ptHandCmdAck->pucDataSendBuf,o_ptHandCmdAck->wSendLen);

	}
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerUpdateRequest
-Description	: TrackerUpdateRequest
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerUpdateRequest(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	u8 ucErr;
	u8 *pucSoftwareReqPackInfo=i_ucBuf;
	u8 *aucRequestAckBuf=o_ptHandCmdAck->pucDataBuf;
	u16 RequestAckBufLen=0;
	
	memset(&g_tSoftwarePackInfo,0,sizeof(g_tSoftwarePackInfo));
	M_ClrBit(g_tSoftwarePackInfo.UpdateFirstPackFlag);
	
	memcpy(&g_tSoftwarePackInfo.ID,pucSoftwareReqPackInfo,4);
	pucSoftwareReqPackInfo+=4;

	memcpy(g_tSoftwarePackInfo.SN,pucSoftwareReqPackInfo,TRACKER_PROTOCOL_SOFTWARE_SN_LEN);		//软件编号//
	pucSoftwareReqPackInfo += TRACKER_PROTOCOL_SOFTWARE_SN_LEN;
	
	memcpy((u8 *)&g_tSoftwarePackInfo.PackageNum, pucSoftwareReqPackInfo,2);
	pucSoftwareReqPackInfo += 2;
	
	g_tSoftwarePackInfo.CurrentCrc=TRACKER_PROTOCOL_SOFTWARE_CRC_START;
	g_tSoftwarePackInfo.CrcNum=pucSoftwareReqPackInfo[0];
	g_tSoftwarePackInfo.CrcNum+=pucSoftwareReqPackInfo[1]*256;//CRC个数是U16进制256
	if(g_tSoftwarePackInfo.CrcNum>8)
	{
		g_tSoftwarePackInfo.CrcNum=8;//最大升级包32K*8=256k
	}	
	pucSoftwareReqPackInfo+=2;
	memcpy((u8 *)g_tSoftwarePackInfo.Crc,pucSoftwareReqPackInfo,(g_tSoftwarePackInfo.CrcNum*2));//每个CRC U16所以*2
	g_tSoftwarePackInfo.CrcIndex=0;
	SPI_FLASH_SectorErase(FLASH_ADDR_UPDATE_CODE);//后续放到支持文件里面
	DebugPrintf(CRIT"Update Software Start!\r\n");
	M_SetBit(g_tSoftwarePackInfo.UpdateStartFlag);
	
	OSSchedLock();
	memset(o_ptHandCmdAck->pucDataBuf,0,o_ptHandCmdAck->wMaxDataLen);
	memcpy(aucRequestAckBuf,i_ucBuf,20);//ID和软件编号
	aucRequestAckBuf+=20;
	RequestAckBufLen+=20;
	*aucRequestAckBuf=TRACKER_PROTOCOL_SOFTWARE_UPDATE_START;
	RequestAckBufLen++;
	OSSchedUnlock();
	OSFlagPost(pstOsFlagForAppUse, VCP_DEBUG_DISABLE_FLAG, OS_FLAG_SET, &ucErr);//升级时关闭打印防止串口升级失败(驱动问题不能忙)			

	FlashRomReadyForSystemUpdate();
	
	o_ptHandCmdAck->wAckCmd=TRACKER_PROTOCOL_UPDATE_REQUEST_ACK;
	TrackerProtocolPackData(o_ptHandCmdAck->wAckCmd,o_ptHandCmdAck->pucDataBuf,RequestAckBufLen,o_ptHandCmdAck->pucDataSendBuf,&o_ptHandCmdAck->wSendLen);
	ret=TrackerProtocolSendData(i_ucProtocolUser,o_ptHandCmdAck->pucDataSendBuf,o_ptHandCmdAck->wSendLen);

	return ret;

}
/*****************************************************************************
-Fuction		: TrackerUpdatePackage
-Description	: TrackerUpdatePackage
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerUpdatePackage(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	u8 ucErr;
	u8 *pucSoftwarePackInfo=i_ucBuf;
	u16 wCurrentPackageIndex=0;
	static u8 ErrCnt=0;
	u16 wPackageInfoLen=0;
	u8 *aucPackAckBuf=o_ptHandCmdAck->pucDataBuf;
	u16 UpdateAckBufLen=0;
	u8 ucIsLastPackageFlg=FALSE;
	
	if(IS_FALSE_BIT(g_tSoftwarePackInfo.UpdateStartFlag))
	{
		OSFlagPost(pstOsFlagForAppUse, VCP_DEBUG_DISABLE_FLAG, OS_FLAG_CLR, &ucErr);//退出升级则恢复串口打印
		DebugPrintf(ERR"Do Not Recv UpdateRequest err!\r\n");
	}
	else
	{
	    	wCurrentPackageIndex = pucSoftwarePackInfo[TRACKER_PROTOCOL_UPDATE_PACK_INDEX_OFFSET] + 
				                        (pucSoftwarePackInfo[TRACKER_PROTOCOL_UPDATE_PACK_INDEX_OFFSET+1] * 256);//u16包个数类型所以256
		/*判断当前的包号是否是所需要的*/
		DebugPrintf("ReceivePackNum:%d\r\n",wCurrentPackageIndex);
		if((wCurrentPackageIndex != 0) && ((g_tSoftwarePackInfo.CurrentRecvPackageNum+1) != wCurrentPackageIndex))
		{
			ErrCnt++;
	        	if(ErrCnt > 5)
	        	{
				M_ClrBit(g_tSoftwarePackInfo.UpdateStartFlag);
				OSFlagPost(pstOsFlagForAppUse, VCP_DEBUG_DISABLE_FLAG, OS_FLAG_CLR, &ucErr);//退出升级则恢复串口打印
				DebugPrintf(ERR"update err 5 times,update cacel!\n");
			}
			else
			{
				M_ClrBit(g_tSoftwarePackInfo.UpdateEndFlag);
			    	DebugPrintf(ERR"update err but not finish!\r\n");
			}
			ret=FALSE;
		}	
		else if((0==wCurrentPackageIndex)&&(IS_TRUE_BIT(g_tSoftwarePackInfo.UpdateFirstPackFlag)))
		{//如果已经有第一包了，则丢掉
			DebugPrintf(ERR"HavedFirstPack,ThrowAwayThePack!\r\n");//防止多次请求回应导致的一直发第一包问题
			ret=FALSE;//平台收到回应后才会去取数据，所以数据都是一样的，可以丢掉重复的
		}//所以同时平台收到回应表示升级开始就不能去更换升级文件
		else
		{
		    	ErrCnt = 0;
		    	if(0==wCurrentPackageIndex)//是第一个包
		    	{
				M_SetBit(g_tSoftwarePackInfo.UpdateFirstPackFlag);
		    	}
			else
		    	{
				g_tSoftwarePackInfo.CurrentRecvPackageNum++;//除第0包外用于判断
		    	}
			
			if(0x00 == pucSoftwarePackInfo[TRACKER_PROTOCOL_UPDATE_LAST_PACK_FLAG_OFFSET])	//非最后一个包//		//非最后一个包//
			{
				ucIsLastPackageFlg=FALSE;
				wPackageInfoLen=pucSoftwarePackInfo[TRACKER_PROTOCOL_UPDATE_PACK_LEN_OFFSET] + 
					                     (pucSoftwarePackInfo[TRACKER_PROTOCOL_UPDATE_PACK_LEN_OFFSET+1] * 256);
				if(TRUE==TrackerProtocolSoftwarePackCrcCheck(&pucSoftwarePackInfo[TRACKER_PROTOCOL_UPDATE_PACK_INFO_OFFSET],
					                                                                                wPackageInfoLen,ucIsLastPackageFlg))
		        	{

					M_ClrBit(g_tSoftwarePackInfo.UpdateEndFlag);
				    	FLASH_WriteDatas(FLASH_ADDR_UPDATE_CODE + g_tSoftwarePackInfo.PackageLen,
							                   &pucSoftwarePackInfo[TRACKER_PROTOCOL_UPDATE_PACK_INFO_OFFSET],wPackageInfoLen);
					g_tSoftwarePackInfo.PackageLen+= wPackageInfoLen;

					ret=TrackerProtocolUpdatePackageAck(i_ucProtocolUser,i_ucBuf,i_wLen,TRACKER_PROTOCOL_SOFTWARE_UPDATE_RECV_SUCCESS,
						                                                     TRACKER_PROTOCOL_UPDATE_PACK_ACK,o_ptHandCmdAck);
				}
		        	else
		        	{
					OSFlagPost(pstOsFlagForAppUse, VCP_DEBUG_DISABLE_FLAG, OS_FLAG_CLR, &ucErr);//退出升级则恢复串口打印
			            	DebugPrintf(ERR"update crc error,update cancel!\n");
					M_ClrBit(g_tSoftwarePackInfo.UpdateStartFlag);
					M_ClrBit(g_tSoftwarePackInfo.UpdateEndFlag);
					ret=TrackerProtocolUpdatePackageAck(i_ucProtocolUser,i_ucBuf,i_wLen,TRACKER_PROTOCOL_SOFTWARE_UPDATE_RECV_FAIL,
						                                                     TRACKER_PROTOCOL_UPDATE_PACK_ACK,o_ptHandCmdAck);
				}
			}
			else
			{
				ret=TrackerProtocolUpdatePackageAck(i_ucProtocolUser,i_ucBuf,i_wLen,TRACKER_PROTOCOL_SOFTWARE_UPDATE_RECV_SUCCESS,
																		 TRACKER_PROTOCOL_UPDATE_PACK_ACK,o_ptHandCmdAck);
				ucIsLastPackageFlg=TRUE;				
				wPackageInfoLen=pucSoftwarePackInfo[TRACKER_PROTOCOL_UPDATE_PACK_LEN_OFFSET] + 
										 (pucSoftwarePackInfo[TRACKER_PROTOCOL_UPDATE_PACK_LEN_OFFSET+1] * 256);
				if(FALSE==TrackerProtocolSoftwarePackCrcCheck(&pucSoftwarePackInfo[TRACKER_PROTOCOL_UPDATE_PACK_INFO_OFFSET],
													  		 wPackageInfoLen,ucIsLastPackageFlg))
		    		{
					ret=TrackerProtocolUpdatePackageAck(i_ucProtocolUser,i_ucBuf,i_wLen,TRACKER_PROTOCOL_SOFTWARE_UPDATE_CRC_ERROR,
																			 TRACKER_PROTOCOL_UPDATE_REQUEST_ACK,o_ptHandCmdAck);
					g_tSoftwarePackInfo.CrcNum=0;
					OSFlagPost(pstOsFlagForAppUse, VCP_DEBUG_DISABLE_FLAG, OS_FLAG_CLR, &ucErr);//退出升级则恢复串口打印
				    	DebugPrintf(ERR"crc error!\r\n");
		    		}
				else
		    		{
					FLASH_WriteDatas(FLASH_ADDR_UPDATE_CODE + g_tSoftwarePackInfo.PackageLen,
										   &pucSoftwarePackInfo[TRACKER_PROTOCOL_UPDATE_PACK_INFO_OFFSET],wPackageInfoLen);
					g_tSoftwarePackInfo.PackageLen+= wPackageInfoLen;

					ret=TrackerProtocolUpdatePackageAck(i_ucProtocolUser,i_ucBuf,i_wLen,TRACKER_PROTOCOL_SOFTWARE_UPDATE_SUCCESS,
																			 TRACKER_PROTOCOL_UPDATE_REQUEST_ACK,o_ptHandCmdAck);					
					M_SetBit(g_tSoftwarePackInfo.UpdateEndFlag);
					OSFlagPost(pstOsFlagForAppUse, VCP_DEBUG_DISABLE_FLAG, OS_FLAG_CLR, &ucErr);//退出升级则恢复串口打印
					DebugPrintf(ERR"updata package transport finish ,System will be restarted after 10s!\r\n");
				}
				M_ClrBit(g_tSoftwarePackInfo.UpdateStartFlag);
		        	DebugPrintf(ERR"update package over!\n");
				OSTimeDly(TRACKER_PROTOCOL_SOFTWARE_UPDATE_WAIT_RESET_TIME);
				if(IS_FALSE_BIT(g_tSoftwarePackInfo.UpdateEndFlag))
				{
				}
				else
				{
					FlashSetMarkForSystemUpdate();
					OSSemPost(ptOsSemUsedForSystemNeedReset);
				}
			}
		}			
	}
	return ret;
}
/*****************************************************************************
-Fuction		: TrackerReadSystemLog
-Description	: TrackerReadSystemLog
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerReadSystemLog(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	u8 *pReadSystemLogBuf=o_ptHandCmdAck->pucDataBuf;
	u16 wReadSystemLogLen=0;
	u8 ucErr=0;
	OSSchedLock();
	memset(o_ptHandCmdAck->pucDataBuf,0,o_ptHandCmdAck->wMaxDataLen);
	ret=SystemLogReadFromFlash(TRACKER_PROTOCOL_SYSTEM_LOG_PACK_NUM,pReadSystemLogBuf,&wReadSystemLogLen);			
	OSSchedUnlock();
	if((FALSE==ret)||(0==wReadSystemLogLen))
	{
		DebugPrintf(ERR"DoNotHaveSystemLogInFlash\r\n");
	}
	else
	{	
		o_ptHandCmdAck->wAckCmd=TRACKER_PROTOCOL_READ_SYSTEM_LOG_ACK;
		TrackerProtocolPackData(o_ptHandCmdAck->wAckCmd,o_ptHandCmdAck->pucDataBuf,wReadSystemLogLen,o_ptHandCmdAck->pucDataSendBuf,&o_ptHandCmdAck->wSendLen);
		ret=TrackerProtocolSendData(i_ucProtocolUser,o_ptHandCmdAck->pucDataSendBuf,o_ptHandCmdAck->wSendLen);
	}			
	return ret;
}

/*****************追踪器协议设置参数表函数*********************/
/*****************************************************************************
-Fuction		: SetEmergencyContactNum
-Description	: SetEmergencyContactNum
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetEmergencyContactNum(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 EmergencyContactNumBuf[(3*PHONE_STR_LEN)];
	u8 *SetDataBuf=i_ucBuf;
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen!=(3*(PHONE_STR_LEN+1)))
	{
		ret=FALSE;
		DebugPrintf(ERR"SetEmergencyContactNum err \r\n");
		DebugPrintf(ERR"wSetdataLen:%d",wSetDataBufLen);
	}
	else if((0x00!=SetDataBuf[0])||(0x01!=SetDataBuf[(PHONE_STR_LEN+1)])||(0x02!=SetDataBuf[((PHONE_STR_LEN+1)*2)]))
	{
		ret=FALSE;
		DebugPrintf(ERR"SetEmergencyContactNum err \r\n");
	}
	else
	{
		memset(EmergencyContactNumBuf,0,sizeof(EmergencyContactNumBuf));
		memcpy(EmergencyContactNumBuf,&SetDataBuf[1],PHONE_STR_LEN);
		memcpy(&EmergencyContactNumBuf[PHONE_STR_LEN],&SetDataBuf[(1+(1+PHONE_STR_LEN))],PHONE_STR_LEN);
		memcpy(&EmergencyContactNumBuf[(2*PHONE_STR_LEN)],&SetDataBuf[(1+(2*(1+PHONE_STR_LEN)))],PHONE_STR_LEN);
		ret=hookTrackerSetParameter("SmsPara",EmergencyContactNum1,EmergencyContactNumBuf,PHONE_STR_LEN);
		ret&=hookTrackerSetParameter("SmsPara",EmergencyContactNum2,&EmergencyContactNumBuf[PHONE_STR_LEN],PHONE_STR_LEN);
		ret&=hookTrackerSetParameter("SmsPara",EmergencyContactNum3,&EmergencyContactNumBuf[(2*PHONE_STR_LEN)],PHONE_STR_LEN);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SetDeviceID
-Description	: SetDeviceID
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetDeviceID(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 DeviceIDBuf[DEVICE_ID_LEN];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	DebugPrintf("DeviceId  len:%d\r\n",wSetDataBufLen);
	if(wSetDataBufLen>DEVICE_ID_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetDeviceID err \r\n");
	}
	else
	{
		memcpy(DeviceIDBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("SmsPara",DeviceID,DeviceIDBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SetSimCardNum
-Description	: SetSimCardNum
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetSimCardNum(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 SimCardNumBuf[(PHONE_STR_LEN)];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>PHONE_STR_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetSimCardNum err \r\n");
	}
	else
	{
		memcpy(SimCardNumBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("SmsPara",SimCardNum,SimCardNumBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SetReportInterval
-Description	: SetReportInterval
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetReportInterval(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 ReportIntervalBuf[4];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>4)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetReportInterval err \r\n");
	}
	else
	{
		memcpy(ReportIntervalBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("CommonPara",ReportInterval,ReportIntervalBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SetDailMode
-Description	: SetDailMode
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetDailMode(u8 *i_ucBuf,u16*o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 DailModeBuf;
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen!=1)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetDailMode err \r\n");
	}
	else
	{
		memcpy(&DailModeBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("NetPara",DailMode,&DailModeBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetDomain
-Description	: SetDomain
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetDomain(u8 *i_ucBuf,u16*o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 DomainBuf[DOMAIN_STR_LEN];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>DOMAIN_STR_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetDomain err \r\n");
	}
	else
	{
		memcpy(DomainBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("NetPara",Domain,DomainBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetIp
-Description	: SetIp
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetIp(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 IpBuf[IP_STR_LEN+1];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>(IP_STR_LEN+1))//由于协议过来正好是四个字节，所以这里加1
	{
		ret=FALSE;
		DebugPrintf(ERR"SetIp err \r\n");
	}
	else
	{
		memcpy(IpBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("NetPara",Ip,IpBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetPort
-Description	: SetPort
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetPort(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 PortBuf[PORT_STR_LEN];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>PORT_STR_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetPort err \r\n");
	}
	else
	{
		memcpy(PortBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("NetPara",Port,PortBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetApn
-Description	: SetApn
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetApn(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 ApnBuf[APN_STR_LEN];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>APN_STR_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetApn err \r\n");
	}
	else
	{
		memcpy(ApnBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("NetPara",Apn,ApnBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetUsrName
-Description	: SetUsrName
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetUsrName(u8 *i_ucBuf,u16*o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 UsrNameBuf[USERNAME_STR_LEN];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>USERNAME_STR_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetUsrName err \r\n");
	}
	else
	{
		memcpy(UsrNameBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("NetPara",UsrName,UsrNameBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetPassword
-Description	: SetPassword
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetPassword(u8 *i_ucBuf,u16*o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 PwdBuf[PASSWORD_STR_LEN];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>PASSWORD_STR_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetPassword err \r\n");
	}
	else
	{
		memcpy(PwdBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("NetPara",Pwd,PwdBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetCenterNum
-Description	: SetCenterNum
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetCenterNum(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 CenterNumBuf[PHONE_STR_LEN];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>PHONE_STR_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetCenterNum err \r\n");
	}
	else
	{
		memcpy(CenterNumBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("SmsPara",CenterNum,CenterNumBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetSmsSecretKey
-Description	: SetSmsSecretKey
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetSmsSecretKey(u8 *i_ucBuf,u16*o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 SmsSecretKeyBuf[SMS_SECRET_KEY_LEN];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>SMS_SECRET_KEY_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetSmsSecretKey err \r\n");
	}
	else
	{
		memcpy(SmsSecretKeyBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("SmsPara",SmsSecretKey,SmsSecretKeyBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetTrackLanguage
-Description	: SetTrackLanguage
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetTrackerLanguage(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 TrackerLanguageBuf;
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen!=1)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetTrackerLanguage err \r\n");
	}
	else
	{
		memcpy(&TrackerLanguageBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("CommonPara",TrackerLanguage,&TrackerLanguageBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetLowPowerAlarmSwitch
-Description	: SetLowPowerAlarmSwitch
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetLowPowerAlarmSwitch(u8 *i_ucBuf,u16*o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 LowPowerAlarmSwitchBuf;
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen!=1)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetLowPowerAlarmSwitch err \r\n");
	}
	else
	{
		memcpy(&LowPowerAlarmSwitchBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("AlarmPara",LowPowerAlarmSwitch,&LowPowerAlarmSwitchBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetSmsAlarmSwitch
-Description	: SetSmsAlarmSwitch
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetSmsAlarmSwitch(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 SmsAlarmSwitchBuf;
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen!=1)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetSmsAlarmSwitch err \r\n");
	}
	else
	{
		memcpy(&SmsAlarmSwitchBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("AlarmPara",SmsAlarmSwitch,&SmsAlarmSwitchBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetDropTrackerAlarmSwitch
-Description	: SetDropTrackerAlarmSwitch
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetDropTrackerAlarmSwitch(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 DropTrackerAlarmSwitchBuf;
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen!=1)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetDropTrackerAlarmSwitch err \r\n");
	}
	else
	{
		memcpy(&DropTrackerAlarmSwitchBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("AlarmPara",DropTrackerAlarmSwitch,&DropTrackerAlarmSwitchBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetCutTrackerAlarmSwitch
-Description	: SetCutTrackerAlarmSwitch
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetCutTrackerAlarmSwitch(u8 *i_ucBuf,u16*o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 CutTrackerAlarmSwitchBuf;
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen!=1)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetCutTrackerAlarmSwitch err \r\n");
	}
	else
	{
		memcpy(&CutTrackerAlarmSwitchBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("AlarmPara",CutTrackerAlarmSwitch,&CutTrackerAlarmSwitchBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetSoftWareVision
-Description	: SetSoftWareVision
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetSoftWareVision(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 SoftWareVisionBuf[SOFTWARE_VISION_LEN];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>SOFTWARE_VISION_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetSoftWareVision err \r\n");
	}
	else
	{
		memcpy(SoftWareVisionBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("AlarmPara",SoftWareVision,SoftWareVisionBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SetHardWareVision
-Description	: SetHardWareVision
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SetHardWareVision(u8 *i_ucBuf,u16 *o_wProcessedLen)
{
	u8 ret=FALSE;
	u16 wSetDataBufLen;
	u8 SetDataLen[2];
	u8 *SetDataBuf=i_ucBuf;
	u8 HardWareVisionBuf[HAEDWARE_VISION_LEN];
	memcpy(SetDataLen,i_ucBuf,2);
	SetDataBuf=i_ucBuf+2;
	wSetDataBufLen=Lsb2U8ToU16(SetDataLen);
	if(wSetDataBufLen>HAEDWARE_VISION_LEN)
	{
		ret=FALSE;
		DebugPrintf(ERR"SetHardWareVision err \r\n");
	}
	else
	{
		memcpy(HardWareVisionBuf,SetDataBuf,wSetDataBufLen);
		ret=hookTrackerSetParameter("CommonPara",HardWareVision,HardWareVisionBuf,wSetDataBufLen);
		*o_wProcessedLen=wSetDataBufLen;
	}
	return ret;
}
/*****************追踪器协议查询参数表函数*********************/
/*****************************************************************************
-Fuction		: SetEmergencyContactNum
-Description	: SetEmergencyContactNum
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetEmergencyContactNum(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 EmergencyContactNumBuf[(3*PHONE_STR_LEN)];
	u16 wEmergencyContactNumLen=3*PHONE_STR_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetEmergencyContactNum o_ucBuf err \r\n");
	}
	else
	{
		memset(EmergencyContactNumBuf,0,sizeof(EmergencyContactNumBuf));
		ret=hookTrackerGetParameter("SmsPara",EmergencyContactNum1,EmergencyContactNumBuf,&wEmergencyContactNumLen);
		ret&=hookTrackerGetParameter("SmsPara",EmergencyContactNum2,&EmergencyContactNumBuf[PHONE_STR_LEN],&wEmergencyContactNumLen);
		ret&=hookTrackerGetParameter("SmsPara",EmergencyContactNum3,&EmergencyContactNumBuf[2*PHONE_STR_LEN],&wEmergencyContactNumLen);
		*o_pTempBuf++=(3*(PHONE_STR_LEN+1));
		*o_pTempBuf++=00;
		*o_pTempBuf++=0x00;
		memcpy(o_pTempBuf,&EmergencyContactNumBuf[0],PHONE_STR_LEN);
		o_pTempBuf+=PHONE_STR_LEN;
		*o_pTempBuf++=0x01;
		memcpy(o_pTempBuf,&EmergencyContactNumBuf[PHONE_STR_LEN],PHONE_STR_LEN);
		o_pTempBuf+=PHONE_STR_LEN;
		*o_pTempBuf++=0x02;
		memcpy(o_pTempBuf,&EmergencyContactNumBuf[2*PHONE_STR_LEN],PHONE_STR_LEN);
		o_pTempBuf+=PHONE_STR_LEN;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetDeviceID
-Description	: GetDeviceID
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetDeviceID(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 DeviceIDBuf[DEVICE_ID_LEN];
	u16 wDeviceIDLen=DEVICE_ID_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetDeviceID o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("SmsPara",DeviceID,DeviceIDBuf,&wDeviceIDLen);
		*o_pTempBuf++=(wDeviceIDLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&DeviceIDBuf[0],wDeviceIDLen);
		o_pTempBuf+=wDeviceIDLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetIMEL
-Description	: GetIMEL
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetIMEL(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;	
	u8 *o_pTempBuf=o_ucBuf;
	u8 ImelBuf[TRACKER_PROTOCOL_IMEL_LEN];
	u16 wImelLen=TRACKER_PROTOCOL_IMEL_LEN;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetIMEL o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetIMEL(ImelBuf,&wImelLen);
		*o_pTempBuf++=(wImelLen);
		*o_pTempBuf++=00;//长度两个字节
		memcpy(o_pTempBuf,&ImelBuf[0],wImelLen);
		o_pTempBuf+=wImelLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetSimCardNum
-Description	: GetSimCardNum
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetSimCardNum(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 SimCardNumBuf[PHONE_STR_LEN]={0};
	u16 wSimCardNumLen=PHONE_STR_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetSimCardNum o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("SmsPara",SimCardNum,SimCardNumBuf,&wSimCardNumLen);
		*o_pTempBuf++=PHONE_STR_LEN;
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&SimCardNumBuf[0],PHONE_STR_LEN);//固定长度是21
		o_pTempBuf+=PHONE_STR_LEN;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetReportInterval
-Description	: GetReportInterval
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetReportInterval(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 ReportIntervalBuf[4];
	u16 wReportIntervalLen=4;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetReportInterval o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("CommonPara",ReportInterval,ReportIntervalBuf,&wReportIntervalLen);
		*o_pTempBuf++=(wReportIntervalLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&ReportIntervalBuf[0],wReportIntervalLen);
		o_pTempBuf+=wReportIntervalLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetDailMode
-Description	: GetDailMode
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetDailMode(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 DailModeBuf;
	u16 wDailModeLen=1;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetDailMode o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("NetPara",DailMode,&DailModeBuf,&wDailModeLen);
		*o_pTempBuf++=(wDailModeLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&DailModeBuf,wDailModeLen);
		o_pTempBuf+=wDailModeLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetDomain
-Description	: GetDomain
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetDomain(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 DomainBuf[DOMAIN_STR_LEN];
	u16 wDomainLen=DOMAIN_STR_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetDomain o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("NetPara",Domain,DomainBuf,&wDomainLen);
		*o_pTempBuf++=(wDomainLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&DomainBuf[0],wDomainLen);
		o_pTempBuf+=wDomainLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetIp
-Description	: GetIp
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetIp(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 IpBuf[IP_STR_LEN];
	u16 wIpLen=IP_STR_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetIp o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("NetPara",Ip,IpBuf,&wIpLen);
		*o_pTempBuf++=(wIpLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&IpBuf[0],wIpLen);
		o_pTempBuf+=wIpLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetPort
-Description	: GetPort
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetPort(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 PortBuf[PORT_STR_LEN];
	u16 wPortLen=PORT_STR_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetPort o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("NetPara",Port,PortBuf,&wPortLen);
		*o_pTempBuf++=(wPortLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&PortBuf[0],wPortLen);
		o_pTempBuf+=wPortLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetApn
-Description	: GetApn
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetApn(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 ApnBuf[APN_STR_LEN];
	u16 wApnLen=APN_STR_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetApn o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("NetPara",Apn,ApnBuf,&wApnLen);
		*o_pTempBuf++=(wApnLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&ApnBuf[0],wApnLen);
		o_pTempBuf+=wApnLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetUsrName
-Description	: GetUsrName
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetUsrName(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 UsrNameBuf[USERNAME_STR_LEN];
	u16 wUsrNameLen=USERNAME_STR_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetUsrName o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("NetPara",UsrName,UsrNameBuf,&wUsrNameLen);
		*o_pTempBuf++=(wUsrNameLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&UsrNameBuf[0],wUsrNameLen);
		o_pTempBuf+=wUsrNameLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetPassword
-Description	: GetPassword
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetPassword(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 PwdBuf[PASSWORD_STR_LEN];
	u16 wPwdLen=PASSWORD_STR_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetPassword o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("NetPara",Pwd,PwdBuf,&wPwdLen);
		*o_pTempBuf++=(wPwdLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&PwdBuf[0],wPwdLen);
		o_pTempBuf+=wPwdLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetCenterNum
-Description	: GetCenterNum
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetCenterNum(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 CenterNumBuf[PHONE_STR_LEN];
	u16 wCenterNumLen=PHONE_STR_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetCenterNum o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("SmsPara",CenterNum,CenterNumBuf,&wCenterNumLen);
		*o_pTempBuf++=(wCenterNumLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&CenterNumBuf[0],wCenterNumLen);
		o_pTempBuf+=wCenterNumLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetSmsSecretKey
-Description	: GetSmsSecretKey
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetSmsSecretKey(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 SmsSecretKeyBuf[SMS_SECRET_KEY_LEN];
	u16 wSmsSecretKeyLen=SMS_SECRET_KEY_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetSmsSecretKey o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("SmsPara",SmsSecretKey,SmsSecretKeyBuf,&wSmsSecretKeyLen);
		*o_pTempBuf++=(wSmsSecretKeyLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&SmsSecretKeyBuf[0],wSmsSecretKeyLen);
		o_pTempBuf+=wSmsSecretKeyLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetTrackerLanguage
-Description	: GetTrackerLanguage
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetTrackerLanguage(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 TrackerLanguageBuf;
	u16 wTrackerLanguageLen=1;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetTrackerLanguage o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("CommonPara",TrackerLanguage,&TrackerLanguageBuf,&wTrackerLanguageLen);
		*o_pTempBuf++=(wTrackerLanguageLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&TrackerLanguageBuf,wTrackerLanguageLen);
		o_pTempBuf+=wTrackerLanguageLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetLowPowerAlarmSwitch
-Description	: GetLowPowerAlarmSwitch
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetLowPowerAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 LowPowerAlarmSwitchBuf;
	u16 wLowPowerAlarmSwitchLen=1;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetLowPowerAlarmSwitch o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("AlarmPara",LowPowerAlarmSwitch,&LowPowerAlarmSwitchBuf,&wLowPowerAlarmSwitchLen);
		*o_pTempBuf++=(wLowPowerAlarmSwitchLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&LowPowerAlarmSwitchBuf,wLowPowerAlarmSwitchLen);
		o_pTempBuf+=wLowPowerAlarmSwitchLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetSmsAlarmSwitch
-Description	: GetSmsAlarmSwitch
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetSmsAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 SmsAlarmSwitchBuf;
	u16 wSmsAlarmSwitchLen=1;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetSmsAlarmSwitch o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("AlarmPara",SmsAlarmSwitch,&SmsAlarmSwitchBuf,&wSmsAlarmSwitchLen);
		*o_pTempBuf++=(wSmsAlarmSwitchLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&SmsAlarmSwitchBuf,wSmsAlarmSwitchLen);
		o_pTempBuf+=wSmsAlarmSwitchLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetDropTrackerAlarmSwitch
-Description	: GetDropTrackerAlarmSwitch
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetDropTrackerAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 DropTrackerAlarmSwitchBuf;
	u16 wDropTrackerAlarmSwitchLen=1;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetDropTrackerAlarmSwitch o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("AlarmPara",DropTrackerAlarmSwitch,&DropTrackerAlarmSwitchBuf,&wDropTrackerAlarmSwitchLen);
		*o_pTempBuf++=(wDropTrackerAlarmSwitchLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&DropTrackerAlarmSwitchBuf,wDropTrackerAlarmSwitchLen);
		o_pTempBuf+=wDropTrackerAlarmSwitchLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetCutTrackerAlarmSwitch
-Description	: GetCutTrackerAlarmSwitch
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetCutTrackerAlarmSwitch(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 CutTrackerAlarmSwitchBuf;
	u16 wCutTrackerAlarmSwitchLen=1;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetCutTrackerAlarmSwitch o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("AlarmPara",CutTrackerAlarmSwitch,&CutTrackerAlarmSwitchBuf,&wCutTrackerAlarmSwitchLen);
		*o_pTempBuf++=(wCutTrackerAlarmSwitchLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&CutTrackerAlarmSwitchBuf,wCutTrackerAlarmSwitchLen);
		o_pTempBuf+=wCutTrackerAlarmSwitchLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetSoftWareVision
-Description	: GetSoftWareVision
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetSoftWareVision(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 SoftWareVisionBuf[SOFTWARE_VISION_LEN];
	u16 wSoftWareVisionLen=SOFTWARE_VISION_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetSoftWareVision o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("CommonPara",SoftWareVision,SoftWareVisionBuf,&wSoftWareVisionLen);
		*o_pTempBuf++=(wSoftWareVisionLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&SoftWareVisionBuf[0],wSoftWareVisionLen);
		o_pTempBuf+=wSoftWareVisionLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetHardWareVision
-Description	: GetHardWareVision
-Input			: 
-Output 		: o_ucBuf o_wLen
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/12    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 GetHardWareVision(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 HardWareVisionBuf[HAEDWARE_VISION_LEN];
	u16 wHardWareVisionLen=HAEDWARE_VISION_LEN;
	u8 *o_pTempBuf=o_ucBuf;
	if(NULL==o_pTempBuf)
	{
		ret=FALSE;
		DebugPrintf(ERR"GetHardWareVision o_ucBuf err \r\n");
	}
	else
	{
		ret=hookTrackerGetParameter("CommonPara",HardWareVision,HardWareVisionBuf,&wHardWareVisionLen);
		*o_pTempBuf++=(wHardWareVisionLen);
		*o_pTempBuf++=00;
		memcpy(o_pTempBuf,&HardWareVisionBuf[0],wHardWareVisionLen);
		o_pTempBuf+=wHardWareVisionLen;
		*o_wLen=(o_pTempBuf-o_ucBuf);
	}
	return ret;
}

/****************************回调底层函数*****************************/
/*****************************************************************************
-Fuction		: hookTrackerRtcTimeUpdate	
-Description	: hookTrackerRtcTimeUpdate 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookTrackerRtcTimeUpdate(u32 i_dwNewRtcTime)
{
	u8 ret=FALSE;
	datetime_t tDataTime;
	tDataTime=app_sys_mktime2dtime(i_dwNewRtcTime);
	//tDataTime.time.hour+=8;
	ret=g_tTime.SetDateTime(tDataTime);
	return ret;
	//.RtcTimeUpdateOpr(i_dwNewRtcTime);
}
/*****************************************************************************
-Fuction		: hookTrackerDeInit
-Description	: hookTrackerDeInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static void hookTrackerDeInit(u8 i_ucProtocolUser)
{
	PT_TrackerProtocolSupport ptTmp = g_ptTrackerProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"TrackerProtocolSupportHead SetParameter Null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->ParaDeInitSupport(i_ucProtocolUser);
			ptTmp = ptTmp->ptNext;
		}
	}	
}
/*****************************************************************************
-Fuction		: hookTrackerGetData
-Description	: hookTrackerGetData 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookTrackerGetData(u8 i_ucProtocolUser,u8 *o_ucBuf, u16 *o_wLen)
{
	u8 ret=FALSE;
	PT_TrackerProtocolSupport ptTmp = g_ptTrackerProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"TrackerProtocolSupportHead GetData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GetDataSupport(i_ucProtocolUser,o_ucBuf,o_wLen);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookTrackerSendData
-Description	: hookTrackerSendData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookTrackerSendData(u8 i_ucProtocolUser,u8 *i_ucBuf, u16 i_wLen)
{
	u8 ret=FALSE;
	PT_TrackerProtocolSupport ptTmp = g_ptTrackerProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"TrackerProtocolSupportHead SendData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->SendDataSupport(i_ucProtocolUser,i_ucBuf,i_wLen);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}

/*****************************************************************************
-Fuction		: hookTrackerSetParameter
-Description	: hookTrackerSetParameter
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookTrackerSetParameter(char *i_Name,ParameterEnum i_SubName,u8 *i_ucBuf,u16 i_wLen)
{
	u8 ret=FALSE;
	PT_TrackerProtocolSupport ptTmp = g_ptTrackerProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"TrackerProtocolSupportHead SetParameter Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->SetParaSupport(i_Name,i_SubName,i_ucBuf,i_wLen);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookTrackerGetParameter
-Description	: hookTrackerGetParameter
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookTrackerGetParameter(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len)
{
	u8 ret=FALSE;
	PT_TrackerProtocolSupport ptTmp = g_ptTrackerProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"TrackerProtocolSupportHead GetParameter Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GetParaSupport(i_Name,i_SubName,o_Buf,o_Len);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookTrackerRelieveAlarm
-Description	: hookTrackerRelieveAlarm
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookTrackerRelieveAlarm(T_Alarm i_tAlarm)
{
	u8 ret=FALSE;
	PT_TrackerProtocolSupport ptTmp = g_ptTrackerProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"TrackerProtocolSupportHead ClrAlarm Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->RelieveAlarmSupport(i_tAlarm);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookTrackerGetData
-Description	: hookTrackerGetData 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookTrackerGetIMEL(u8 *o_ucBuf, u16 *o_wLen)
{
	u8 ret=FALSE;
	PT_TrackerProtocolSupport ptTmp = g_ptTrackerProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"TrackerProtocolSupportHead GetIMEL Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GetImelSupport(o_ucBuf,o_wLen);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}

/************************注册底层支持函数***************************/

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
void RegisterTrackerProtocolSupport(PT_TrackerProtocolSupport i_ptTrackerProtocolSupport)
{
	PT_TrackerProtocolSupport ptTmp;
	if (!g_ptTrackerProtocolSupportHead)
	{
		g_ptTrackerProtocolSupportHead   = i_ptTrackerProtocolSupport;
		g_ptTrackerProtocolSupportHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptTrackerProtocolSupportHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptTrackerProtocolSupport;
		i_ptTrackerProtocolSupport->ptNext = NULL;
	}
}

