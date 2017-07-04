/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_Wifi.c
* Description		: 	Wifi Drive operation center
* Created			: 	2016.09.29.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "stm32f4xx_hal.h"
#include "cBasicTools.h"
#include "Config.h"
#include "ucos_ii.h"
#include "main.h"
#include "app_WifiSupport.h"
static void WifiDevConfig(void);
static u8 USART6SendDataDev(u8 *i_buf,u16 i_Len);
static u8 USART6RecvDataDev();
static void WifiPowerOnDev(void);
static void WifiPowerOffDev(void);

static UART_HandleTypeDef UART6_Handler;
u8 USART6RecvDataBuffer[USART6_USART_RECV_SIZE];
static T_GeneralWifiDevManage g_tGeneralWifiDev={
	.name="GeneralWifiDev",
	.GeneralWifiConfigDev		=WifiDevConfig,
	.GeneralWifiSendDataDev	=USART6SendDataDev,
	.GeneralWifiRecvDataDev	=USART6RecvDataDev,
	.GeneralWifiPowerOnDev	=WifiPowerOnDev,
	.GeneralWifiPowerOffDev	=WifiPowerOffDev
};
/*****************************************************************************
-Fuction		: GeneralWifiDevInit
-Description	: GeneralWifiDevInit
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void GeneralWifiDevInit()
{
	RegisterGeneralWifiDev(&g_tGeneralWifiDev);
}

/*****************************************************************************
-Fuction		: USART6DevConfig
-Description	: USART6DevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void USART6DevConfig(void)
{
		//UART 初始化设置GPIO的顺序要在串口设置前，顺序错误中断无用
	GPIO_InitTypeDef GPIO_Initure;	
	__HAL_RCC_GPIOC_CLK_ENABLE();			//使能GPIOB时钟
	__HAL_RCC_USART6_CLK_ENABLE();			//使能USART6时钟

	GPIO_Initure.Pin=GPIO_PIN_6;			//Pc6
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_FAST; 	//高速
	GPIO_Initure.Alternate=GPIO_AF8_USART6; //复用为USART6
	HAL_GPIO_Init(GPIOC,&GPIO_Initure); 	//初始化P

	GPIO_Initure.Pin=GPIO_PIN_7;			//P
	HAL_GPIO_Init(GPIOC,&GPIO_Initure); 	//初始化P

	UART6_Handler.Instance=USART6;						//USART6
	UART6_Handler.Init.BaudRate=Wifi_USART_BAUDRATE; 				//波特率
	UART6_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//字长为8位数据格式
	UART6_Handler.Init.StopBits=UART_STOPBITS_1;		//一个停止位
	UART6_Handler.Init.Parity=UART_PARITY_NONE; 		//无奇偶校验位
	UART6_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//无硬件流控
	UART6_Handler.Init.Mode=UART_MODE_TX_RX;			//收发模式
	HAL_UART_Init(&UART6_Handler);						//HAL_UART_Init()会使能UART6
	HAL_NVIC_EnableIRQ(USART6_IRQn);		//使能USART2中断通道
	HAL_NVIC_SetPriority(USART6_IRQn,2,2);	//抢占优先级2，子优先级2		
	HAL_UART_Receive_IT(&UART6_Handler, (u8 *)USART6RecvDataBuffer, USART6_USART_RECV_SIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
}

/*****************************************************************************
-Fuction		: WifiDevConfig
-Description	: WifiDevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void WifiDevConfig(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	USART6DevConfig();
	
	__HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
	GPIO_Initure.Pin=GPIO_PIN_8; //PC1
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_Initure.Pull=GPIO_NOPULL;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//高速
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

	
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);	//Pc1置0，默认关闭
	
}
/*****************************************************************************
-Fuction		: WifiPowerOnDev
-Description	: WifiPowerOnDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void WifiPowerOnDev(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);	//Pc1置1，开启
}
/*****************************************************************************
-Fuction		: WifiPowerOffDev
-Description	: WifiPowerOffDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void WifiPowerOffDev(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);	//Pc
}

/*****************************************************************************
-Fuction		: USART6SendDataDev
-Description	: USART6SendDataDev
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 USART6SendDataDev(u8 *i_buf,u16 i_Len)
{
	u8 ret=FALSE;
	u16 wDataCount=0;
	u8 *DataBuf=i_buf;
	u32 dwTimeout=0; 
	if((NULL==i_buf)||(0==i_Len))
	{
		ret=FALSE;
	}
	else
	{
		dwTimeout=i_Len+10;//一个字节大约需要1ms，多加10ms的超时
		if(HAL_OK!=HAL_UART_Transmit(&UART6_Handler,(uint8_t*)i_buf,i_Len,dwTimeout))//发送接收到的数据
		{
			ret=FALSE;			
		}
		else
		{
			ret=TRUE;
		}
	}
	//	FW_UartDataRecv(UART1_DWL);
	/*if(IS_FALSE_BIT(__HAL_USART_GET_FLAG(&UART6_Handler, USART_SR_TXE))) {;		
	}
	else	{
		__HAL_USART_CLEAR_FLAG(&UART6_Handler, USART_SR_TXE); 

		if(NULL == DataBuf) {
			__HAL_USART_DISABLE_IT(&UART6_Handler, USART_SR_TXE);
		}
		else	{
			if(wDataCount < i_Len) {
				
				UART6_Handler.Instance->DR = (*( DataBuf + wDataCount++)& (uint16_t)0x01FFU);
			}
			else	{
				__HAL_USART_DISABLE_IT(&UART6_Handler, USART_SR_TXE);
			}
		}
	}
	__HAL_USART_CLEAR_FLAG(&UART6_Handler, USART_SR_PE); 
	__HAL_USART_CLEAR_FLAG(&UART6_Handler, USART_SR_FE); 
	__HAL_USART_CLEAR_FLAG(&UART6_Handler, USART_SR_NE); 	
	__HAL_USART_CLEAR_FLAG(&UART6_Handler, USART_SR_ORE); */
	return ret;
}

/*****************************************************************************
-Fuction		: USART6RecvDataDev
-Description	: USART6RecvDataDev
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 USART6RecvDataDev()
{
	u8 ret=FALSE;
	u32 dwTimeOut=0;
	u8 ucData;
	OSIntEnter();
	
	if(IS_FALSE_BIT(__HAL_USART_GET_FLAG(&UART6_Handler, USART_SR_RXNE))) {;		
	}
	else	{
		__HAL_USART_CLEAR_FLAG(&UART6_Handler, USART_SR_RXNE); 

		ucData = (u8)(UART6_Handler.Instance->DR & (u8)0xFF);
		wifiRecv(ucData);
	}
	__HAL_USART_CLEAR_FLAG(&UART6_Handler, USART_SR_PE); 
	__HAL_USART_CLEAR_FLAG(&UART6_Handler, USART_SR_FE); 
	__HAL_USART_CLEAR_FLAG(&UART6_Handler, USART_SR_NE); 	
	__HAL_USART_CLEAR_FLAG(&UART6_Handler, USART_SR_ORE); 

	ret=TRUE;
	/*HAL_UART_IRQHandler(&UART6_Handler);	//调用HAL库中断处理公用函数 
	for(dwTimeOut=0;dwTimeOut<3;dwTimeOut++)
	{
		if(HAL_UART_STATE_READY==HAL_UART_GetState(&UART6_Handler))
		{
			break;
		}
		else
		{
		}
	}
	for(dwTimeOut=0;dwTimeOut<3;dwTimeOut++)
	{
		if(HAL_OK==HAL_UART_Receive_IT(&UART6_Handler, (u8 *)USART6RecvDataBuffer, USART6_USART_RECV_SIZE) )
		{
			break;
		}
		else
		{
		}
	}	
	
	/*else
	{
		HAL_UART_IRQHandler(&UART6_Handler);	//调用HAL库中断处理公用函数 
		if(HAL_UART_GetState(&UART6_Handler) != HAL_UART_STATE_READY)//等待就绪
		{
			
		}
		else
		{
			//每次进入中断接收一个字节，并且重新开启中断
			if(HAL_OK!=HAL_UART_Receive_IT(&UART6_Handler, (u8 *)USART6RecvDataBuffer, USART6_USART_RECV_SIZE))
			{
			}
			else
			{			
				wifiRecv(USART6RecvDataBuffer[0]);
				ret=TRUE;
			}
		}
	}*/
	OSIntExit();
	
	return ret;
}




