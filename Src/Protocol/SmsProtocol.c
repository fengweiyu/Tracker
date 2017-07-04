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

static PT_SmsProtocolSupport g_ptSmsProtocolSupportHead=NULL;
static u8 hookSmsRtcTimeUpdate(u32 i_dwNewRtcTime);
static void hookSmsDeInit();
static u8 hookSmsGetData(T_SmsInfo *o_ptSmsInfo);
static u8 hookSmsSendData(u8 *i_PhoneNumBuf,u8 *i_ucBuf, u16 i_wLen);
static u8 hookSmsSetParameter(char *i_Name,ParameterEnum i_SubName,u8 *i_ucBuf,u16 i_wLen);
static u8 hookSmsGetParameter(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);
static u8 hookSmsGetIMEL(u8 *o_ucBuf,u16 *o_wLen);
static u8 hookSmsSendAlarmData(u8 *i_ucBuf, u16 i_wLen);
static u8 hookSmsRestartSystem();
static u8 hookSmsGetGpsLocationInfo(LocationVectorInfo *o_ptGpsLocationInfo);
static u8 hookSmsLock();
static u8 hookSmsUnLock();
static u8 hookSmsTimeDelay(u16 i_wSecond,u16 i_wMilliSecond);
static u8 hookSmsGetGsmCsq(u8 *o_pucCsq);
static s16 hookSmsPduEncode( u8  *pPdu, u8  *pSCA, u8  *pDA, u8  *pUD, s16 iUDL, s16 iDCS );

static u8 SmsGetLBG(PT_SmsParaFormat i_pSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsGetLBS(PT_SmsParaFormat i_pSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsGetNetPara(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsGetVersion(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsGetTrackerIDInformation(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsDefaultInitPara(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsRestartSystem(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsSetTrackerLanguageEn(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsSetTrackerLanguageCn(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsSetEmergencyContactNum(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsDelAllEmergencyContactNum(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsSetReportInterval(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsSetSimCardNum(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsSetNetPara(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);
static u8 TrackerProtocolSmsSetNetPara(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen);

u8 *LowerToUpper(u8 *i_pucStr, u16 i_wLen);
u16 SmsProtocolParaGet( u8 *i_pucStart, u8 i_ucEndSign, u16 i_wMaxLen);
static void SmsProtocolToParaFormat(u8 *i_pucBuf,u16 i_wSmsLen,T_SmsParaFormat *o_ptParaBuf);
static u8 SmsProtocolGetGpsLocateInfo(LocationVectorInfo *o_ptGpsLocationInfo);
static u8 SmsProtocolReportInfoPack(LocationVectorInfo *i_ptGpsLocationInfo,const char **i_pstrCmd,u16 i_wReportMaxLen,u8 *o_pReportBuf,u16 *o_pwReportBufLen);
static u8 SmsProtocolGetData(u8 i_ucProtocolUser,u8 *o_pucDataBuf,u16 *o_pwDataLen,u16 i_wDataBufMaxLen);
static u8 SmsProtocolCheckData(u8 *i_pucDataBuf,u16 i_wDataLen);
static u8 SmsProtocolAnalyseData(u8 i_ucProtocolUser,u8 *i_pucDataBuf,u16 i_wDataLen,T_HandleCmdAck *o_ptHandCmdAck);
static u8 SmsProtocolPackData(u8 *i_pucPhoneNumBuf,u8 *i_pucBuf,u16 i_wLen,u8 *o_pucBuf,u16 *o_wLen);
static u8 SmsProtocolSendData(u8 *i_PhoneNumBuf,u8 *i_ucBuf,u16 i_wLen);
static u8 SmsProtocolReportAlarm(T_Alarm i_tAlarm,u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen);
static u8 SmsProtocolReportLocation(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen);

static T_SmsInfo g_tSmsInfo={0};
//static u16 g_wSmsHandleDataBufLen=SMS_PROTOCOL_MAX_LEN;
//static u8   g_SmsHandleDataBuf[SMS_PROTOCOL_MAX_LEN]={0};
//static u16 g_wSmsReportAlarmBufLen=SMS_PROTOCOL_MAX_SEND_LEN;
//static u8   g_SmsReportAlarmBuf[SMS_PROTOCOL_MAX_SEND_LEN]={0};
//static u16 g_wSmsReportBufLen=SMS_PROTOCOL_MAX_SEND_LEN;
//static u8   g_SmsReportBuf[SMS_PROTOCOL_MAX_SEND_LEN]={0};
//static u16 g_wSmsSendDataBufLen=SMS_PROTOCOL_MAX_LEN;//141的发送长度不够
//static u8   g_SmsSendDataBuf[SMS_PROTOCOL_MAX_LEN]={0};
//static u16 g_wSmsPhoneNumLen=PHONE_STR_LEN;
//static u8   g_SmsPhoneNumBuf[PHONE_STR_LEN+1]={0};
static u8   g_ucSmsCheckGetDataFlag=FALSE;
static u8   g_ucSmsSendDataFlag=FALSE;
static u8   g_ucSmsSendDataBusyFlag=FALSE;
 static u8	g_ucSetNetParaNeedResetFlag=FALSE;

 static T_ProtocolManage g_tSmsProtocol={
	.name="SmsProtocol",
	.ProtocolGetData		=SmsProtocolGetData,
	.ProtocolCheckData		=SmsProtocolCheckData,
	.ProtocolAnalyseData	=SmsProtocolAnalyseData,
	.ProtocolReportAlarm  	=SmsProtocolReportAlarm,
	.ProtocolReport		=SmsProtocolReportLocation,
};
static const char g_strSmsLowVoltageAlarmEn[]			="Low voltage";
static const char g_strSmsLowVoltageAlarmCn[]			="低电压";
static const char *g_pstrSmsLowVoltageAlarm[] ={ 
	g_strSmsLowVoltageAlarmEn,
	g_strSmsLowVoltageAlarmCn,
};
static const char g_strSmsCutOffAlarmEn[]				="Cut off";
static const char g_strSmsCutOffAlarmCn[]				="剪断报警";
static const char *g_pstrSmsCutOffAlarm[] ={ 
	g_strSmsCutOffAlarmEn, 
	g_strSmsCutOffAlarmCn,
};
static const char g_strSmsFallOffAlarmEn[]				="Fall off";
static const char g_strSmsFallOffAlarmCn[]				="脱落报警";
static const char *g_pstrSmsFallOffAlarm[] ={ 
	g_strSmsFallOffAlarmEn,
	g_strSmsFallOffAlarmCn,
};
static const char g_strSmsLocationLinkEn[] 				= "http://maps.google.com/?q=";
static const char g_strSmsLocationLinkCn[] 				= "http://ditu.google.cn/?q=";
static const char *g_pstrSmsLocationLink[] ={
	g_strSmsLocationLinkEn,
	g_strSmsLocationLinkCn,
};

 /*follow this string have to use lower case*/
static const char g_strSmsReadLocationBasedGps[] 		= "LBG";  
static const char g_strSmsReadLocationBasedServices[] 	= "LBS"; 
static const char g_strSmsReadNetPara[] 				= "RG";
static const char g_strSmsReadVersion[] 				= "RV"; 
static const char g_strSmsReadTrackerIDInformation[]	= "RI";
static const char g_strSmsDefaultInitPara[] 				= "INIT";
static const char g_strSmsRestart[] 					= "RESTART";
static const char g_strSmsSetTrackerLanguageEn[] 		= "EN"; 
static const char g_strSmsSetTrackerLanguageCn[] 		= "CN"; 
static const char g_strSmsSetEmergencyContactNum[] 	= "SP"; 
static const char g_strSmsSetReportInterval[] 			= "SI"; 
static const char g_strSmsSetSimCardNum[] 			= "SS";
static const char g_strSmsSetNetPara[] 					= "SG"; 
static const char g_strSmsSetNetworkPara[]				= "SET;GPRS-PARA";

static T_SmsProtocolHandleMenu g_tSmsProtocolHandleMenu[] = 
{
	{g_strSmsReadLocationBasedGps, 		1, SmsGetLBG}, 
	{g_strSmsReadLocationBasedServices, 	1, SmsGetLBS}, 
	{g_strSmsReadNetPara,				1, SmsGetNetPara}, 
	{g_strSmsReadVersion,					1, SmsGetVersion}, 
	{g_strSmsReadTrackerIDInformation,	1, SmsGetTrackerIDInformation},
	{g_strSmsDefaultInitPara,				1, SmsDefaultInitPara},
	{g_strSmsRestart,						1, SmsRestartSystem},
	{g_strSmsSetTrackerLanguageEn, 		1, SmsSetTrackerLanguageEn}, 
	{g_strSmsSetTrackerLanguageCn, 		1, SmsSetTrackerLanguageCn}, 
	{g_strSmsSetEmergencyContactNum, 	4, SmsSetEmergencyContactNum}, 
	{g_strSmsSetEmergencyContactNum, 	1, SmsDelAllEmergencyContactNum}, 
	{g_strSmsSetReportInterval, 			2, SmsSetReportInterval}, 
	{g_strSmsSetSimCardNum, 				2, SmsSetSimCardNum},
	{g_strSmsSetNetPara, 					7, SmsSetNetPara}, 
	{g_strSmsSetNetworkPara,				7, TrackerProtocolSmsSetNetPara}, 
}; 
#define SmsProtocolHandleMenuNum  (sizeof(g_tSmsProtocolHandleMenu) / sizeof(T_SmsProtocolHandleMenu))
 /*****************************************************************************
-Fuction		: SmsProtocolInit
-Description	: SmsProtocolInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
void SmsProtocolInit()
{
	SmsProtocolSupportInit();
	RegisterProtocol(&g_tSmsProtocol);
}
/*****************************************************************************
-Fuction		: SmsProtocolGetData
-Description	: SmsProtocolGetData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolGetData(u8 i_ucProtocolUser,u8 *o_pucDataBuf,u16 *o_pwDataLen,u16 i_wDataBufMaxLen)
{
	u8 ret=FALSE;
	hookSmsLock();
	memset(&g_tSmsInfo,0,sizeof(T_SmsInfo));
	ret=hookSmsGetData(&g_tSmsInfo);
	hookSmsUnLock();
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolCheckData
-Description	: SmsProtocolCheckData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolCheckData(u8 *i_pucDataBuf,u16 i_wDataLen)
{
	u8 ret=FALSE;
	u8 aucGetSmsSecretKeyBuf[SMS_SECRET_KEY_LEN]={0};
	u16 wGetSmsSecretKeyLen=0;
	u8 *aucSmsGetDataBuf=g_tSmsInfo.aucSmsContent;
	u16 wSmsGetDataBufLen=g_tSmsInfo.wSmsContentLen;
	ret=hookSmsGetParameter("SmsPara",SmsSecretKey,aucGetSmsSecretKeyBuf,&wGetSmsSecretKeyLen);
	hookSmsLock();
	if(FALSE==ret)
	{
		DebugPrintf(ERR"Get Sms secret key err! \r\n");
		M_ClrBit(g_ucSmsCheckGetDataFlag);
	}
	else
	{
		if(((wSmsGetDataBufLen<wGetSmsSecretKeyLen)||(0!=memcmp(aucGetSmsSecretKeyBuf,aucSmsGetDataBuf,wGetSmsSecretKeyLen)))&&
		    (0!=memcmp("*name",aucSmsGetDataBuf,strlen("*name"))))
		{
			ret=FALSE;
			DebugPrintf(ERR"Check Sms secret key err\r\n");
			M_ClrBit(g_ucSmsCheckGetDataFlag);
		}
		else
		{
			ret=TRUE;
			M_SetBit(g_ucSmsCheckGetDataFlag);
		}
	}	
	hookSmsUnLock();
	return ret;	
}
/*****************************************************************************
-Fuction		: SmsProtocolAnalyseData
-Description	: SmsProtocolAnalyseData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolAnalyseData(u8 i_ucProtocolUser,u8 *i_pucDataBuf,u16 i_wDataLen,T_HandleCmdAck *o_ptHandCmdAck)
{
	u8 ret=FALSE;
	u8 ucSmsCheckGetDataFlag=FALSE;
	static T_SmsParaFormat tSmsPara={0};//tSmsPara指向这个g_tSmsInfo.aucSmsContent
	u8 *pucSmsGetDataBuf=g_tSmsInfo.aucSmsContent;
	u16 wSmsGetDataBufLen=g_tSmsInfo.wSmsContentLen;
	u8 *pucHandleDataBuf=o_ptHandCmdAck->pucDataBuf;
	u16 wHandletDataBufLen=o_ptHandCmdAck->wMaxDataLen;
	u16 wCmdLen=SMS_PROTOCOL_MAX_CMD_LEN;
	static u8 aucCmdBuf[SMS_PROTOCOL_MAX_CMD_LEN+1]={0}; //max 32 len cmd
	s32 i;
	hookSmsLock();
	ucSmsCheckGetDataFlag=g_ucSmsCheckGetDataFlag;
	hookSmsUnLock();
	if(IS_FALSE_BIT(ucSmsCheckGetDataFlag))
	{
		ret=FALSE;
		DebugPrintf(ERR"Sms Is not Check Data ok \r\n");
	}
	else
	{
		memset(aucCmdBuf,0,sizeof(aucCmdBuf));
		memset(&tSmsPara,0,sizeof(T_SmsParaFormat));	
		memset(o_ptHandCmdAck->pucDataBuf,0,o_ptHandCmdAck->wMaxDataLen);	
		hookSmsLock();
		SmsProtocolToParaFormat(pucSmsGetDataBuf,wSmsGetDataBufLen,&tSmsPara);		
		hookSmsUnLock();
		for(i=0;i<SmsProtocolHandleMenuNum;i++)
		{
			wCmdLen=M_MIN(wCmdLen, tSmsPara.tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
			memcpy(aucCmdBuf,tSmsPara.tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,wCmdLen);
			LowerToUpper(aucCmdBuf, wCmdLen);
			if((0!=strncmp((char const *)aucCmdBuf, g_tSmsProtocolHandleMenu[i].pStr,SMS_PROTOCOL_MAX_CMD_LEN))
				|| ((g_tSmsProtocolHandleMenu[i].ucDelimiterNum + 1) != tSmsPara.ucParaNum) )
			{
				NULL;
			}
			else
			{
				if(NULL==g_tSmsProtocolHandleMenu[i].ProtocolHandle)
				{
					DebugPrintf(ERR"%s Fuction is empty!\r\n",g_tSmsProtocolHandleMenu[i].pStr);
					ret=FALSE;
				}
				else
				{					
					//hookSmsLock();//里面循环时间太长，不用，暂时只有一个短信任务调用暂无关系
					ret=g_tSmsProtocolHandleMenu[i].ProtocolHandle(&tSmsPara,pucHandleDataBuf,&wHandletDataBufLen);
					//hookSmsUnLock();
				}
			}
		}
		if(FALSE==ret)
		{
			DebugPrintf(ERR"SmsProtocolHandle err\r\n");
			//保存到FLASH中，当其他普通短信存储  暂不需要
			//SPI_FLASH_SectorErase(FLASH_ADDR_SMS_INFO_NEW);
			//FLASH_WriteDatas(FLASH_ADDR_SMS_INFO_NEW,pucSmsGetDataBuf,wSmsGetDataBufLen);
		}
		else
		{			
			//SchedLock();//和上诉同样的道理,函数里面进行保护
			ret=SmsProtocolPackData(g_tSmsInfo.strPhoneNum,pucHandleDataBuf,wHandletDataBufLen,o_ptHandCmdAck->pucDataSendBuf,&o_ptHandCmdAck->wSendLen);			
			//SchedUnlock();
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: SmsProtocolPackData
-Description	: SmsProtocolPackData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolPackData(u8 *i_pucPhoneNumBuf,u8 *i_pucBuf,u16 i_wLen,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u16 wDataLen=i_wLen;
	u16 wIndex;
	u16 wFirstPartLen;
	u16 wSecondPartLen;
	char *pcTmp;
	u8 FindLinkAddressFlg=0;
	u8 *pSendDataBuf=o_pucBuf;
	u16 wSendDataBufLen=0;
	u8 *pucSecondPartSendData;
	if((NULL==i_pucBuf)||(0==i_wLen))
	{
		ret=FALSE;
		DebugPrintf(ERR"SmsProtocolPackAlarmData err \r\n");
	}
	else
	{
		if(wDataLen < (70 + 1))
		{ 		
			hookSmsLock();
			//wSendDataBufLen = hookSmsPduEncode(pSendDataBuf, NULL, i_pucPhoneNumBuf, i_pucBuf, wDataLen, SMS_PROTOCOL_UCS2_ENCODE);
			memcpy(pSendDataBuf,i_pucBuf,wDataLen);
			wSendDataBufLen=wDataLen;
			hookSmsUnLock();						
			ret=SmsProtocolSendData(i_pucPhoneNumBuf,pSendDataBuf, wSendDataBufLen);			
			if(FALSE==ret)
			{
				DebugPrintf(ERR"%s,Line:%d err\n", __FUNCTION__, __LINE__);
			}
			else{
			}
		}
		else
		{
			pcTmp = (char *)i_pucBuf;
			M_ClrBit(FindLinkAddressFlg);
			for(wIndex = wDataLen - 1; wIndex > 0; wIndex--)
			{
				if(SMS_PROTOCOL_LINK_ADDR_SYMBOL != *(pcTmp +wIndex))
				{
					NULL;
				}
				else
				{
					M_SetBit(FindLinkAddressFlg);
					break;
				}
			}
			if(IS_FALSE_BIT(FindLinkAddressFlg))
			{
				wFirstPartLen = 70;
				wSecondPartLen = wDataLen - wFirstPartLen;
				wSecondPartLen > 70 ? (wSecondPartLen = 70) : 0;
			}
			else
			{
				wIndex++;	//skip !
				wFirstPartLen =wIndex;
				wFirstPartLen > 70 ? (wFirstPartLen = 70) : 0; 
				wSecondPartLen = wDataLen - wFirstPartLen;
				wSecondPartLen > 70 ? (wSecondPartLen = 70) : 0;
			}			
			hookSmsLock();
			//wSendDataBufLen = hookSmsPduEncode(pSendDataBuf,NULL, i_pucPhoneNumBuf, i_pucBuf, wFirstPartLen, SMS_PROTOCOL_UCS2_ENCODE);
			memcpy(pSendDataBuf,i_pucBuf,wFirstPartLen);
			wSendDataBufLen=wFirstPartLen;
			hookSmsUnLock();					
			ret=SmsProtocolSendData(i_pucPhoneNumBuf,pSendDataBuf, wSendDataBufLen);
			if(FALSE==ret)
			{
				DebugPrintf(ERR"%s,Line:%d err\n", __FUNCTION__, __LINE__);
			}
			else{
			}
			hookSmsLock();
			//wSendDataBufLen = hookSmsPduEncode(pSendDataBuf,NULL, i_pucPhoneNumBuf,i_pucBuf+ wFirstPartLen, wSecondPartLen, SMS_PROTOCOL_UCS2_ENCODE);
			pucSecondPartSendData=i_pucBuf+wFirstPartLen;
			memcpy(pSendDataBuf,pucSecondPartSendData,wSecondPartLen);
			wSendDataBufLen=wSecondPartLen;
			hookSmsUnLock();	
			ret&=SmsProtocolSendData(i_pucPhoneNumBuf,pSendDataBuf, wSendDataBufLen);		
			if(FALSE==ret)
			{
				DebugPrintf(ERR"%s,Line:%d err\n", __FUNCTION__, __LINE__);
			}
			else{
			}
		}
	}	
	return ret;
}

/*****************************************************************************
-Fuction		: SmsProtocolSendData
-Description	: SmsProtocolSendData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolSendData(u8 *i_PhoneNumBuf,u8 *i_ucBuf,u16 i_wLen)
{
	u8 ret=FALSE;
	u8 ucCount=0;
	
	for(ucCount=0;ucCount<3;ucCount++)
	{
		ret=hookSmsSendData(i_PhoneNumBuf,i_ucBuf,i_wLen);//需要让出给别人运行给你数据，所以不能锁 	
		if(FALSE==ret)
		{
		}
		else
		{
			break;
		}
	}
	
	if(IS_FALSE_BIT(g_ucSetNetParaNeedResetFlag))
	{
	}
	else
	{	
		hookSmsRestartSystem();
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SmsProtocolSendData
-Description	: SmsProtocolSendData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolSendAlarmData(u8 *i_ucBuf,u16 i_wLen)
{
	u8 ret=FALSE;
	u8 ucCount=0;
	u8 ucSmsSendDataBusyFlag=FALSE;
	u8 ucSmsSendDataFlag=FALSE;
	hookSmsLock();
	ucSmsSendDataBusyFlag=g_ucSmsSendDataBusyFlag;
	ucSmsSendDataFlag=g_ucSmsSendDataFlag;
	hookSmsUnLock();
	if((IS_TRUE_BIT(ucSmsSendDataBusyFlag))||(IS_FALSE_BIT(ucSmsSendDataFlag)))
	{
		ret=FALSE;
		DebugPrintf(ERR"SmsProtocolSendData err \r\n");
	}
	else
	{	
		hookSmsLock();
		M_SetBit(g_ucSmsSendDataBusyFlag);		
		hookSmsUnLock();
		for(ucCount=0;ucCount<3;ucCount++)
		{
			ret=hookSmsSendAlarmData(i_ucBuf,i_wLen);//需要让出给别人运行给你数据，所以不能锁		
			if(FALSE==ret)
			{
			}
			else
			{
				break;
			}
		}
	}	
	hookSmsLock();
	M_ClrBit(g_ucSmsSendDataBusyFlag);
	M_ClrBit(g_ucSmsSendDataFlag);
	hookSmsUnLock();
	return ret;
}

/*****************************************************************************
-Fuction		: SmsProtocolPackData
-Description	: SmsProtocolPackData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/08    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolPackAlarmData(u8 *i_pucPhoneNumBuf,u8 *i_pucBuf,u16 i_wLen,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u16 wDataLen=i_wLen;
	u16 wIndex;
	u16 wFirstPartLen;
	u16 wSecondPartLen;
	char *pcTmp;
	u8 FindLinkAddressFlg=0;
	u8 ucSmsSendDataBusyFlag=FALSE;
	u8 *pSendDataBuf=o_pucBuf;
	u16 wSendDataBufLen=SMS_PROTOCOL_MAX_LEN;
	//static u8 ucPhoneNumBuf[PHONE_STR_LEN]={0};
	//memset(g_SmsPhoneNumBuf,0,sizeof(g_SmsPhoneNumBuf));
	//memcpy(g_SmsPhoneNumBuf,i_pucPhoneNumBuf,strnlen(i_pucPhoneNumBuf,PHONE_STR_LEN));
	if((NULL==i_pucBuf)||(0==i_wLen))
	{
		ret=FALSE;
		DebugPrintf(ERR"SmsProtocolPackData err \r\n");
	}
	else
	{
		hookSmsLock();
		memset(pSendDataBuf, 0, wSendDataBufLen);
		hookSmsUnLock();
		if(wDataLen < (70 + 1))
		{		
			hookSmsLock();
			wSendDataBufLen = hookSmsPduEncode(pSendDataBuf, NULL, i_pucPhoneNumBuf, i_pucBuf, wDataLen, SMS_PROTOCOL_UCS2_ENCODE);
			ucSmsSendDataBusyFlag=g_ucSmsSendDataBusyFlag;		
			hookSmsUnLock();
			if(IS_TRUE_BIT(ucSmsSendDataBusyFlag))
			{
				DebugPrintf(ERR"SmsSendDataBusy1!\r\n");
				hookSmsTimeDelay(5, 0); //等待发送
			}
			else{
			}
			hookSmsLock();
			M_SetBit(g_ucSmsSendDataFlag);			
			hookSmsUnLock();
			
			ret=SmsProtocolSendAlarmData(pSendDataBuf, wSendDataBufLen); 		
		}
		else
		{
			pcTmp = (char *)i_pucBuf;
			M_ClrBit(FindLinkAddressFlg);
			for(wIndex = wDataLen - 1; wIndex > 0; wIndex--)
			{
				if(SMS_PROTOCOL_LINK_ADDR_SYMBOL != *(pcTmp +wIndex))
				{
					NULL;
				}
				else
				{
					M_SetBit(FindLinkAddressFlg);
					break;
				}
			}
			if(IS_FALSE_BIT(FindLinkAddressFlg))
			{
				wFirstPartLen = 70;
				wSecondPartLen = wDataLen - wFirstPartLen;
				wSecondPartLen > 70 ? (wSecondPartLen = 70) : 0;
			}
			else
			{
				wIndex++;	//skip !
				wFirstPartLen =wIndex;
				wFirstPartLen > 70 ? (wFirstPartLen = 70) : 0; 
				wSecondPartLen = wDataLen - wFirstPartLen;
				wSecondPartLen > 70 ? (wSecondPartLen = 70) : 0;
			}			
			hookSmsLock();
			wSendDataBufLen = hookSmsPduEncode(pSendDataBuf,NULL, i_pucPhoneNumBuf, i_pucBuf, wFirstPartLen, SMS_PROTOCOL_UCS2_ENCODE);
			ucSmsSendDataBusyFlag=g_ucSmsSendDataBusyFlag;			
			hookSmsUnLock();
			if(IS_TRUE_BIT(ucSmsSendDataBusyFlag))
			{
				DebugPrintf(ERR"SmsSendDataBusy2!\r\n");
				hookSmsTimeDelay(5, 0); //等待发送
			}
			else{
			}
			
			hookSmsLock();
			M_SetBit(g_ucSmsSendDataFlag);
			ucSmsSendDataBusyFlag=g_ucSmsSendDataBusyFlag;			
			hookSmsUnLock();
			
			ret=SmsProtocolSendAlarmData(pSendDataBuf, wSendDataBufLen);
			if(IS_TRUE_BIT(ucSmsSendDataBusyFlag))
			{
				DebugPrintf(ERR"SmsSendDataBusy3!\r\n");
				hookSmsTimeDelay(5, 0); //等待发送
			}
			else{
			}
			hookSmsTimeDelay(3, 0); //等待发送
			
			hookSmsLock();
			M_SetBit(g_ucSmsSendDataFlag);			
			memset(pSendDataBuf, 0, SMS_PROTOCOL_MAX_LEN);
			wSendDataBufLen = hookSmsPduEncode(pSendDataBuf,NULL, i_pucPhoneNumBuf,i_pucBuf+ wFirstPartLen, wSecondPartLen, SMS_PROTOCOL_UCS2_ENCODE);
			hookSmsUnLock();

			ret&=SmsProtocolSendAlarmData(pSendDataBuf, wSendDataBufLen);		
		}
	}	
	return ret;
}


/*****************************************************************************
-Fuction		: SmsProtocolReportAlarm
-Description	: SmsProtocolReportAlarm
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/21    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolReportAlarm(T_Alarm i_tAlarm,u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen)
{
	u8 ret=FALSE;
	u8 *pucSmsReportAlarm=i_pucDataBuf;
	LocationVectorInfo tGpsLocationInfo={0};
	u16 wSmsReportAlarmLen=i_wDataLen;	
	static u8 ucEmergencyContactPhoneNum[3][PHONE_STR_LEN+1]={0};
	u16 wEmergencyContactPhoneNumLen[3]={0};
	memset(ucEmergencyContactPhoneNum,0,sizeof(ucEmergencyContactPhoneNum));
	hookSmsGetParameter("SmsPara",EmergencyContactNum1,ucEmergencyContactPhoneNum[0],&wEmergencyContactPhoneNumLen[0]);
	hookSmsGetParameter("SmsPara",EmergencyContactNum2,ucEmergencyContactPhoneNum[1],&wEmergencyContactPhoneNumLen[1]);
	hookSmsGetParameter("SmsPara",EmergencyContactNum3,ucEmergencyContactPhoneNum[2],&wEmergencyContactPhoneNumLen[2]);
	
	memset(i_pucDataBuf,0,i_wDataLen);
	SmsProtocolGetGpsLocateInfo(&tGpsLocationInfo);
	if(1==i_tAlarm.CutTrackerAlarm)//单独一条短信
	{
		SmsProtocolReportInfoPack(&tGpsLocationInfo,g_pstrSmsCutOffAlarm,i_wDataLen,pucSmsReportAlarm,&wSmsReportAlarmLen);
		if(0==wEmergencyContactPhoneNumLen[0])
		{			
		}
		else
		{
			ret=SmsProtocolPackData(ucEmergencyContactPhoneNum[0],i_pucDataBuf,wSmsReportAlarmLen,o_pucDataSendBuf,o_wSendLen);
		}	
		if(0==wEmergencyContactPhoneNumLen[1])
		{			
		}
		else
		{
			ret=SmsProtocolPackData(ucEmergencyContactPhoneNum[1],i_pucDataBuf,wSmsReportAlarmLen,o_pucDataSendBuf,o_wSendLen);
		}	
		if(0==wEmergencyContactPhoneNumLen[2])
		{			
		}
		else
		{
			ret=SmsProtocolPackData(ucEmergencyContactPhoneNum[2],i_pucDataBuf,wSmsReportAlarmLen,o_pucDataSendBuf,o_wSendLen);
		}
	}
	else
	{
	}
	if(1==i_tAlarm.DropTrackerAlarm)//单独一条短信
	{
		SmsProtocolReportInfoPack(&tGpsLocationInfo,g_pstrSmsFallOffAlarm,i_wDataLen,pucSmsReportAlarm,&wSmsReportAlarmLen);
		if(0==wEmergencyContactPhoneNumLen[0])
		{			
		}
		else
		{
			ret=SmsProtocolPackData(ucEmergencyContactPhoneNum[0],i_pucDataBuf,wSmsReportAlarmLen,o_pucDataSendBuf,o_wSendLen);
		}	
		if(0==wEmergencyContactPhoneNumLen[1])
		{			
		}
		else
		{
			ret=SmsProtocolPackData(ucEmergencyContactPhoneNum[1],i_pucDataBuf,wSmsReportAlarmLen,o_pucDataSendBuf,o_wSendLen);
		}	
		if(0==wEmergencyContactPhoneNumLen[2])
		{			
		}
		else
		{
			ret=SmsProtocolPackData(ucEmergencyContactPhoneNum[2],i_pucDataBuf,wSmsReportAlarmLen,o_pucDataSendBuf,o_wSendLen);
		}
	}
	else
	{
	}
	if(1==i_tAlarm.LowPowerAlarm)//单独一条短信
	{
		SmsProtocolReportInfoPack(&tGpsLocationInfo,g_pstrSmsLowVoltageAlarm,i_wDataLen,pucSmsReportAlarm,&wSmsReportAlarmLen);
		if(0==wEmergencyContactPhoneNumLen[0])
		{			
		}
		else
		{
			ret=SmsProtocolPackData(ucEmergencyContactPhoneNum[0],i_pucDataBuf,wSmsReportAlarmLen,o_pucDataSendBuf,o_wSendLen);
		}	
		if(0==wEmergencyContactPhoneNumLen[1])
		{			
		}
		else
		{
			ret=SmsProtocolPackData(ucEmergencyContactPhoneNum[1],i_pucDataBuf,wSmsReportAlarmLen,o_pucDataSendBuf,o_wSendLen);
		}	
		if(0==wEmergencyContactPhoneNumLen[2])
		{			
		}
		else
		{
			ret=SmsProtocolPackData(ucEmergencyContactPhoneNum[2],i_pucDataBuf,wSmsReportAlarmLen,o_pucDataSendBuf,o_wSendLen);
		}
	}
	else
	{
	}	
	return ret;
}
/*****************************************************************************
-Fuction		: SmsProtocolReportAlarm
-Description	: SmsProtocolReportAlarm
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/21    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsProtocolReportLocation(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen)
{
	u8 ret=FALSE;
	u8 *pucSmsReport=i_pucDataBuf;
	u16 wSmsReportLen=0;	
	LocationVectorInfo tGpsLocationInfo={0};
	u8 *o_pucDataBuf;//由于在一个任务里所以不传入内存
	u16 o_wDataLen;//这里只是一个形式，函数内部不会使用
	u16 i_wDataBufMaxLen;//后续进行优化
	
	memset(i_pucDataBuf,0,i_wDataLen);
	SmsProtocolGetGpsLocateInfo(&tGpsLocationInfo);
	SmsProtocolReportInfoPack(&tGpsLocationInfo,NULL,i_wDataLen,pucSmsReport,&wSmsReportLen);
	
	ret=SmsProtocolGetData(PROTOCOL_USED_FOR_SMS,o_pucDataBuf,&o_wDataLen,i_wDataBufMaxLen);
	if(FALSE==ret)
	{
		DebugPrintf(ERR"SmsProtocolGetData err\r\n");
	}
	else
	{	
		ret=SmsProtocolPackData(g_tSmsInfo.strPhoneNum,i_pucDataBuf,wSmsReportLen,o_pucDataSendBuf,o_wSendLen);
	}
	return ret;
}

/*************************短信操作支持函数**************************/

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
static u8 SmsProtocolGetGpsLocateInfo(LocationVectorInfo *o_ptGpsLocationInfo)
{
	u8 ret=FALSE;	
	ret=hookSmsGetGpsLocationInfo(o_ptGpsLocationInfo);
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
static u8 SmsProtocolReportInfoPack(LocationVectorInfo *i_ptGpsLocationInfo,const char **i_pstrCmd,u16 i_wReportMaxLen,u8 *o_pReportBuf,u16 *o_pwReportBufLen)
{
	u8 ret=FALSE;
	u8 ucErr;
	u8 ucCsqValue=0;
	s8 cLocateFlag=0;	
	u8 ucTrackerLanguage=0;
	u16 wTempLen=0;
	u16 wYear=0;
	static u8 DeviceIdBuf[DEVICE_ID_LEN]={0};	
	s8 cLatitudeSymbol[2] = {0};
	s8 cLongitudeSymbol[2] = {0};
	u8 *pucSmsReport=o_pReportBuf;
	u16 wSmsReportLen=0;	
	if(i_ptGpsLocationInfo->locationCommonPara.fixStatus!=FIX_3D)
	{
		cLocateFlag='V';
	}
	else
	{
		
		cLocateFlag='A';
	}
	
	hookSmsGetParameter("SmsPara",DeviceID,DeviceIdBuf,&wTempLen);	
	hookSmsGetParameter("CommonPara",TrackerLanguage,&ucTrackerLanguage,&wTempLen);
	hookSmsGetGsmCsq(&ucCsqValue);
	wTempLen = snprintf( (s8 *)pucSmsReport,i_wReportMaxLen, "%s,",DeviceIdBuf);
	wSmsReportLen+=wTempLen;
	pucSmsReport+=wTempLen;
	
	if(NULL==i_pstrCmd)
	{
	}
	else
	{
		if(SMS_PROTOCOL_CHINESE_LANGUAGE==ucTrackerLanguage)
		{
			wTempLen = snprintf( (s8 *)pucSmsReport,i_wReportMaxLen, "%s,",i_pstrCmd[1]);
			wSmsReportLen+=wTempLen;
			pucSmsReport+=wTempLen;
		}
		else if(SMS_PROTOCOL_ENGLISH_LANGUAGE==ucTrackerLanguage)
		{
			wTempLen = snprintf( (s8 *)pucSmsReport,i_wReportMaxLen, "%s,",i_pstrCmd[0]);
			wSmsReportLen+=wTempLen;
			pucSmsReport+=wTempLen;
		}
		else
		{
			DebugPrintf(ERR"get TrackerLanguage err\r\n");
		}
	}
	
	wTempLen = snprintf( (s8 *)pucSmsReport,i_wReportMaxLen, "%d,%d,%c,",ucCsqValue,i_ptGpsLocationInfo->locationCommonPara.satelliteNum,cLocateFlag);
	wSmsReportLen+=wTempLen;
	pucSmsReport+=wTempLen;
	
	wYear=i_ptGpsLocationInfo->dataTime.date.year+2000;
	wTempLen = snprintf( (s8 *)pucSmsReport,i_wReportMaxLen, "%04d-%02d-%02d %02d:%02d:%02d!",
						wYear,
						i_ptGpsLocationInfo->dataTime.date.month,
						i_ptGpsLocationInfo->dataTime.date.day,
						i_ptGpsLocationInfo->dataTime.time.hour,				
						i_ptGpsLocationInfo->dataTime.time.minute,
						i_ptGpsLocationInfo->dataTime.time.second);
	wSmsReportLen+=wTempLen;
	pucSmsReport+=wTempLen;
	
	if(SMS_PROTOCOL_CHINESE_LANGUAGE==ucTrackerLanguage)
	{
		wTempLen = snprintf( (s8 *)pucSmsReport,i_wReportMaxLen, "%s",g_pstrSmsLocationLink[1]);
		wSmsReportLen+=wTempLen;
		pucSmsReport+=wTempLen;
	}
	else if(SMS_PROTOCOL_ENGLISH_LANGUAGE==ucTrackerLanguage)
	{
		wTempLen = snprintf( (s8 *)pucSmsReport,i_wReportMaxLen, "%s",g_pstrSmsLocationLink[0]);
		wSmsReportLen+=wTempLen;
		pucSmsReport+=wTempLen;
	}
	else
	{
		DebugPrintf(ERR"get TrackerLanguage err\r\n");
	}
	memset(cLatitudeSymbol,0,sizeof(cLatitudeSymbol));	
	memset(cLongitudeSymbol,0,sizeof(cLongitudeSymbol));
	if(LATITUDE_SOUTH==i_ptGpsLocationInfo->positionInfo.latRegion)
	{
		cLatitudeSymbol[0]='-';
	}
	else
	{	
		cLatitudeSymbol[0]='\0';
	}	
	if(LONGITUDE_WEST==i_ptGpsLocationInfo->positionInfo.lonRegion)
	{
		cLongitudeSymbol[0]='-';
	}
	else
	{	
		cLongitudeSymbol[0]='\0';
	}
	wTempLen = snprintf( (s8 *)pucSmsReport,i_wReportMaxLen, "%s%u.%06d,%s%u.%06d",
						cLatitudeSymbol,
						i_ptGpsLocationInfo->positionInfo.lat / SMS_PROTOCOL_POSITION_INFO_UNIT,
						i_ptGpsLocationInfo->positionInfo.lat % SMS_PROTOCOL_POSITION_INFO_UNIT,
						cLongitudeSymbol,
						i_ptGpsLocationInfo->positionInfo.lon / SMS_PROTOCOL_POSITION_INFO_UNIT,
						i_ptGpsLocationInfo->positionInfo.lon % SMS_PROTOCOL_POSITION_INFO_UNIT );
	wSmsReportLen+=wTempLen;
	pucSmsReport+=wTempLen;

	*o_pwReportBufLen=wSmsReportLen;
	ret=TRUE;
	return ret;
}


/*****************************************************************************
-Fuction		: LowerToUpper
-Description	: 使用BD短信里面的直接copy过来的
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
u8 *LowerToUpper(u8 *i_pucStr, u16 i_wLen)
{
	s32 i;
	if((NULL == i_pucStr) || (IS_FALSE_BIT(i_wLen)))
	{
		NULL;
	}
	else
	{
		for(i = 0; i < i_wLen; i++)
		{
			if((*(i_pucStr + i) >= 'a') && (*(i_pucStr + i) <= 'z'))
			{
				*(i_pucStr + i) -= 32;
			}
			else
			{
				NULL;
			}
		}
	}	
	return i_pucStr; 
}
/*****************************************************************************
-Fuction		: SmsProtocolParaGet
-Description	: 使用BD短信里面的直接copy过来的
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
u16 SmsProtocolParaGet( u8 *i_pucStart, u8 i_ucEndSign, u16 i_wMaxLen)
{
	u16 wLen=0;
	while( *(i_pucStart + wLen) != i_ucEndSign )
	{
		wLen++;
		if(wLen < i_wMaxLen)
		{
			NULL;
		}
		else
		{
			break;
		}
	}
	return wLen;
}
/*****************************************************************************
-Fuction		: SmsProtocolToParaFormat
-Description	: 使用BD短信里面的直接copy过来的
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static void SmsProtocolToParaFormat(u8 *i_pucBuf,u16 i_wSmsLen,T_SmsParaFormat *o_ptParaBuf)
{
	u16 wIndex=0;
	u16 wLen;
	u8 *pucTmp;
	s32 i;
	u16 wSmsLen;
	memset(o_ptParaBuf, 0, sizeof(T_SmsParaFormat));
	wSmsLen = i_wSmsLen;
	pucTmp = i_pucBuf;
	for(i = 0; i < SMS_PROTOCOL_MAX_PARA_NUM; i++)
	{
		if(wSmsLen > wIndex)
		{
			NULL;
		}
		else
		{
			break;
		}		
		o_ptParaBuf->tSmsPara[o_ptParaBuf->ucParaNum].pucPara= (pucTmp + wIndex);
		wLen = SmsProtocolParaGet(pucTmp + wIndex,
								  SMS_PROTOCOL_FORMAT_DELIMITER,
								  wSmsLen - wIndex);
		o_ptParaBuf->tSmsPara[o_ptParaBuf->ucParaNum++].wLen= wLen;
		wIndex += wLen;
		if(wSmsLen > wIndex)
		{
			NULL;
		}
		else
		{
			break;
		}
		wIndex++;	// skip SMS_MAINTAIN_BY_KEY_FORMAT_DELIMITER
		if(wSmsLen > wIndex)
		{
			NULL;
		}
		else
		{
			if(SMS_PROTOCOL_FORMAT_DELIMITER != *(pucTmp + wIndex - 1))
			{
				NULL;
			}
			else		//针对最后一个是分隔符后无内容的
			{
				o_ptParaBuf->tSmsPara[o_ptParaBuf->ucParaNum].pucPara = NULL;
				M_ClrBit(o_ptParaBuf->tSmsPara[o_ptParaBuf->ucParaNum++].wLen);
			}
			break;
		}
	}
}
/***********************短信协议处理函数****************************/

/*****************************************************************************
-Fuction		: SmsGetLBG
-Description	: SmsGetLBG 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsGetLBG(PT_SmsParaFormat i_pSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	LocationVectorInfo tGpsLocationInfo={0};
	SmsProtocolGetGpsLocateInfo(&tGpsLocationInfo);
	ret=SmsProtocolReportInfoPack(&tGpsLocationInfo,NULL,SMS_PROTOCOL_MAX_SEND_LEN,o_pucBuf,o_wLen);
	return ret;
}
/*****************************************************************************
-Fuction		: SmsGetLBG
-Description	: SmsGetLBG 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsGetLBS(PT_SmsParaFormat i_pSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;

	return ret;
}
/*****************************************************************************
-Fuction		: SmsGetNetPara
-Description	: SmsGetNetPara 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/20    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsGetNetPara(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 aucGetDataBuf[20]={0};
	u16 wGetDataBufLen=20;
	u8 *pucSendBuf=o_pucBuf;
	u16 wPort=0;
	if((NULL == o_pucBuf) || (NULL == o_wLen) ||(NULL==i_ptSmsPara)) 
	{
		DebugPrintf(ERR"SmsGetNetPara err\r\n");
		ret=FALSE;
	}
	else 
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
			      i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;

		hookSmsGetParameter("NetPara",DailMode,aucGetDataBuf,&wGetDataBufLen);
		if(IP_DIAL_MODE==aucGetDataBuf[0])
		{
			*pucSendBuf++ ='1';
		}
		else
		{
			*pucSendBuf++ ='0';
		}
		wGetDataBufLen=0;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		if(IP_DIAL_MODE==aucGetDataBuf[0])
		{
			hookSmsGetParameter("NetPara",Ip,pucSendBuf,&wGetDataBufLen);
		}
		else
		{
			hookSmsGetParameter("NetPara",Domain,pucSendBuf,&wGetDataBufLen);
		}
		pucSendBuf+=wGetDataBufLen;
		wGetDataBufLen=0;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		
		hookSmsGetParameter("NetPara",Port,pucSendBuf,&wGetDataBufLen);
		wPort=Lsb2U8ToU16(pucSendBuf);
		wGetDataBufLen=snprintf((char *)aucGetDataBuf,20,"%d",wPort);
		memcpy(pucSendBuf,aucGetDataBuf,wGetDataBufLen);
		pucSendBuf+=wGetDataBufLen;
		wGetDataBufLen=0;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		
		hookSmsGetParameter("NetPara",Apn,pucSendBuf,&wGetDataBufLen);
		pucSendBuf+=wGetDataBufLen;
		wGetDataBufLen=0;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		
		hookSmsGetParameter("NetPara",UsrName,pucSendBuf,&wGetDataBufLen);
		pucSendBuf+=wGetDataBufLen;
		wGetDataBufLen=0;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		
		hookSmsGetParameter("NetPara",Pwd,pucSendBuf,&wGetDataBufLen);
		pucSendBuf+=wGetDataBufLen;
		wGetDataBufLen=0;
		
		*o_wLen=(pucSendBuf-o_pucBuf);		
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SmsGetVersion
-Description	: SmsGetVersion 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/20    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsGetVersion(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u16 wGetDataBufLen=0;
	u8 *pucSendBuf=o_pucBuf;
	if((NULL == o_pucBuf) || (NULL == o_wLen) ||(NULL==i_ptSmsPara)) 
	{
		DebugPrintf(ERR"SmsGetVersion err\r\n");
		ret=FALSE;
	}
	else 
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
			      i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;

		hookSmsGetParameter("CommonPara",SoftWareVision,pucSendBuf,&wGetDataBufLen);
		pucSendBuf+=wGetDataBufLen;
		wGetDataBufLen=0;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		
		hookSmsGetParameter("CommonPara",HardWareVision,pucSendBuf,&wGetDataBufLen);
		pucSendBuf+=wGetDataBufLen;
		wGetDataBufLen=0;

		*o_wLen=(pucSendBuf-o_pucBuf);	
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SmsGetTrackerIDInformation
-Description	: SmsGetTrackerIDInformation
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/20    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsGetTrackerIDInformation(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u16 wGetDataBufLen=0;
	u8 *pucSendBuf=o_pucBuf;
	if((NULL == o_pucBuf) || (NULL == o_wLen) ||(NULL==i_ptSmsPara)) 
	{
		DebugPrintf(ERR"SmsGetTrackerIDInformation err\r\n");
		ret=FALSE;
	}
	else 
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
			      i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;

		hookSmsGetParameter("SmsPara",DeviceID,pucSendBuf,&wGetDataBufLen);
		pucSendBuf+=wGetDataBufLen;
		wGetDataBufLen=0;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		
		hookSmsGetIMEL(pucSendBuf,&wGetDataBufLen);
		pucSendBuf+=wGetDataBufLen;
		wGetDataBufLen=0;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		
		hookSmsGetParameter("SmsPara",SimCardNum,pucSendBuf,&wGetDataBufLen);
		pucSendBuf+=wGetDataBufLen;
		wGetDataBufLen=0;

		*o_wLen=(pucSendBuf-o_pucBuf);		
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SmsDefaultInitPara
-Description	: SmsDefaultInitPara
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/20    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsDefaultInitPara(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 *pucSendBuf=o_pucBuf;
	if((NULL == o_pucBuf) || (NULL == o_wLen)||(NULL==i_ptSmsPara)) 
	{
		DebugPrintf(ERR"SmsDefaultInitPara err\r\n");
		ret=FALSE;
	}
	else 
	{
		hookSmsDeInit();
		ret=TRUE;
	}
	if(FALSE==ret)
	{
	}
	else
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
				  i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		memcpy(pucSendBuf,SMS_PROTOCOL_SET_SUCCESS_STR,
					  strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR)));
		pucSendBuf+=strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR));
		*o_wLen=(pucSendBuf-o_pucBuf);
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SmsDefaultInitPara
-Description	: SmsDefaultInitPara
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/20    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsRestartSystem(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 *pucSendBuf=o_pucBuf;
	if((NULL == o_pucBuf) || (NULL == o_wLen)||(NULL==i_ptSmsPara)) 
	{
		DebugPrintf(ERR"SmsRestartSystem err\r\n");
		ret=FALSE;
	}
	else 
	{
		hookSmsRestartSystem();
		ret=TRUE;
	}
	if(FALSE==ret)
	{
	}
	else
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
				  i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		memcpy(pucSendBuf,SMS_PROTOCOL_SET_SUCCESS_STR,
					  strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR)));
		pucSendBuf+=strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR));
		*o_wLen=(pucSendBuf-o_pucBuf);
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SmsSetTrackerLanguageEn
-Description	: SmsSetTrackerLanguageEn 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsSetTrackerLanguageEn(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 aucSetDataBuf;
	u16 wSetDataBufLen=1;
	u8 *pucSendBuf=o_pucBuf;
	if((NULL == o_pucBuf) || (NULL == o_wLen)||(NULL==i_ptSmsPara)) 
	{
		DebugPrintf(ERR"SmsSetDevLanguageEn err\r\n");
		ret=FALSE;
	}
	else 
	{
		aucSetDataBuf=ENGLISH_LANGUAGE;
		wSetDataBufLen=1;
		ret=hookSmsSetParameter("CommonPara",TrackerLanguage,&aucSetDataBuf,wSetDataBufLen);
	}
	if(FALSE==ret)
	{
	}
	else
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
			      i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		memcpy(pucSendBuf,SMS_PROTOCOL_SET_SUCCESS_STR,
	     		      strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR)));
		pucSendBuf+=strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR));
		*o_wLen=(pucSendBuf-o_pucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SmsSetTrackerLanguageCn
-Description	: SmsSetTrackerLanguageCn 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsSetTrackerLanguageCn(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 aucSetDataBuf;
	u16 wSetDataBufLen=1;
	u8 *pucSendBuf=o_pucBuf;
	if((NULL == o_pucBuf) || (NULL == o_wLen) ||(NULL==i_ptSmsPara)) 
	{
		DebugPrintf(ERR"SmsSetDevLanguageEn err\r\n");
		ret=FALSE;
	}
	else 
	{
		aucSetDataBuf=CHINESE_LANGUAGE;
		wSetDataBufLen=1;
		ret=hookSmsSetParameter("CommonPara",TrackerLanguage,&aucSetDataBuf,wSetDataBufLen);
	}
	if(FALSE==ret)
	{
	}
	else
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
			      i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		memcpy(pucSendBuf,SMS_PROTOCOL_SET_SUCCESS_STR,
	     		      strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR)));
		pucSendBuf+=strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR));
		*o_wLen=(pucSendBuf-o_pucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SmsSetEmergencyContactNum
-Description	: SmsSetEmergencyContactNum 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsSetEmergencyContactNum(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 ucIndex;
	u8 aucSetDataBuf[PHONE_STR_LEN];
	u16 wSetDataBufLen=PHONE_STR_LEN;
	u8 *pucSendBuf=o_pucBuf;
	if((NULL == o_pucBuf) || (NULL == o_wLen)||(NULL==i_ptSmsPara)||(2==i_ptSmsPara->ucParaNum) ) 
	{
		DebugPrintf(ERR"SmsSetEmergencyContactNum err\r\n");
		ret=FALSE;
	}
	else 
	{
		if((i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].wLen>PHONE_STR_LEN)||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+1].wLen>PHONE_STR_LEN)||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+2].wLen>PHONE_STR_LEN))
		{
			DebugPrintf(ERR"SmsSetEmergencyContactNum Format err\r\n");
			ret=FALSE;
		}
		else
		{		
			ucIndex = SMS_PROTOCOL_PARA_START;
			memset(aucSetDataBuf,0,sizeof(aucSetDataBuf));
			wSetDataBufLen=sizeof(aucSetDataBuf);
			ret=TRUE;
			if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
			{
				wSetDataBufLen=i_ptSmsPara->tSmsPara[ucIndex].wLen;
				memcpy(aucSetDataBuf,i_ptSmsPara->tSmsPara[ucIndex].pucPara,wSetDataBufLen);
				ret&=hookSmsSetParameter("SmsPara",EmergencyContactNum1,aucSetDataBuf,wSetDataBufLen);
			}
			else
			{
				NULL;
			}
			ucIndex++;
			if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
			{
				wSetDataBufLen=i_ptSmsPara->tSmsPara[ucIndex].wLen;
				memcpy(aucSetDataBuf,i_ptSmsPara->tSmsPara[ucIndex].pucPara,wSetDataBufLen);
				ret&=hookSmsSetParameter("SmsPara",EmergencyContactNum2,aucSetDataBuf,wSetDataBufLen);
			}
			else
			{
				NULL;
			}
			ucIndex++;
			if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
			{
				wSetDataBufLen=i_ptSmsPara->tSmsPara[ucIndex].wLen;
				memcpy(aucSetDataBuf,i_ptSmsPara->tSmsPara[ucIndex].pucPara,wSetDataBufLen);
				ret&=hookSmsSetParameter("SmsPara",EmergencyContactNum3,aucSetDataBuf,wSetDataBufLen);
			}
			else
			{
				NULL;
			}		
		}
	}
	if(FALSE==ret)
	{
	}
	else
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
			      i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		for(ucIndex=0;ucIndex<3;ucIndex++)
		{
			memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+ucIndex].pucPara,
					  i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+ucIndex].wLen);
			pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+ucIndex].wLen;
			*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		}
		memcpy(pucSendBuf,SMS_PROTOCOL_SET_SUCCESS_STR,
	     		      strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR)));
		pucSendBuf+=strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR));
		*o_wLen=(pucSendBuf-o_pucBuf);
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SmsDelAllEmergencyContactNum
-Description	: SmsDelAllEmergencyContactNum 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsDelAllEmergencyContactNum(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 aucSetDataBuf[PHONE_STR_LEN];
	u16 wSetDataBufLen=PHONE_STR_LEN;
	u8 *pucSendBuf=o_pucBuf;
	if((NULL == o_pucBuf) || (NULL == o_wLen) ||(NULL==i_ptSmsPara)||(2!=i_ptSmsPara->ucParaNum))
	{
		DebugPrintf(ERR"SmsSetEmergencyContactNum err\r\n");
		ret=FALSE;
	}
	else 
	{
		memset(aucSetDataBuf,0,sizeof(aucSetDataBuf));
		wSetDataBufLen=sizeof(aucSetDataBuf);
		ret=hookSmsSetParameter("SmsPara",EmergencyContactNum1,aucSetDataBuf,wSetDataBufLen);
		ret&=hookSmsSetParameter("SmsPara",EmergencyContactNum2,aucSetDataBuf,wSetDataBufLen);
		ret&=hookSmsSetParameter("SmsPara",EmergencyContactNum3,aucSetDataBuf,wSetDataBufLen);
	}
	if(FALSE==ret)
	{
	}
	else
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
			      i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		memcpy(pucSendBuf,SMS_PROTOCOL_SET_SUCCESS_STR,
	     		      strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR)));
		pucSendBuf+=strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR));
		*o_wLen=(pucSendBuf-o_pucBuf);
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SmsSetFixedUploadInterval
-Description	: SmsSetFixedUploadInterval 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/20    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsSetReportInterval(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 ucIndex;
	u8 aucSetDataBuf[4];
	u16 wSetDataBufLen=4;
	u8 *pucSendBuf=o_pucBuf;
	u32 dwUploadInterval=0;
	if((NULL == o_pucBuf) || (NULL == o_wLen) ||(NULL==i_ptSmsPara)) 
	{
		DebugPrintf(ERR"SmsSetReportInterval err\r\n");
		ret=FALSE;
	}
	else 
	{
		if((IS_FALSE_BIT(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].wLen))||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].wLen>SMS_PROTOCOL_MAX_REPORT_INTERVAL_LEN))
		{
			DebugPrintf(ERR"SmsSetReportInterval Format err\r\n");
			ret=FALSE;
		}
		else
		{
			ucIndex = SMS_PROTOCOL_PARA_START;
			dwUploadInterval=atoi((char const *)i_ptSmsPara->tSmsPara[ucIndex].pucPara);
			LsbU32To4U8(aucSetDataBuf,dwUploadInterval);
			wSetDataBufLen=4;
			ret=hookSmsSetParameter("CommonPara",ReportInterval,aucSetDataBuf,wSetDataBufLen);
		}
	}
	if(FALSE==ret)
	{
	}
	else
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
			      i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		memcpy(pucSendBuf,SMS_PROTOCOL_SET_SUCCESS_STR,
	     		      strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR)));
		pucSendBuf+=strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR));
		*o_wLen=(pucSendBuf-o_pucBuf);
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SmsSetSimCardNum
-Description	: SmsSetSimCardNum 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/20    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsSetSimCardNum(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 ucIndex=0;
	u8 aucSetDataBuf[PHONE_STR_LEN];
	u16 wSetDataBufLen=PHONE_STR_LEN;
	u8 *pucSendBuf=o_pucBuf;
	if((NULL == o_pucBuf) || (NULL == o_wLen) ||(NULL==i_ptSmsPara)) 
	{
		DebugPrintf(ERR"SmsSetSimCardNum err\r\n");
		ret=FALSE;
	}
	else 
	{
		if((3!=i_ptSmsPara->ucParaNum)||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].wLen>PHONE_STR_LEN))
		{
			DebugPrintf(ERR"SmsSetSimCardNum Format err\r\n");
			ret=FALSE;
		}
		else
		{
			ucIndex = SMS_PROTOCOL_PARA_START;
			if( (0x01==i_ptSmsPara->tSmsPara[ucIndex].wLen) && (' '==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0]) )
			{
				memset(aucSetDataBuf,0,sizeof(aucSetDataBuf));
				wSetDataBufLen=sizeof(aucSetDataBuf);
				ret=hookSmsSetParameter("SmsPara",SimCardNum,aucSetDataBuf,wSetDataBufLen);
			}
			else
			{
				if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
				{
					ret=hookSmsSetParameter("SmsPara",SimCardNum,i_ptSmsPara->tSmsPara[ucIndex].pucPara,i_ptSmsPara->tSmsPara[ucIndex].wLen);
				}
				else
				{
					NULL;
				}
			}
		}
	}
	if(FALSE==ret)
	{
	}
	else
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
			      i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		memcpy(pucSendBuf,SMS_PROTOCOL_SET_SUCCESS_STR,
	     		      strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR)));
		pucSendBuf+=strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR));
		*o_wLen=(pucSendBuf-o_pucBuf);
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SmsSetNetPara
-Description	: SmsSetNetPara 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/20    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 SmsSetNetPara(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 ucIndex;
	u8 aucSetDataBuf[2]={0};
	u16 wSetDataBufLen=2;
	u8 *pucSendBuf=o_pucBuf;
	u16 wPort=0;
	if((NULL == o_pucBuf) || (NULL == o_wLen) ||(NULL==i_ptSmsPara)) 
	{
		DebugPrintf(ERR"SmsSetNetPara err\r\n");
		ret=FALSE;
	}
	else 
	{
		if((('0'!=i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].pucPara[0])&&
		    ('1'!=i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].pucPara[0]))||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].wLen>1)||
		    (('0'==i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].pucPara[0])&&
		      (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+1].wLen>DOMAIN_STR_LEN))||
		    (('1'==i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].pucPara[0])&&
		      (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+1].wLen>IP_STR_LEN))||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+2].wLen>SMS_PROTOCOL_MAX_PORT_LEN)||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+3].wLen>APN_STR_LEN)||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+4].wLen>USERNAME_STR_LEN)||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+5].wLen>PASSWORD_STR_LEN))
		{
			DebugPrintf(ERR"SmsSetNetPara Format err\r\n");
			ret=FALSE;
		}
		else
		{
			ret=TRUE;
			ucIndex = SMS_PROTOCOL_PARA_START;
			if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
			{
				if('0'==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0])
				{
					aucSetDataBuf[0]=DOMAIN_DIAL_MODE;// 个人追踪器协议与此相反
				}
				else if('1'==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0])
				{
					aucSetDataBuf[0]=IP_DIAL_MODE;// 个人追踪器协议与此相反				
				}
				else
				{
				}
				wSetDataBufLen=1;
				ret&=hookSmsSetParameter("NetPara",DailMode,aucSetDataBuf,wSetDataBufLen);
			}
			else
			{
				NULL;
			}
			
			ucIndex++;
			if( (0x01==i_ptSmsPara->tSmsPara[ucIndex].wLen) && (' '==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0]) )
			{
				if(DOMAIN_DIAL_MODE==aucSetDataBuf[0])
				{
					ret&=hookSmsSetParameter("NetPara",Domain,i_ptSmsPara->tSmsPara[ucIndex].pucPara,0);
				}
				else
				{
					ret&=hookSmsSetParameter("NetPara",Ip,i_ptSmsPara->tSmsPara[ucIndex].pucPara,0);
				}
			}
			else
			{
				if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
				{
					if(DOMAIN_DIAL_MODE==aucSetDataBuf[0])
					{
						ret&=hookSmsSetParameter("NetPara",Domain,i_ptSmsPara->tSmsPara[ucIndex].pucPara,i_ptSmsPara->tSmsPara[ucIndex].wLen);
					}
					else
					{
						ret&=hookSmsSetParameter("NetPara",Ip,i_ptSmsPara->tSmsPara[ucIndex].pucPara,i_ptSmsPara->tSmsPara[ucIndex].wLen);				
					}
				}
				else
				{
					NULL;
				}
			}
			
			ucIndex++;
			if( (0x01==i_ptSmsPara->tSmsPara[ucIndex].wLen) && (' '==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0]) )
			{
				ret&=hookSmsSetParameter("NetPara",Port,aucSetDataBuf,0);
			}
			else
			{
				if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
				{
					wPort=atoi((char const *)i_ptSmsPara->tSmsPara[ucIndex].pucPara);
					LsbU16To2U8(aucSetDataBuf,wPort);
					wSetDataBufLen=2;
					ret&=hookSmsSetParameter("NetPara",Port,aucSetDataBuf,wSetDataBufLen);
				}
				else
				{
					NULL;
				}
			}
			
			ucIndex++;
			if( (0x01==i_ptSmsPara->tSmsPara[ucIndex].wLen) && (' '==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0]) )
			{
				ret&=hookSmsSetParameter("NetPara",Apn,i_ptSmsPara->tSmsPara[ucIndex].pucPara,0);				
			}
			else
			{
				if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
				{
					ret&=hookSmsSetParameter("NetPara",Apn,i_ptSmsPara->tSmsPara[ucIndex].pucPara,i_ptSmsPara->tSmsPara[ucIndex].wLen); 			
				}
				else
				{
					NULL;
				}
			}
			
			ucIndex++;
			if( (0x01==i_ptSmsPara->tSmsPara[ucIndex].wLen) && (' '==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0]) )
			{
				ret&=hookSmsSetParameter("NetPara",UsrName,i_ptSmsPara->tSmsPara[ucIndex].pucPara,0);				
			}
			else
			{
				if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
				{
					ret&=hookSmsSetParameter("NetPara",UsrName,i_ptSmsPara->tSmsPara[ucIndex].pucPara,i_ptSmsPara->tSmsPara[ucIndex].wLen); 			
				}
				else
				{
					NULL;
				}
			}
			
			ucIndex++;
			if( (0x01==i_ptSmsPara->tSmsPara[ucIndex].wLen) && (' '==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0]) )
			{
				ret&=hookSmsSetParameter("NetPara",Pwd,i_ptSmsPara->tSmsPara[ucIndex].pucPara,0);				
			}
			else
			{
				if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
				{
					ret&=hookSmsSetParameter("NetPara",Pwd,i_ptSmsPara->tSmsPara[ucIndex].pucPara,i_ptSmsPara->tSmsPara[ucIndex].wLen); 			
				}
				else
				{
					NULL;
				}
			}

		}
	}
	if(FALSE==ret)
	{
	}
	else
	{
		LowerToUpper(i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara, 
					   i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		memcpy(pucSendBuf,i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].pucPara,
			      i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen);
		pucSendBuf+= i_ptSmsPara->tSmsPara[SMS_PROTOCOL_CMD_LOCATION].wLen;
		*pucSendBuf++ = SMS_PROTOCOL_FORMAT_DELIMITER;
		memcpy(pucSendBuf,SMS_PROTOCOL_SET_SUCCESS_STR,
	     		      strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR)));
		pucSendBuf+=strnlen(SMS_PROTOCOL_SET_SUCCESS_STR,sizeof(SMS_PROTOCOL_SET_SUCCESS_STR));
		*o_wLen=(pucSendBuf-o_pucBuf);
		M_SetBit(g_ucSetNetParaNeedResetFlag);
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SmsSetNetPara
-Description	: SmsSetNetPara 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/20    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 TrackerProtocolSmsSetNetPara(PT_SmsParaFormat i_ptSmsPara,u8 *o_pucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	u8 ucIndex;
	u8 aucSetDataBuf[2]={0};
	u16 wSetDataBufLen=2;
	u8 *pucSendBuf=o_pucBuf;
	u16 wPort=0;
	u8 ucSmsEndIndex;
	if((NULL == o_pucBuf) || (NULL == o_wLen) ||(NULL==i_ptSmsPara)) 
	{
		DebugPrintf(ERR"SmsSetNetPara err\r\n");
		ret=FALSE;
	}
	else 
	{
		ucSmsEndIndex=i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+5].wLen-1;//指向协议的#号
		if((('0'!=i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].pucPara[0])&&
		    ('1'!=i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].pucPara[0]))||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].wLen>1)||
		    (('0'==i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].pucPara[0])&&
		      (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+1].wLen>DOMAIN_STR_LEN))||
		    (('1'==i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START].pucPara[0])&&
		      (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+1].wLen>IP_STR_LEN))||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+2].wLen>SMS_PROTOCOL_MAX_PORT_LEN)||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+3].wLen>APN_STR_LEN)||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+4].wLen>USERNAME_STR_LEN)||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+5].wLen>PASSWORD_STR_LEN)||
		    (i_ptSmsPara->tSmsPara[SMS_PROTOCOL_PARA_START+5].pucPara[ucSmsEndIndex]!='#'))
		{
			DebugPrintf(ERR"SmsSetNetPara Format err\r\n");
			ret=FALSE;
		}
		else
		{
			ret=TRUE;
			ucIndex = SMS_PROTOCOL_PARA_START;
			if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
			{
				if('0'==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0])
				{
					aucSetDataBuf[0]=IP_DIAL_MODE;// 个人追踪器协议
				}
				else if('1'==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0])
				{
					aucSetDataBuf[0]=DOMAIN_DIAL_MODE;// 个人追踪器协议	
				}
				else
				{
				}
				wSetDataBufLen=1;
				ret&=hookSmsSetParameter("NetPara",DailMode,aucSetDataBuf,wSetDataBufLen);
			}
			else
			{
				NULL;
			}

			ucIndex++;
			if( (0x01==i_ptSmsPara->tSmsPara[ucIndex].wLen) && (' '==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0]) )
			{
				if(DOMAIN_DIAL_MODE==aucSetDataBuf[0])
				{
					ret&=hookSmsSetParameter("NetPara",Domain,i_ptSmsPara->tSmsPara[ucIndex].pucPara,0);
				}
				else
				{
					ret&=hookSmsSetParameter("NetPara",Ip,i_ptSmsPara->tSmsPara[ucIndex].pucPara,0);
				}
			}
			else
			{
				if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
				{
					if(DOMAIN_DIAL_MODE==aucSetDataBuf[0])
					{
						ret&=hookSmsSetParameter("NetPara",Domain,i_ptSmsPara->tSmsPara[ucIndex].pucPara,i_ptSmsPara->tSmsPara[ucIndex].wLen);
					}
					else
					{
						ret&=hookSmsSetParameter("NetPara",Ip,i_ptSmsPara->tSmsPara[ucIndex].pucPara,i_ptSmsPara->tSmsPara[ucIndex].wLen);				
					}
				}
				else
				{
					NULL;
				}
			}
			 			
			ucIndex++;
			if( (0x01==i_ptSmsPara->tSmsPara[ucIndex].wLen) && (' '==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0]) )
			{
				ret&=hookSmsSetParameter("NetPara",Port,aucSetDataBuf,0);
			}
			else
			{
				if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
				{
					wPort=atoi((char const *)i_ptSmsPara->tSmsPara[ucIndex].pucPara);
					LsbU16To2U8(aucSetDataBuf,wPort);
					wSetDataBufLen=2;
					ret&=hookSmsSetParameter("NetPara",Port,aucSetDataBuf,wSetDataBufLen);
				}
				else
				{
					NULL;
				}
			}
			
			ucIndex++;
			if( (0x01==i_ptSmsPara->tSmsPara[ucIndex].wLen) && (' '==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0]) )
			{
				ret&=hookSmsSetParameter("NetPara",Apn,i_ptSmsPara->tSmsPara[ucIndex].pucPara,0);				
			}
			else
			{
				if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
				{
					ret&=hookSmsSetParameter("NetPara",Apn,i_ptSmsPara->tSmsPara[ucIndex].pucPara,i_ptSmsPara->tSmsPara[ucIndex].wLen); 			
				}
				else
				{
					NULL;
				}
			}
			
			ucIndex++;
			if( (0x01==i_ptSmsPara->tSmsPara[ucIndex].wLen) && (' '==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0]) )
			{
				ret&=hookSmsSetParameter("NetPara",UsrName,i_ptSmsPara->tSmsPara[ucIndex].pucPara,0);				
			}
			else
			{
				if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen) )
				{
					ret&=hookSmsSetParameter("NetPara",UsrName,i_ptSmsPara->tSmsPara[ucIndex].pucPara,i_ptSmsPara->tSmsPara[ucIndex].wLen); 			
				}
				else
				{
					NULL;
				}
			}
			
			ucIndex++;
			if( (0x01==i_ptSmsPara->tSmsPara[ucIndex].wLen-1) && (' '==i_ptSmsPara->tSmsPara[ucIndex].pucPara[0]) )
			{
				ret&=hookSmsSetParameter("NetPara",Pwd,i_ptSmsPara->tSmsPara[ucIndex].pucPara,0);	 		
			}
			else
			{
				if( IS_TRUE_BIT(i_ptSmsPara->tSmsPara[ucIndex].wLen-1) )//去掉#号	
				{
					ret&=hookSmsSetParameter("NetPara",Pwd,i_ptSmsPara->tSmsPara[ucIndex].pucPara,i_ptSmsPara->tSmsPara[ucIndex].wLen-1);	//去掉#号			
				}
				else
				{
					NULL;
				}
			}
 		}
	}
	memcpy(pucSendBuf,"*name,setr;<GPSTracker://setr;gprs-para,",strlen("*name,setr;<GPSTracker://setr;gprs-para,"));
	pucSendBuf+=strlen("*name,setr;<GPSTracker://setr;gprs-para,");
	if(FALSE==ret)
	{
		memcpy(pucSendBuf,"fail",strlen("fail"));
		pucSendBuf+=strlen("fail");
	}
	else
	{
		memcpy(pucSendBuf,"ok",strlen("ok"));
		pucSendBuf+=strlen("ok");
		M_SetBit(g_ucSetNetParaNeedResetFlag);
	}
	memcpy(pucSendBuf,">#",strlen(">#"));
	pucSendBuf+=strlen(">#");
	*o_wLen=(pucSendBuf-o_pucBuf);
	ret=TRUE;
	return ret;
}

/****************************回调底层函数*****************************/

/*****************************************************************************
-Fuction		: hookSmsDeInit
-Description	: hookSmsDeInit  
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static void hookSmsDeInit()
{
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead SetParameter Null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->ParaDeInitSupport();
			ptTmp = ptTmp->ptNext;
		}
	}	
}
/*****************************************************************************
-Fuction		: hookSmsGetData
-Description	: hookSmsGetData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsGetData(T_SmsInfo *o_ptSmsInfo)
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead GetData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GetDataSupport(o_ptSmsInfo);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookSmsSendData
-Description	: hookSmsSendData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsSendData(u8 *i_PhoneNumBuf,u8 *i_ucBuf, u16 i_wLen)
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead SendData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->SendDataSupport(i_PhoneNumBuf,i_ucBuf,i_wLen);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}

/*****************************************************************************
-Fuction		: hookSmsSetParameter
-Description	: hookSmsSetParameter
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsSetParameter(char *i_Name,ParameterEnum i_SubName,u8 *i_ucBuf,u16 i_wLen)
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead SetParameter Null\r\n");
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
-Fuction		: hookSmsGetParameter
-Description	: hookSmsGetParameter
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsGetParameter(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len)
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead GetParameter Null\r\n");
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
-Fuction		: hookSmsGetIMEL
-Description	: hookSmsGetIMEL
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsGetIMEL(u8 *o_ucBuf,u16 *o_wLen)
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead GetIMEL Null\r\n");
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
/*****************************************************************************
-Fuction		: hookSmsSendAlarmData
-Description	: hookSmsSendAlarmData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsSendAlarmData(u8 *i_ucBuf, u16 i_wLen)
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead SendAlarmData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->SendAlarmDataSupport(i_ucBuf,i_wLen);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookSmsSendAlarmData
-Description	: hookSmsSendAlarmData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsRestartSystem()
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead SendData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->RestartSystemSupport();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookSmsSendAlarmData
-Description	: hookSmsSendAlarmData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsGetGpsLocationInfo(LocationVectorInfo *o_ptGpsLocationInfo)
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead SendData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GetGpsLocationInfoSupport(o_ptGpsLocationInfo);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookSmsSendAlarmData
-Description	: hookSmsSendAlarmData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsLock()
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead SendData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->LockSupport();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookSmsSendAlarmData
-Description	: hookSmsSendAlarmData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsUnLock()
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead SendData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->UnlockSupport();
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookSmsSendAlarmData
-Description	: hookSmsSendAlarmData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsTimeDelay(u16 i_wSecond,u16 i_wMilliSecond)
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead SendData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->TimeDelaySupport(i_wSecond,i_wMilliSecond);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookSmsSendAlarmData
-Description	: hookSmsSendAlarmData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static u8 hookSmsGetGsmCsq(u8 *o_pucCsq)
{
	u8 ret=FALSE;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead SendData Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->GetCsqSupport(o_pucCsq);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;	
}
/*****************************************************************************
-Fuction		: hookSmsSendAlarmData
-Description	: hookSmsSendAlarmData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/09/18     V1.0.0		Yu Weifeng 	Created
******************************************************************************/
static s16 hookSmsPduEncode( u8  *pPdu, u8  *pSCA, u8  *pDA, u8  *pUD, s16 iUDL, s16 iDCS )
{
	u16 wDataLen=0;
	PT_SmsProtocolSupport ptTmp = g_ptSmsProtocolSupportHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"SmsProtocolSupportHead SendData Null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			wDataLen=ptTmp->PduEncodeSupport( pPdu, pSCA, pDA, pUD, iUDL, iDCS );
			ptTmp = ptTmp->ptNext;
		}
	}
	return wDataLen;	
}
/************************注册底层支持函数***************************/

/*****************************************************************************
-Fuction		: RegisterSmsProtocolSupport
-Description	: RegisterSmsProtocolSupport
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/18    V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterSmsProtocolSupport(PT_SmsProtocolSupport i_ptSmsProtocolSupport)
{
	PT_SmsProtocolSupport ptTmp;
	if (!g_ptSmsProtocolSupportHead)
	{
		g_ptSmsProtocolSupportHead   = i_ptSmsProtocolSupport;
		g_ptSmsProtocolSupportHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptSmsProtocolSupportHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptSmsProtocolSupport;
		i_ptSmsProtocolSupport->ptNext = NULL;
	}
}
