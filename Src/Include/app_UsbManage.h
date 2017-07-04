/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_RTC.c
* Description		: 	RTC application operation center
* Created			: 	2016.08.29.
* Author			: 	Yu Weifeng
* Function List 		: 	RtcTimeInit
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_USB_MANAGE_H
#define _APP_USB_MANAGE_H

#include"cBasicTools.h"

#define USB_IS_CONNECT_STATE			(0)
#define USB_IS_NOT_CONNECT_STATE	(1)

typedef struct UsbManage
{
	char *name;
	u8 (*UsbConfig)(void);	
	u8 (*UsbDeConfig)(void);
	u8 (*UsbVbusSetFallExti)(void);
	u8 (*UsbSendData)(u8 *i_buf,u16 i_Len);
	u8 (*UsbRecvData)(u8 *i_buf,u16 i_Len);
	u8 (*UsbGetData)(u8 *o_buf,u16 *o_Len);	
	u8 (*UsbGetState)(u8 *o_pucUsbState);
	struct UsbManage *ptNext;
}T_UsbManage,*PT_UsbManage;

typedef struct UsbOpr
{
	void (*Init)(void);
	u8 (*Config)(void);	
	u8 (*DeConfig)(void);
	u8 (*VbusSetFallExti)(void);
	u8 (*SendDataBuf)(u8 *i_buf,u16 i_Len);
	u8 (*RecvDataBuf)(u8 *i_buf,u16 i_Len);
	u8 (*GetDataBuf)(u8 *o_buf,u16 *o_Len);
	u8 (*DbgPrintf)(const char *pcFormat, ...);	
	u8 (*DbgSend)(u8 *i_buf,u16 i_Len);
	u8 (*GetUsbState)(u8 *o_pucUsbState);
	u8 (*GetUsbOpr)(char *i_pcName,PT_UsbManage o_ptUsbOpr);
}T_UsbOpr,*PT_UsbOpr;



extern T_UsbOpr const g_tUsb;
void RegisterUsb(PT_UsbManage i_ptUsbOpr);
void UsbVirtualComInit();


#endif

