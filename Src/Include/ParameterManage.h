/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 ParameterManage.c
* Description		: 	ParameterManage application operation center
* Created			: 	2016.9.7.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef 	_PARAMETER_MANAGE_H
#define	_PARAMETER_MANAGE_H

#include "CBasicTools.h"
#include "Config.h"

#define APN_STR_LEN		            		(50)
#define USERNAME_STR_LEN	            	(20)//与GSM库保持一致
#define PASSWORD_STR_LEN	            	(20)//与GSM库保持一致
#define DOMAIN_STR_LEN              		(50)
#define IP_STR_LEN		            		(15)
#define PORT_STR_LEN	            		(5)


#define DEVICE_ID_LEN                  		(20)
#define PHONE_STR_LEN	            		(21)
#define SMS_SECRET_KEY_LEN		(6)

#define SOFTWARE_VISION_LEN		(20)
#define HAEDWARE_VISION_LEN		(20)

#define CHINESE_LANGUAGE			(1)
#define ENGLISH_LANGUAGE			(0)

#define DOMAIN_DIAL_MODE			(1)
#define IP_DIAL_MODE				(0)
/*********************************网络参数********************************/
typedef struct NetPara
{
	u8 Ip[IP_STR_LEN +1];
	u8 Domain[DOMAIN_STR_LEN + 1];	
	u8 Apn[APN_STR_LEN +1];
	u8 UsrName[USERNAME_STR_LEN +1];
	u8 Pwd[PASSWORD_STR_LEN +1];
	u8 DailMode;//=0x01：域名拨号=0x00：IP拨号	
	u16 Port;
	u8 Reserver[29]; //字节对齐
}T_NetPara,*PT_NetPara;
/*********************************短信参数********************************/
typedef struct SmsPara
{
	u8 SimCardNum[PHONE_STR_LEN +1];
	u8 CenterNum[PHONE_STR_LEN +1];
	u8 EmergencyContactNum[3][PHONE_STR_LEN +1];
	u8 DeviceID[DEVICE_ID_LEN + 1];
	s8 SmsSecretKey[SMS_SECRET_KEY_LEN]; 
	u8 Reserver[31]; //4字节对齐
}T_SmsPara,*PT_SmsPara;
/*********************************通用参数********************************/
typedef struct CommonPara
{
	u32 ReportInterval;	
	u8 TrackerLanguage;// 0: 英文，1:中文
	char SoftWareVision[SOFTWARE_VISION_LEN+1];
	char HardWareVision[HAEDWARE_VISION_LEN+1];	
	u8 Reserver[29]; //4//4字节对齐
}T_CommonPara,*PT_CommonPara;

/*********************************报警参数********************************/
typedef struct AlarmPara
{
	u8 LowPowerAlarmSwitch;
	u8 DropTrackerAlarmSwitch;
	u8 CutTrackerAlarmSwitch;
	u8 SmsAlarmSwitch;
	u8 Reserver[12]; 
}T_AlarmPara,*PT_AlarmPara;
/**********************所有参数用于读取与保存*********************/
typedef struct AllSysPara
{
	T_NetPara tNetPara;
	T_AlarmPara tAlarmPara;
	T_SmsPara tSmsPara;
	T_CommonPara tCommonPara;
	u8 Reserver[16]; 
	u32 dwCrcForFlash;
}T_AllSysPara,*PT_AllSysPara;
/*********************************参数选择********************************/
typedef enum ParameterEnum
{
	Ip=0,
	Port,
	Apn,
	UsrName,
	Pwd,
	Domain,
	DailMode,
	SimCardNum,
	CenterNum,
	SmsSecretKey,
	EmergencyContactNum1,
	EmergencyContactNum2,
	EmergencyContactNum3,
	DeviceID,
	ReportInterval,
	TrackerLanguage,
	SoftWareVision,
	HardWareVision,
	LowPowerAlarmSwitch,
	DropTrackerAlarmSwitch,
	CutTrackerAlarmSwitch,
	SmsAlarmSwitch,
}ParameterEnum;


/*********************************管理函数********************************/
typedef struct ParameterManage
{
	char *name;
	u8 (*ParaOprInit)(T_AllSysPara * i_ptAllSysPara);
	void (*ParaOprDeInit)(void);
	u8 (*SetParaOpr)(ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len);
	u8 (*GetParaOpr)(ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);
	struct ParameterManage *ptNext;
}T_ParameterManage,*PT_ParameterManage;

/*********************************操作函数********************************/
typedef struct ParameterOpr
{
	void (*RegisterPara)(void);
	u8 (*ParaInit)(void);
	void (*ParaDeInit)(void);	
	void (*ParaDeInitExceptNetPara)(void);
	u8 (*SetPara)(char *i_Name,ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len);
	u8 (*GetPara)(char *i_Name,ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);
}T_ParameterOpr,*PT_ParameterOpr;

extern T_ParameterOpr const g_tParameterOpr;
extern T_AllSysPara g_tAllSysPara;

void RegisterParaOpr(PT_ParameterManage i_ptParaOpr);
void RegisterNetPara();
void RegisterSmsPara();
void RegisterAlramPara();
void RegisterCommonPara();

#endif
