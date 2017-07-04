/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_Gsm.c
* Description		: 	app_Gsm application operation center
* Created			: 	2016.09.27.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_GSM_H
#define _APP_GSM_H
#include "cBasicTypeDefine.h"
#include "SmsProtocol.h"
#include "app_gsm_Oper.h"
#include "GsmDriverTypeDef.h"

#define GSM_USART_BAUDRATE  115200
#define USART2_USART_RECV_SIZE   1
#define MAX_GSM_CELL_TOWER_NUM				(MAX_CELL_TOWER_NUM)

typedef struct GeneralGsmDevManage
{
	char *name;
	void (*GeneralGsmConfigDev)(void);
	u8 (*GeneralGsmSendDataDev)(u8 *i_buf,u16 i_Len);
	u8 (*GeneralGsmRecvDataDev)(void);	
	u8 (*GeneralGsmCheckGsmStateDev)(void);
	void (*GeneralGsmResetDev)();
	void(*GeneralGsmPowerOnDev)(void);
	void(*GeneralGsmPowerOffDev)(void);	
	void(*GeneralGsmWakeUpDev)(void);
	void(*GeneralGsmEnterSleepModeDev)(void);
	struct GeneralGsmDevManage *ptNext;
}T_GeneralGsmDevManage,*PT_GeneralGsmDevManage;
typedef struct GsmOpr
{
	void (*Init)(void);
	void (*Config)(void);
	u8 (*GsmSendData)(u8 *i_buf,u16 i_Len);
	u8 (*GsmRecvData)(void);
	u8(*GsmPowerOn)(void);
	u8(*GsmPowerOff)(void);	
	u8 (*GsmReset)();	
	u8 (*GsmWriteGprsData)(u8 *i_pucDataBuf,u16 i_wDataLen);	
	u8(*GsmReadGprsData)(u8 *o_pucDataBuf,u16 *o_wDataLen);
	u8 (*GsmWriteSmsData)(sms_info_t *i_ptDataBuf,u16 i_wPDCS);
	u8 (*GsmReadSmsData)(T_SmsInfo *o_ptSmsInfo);
	u8 (*GsmSetLbsLocationUpdateReq)(void);	
	u8 (*GsmReadLbsLocationData)(CellTowerInfoSt *o_ptCellTowerInfo);	
	u8 (*GsmCheckGsmState)(void);	
	u8(*GsmWakeUp)(void);	
	u8 (*GsmEnterSleepMode)(void);	
}T_GsmOpr,*PT_GsmOpr;

void RegisterGeneralGsmDev(PT_GeneralGsmDevManage i_ptGeneralGsmDev);
void GeneralGsmDevInit();

extern T_GsmOpr const g_tGsm; 
#endif
