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
		//UART ��ʼ������GPIO��˳��Ҫ�ڴ�������ǰ��˳������ж�����
	GPIO_InitTypeDef GPIO_Initure;	
	__HAL_RCC_GPIOC_CLK_ENABLE();			//ʹ��GPIOBʱ��
	__HAL_RCC_USART6_CLK_ENABLE();			//ʹ��USART6ʱ��

	GPIO_Initure.Pin=GPIO_PIN_6;			//Pc6
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_FAST; 	//����
	GPIO_Initure.Alternate=GPIO_AF8_USART6; //����ΪUSART6
	HAL_GPIO_Init(GPIOC,&GPIO_Initure); 	//��ʼ��P

	GPIO_Initure.Pin=GPIO_PIN_7;			//P
	HAL_GPIO_Init(GPIOC,&GPIO_Initure); 	//��ʼ��P

	UART6_Handler.Instance=USART6;						//USART6
	UART6_Handler.Init.BaudRate=Wifi_USART_BAUDRATE; 				//������
	UART6_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//�ֳ�Ϊ8λ���ݸ�ʽ
	UART6_Handler.Init.StopBits=UART_STOPBITS_1;		//һ��ֹͣλ
	UART6_Handler.Init.Parity=UART_PARITY_NONE; 		//����żУ��λ
	UART6_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//��Ӳ������
	UART6_Handler.Init.Mode=UART_MODE_TX_RX;			//�շ�ģʽ
	HAL_UART_Init(&UART6_Handler);						//HAL_UART_Init()��ʹ��UART6
	HAL_NVIC_EnableIRQ(USART6_IRQn);		//ʹ��USART2�ж�ͨ��
	HAL_NVIC_SetPriority(USART6_IRQn,2,2);	//��ռ���ȼ�2�������ȼ�2		
	HAL_UART_Receive_IT(&UART6_Handler, (u8 *)USART6RecvDataBuffer, USART6_USART_RECV_SIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
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
	
	__HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOCʱ��
	GPIO_Initure.Pin=GPIO_PIN_8; //PC1
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//�������
	GPIO_Initure.Pull=GPIO_NOPULL;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//����
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

	
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);	//Pc1��0��Ĭ�Ϲر�
	
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
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);	//Pc1��1������
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
		dwTimeout=i_Len+10;//һ���ֽڴ�Լ��Ҫ1ms�����10ms�ĳ�ʱ
		if(HAL_OK!=HAL_UART_Transmit(&UART6_Handler,(uint8_t*)i_buf,i_Len,dwTimeout))//���ͽ��յ�������
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
	/*HAL_UART_IRQHandler(&UART6_Handler);	//����HAL���жϴ����ú��� 
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
		HAL_UART_IRQHandler(&UART6_Handler);	//����HAL���жϴ����ú��� 
		if(HAL_UART_GetState(&UART6_Handler) != HAL_UART_STATE_READY)//�ȴ�����
		{
			
		}
		else
		{
			//ÿ�ν����жϽ���һ���ֽڣ��������¿����ж�
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




