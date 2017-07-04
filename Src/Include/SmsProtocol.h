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
#ifndef	_SMS_PROTOCOL_H	
#define	_SMS_PROTOCOL_H
#include "cBasicTools.h"
#include "Config.h"
#include "Ucos_ii.h"
#include "ProtocolManage.h"
#include "GsmDriverTypeDef.h"
#include "ParameterManage.h"
#include "Pdu.h"
#include "app_GpsManage.h"
#include "app_TimeManage.h"

#define SMS_PROTOCOL_MAX_LEN 						(281)
#define SMS_PROTOCOL_MAX_SEND_LEN 					(141)
#define SMS_PROTOCOL_MAX_PARA_NUM				(13)
#define SMS_PROTOCOL_MAX_CMD_LEN					(32)
#define SMS_PROTOCOL_CMD_LOCATION					(1)
#define SMS_PROTOCOL_PARA_START					(SMS_PROTOCOL_CMD_LOCATION+1)
#define SMS_PROTOCOL_FORMAT_DELIMITER			','
#define SMS_PROTOCOL_LINK_ADDR_SYMBOL			'!'
#define SMS_PROTOCOL_SET_SUCCESS_STR				"ok"
#define SMS_PROTOCOL_MAX_REPORT_INTERVAL_LEN	(5)
#define SMS_PROTOCOL_MAX_PORT_LEN					(5)
#define SMS_PROTOCOL_PDU7_ENCODE					(0)
#define SMS_PROTOCOL_PDU8_ENCODE					(4)
#define SMS_PROTOCOL_UCS2_ENCODE					(8)
#define SMS_PROTOCOL_PHONE_NUM_LEN				(GSM_PHONE_STR_LEN+1)

#define SMS_PROTOCOL_CHINESE_LANGUAGE			(CHINESE_LANGUAGE)
#define SMS_PROTOCOL_ENGLISH_LANGUAGE			(ENGLISH_LANGUAGE)
#define SMS_PROTOCOL_POSITION_INFO_UNIT			(1000000)

typedef struct SmsParameter
{
	u8 *pucPara;
	u16 wLen;
}T_SmsParameter,*PT_SmsParameter;
typedef struct SmsParaFormat
{
	u8 ucParaNum;
	T_SmsParameter tSmsPara[SMS_PROTOCOL_MAX_PARA_NUM];
}T_SmsParaFormat,*PT_SmsParaFormat;
typedef struct SmsInfo
{
	u8 strPhoneNum[SMS_PROTOCOL_PHONE_NUM_LEN];
	u8 aucSmsContent[SMS_PROTOCOL_MAX_LEN];
	u16 wSmsContentLen;
	u16 wPDCS;
}T_SmsInfo,*PT_SmsInfo;
typedef struct SmsProtocolHandleMenu
{
	const char *pStr; 
	u8 ucDelimiterNum; //Delimiter : fixed by protocol
	u8   (*ProtocolHandle)(PT_SmsParaFormat i_pSmsPara,u8 *o_pucBuf,u16 *o_wLen);
}T_SmsProtocolHandleMenu,*PT_SmsProtocolHandleMenu;



typedef struct SmsProtocolSupport
{
	char *name;
	void (*ParaDeInitSupport)(void);
	u8 (*GetDataSupport)(T_SmsInfo *o_ptSmsInfo);
	u8 (*SendDataSupport)(u8 *i_PhoneNumBuf,u8 *i_buf,u16 i_Len);
	u8 (*SetParaSupport)(char *i_Name,ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len);
	u8 (*GetParaSupport)(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);
	u8 (*GetImelSupport)(u8 *o_buf,u16 *o_Len);	
	u8 (*SendAlarmDataSupport)(u8 *i_buf,u16 i_Len);
	u8 (*RestartSystemSupport)();	
	u8 (*GetGpsLocationInfoSupport)(LocationVectorInfo *o_ptGpsLocationInfo);
	u8 (*LockSupport)();	
	u8 (*UnlockSupport)();	
	u8 (*TimeDelaySupport)(u16 i_wSecond,u16 i_wMilliSecond);		
	u8 (*GetCsqSupport)(u8 *o_pucCsq);
	s16 (*PduEncodeSupport)( u8  *pPdu, u8  *pSCA, u8  *pDA, u8  *pUD, s16 iUDL, s16 iDCS );
	struct SmsProtocolSupport *ptNext;
}T_SmsProtocolSupport,*PT_SmsProtocolSupport;

void RegisterSmsProtocolSupport(PT_SmsProtocolSupport i_ptSmsProtocolSupport);

void SmsProtocolSupportInit();


#endif
