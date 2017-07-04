/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_RTC.c
* Description		: 	RTC Drive operation center
* Created			: 	2016.08.29.
* Author			: 	Yu Weifeng
* Function List 		: 	RtcTimeInit
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "stm32f4xx_hal.h"
#include "cBasicTypeDefine.h"
#include "usb_device.h"
#include "app_UsbVirtualCom.h"
#include "usbd_def.h"
#include "Config.h"
#include "Ucos_ii.h"
#include "Main.h"
#include "TrackerProtocol.h"

static void UsbVbusDevConfig(void);
static void UsbVirtualComDevConfig(void);
static void UsbVirtualComDevDeConfig(void);
static void SetFallExti9Dev();
static u8 SendDataDev(u8 *i_buf,u16 i_Len);
static u8 RecvDataDev(u8 *i_buf,u16 i_Len);
static u8 GetDataDev(u8 *o_buf,u16 *o_Len);
static u8 GetUsbStateDev(u8 *o_pucUsbState);

uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);



static u8 UsbVirtualComRecvDatabuf[USB_VIRTUAL_COM_RECV_SIZE]={0};
static u8 ReceiveRightUsbDataCompleteFlag=FALSE;
static u8 g_ucUsbIsConfigOk=FALSE;
static u16 g_wRecvUsbDataLen=0;
static u16 g_wPackageDataLen=0;
static T_UsbVirtualComDevManage g_tUsbVirtualComDev ={
	.name="UsbVirtualCom",
	.DevConfig		=UsbVirtualComDevConfig,
	.DevDeConfig		=UsbVirtualComDevDeConfig,
	.DevVbusSetFallExti	=SetFallExti9Dev,
	.DevSendData		=SendDataDev,
	.DevRecvData		=RecvDataDev,
	.DevGetData    	=GetDataDev,
	.DevGetUsbState	=GetUsbStateDev
};

/*****************************************************************************
-Fuction		: UsbVirtualComDevInit
-Description	: UsbVirtualComDevInit
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void UsbVirtualComDevInit()
{
	RegisterUsbVirtualComDev(&g_tUsbVirtualComDev);
	UsbVbusDevConfig();
}

/*****************************************************************************
-Fuction		: UsbVirtualComDevConfig
-Description	: UsbVirtualComDevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void UsbVbusDevConfig(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
	__HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOa时钟
    
	GPIO_Initure.Pin=GPIO_PIN_9;           //Pa9
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
	GPIO_Initure.Pull=GPIO_PULLUP;          //
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);                              //初始化 

}
/*****************************************************************************
-Fuction		: SetFallEXTIDev
-Description	: SetFallEXTIDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void SetFallExti9Dev()
{
    	GPIO_InitTypeDef GPIO_Initure;    
  	GPIO_Initure.Pin=GPIO_PIN_9;               //Pa9
    	GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    	GPIO_Initure.Pull=GPIO_PULLUP;
    	HAL_GPIO_Init(GPIOA,&GPIO_Initure);

  	HAL_NVIC_SetPriority(EXTI9_5_IRQn,3,2);       //抢占优先级为3，子优先级为2
  	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);             //使能中断线4
}

/*****************************************************************************
-Fuction		: UsbVirtualComDevConfig
-Description	: UsbVirtualComDevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void UsbVirtualComDevConfig(void)
{
  	MX_USB_DEVICE_Init();
	OSSchedLock();
	M_SetBit(g_ucUsbIsConfigOk);
	OSSchedUnlock();
	//UsbVbusDevConfig();
}
/*****************************************************************************
-Fuction		: UsbVirtualComDevDeConfig
-Description	: 停止USB降低功耗
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void UsbVirtualComDevDeConfig(void)
{
	OSSchedLock();
	M_ClrBit(g_ucUsbIsConfigOk);
	g_wPackageDataLen=0;
	OSSchedUnlock();
	MX_USB_DEVICE_DeInit();
}

/*****************************************************************************
-Fuction		: GetUsbStateDev
-Description	: GetUsbStateDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetUsbStateDev(u8 *o_pucUsbState)
{
	u8 ret=FALSE;
	*o_pucUsbState=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9); //
	ret=TRUE;
	return ret;
}
/*****************************************************************************
-Fuction		: SendDataDev
-Description	: SendDataDev  由于发送的底下自动分包，所以
实际测试超过256都没有关系
-Input			: i_buf  i_Len
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SendDataDev(u8 *i_buf,u16 i_Len)
{
	u8 ret=USBD_OK;
	u8 ucUsbIsConfigOkFlag=FALSE;
	s32 i;
	
	OSSchedLock();
	ucUsbIsConfigOkFlag=g_ucUsbIsConfigOk;
	OSSchedUnlock();
	if(IS_FALSE_BIT(ucUsbIsConfigOkFlag))
	{
	}
	else
	{		
		for(i=0;i<3;i++)
		{
			ret|=CDC_Transmit_FS(i_buf,i_Len);
			if(USBD_OK!=ret)
			{
				//OSTimeDly(1);
			}
			else
			{
				break;
			}
		}
		//OSTimeDly(1);
		if(ret!=USBD_OK)
		{
			ret=FALSE;
		}
		else
		{
			ret=TRUE;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RecvDataDev
-Description	: RecvDataDev
-Input			: i_buf	i_Len
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 RecvDataDev(u8 *i_buf,u16 i_Len)
{
	u8 ret=FALSE;
	static enum
	{
		VCP_DAT_RX_IDLE,
		VCP_DAT_RX_RXING,
		VCP_DAT_RX_WAIT_END_1,
		VCP_DAT_RX_WAIT_END_2
	}eVcpDataRx; 

	OSIntEnter();
	
	if(NULL==i_buf)
	{
		DebugPrintf(ERR"WriteVcpData err\r\n");
	}
	else
	{
		if((TRACKER_PROTOCOL_FIRST != i_buf[0])||(TRACKER_PROTOCOL_SECOND!= i_buf[1])||
			(g_wPackageDataLen!=0))
		{
		}
		else 
		{
			eVcpDataRx = VCP_DAT_RX_IDLE; 
		}
		switch(eVcpDataRx) 
		{
			case VCP_DAT_RX_IDLE: 
				if((TRACKER_PROTOCOL_FIRST != i_buf[0])||(TRACKER_PROTOCOL_SECOND != i_buf[1])||IS_TRUE_BIT(ReceiveRightUsbDataCompleteFlag))
				{
					DebugPrintf(ERR"Start VcpDataRx Format err\r\n");
				}
				else	
				{
					g_wPackageDataLen=Lsb2U8ToU16(&i_buf[2]);
					//OSSchedLock();
					memset(UsbVirtualComRecvDatabuf,0,sizeof(UsbVirtualComRecvDatabuf));
					memcpy(UsbVirtualComRecvDatabuf,i_buf,i_Len);
					g_wRecvUsbDataLen=i_Len;
					//OSSchedUnlock();	
					if((g_wRecvUsbDataLen==g_wPackageDataLen)&&
						(TRACKER_PROTOCOL_SECOND_LAST==UsbVirtualComRecvDatabuf[g_wRecvUsbDataLen-2])&&
						(TRACKER_PROTOCOL_LAST==UsbVirtualComRecvDatabuf[g_wRecvUsbDataLen-1]))
					{
						M_SetBit(ReceiveRightUsbDataCompleteFlag);
						ret=TRUE;
						g_wPackageDataLen=0;
					}
					else if(g_wRecvUsbDataLen<g_wPackageDataLen)
					{
						eVcpDataRx = VCP_DAT_RX_RXING; 
					}
					else
					{
						DebugPrintf(ERR"VcpRx too long,len:%d,AllLen:%d\r\n",g_wRecvUsbDataLen,g_wPackageDataLen);
						g_wPackageDataLen=0;
						ret=FALSE;
					}
					if(g_wPackageDataLen>sizeof(UsbVirtualComRecvDatabuf))
					{
						DebugPrintf(ERR"VcpRxTooMany,len:%d,AllLen:%d\r\n",g_wRecvUsbDataLen,g_wPackageDataLen);
						g_wPackageDataLen=0;
						ret=FALSE;
					}
					else
					{
					}
				}
				break;
			
			case VCP_DAT_RX_RXING:
				//OSSchedLock();
				memcpy(&UsbVirtualComRecvDatabuf[g_wRecvUsbDataLen],i_buf,i_Len);
				g_wRecvUsbDataLen+=i_Len;
				//OSSchedUnlock();				
				if((g_wRecvUsbDataLen==g_wPackageDataLen)&&
					(TRACKER_PROTOCOL_SECOND_LAST==UsbVirtualComRecvDatabuf[g_wRecvUsbDataLen-2])&&
					(TRACKER_PROTOCOL_LAST==UsbVirtualComRecvDatabuf[g_wRecvUsbDataLen-1]))
				{
					M_SetBit(ReceiveRightUsbDataCompleteFlag);
					ret=TRUE;
					g_wPackageDataLen=0;
					eVcpDataRx = VCP_DAT_RX_IDLE; 
				}
				else if(g_wRecvUsbDataLen<g_wPackageDataLen)
				{
					eVcpDataRx = VCP_DAT_RX_RXING; 
				}
				else
				{
					DebugPrintf(ERR"RXING VcpDataRx Format err\r\n");
					g_wPackageDataLen=0;
					ret=FALSE;
					eVcpDataRx = VCP_DAT_RX_IDLE; 
				}
				break;
				
		}
	}
	if(FALSE==ret)
	{
	}
	else
	{
		OSSemPost(ptOsSemUsedForVcpCmdAck);
	}
	
	OSIntExit();
	return ret;
}
/*****************************************************************************
-Fuction		: GetDataDev
-Description	: GetDataDev
-Input			: 
-Output 		: o_buf  o_Len
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetDataDev(u8 *o_buf,u16 *o_Len)
{
	u8 ret=FALSE;
	if(IS_FALSE_BIT(ReceiveRightUsbDataCompleteFlag))
	{
		ret=FALSE;
	}
	else
	{
		*o_Len=g_wRecvUsbDataLen;
		memcpy(o_buf,UsbVirtualComRecvDatabuf,g_wRecvUsbDataLen);
		memset(UsbVirtualComRecvDatabuf,0,sizeof(UsbVirtualComRecvDatabuf));
		g_wRecvUsbDataLen=0;
		M_ClrBit(ReceiveRightUsbDataCompleteFlag);
		ret=TRUE;
	}
	return ret;
}




