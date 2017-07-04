/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 BSP
* Description		: 	BSP.h
* Created			: 	2016.09.20.
* Author			: 	Yu Weifeng
* Function List 		: 	none
* Last Modified 	: 	
* History			: 	
******************************************************************************/

#ifndef _BSP_H
#define _BSP_H
#include "CBasictools.h"
#include "Ucos_ii.h"
#include "Myqueue.h"

void SysInit();
void BaseSupportInit();
void SystemReset(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen);
void IWDG_Feed(void);
void DebugStopPeripheral();
void SystemEnterStopMode();
void SystemEnterSleepMode();
void SystemEnterStandByMode();
u8 DevOsRxQWrite(OS_EVENT *pstOsQ, myevent *event);
u8 DevOsRxQReadPend(OS_EVENT *pstOsQ, myevent **event, u16 u16PendTimeMs);
u16 GetDurationSecTimer(u32 *pdwBackupTime);
void SetRtcAlarmTime(u16 i_wDataTime);
void UsbResetSystemFunction();
extern   char *g_pcLogo;

#endif
