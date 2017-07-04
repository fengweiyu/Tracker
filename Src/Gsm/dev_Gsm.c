/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_Gsm.c
* Description		: 	Gsm Drive operation center
* Created			: 	2016.09.02.
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
#include "dev_gsm_Database.h"
#include "GsmDriverPort.h"
#include "app_Gsm.h"

static void GsmDevConfig(void);
static u8 USART2SendDataDev(u8 *i_buf,u16 i_Len);
static u8 USART2RecvDataDev();
static void GsmResetDev();
static void GsmPowerOnDev(void);
static void GsmPowerOffDev(void);
static u8 GsmCheckGsmStateDev();
static void GsmWakeUpDev(void);
static void GsmEnterSleepModeDev(void);

static UART_HandleTypeDef UART2_Handler;
u8 Usart2RecvDataBuffer[USART2_USART_RECV_SIZE];
static T_GeneralGsmDevManage g_tGeneralGsmDev={
	.name="GeneralGsmDev",
	.GeneralGsmConfigDev			=GsmDevConfig,
	.GeneralGsmSendDataDev		=USART2SendDataDev,
	.GeneralGsmRecvDataDev		=USART2RecvDataDev,
	.GeneralGsmResetDev			=GsmResetDev,
	.GeneralGsmPowerOnDev		=GsmPowerOnDev,
	.GeneralGsmPowerOffDev		=GsmPowerOffDev,
	.GeneralGsmCheckGsmStateDev	=GsmCheckGsmStateDev,
	.GeneralGsmWakeUpDev		=GsmWakeUpDev,
	.GeneralGsmEnterSleepModeDev	=GsmEnterSleepModeDev
};
/*****************************************************************************
-Fuction		: GeneralGsmDevInit
-Description	: GeneralGsmDevInit
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void GeneralGsmDevInit()
{
	RegisterGeneralGsmDev(&g_tGeneralGsmDev);
	dev_gsm_DataBaseInit();//根据GSM库来的
}

/*****************************************************************************
-Fuction		: Usart2DevConfig
-Description	: Usart2DevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void USART2DevConfig(void)
{
		//UART 初始化设置GPIO的顺序要在串口设置前，顺序错误中断无用
	GPIO_InitTypeDef GPIO_Initure;	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
	__HAL_RCC_USART2_CLK_ENABLE();			//使能USART1时钟

	GPIO_Initure.Pin=GPIO_PIN_2;			//PA2
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	//复用为USART1
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

	GPIO_Initure.Pin=GPIO_PIN_3;			//PA3
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA10

	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=GSM_USART_BAUDRATE;				    //波特率
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()会使能UART2
	HAL_NVIC_EnableIRQ(USART2_IRQn);		//使能USART2中断通道
	HAL_NVIC_SetPriority(USART2_IRQn,2,0);	//抢占优先级3，子优先级3			
	HAL_UART_Receive_IT(&UART2_Handler, (u8 *)Usart2RecvDataBuffer, USART2_USART_RECV_SIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
}
/*****************************************************************************
-Fuction		: GsmExti1Init
-Description	: GsmExti1Init
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GsmExti14Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();               //开启GPIO时钟
        
    GPIO_Initure.Pin=GPIO_PIN_14;               //P
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
        
    //中断线1
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,3,0);   //抢占优先级为2，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //使能中断线1
}

/*****************************************************************************
-Fuction		: GsmDevConfig
-Description	: GsmDevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GsmDevConfig(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	USART2DevConfig();
	
	__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟
	GPIO_Initure.Pin=GPIO_PIN_10; //PC1
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_Initure.Pull=GPIO_NOPULL;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//高速
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);

	GPIO_Initure.Pin=GPIO_PIN_5;			//Pb5
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//高速
	HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	//初始化P

	GPIO_Initure.Pin=GPIO_PIN_13;			//Pb13
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//高速
	HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	//初始化P

	GPIO_Initure.Pin=GPIO_PIN_12;			//Pb12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_Initure.Pull=GPIO_NOPULL;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//高速
	HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	//初始化P

	GPIO_Initure.Pin=GPIO_PIN_1;			//Pa1
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_Initure.Pull=GPIO_NOPULL;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//高速
	HAL_GPIO_Init(GPIOA,&GPIO_Initure); 	//初始化P

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);	//置0，默认断电
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);	//Pc1置1，默认不复位

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);	//置1输出高，默认不唤醒

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);	//Pc1置0输出高，默认关机

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);	//置0，默认拉低RTS

	GsmExti14Init();
	//GsmExti8Init();
}
/*****************************************************************************
-Fuction		: GsmReadExti1State
-Description	: GsmReadExti1State
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GsmReadExti14State(u8 *o_pucExtiState)
{
	u8 ret=FALSE;
	*o_pucExtiState=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14); //
	ret=TRUE;
	return ret;
}
static void GsmDelayUs(u16 i_ucTime)
{
	u32 wTime = 84*i_ucTime;
	while(wTime--);
}
static void GsmDelayMs(u16 i_ucTime)
{
	u16 wTime = i_ucTime;
	while(wTime--)
	{
		GsmDelayUs(1000);
	};
}

/*****************************************************************************
-Fuction		: GsmCheckGsmStateDev
-Description	: 检测GSM	RING是否是低电平
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GsmCheckGsmStateDev()
{
	u8 ret=FALSE;
	u8 ucGsmState=0;
	OSTimeDly(2);
	GsmReadExti14State(&ucGsmState); //
	if(0==ucGsmState)
	{
		ret=TRUE;
	}
	else
	{
		ret=FALSE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: GsmPowerOnDev
-Description	: 使用POWERKEY和上电结合开机，POWERKEY设置为低则输出高
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GsmPowerOnDev(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET);	
	OSTimeDly(300);//延时3s 上电
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);	//Pc
	OSTimeDly(120);//拉低1s多0.2防止万一
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);	//Pc
	OSTimeDly(280);//延时2.4s多0.4防止万一开机完成
}
/*****************************************************************************
-Fuction		: GsmPowerOffDev
-Description	: 使用断电关机
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GsmPowerOffDev(void)
{
	//关机不使用关机由于开机也是如此时序会导致不断重启
	/*HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);	//Pc
	OSTimeDly(150);//拉低1.5s
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);	//Pc
	OSTimeDly(60);//延时600ms关机完成
	*/	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);	//置0，默认断电
	OSTimeDly(300);//延时3s
}
/*****************************************************************************
-Fuction		: GsmResetDev
-Description	: GsmResetDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GsmResetDev()
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);	//
	OSTimeDly(12);//拉低120ms
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);	//
	OSTimeDly(300);//延时2.6s多0.4防止万一，GSM工作正常
}

/*****************************************************************************
-Fuction		: GsmPowerOnDev
-Description	: GsmPowerOnDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GsmWakeUpDev(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);	//
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);	//置0，拉低RTS
	OSTimeDly(20);//拉低60ms不够，用200ms进行唤醒
}

/*****************************************************************************
-Fuction		: GsmPowerOnDev
-Description	: GsmPowerOnDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GsmEnterSleepModeDev(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);	//
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);	//拉高RTS
}

/*****************************************************************************
-Fuction		: Usart2SendDataDev
-Description	: Usart2SendDataDev
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 USART2SendDataDev(u8 *i_buf,u16 i_Len)
{
	u8 ret=FALSE;
	u32 dwTimeout=0;
	if((NULL==i_buf)||(0==i_Len))
	{
		ret=FALSE;
	}
	else
	{		
		dwTimeout=i_Len+10;//一个字节大约需要1ms，多加10ms的超时
		if(HAL_OK!=HAL_UART_Transmit(&UART2_Handler,(uint8_t*)i_buf,i_Len,dwTimeout))//发送接收到的数据
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
-Fuction		: Usart2RecvDataDev
-Description	: Usart2RecvDataDev
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 USART2RecvDataDev()
{
	u8 ret=FALSE;	
	u32 dwTimeOut;
	u8 ucData;
	OSIntEnter();

	if(IS_FALSE_BIT(__HAL_USART_GET_FLAG(&UART2_Handler, USART_SR_RXNE))) {;		
	}
	else	{
		__HAL_USART_CLEAR_FLAG(&UART2_Handler, USART_SR_RXNE); 

		ucData = (u8)(UART2_Handler.Instance->DR & (u8)0xFF);
		dev_gsm_DataRecv(ucData);
	}
	__HAL_USART_CLEAR_FLAG(&UART2_Handler, USART_SR_PE); 
	__HAL_USART_CLEAR_FLAG(&UART2_Handler, USART_SR_FE); 
	__HAL_USART_CLEAR_FLAG(&UART2_Handler, USART_SR_NE); 	
	__HAL_USART_CLEAR_FLAG(&UART2_Handler, USART_SR_ORE); 

	ret=TRUE;
	
	/*HAL_UART_IRQHandler(&UART2_Handler);	//调用HAL库中断处理公用函数 
	for(dwTimeOut=0;dwTimeOut<3;dwTimeOut++)
	{
		if(HAL_UART_STATE_READY==HAL_UART_GetState(&UART2_Handler))
		{
			break;
		}
		else
		{
		}
	}
	for(dwTimeOut=0;dwTimeOut<3;dwTimeOut++)
	{
		if(HAL_OK==HAL_UART_Receive_IT(&UART2_Handler, (u8 *)Usart2RecvDataBuffer, USART2_USART_RECV_SIZE) )
		{
			break;
		}
		else
		{
		}
	}	
	ret=TRUE;
	/*HAL_UART_IRQHandler(&UART2_Handler);	//调用HAL库中断处理公用函数	
    	if(HAL_UART_GetState(&UART2_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
		
	}
	else
	{
		//每次进入中断接收一个字节，并且重新开启中断
		if(HAL_OK!=HAL_UART_Receive_IT(&UART2_Handler, (u8 *)Usart2RecvDataBuffer, USART2_USART_RECV_SIZE))
		{
		}
		else
		{
			ret=GpsRecvDataDev(Usart2RecvDataBuffer[0]);	
		}
	}*/
	OSIntExit();
	return ret;
}


