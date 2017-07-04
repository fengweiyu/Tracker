/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 ProtocolManage.c
* Description		: 	ProtocolManage application operation center
* Created			: 	2016.9.7.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef 	_PROTOCOL_MANAGE_H
#define	_PROTOCOL_MANAGE_H

#include "CBasicTools.h"
#include "Alarm.h"
#define PROTOCOL_USED_FOR_VCP						(0)
#define PROTOCOL_USED_FOR_GPRS						(1)
#define PROTOCOL_USED_FOR_SMS						(2)

typedef struct HandleCmdAck
{
	u8 ucNeedSendDataFlag;	
	u8 ucNeedSendDataAgainFlag;
	u16 wAckCmd;
	u8 *pucDataBuf;
	u16 wDataLen;
	u16 wMaxDataLen;
	
	u8 *pucDataSendBuf;
	u16 wSendLen;
}T_HandleCmdAck,*PT_HandleCmdAck;

typedef struct ProtocolManage
{
	const char *name;
	u8 (*ProtocolGetData)(u8 i_ucProtocolUser,u8 *o_pucDataBuf,u16 *o_pwDataLen,u16 i_wDataBufMaxLen);
	u8 (*ProtocolCheckData)(u8 *i_pucDataBuf,u16 i_wDataLen);
	u8 (*ProtocolAnalyseData)(u8 i_ucProtocolUser,u8 *i_pucDataBuf,u16 i_wDataLen,T_HandleCmdAck *o_ptHandCmdAck);
	u8 (*ProtocolReportAlarm)(T_Alarm i_tAlarm,u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen);
	u8 (*ProtocolLogin)(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen);
	u8 (*ProtocolHeartBeat)(u8 *o_pucDataSendBuf,u16 *o_wSendLen);
	u8 (*ProtocolLogout)(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen);
	u8 (*ProtocolReport)(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen);
	struct ProtocolManage *ptNext;
}T_ProtocolManage,*PT_ProtocolManage;
typedef struct ProtocolOpr
{
	void (*Init)(void);
	u8 (*GetData)(u8 i_ucProtocolUser,u8 *o_pucDataBuf,u16 *o_pwDataLen,u16 i_wDataBufMaxLen,const char *i_pcName);
	u8 (*CheckData)(u8 *i_pucDataBuf,u16 i_wDataLen,const char *i_pcName);
	u8 (*AnalyseData)(u8 i_ucProtocolUser,u8 *i_pucDataBuf,u16 i_wDataLen,T_HandleCmdAck *o_ptHandCmdAck,const char *i_pcName);
	u8 (*GetProtocolOpr)(const char *i_pcName,PT_ProtocolManage *o_ptPtotocolOpr);
}T_ProtocolOpr,*PT_ProtocolOpr;

extern T_ProtocolOpr const g_tProtocol;
void RegisterProtocol(PT_ProtocolManage i_ptProtocol);
void TrackerProtocolInit();
void SmsProtocolInit();

#endif

