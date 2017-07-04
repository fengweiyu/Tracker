/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 TrackerProtocol.c
* Description		: 	TrackerProtocol  operation center
* Created			: 	2016.09.09.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef	_TRACK_PROTOCOL_H	
#define	_TRACK_PROTOCOL_H

#include"cBasicTools.h"
#include "ProtocolManage.h"
#include "ParameterManage.h"
#include "Alarm.h"
#include "app_Gsm_Oper.h"

#define TRACKER_PROTOCOL_VERSION								(19)
#define TRACKER_PROTOCOL_MIN_RECV_LEN						(25)
#define TRACKER_PROTOCOL_DEVICE_ID_OFFSET						(5)
#define TRACKER_PROTOCOL_BASE_LEN								(31)//HEADER 2,len 2, VISION1,ID20,cmd id 2, checksum 2 LAST2
#define TRACKER_PROTOCOL_TLV_SET_BASE_LEN					(7)//seq 2 tagCount 1 tlv 2 tlvLen 2
#define TRACKER_PROTOCOL_TLV_GET_BASE_LEN					(5)//seq 2 tagCount 1 tlv 2
#define TRACKER_PROTOCOL_CMD_OFFSET							(25)
#define TRACKER_PROTOCOL_DATA_OFFSET							(27)
#define TRACKER_PROTOCOL_FIX_UPLOAD_OFFSET					(29)
#define TRACKER_PROTOCOL_CMD_SEQ_LEN							(2)
#define TRACKER_PROTOCOL_CMD_SUCCESS_LEN					(1)
#define TRACKER_PROTOCOL_SUBCMD_LEN							(2)
#define TRACKER_PROTOCOL_SUBCMD_DATA_LEN					(2)
#define TRACKER_PROTOCOL_SUBCMD_OFFSET						(3)
#define TRACKER_PROTOCOL_UPDATE_LAST_PACK_FLAG_OFFSET		(4)
#define TRACKER_PROTOCOL_UPDATE_PACK_INDEX_OFFSET			(5)
#define TRACKER_PROTOCOL_UPDATE_PACK_LEN_OFFSET			(7)
#define TRACKER_PROTOCOL_UPDATE_PACK_INFO_OFFSET			(9)

#define TRACKER_PROTOCOL_MAX_RECV_SMS_COUNT				(1)
#define TRACKER_PROTOCOL_MAX_LEN								(1100)//升级一次1k
#define TRACKER_PROTOCOL_LOCATION_INFO_MAX_LEN				(256)
#define TRACKER_PROTOCOL_ACK_MAX_LEN							(360)
#define TRACKER_PROTOCOL_MIN_ALARM_ACK_LEN					(5)
#define TRACKER_PROTOCOL_ALARM_STATE_LEN					(4)
#define TRACKER_PROTOCOL_MAX_SMS_LEN							(141)
#define TRACKER_PROTOCOL_LOCATION_INFO_SAVE_MAX_LEN		(TRACKER_PROTOCOL_LOCATION_INFO_MAX_LEN)
#define TRACKER_PROTOCOL_HISTORY_LOCATION_INFO_PACK_NUM	(4)//超过4则最大长度1k放不下
#define TRACKER_PROTOCOL_SYSTEM_LOG_PACK_NUM				(5)//超过5则最大长度320放不下

#define TRACKER_PROTOCOL_CUT_TRACKER_STATE					(0x02)
#define TRACKER_PROTOCOL_DROP_TRACKER_STATE				(0x01)
#define TRACKER_PROTOCOL_LOW_POWER_STATE					(0x04)
#define TRACKER_PROTOCOL_FIRST									('@')
#define TRACKER_PROTOCOL_SECOND								('@')
#define TRACKER_PROTOCOL_SECOND_LAST							(0x0D)
#define TRACKER_PROTOCOL_LAST									(0x0A)

#define TRACKER_PROTOCOL_LOGIN_CMD				(0x4001)
#define TRACKER_PROTOCOL_LOGOUT_CMD				(0x4002)
#define TRACKER_PROTOCOL_HEAET_BEAT_CMD			(0x4003)
#define TRACKER_PROTOCOL_REPORT_ALARM_CMD		(0x4203)
#define TRACKER_PROTOCOL_REPORT_CMD				(0x4206)

#define TRACKER_PROTOCOL_READ_SYSTEM_LOG_ACK	(0x8602)
#define TRACKER_PROTOCOL_UPDATE_REQUEST_ACK	(0x4222)
#define TRACKER_PROTOCOL_UPDATE_PACK_ACK		(0x4223)
#define TRACKER_PROTOCOL_CALL_THE_ROLL_ACK		(0x8101)
#define TRACKER_PROTOCOL_RESET_ACK				(0x8581)
#define TRACKER_PROTOCOL_DEFAULT_INIT_ACK		(0x8582)
#define TRACKER_PROTOCOL_SET_CMD_ACK				(0x9101)
#define TRACKER_PROTOCOL_GET_CMD_ACK				(0x9201)



#define TRACKER_PROTOCOL_MAX_ACCESS_POINT_NUM				(AT_WIFI_AP_NUM)
#define TRACKER_PROTOCOL_ENOUGH_AP_NUM						(3)
#define TRACKER_PROTOCOL_MAX_CELL_TOWER_NUM				(MAX_CELL_TOWER_NUM)
#define TRACKER_PROTOCOL_CELL_TOWER_INFO_LEN				(16)
#define TRACKER_PROTOCOL_IMEL_LEN								(GSM_IMEI_LEN)

#define TRACKER_PROTOCOL_SOFTWARE_UPDATE_RECV_FAIL			(0x00)
#define TRACKER_PROTOCOL_SOFTWARE_UPDATE_RECV_SUCCESS		(0x01)
#define TRACKER_PROTOCOL_SOFTWARE_SN_LEN						(16)
#define TRACKER_PROTOCOL_SOFTWARE_CRC_START					(FCS_START)
#define TRACKER_PROTOCOL_SOFTWARE_UPDATE_CANCEL				(0x00)
#define TRACKER_PROTOCOL_SOFTWARE_UPDATE_START			    	(0x01)
#define TRACKER_PROTOCOL_SOFTWARE_UPDATE_SUCCESS		        	(0x02)
#define TRACKER_PROTOCOL_SOFTWARE_UPDATE_CRC_ERROR			(0x03)
#define TRACKER_PROTOCOL_SOFTWARE_UPDATE_CONTENT_ERROR		(0x04)
#define TRACKER_PROTOCOL_SOFTWARE_UPDATE_WAIT_RESET_TIME  	(10*OS_TICKS_PER_SEC)//10S
typedef struct SoftwarePackInfo
{
    u8    SN[TRACKER_PROTOCOL_SOFTWARE_SN_LEN];
    u32  ID;
    u16  PackageNum;
    u16  CurrentRecvPackageNum;
    u32  PackageLen;					//32K包长计数//
    u16  CrcNum;					//总包的CRC个数//
    u16  CrcIndex;					//CRC的序号//
    u16  CurrentCrc;					//当前32K包的CRC//
    u16  Crc[ 8 ];					//服务器给的所有的CRC//
    u8    UpdateStartFlag;
    u8    UpdateEndFlag;
    u8    UpdateFirstPackFlag;		//防止多次升级请求导致最后一次升级发很多的第一包
}T_SoftwarePackInfo,*PT_SoftwarePackInfo;

typedef struct TrackerProtocolHandleMenu
{
	u16 Cmd;
	u8   (*ProtocolHandle)(u8 i_ucProtocolUser,u8 *i_ucBuf,u16 i_wLen,T_HandleCmdAck *o_ptHandCmdAck);
}T_TrackerProtocolHandleMenu,*PT_TrackerProtocolHandleMenu;

typedef struct TrackerProtocolSetHandleMenu
{
	u16 Cmd;
	u8   (*ProtocolHandle)(u8 *i_ucBuf,u16 *o_wProcessedLen);
}T_TrackerProtocolSetHandleMenu,*PT_TrackerProtocolSetHandleMenu;
typedef struct TrackerProtocolGetHandleMenu
{
	u16 Cmd;
	u8   (*ProtocolHandle)(u8 *o_ucBuf,u16 *o_wLen);
}T_TrackerProtocolGetHandleMenu,*PT_TrackerProtocolGetHandleMenu;



typedef struct TrackerProtocolSupport
{
	char *name;
	void (*ParaDeInitSupport)(u8 i_ucProtocolUser);
	u8 (*GetDataSupport)(u8 i_ucProtocolUser,u8 *o_buf,u16 *o_Len);
	u8 (*SendDataSupport)(u8 i_ucProtocolUser,u8 *i_buf,u16 i_Len);
	u8 (*SetParaSupport)(char *i_Name,ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len);
	u8 (*GetParaSupport)(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);
	u8 (*RelieveAlarmSupport)(T_Alarm i_tAlarm);
	u8 (*GetImelSupport)(u8 *o_buf,u16 *o_Len);
	struct TrackerProtocolSupport *ptNext;
}T_TrackerProtocolSupport,*PT_TrackerProtocolSupport;

void RegisterTrackerProtocolSupport(PT_TrackerProtocolSupport i_ptTrackerProtocolSupport);

void TrackerProtocolSupportInit();

#endif
