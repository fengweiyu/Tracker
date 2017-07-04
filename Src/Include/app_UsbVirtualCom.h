/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_RTC.c
* Description		: 	RTC application operation center
* Created			: 	2016.08.29.
* Author			: 	Yu Weifeng
* Function List 		: 	RtcDev
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_USB_VIRTUAL_COM_H
#define _APP_USB_VIRTUAL_COM_H
#include "cBasicTypeDefine.h"

#define USB_VIRTUAL_COM_BAUDRATE  		115200
#define USB_VIRTUAL_COM_RECV_SIZE   		1100

typedef struct UsbVirtualComDevManage
{
	char *name;
	void (*DevConfig)(void);
	void (*DevDeConfig)(void);
	void (*DevVbusSetFallExti)(void);
	u8 (*DevSendData)(u8 *i_buf,u16 i_Len); 
	u8 (*DevRecvData)(u8 *i_buf,u16 i_Len); 
	u8 (*DevGetData)(u8 *o_buf,u16 *o_Len);	
	u8 (*DevGetUsbState)(u8 *o_pucUsbState);
	struct UsbVirtualComDevManage *ptNext;
}T_UsbVirtualComDevManage,*PT_UsbVirtualComDevManage;

void RegisterUsbVirtualComDev(PT_UsbVirtualComDevManage i_ptUsbVirtualComDev);
void UsbVirtualComDevInit();


#endif
