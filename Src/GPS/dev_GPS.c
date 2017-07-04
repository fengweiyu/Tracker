/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_GPS.c
* Description		: 	GPS Drive operation center
* Created			: 	2016.09.02.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "stm32f4xx_hal.h"
#include "cBasicTools.h"
#include "app_GPS.h"
#include "Config.h"
#include "ucos_ii.h"
#include "main.h"
#include "Bsp.h"

static void GpsDevConfig(void);
static u8 USART1SendDataDev(u8 *i_buf,u16 i_Len);
static u8 USART1RecvDataDev();
static u8 USART1GetDataDev(u8 *o_buf,u16 *o_Len);
static void GpsPowerOnDev(void);
static void GpsPowerOffDev(void);


static UART_HandleTypeDef UART1_Handler;
u8 Usart1RecvDataBuffer[USART1_USART_RECV_SIZE];
static u16 Count=0;
static u8 ReceiveRightDataCompleteFlag=0;
static u8 GpsRecvDataBuffer[GPS_USART_RECV_SIZE];
static T_GeneralGpsDevManage g_tGeneralGpsDev={
	.name="GeneralGpsDev",
	.GeneralGpsConfigDev		=GpsDevConfig,
	.GeneralGpsSendDataDev	=USART1SendDataDev,
	.GeneralGpsRecvDataDev	=USART1RecvDataDev,
	.GeneralGpsGetDataDev		=USART1GetDataDev,
	.GeneralGpsPowerOnDev	=GpsPowerOnDev,
	.GeneralGpsPowerOffDev	=GpsPowerOffDev
};
/*****************************************************************************
-Fuction		: GeneralGpsDevInit
-Description	: GeneralGpsDevInit
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void GeneralGpsDevInit()
{
	RegisterGeneralGpsDev(&g_tGeneralGpsDev);
}

/*****************************************************************************
-Fuction		: USART1DevConfig
-Description	: USART1DevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void USART1DevConfig(void)
{
		//UART 初始化设置GPIO的顺序要在串口设置前，顺序错误中断无用
	GPIO_InitTypeDef GPIO_Initure;	
	__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟
	__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟

	GPIO_Initure.Pin=GPIO_PIN_6;			//Pb2
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_FAST; 	//高速
	GPIO_Initure.Alternate=GPIO_AF7_USART1; //复用为USART1
	HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	//初始化P

	GPIO_Initure.Pin=GPIO_PIN_7;			//P
	HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	//初始化P

	UART1_Handler.Instance=USART1;						//
	UART1_Handler.Init.BaudRate=GPS_USART_BAUDRATE; 				//波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;		//一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE; 		//无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;			//收发模式
	HAL_UART_Init(&UART1_Handler);						//HAL_UART_Init()会使能UART1
	HAL_NVIC_EnableIRQ(USART1_IRQn);		//使能USART2中断通道
	HAL_NVIC_SetPriority(USART1_IRQn,2,3);	//抢占优先级1，子优先级2		
	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)Usart1RecvDataBuffer, USART1_USART_RECV_SIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
}

/*****************************************************************************
-Fuction		: SetRiseEXTIDev
-Description	: SetRiseEXTIDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GpsSetRiseEXTIDev()
{
	u8 ret=FALSE;
    	GPIO_InitTypeDef GPIO_Initure;    
	__HAL_RCC_GPIOC_CLK_ENABLE();				//开启GPIO时钟

  	GPIO_Initure.Pin=GPIO_PIN_13;               //PC13
    	GPIO_Initure.Mode=GPIO_MODE_IT_RISING;     //
    	GPIO_Initure.Pull=GPIO_PULLDOWN;
    	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

  	HAL_NVIC_SetPriority(EXTI15_10_IRQn,3,0);       //抢占优先级为3，子优先级为2
  	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);             //使能中断线4

	ret=TRUE;
	return ret;
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
static u8 GpsSetFallEXTIDev()
{
	u8 ret=FALSE;
    	GPIO_InitTypeDef GPIO_Initure;  
	__HAL_RCC_GPIOC_CLK_ENABLE();				//开启GPIO时钟
  	GPIO_Initure.Pin=GPIO_PIN_13;               //PC4
    	GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    	GPIO_Initure.Pull=GPIO_PULLUP;
    	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

  	HAL_NVIC_SetPriority(EXTI15_10_IRQn,3,0);       //抢占优先级为3，子优先级为2
  	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);             //使能中断线4

	ret=TRUE;
	return ret;
}

/*****************************************************************************
-Fuction		: RtcTimeInit
-Description	: RtcTimeInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GpsDevConfig(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	USART1DevConfig();
	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOC时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOC时钟
	
	GPIO_Initure.Pin=GPIO_PIN_4; //PC10
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_Initure.Pull=GPIO_NOPULL;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//高速
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	GpsPowerOffDev();	//PB0置0，默认关闭
	//GpsSetFallEXTIDev();//硬件修改为PC13,暂不使用秒秒冲
}
/*****************************************************************************
-Fuction		: RtcTimeInit
-Description	: RtcTimeInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GpsPowerOnDev(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);	//PB0置1，默认开启
}
/*****************************************************************************
-Fuction		: RtcTimeInit
-Description	: RtcTimeInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GpsPowerOffDev(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);	//PB0置1，默认开启
}

/*****************************************************************************
-Fuction		: USART1SendDataDev
-Description	: USART1SendDataDev
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 USART1SendDataDev(u8 *i_buf,u16 i_Len)
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
		if(HAL_OK!=HAL_UART_Transmit(&UART1_Handler,(uint8_t*)i_buf,i_Len,dwTimeout))//发送接收到的数据
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
-Fuction		: USART1RecvDataDev
-Description	: USART1RecvDataDev
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 USART1RecvDataDev()
{
	u8 ret=FALSE;
	u32 dwTimeOut;
	u8 ucData;
	OSIntEnter();
	
	if(IS_FALSE_BIT(__HAL_USART_GET_FLAG(&UART1_Handler, USART_SR_RXNE))) {;		
	}
	else	{
		__HAL_USART_CLEAR_FLAG(&UART1_Handler, USART_SR_RXNE); 

		ucData = (u8)(UART1_Handler.Instance->DR & (u8)0xFF);
		GpsRecvDataDev(ucData);
	}
	__HAL_USART_CLEAR_FLAG(&UART1_Handler, USART_SR_PE); 
	__HAL_USART_CLEAR_FLAG(&UART1_Handler, USART_SR_FE); 
	__HAL_USART_CLEAR_FLAG(&UART1_Handler, USART_SR_NE); 	
	__HAL_USART_CLEAR_FLAG(&UART1_Handler, USART_SR_ORE); 

	ret=TRUE;
	/*HAL_UART_IRQHandler(&UART1_Handler);	//调用HAL库中断处理公用函数 
	for(dwTimeOut=0;dwTimeOut<3;dwTimeOut++)
	{
		if(HAL_UART_STATE_READY==HAL_UART_GetState(&UART1_Handler))
		{
			break;
		}
		else
		{
		}
	}
	for(dwTimeOut=0;dwTimeOut<3;dwTimeOut++)
	{
		if(HAL_OK==HAL_UART_Receive_IT(&UART1_Handler, (u8 *)Usart1RecvDataBuffer, USART1_USART_RECV_SIZE) )
		{
			break;
		}
		else
		{
		}
	}
	ret=TRUE;
	/*HAL_UART_IRQHandler(&UART1_Handler);	//调用HAL库中断处理公用函数	
	if(HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
		
	}
	else
	{
		//每次进入中断接收一个字节，并且重新开启中断
		if(HAL_OK!=HAL_UART_Receive_IT(&UART1_Handler, (u8 *)Usart1RecvDataBuffer, USART1_USART_RECV_SIZE))
		{
		}
		else
		{	
			//OS_CPU_SR  cpu_sr;
			//OS_ENTER_CRITICAL();
			dev_gsm_DataRecv(Usart1RecvDataBuffer[0]);
			//OS_EXIT_CRITICAL();
                        ret=TRUE;
		}
	}*/
	OSIntExit();
	
	return ret;
}

/*****************************************************************************
-Fuction		: Usart2GetDataDev
-Description	: Usart2GetDataDev
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 USART1GetDataDev(u8 *o_buf,u16 *o_Len)
{
	u8 ret=FALSE;
	if(IS_FALSE_BIT(ReceiveRightDataCompleteFlag))
	{
		ret=FALSE;
	}
	else
	{
		OSSchedLock();
		*o_Len=Count;
		memcpy(o_buf,GpsRecvDataBuffer,Count);
		OSSchedUnlock();
		memset(GpsRecvDataBuffer,0,sizeof(GpsRecvDataBuffer));
		Count=0;
		M_ClrBit(ReceiveRightDataCompleteFlag);
		ret=TRUE;
	}
	return ret;
}
/***********************接收驱动支持函数****************************/
/*****************************************************************************
-Fuction		: GpsRecvDataDev
-Description	: GpsRecvDataDev
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
/*串口数据接收*/
static u8 GpsDevRxQWrite(u8 *i_pucBuf, u16 i_wLen)
{
	myevent event; 
	u8 ret=FALSE;
	event.buff = i_pucBuf; 
	event.size = i_wLen; 

	ret=DevOsRxQWrite(ptOsQForGpsRx, (myevent *)&event); 
        return ret;
}

/*****************************************************************************
-Fuction		: GpsRecvDataDev
-Description	: GpsRecvDataDev
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/22	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
 u8 GpsRecvDataDev(u8 i_ucGpsData)
{
	u8 *pucGpsRxBuf; 
	u8 ret=FALSE;
	static enum
	{
		GPS_DAT_RX_IDLE,
		GPS_DAT_RX_RXING,
		GPS_DAT_RX_WAIT_VERIFY_1,
		GPS_DAT_RX_WAIT_VERIFY_2,
		GPS_DAT_RX_WAIT_END_1,
		GPS_DAT_RX_WAIT_END_2
	}eGpsDataRx; 	
	pucGpsRxBuf = GpsRecvDataBuffer; 
	if('$' != i_ucGpsData) 
	{
		;
	}
	else 
	{
		eGpsDataRx = GPS_DAT_RX_IDLE; 
	}
	switch(eGpsDataRx) 
	{
		case GPS_DAT_RX_IDLE: 
			if('$' != i_ucGpsData) 
			{
				;
			}
			else	
			{
				memset(pucGpsRxBuf, 0, sizeof(GpsRecvDataBuffer)); 
				Count=0;								
				GpsRecvDataBuffer[Count++]=i_ucGpsData;
				eGpsDataRx = GPS_DAT_RX_RXING; 
			}		
			break; 
			
		case GPS_DAT_RX_RXING:
			GpsRecvDataBuffer[Count++]=i_ucGpsData;
			if(Count > 5) 
			{
				;//针对196数据类似GPS数据开头所做
			}
			else 
			{
				if(IsLower(i_ucGpsData) || IsUpper(i_ucGpsData)) 
				{
					;
				}
				else
				{
					eGpsDataRx = GPS_DAT_RX_IDLE; 
					break; 
				}
			}		
			if(Count < (GPS_USART_RECV_SIZE - 1)) 
			{
				if('*' != i_ucGpsData) 
				{
					if((0x0D != i_ucGpsData) && (0x0A != i_ucGpsData))	
					{
						; 
					}
					else	
					{
						DebugPrintf(ERR"gps data last err1!\n");
						eGpsDataRx = GPS_DAT_RX_IDLE; 
					}
				}
				else	
				{
					eGpsDataRx = GPS_DAT_RX_WAIT_VERIFY_1; 
				}
			}
			else	
			{ 
				DebugPrintf(ERR"gps data len Err1!\n");
				eGpsDataRx = GPS_DAT_RX_IDLE; 
			}		
			break; 

		case GPS_DAT_RX_WAIT_VERIFY_1:
			GpsRecvDataBuffer[Count++]=i_ucGpsData;
			if(Count < (GPS_USART_RECV_SIZE - 1)) 
			{
				if((0x0D != i_ucGpsData) && (0x0A != i_ucGpsData))	
				{ 
					eGpsDataRx = GPS_DAT_RX_WAIT_VERIFY_2; 
				}
				else	
				{
					DebugPrintf(ERR"gps data last err2!\n");
					eGpsDataRx = GPS_DAT_RX_IDLE; 
				}
			}
			else	
			{ 
				DebugPrintf(ERR"gps data len Err2!\n");
				eGpsDataRx = GPS_DAT_RX_IDLE; 
			}
			break; 

		case GPS_DAT_RX_WAIT_VERIFY_2:
			GpsRecvDataBuffer[Count++]=i_ucGpsData;
			if(Count < (GPS_USART_RECV_SIZE - 1)) 
			{
				if((0x0D != i_ucGpsData) && (0x0A != i_ucGpsData))	
				{ 
					eGpsDataRx = GPS_DAT_RX_WAIT_END_1; 
				}
				else	
				{
					DebugPrintf(ERR"gps data last err3!\n");
					eGpsDataRx = GPS_DAT_RX_IDLE; 
				}
			}
			else	
			{ 
				DebugPrintf(ERR"gps data len Err3!\n");
				eGpsDataRx = GPS_DAT_RX_IDLE; 
			}
			break; 

		case GPS_DAT_RX_WAIT_END_1:
			GpsRecvDataBuffer[Count++]=i_ucGpsData;
			if(Count < (GPS_USART_RECV_SIZE - 1)) 
			{
				if(0x0D != i_ucGpsData) 
				{
					DebugPrintf(ERR"gps data last cr err!\n");
					eGpsDataRx = GPS_DAT_RX_IDLE; 
				}
				else	
				{
					eGpsDataRx = GPS_DAT_RX_WAIT_END_2; 
				}
			}
			else	
			{ 
				DebugPrintf(ERR"gps data len Err4!\n");
				eGpsDataRx = GPS_DAT_RX_IDLE; 
			}
			break; 
		
		case GPS_DAT_RX_WAIT_END_2:
			GpsRecvDataBuffer[Count++]=i_ucGpsData;
			if(Count< (GPS_USART_RECV_SIZE + 1)) 
			{
				if(0x0A != i_ucGpsData) 
				{
					DebugPrintf(ERR"gps data last lf err!\n");
				}
				else	
				{
					ret=GpsDevRxQWrite(GpsRecvDataBuffer,Count);
					if(FALSE==ret)
					{
						DebugPrintf(ERR"Gps Rx Write err\r\n");
					}
					else
					{
						memset(pucGpsRxBuf, 0, sizeof(GpsRecvDataBuffer)); 
						Count=0;								
					}
				}
				eGpsDataRx = GPS_DAT_RX_IDLE; 
			}
			else	
			{ 
				DebugPrintf(ERR"gps data len Err5!\n");
				eGpsDataRx = GPS_DAT_RX_IDLE; 
			}
			break; 
		default:
			eGpsDataRx = GPS_DAT_RX_IDLE; 
			break; 
	}
	return ret;
}

