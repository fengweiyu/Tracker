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
	dev_gsm_DataBaseInit();//����GSM������
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
		//UART ��ʼ������GPIO��˳��Ҫ�ڴ�������ǰ��˳������ж�����
	GPIO_InitTypeDef GPIO_Initure;	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
	__HAL_RCC_USART2_CLK_ENABLE();			//ʹ��USART1ʱ��

	GPIO_Initure.Pin=GPIO_PIN_2;			//PA2
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	//����ΪUSART1
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA9

	GPIO_Initure.Pin=GPIO_PIN_3;			//PA3
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA10

	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=GSM_USART_BAUDRATE;				    //������
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()��ʹ��UART2
	HAL_NVIC_EnableIRQ(USART2_IRQn);		//ʹ��USART2�ж�ͨ��
	HAL_NVIC_SetPriority(USART2_IRQn,2,0);	//��ռ���ȼ�3�������ȼ�3			
	HAL_UART_Receive_IT(&UART2_Handler, (u8 *)Usart2RecvDataBuffer, USART2_USART_RECV_SIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
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
    
    __HAL_RCC_GPIOB_CLK_ENABLE();               //����GPIOʱ��
        
    GPIO_Initure.Pin=GPIO_PIN_14;               //P
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
        
    //�ж���1
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,3,0);   //��ռ���ȼ�Ϊ2�������ȼ�Ϊ1
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //ʹ���ж���1
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
	
	__HAL_RCC_GPIOB_CLK_ENABLE();			//ʹ��GPIOBʱ��
	GPIO_Initure.Pin=GPIO_PIN_10; //PC1
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//�������
	GPIO_Initure.Pull=GPIO_NOPULL;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//����
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);

	GPIO_Initure.Pin=GPIO_PIN_5;			//Pb5
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//�������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//����
	HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	//��ʼ��P

	GPIO_Initure.Pin=GPIO_PIN_13;			//Pb13
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//�������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//����
	HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	//��ʼ��P

	GPIO_Initure.Pin=GPIO_PIN_12;			//Pb12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//�������
	GPIO_Initure.Pull=GPIO_NOPULL;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//����
	HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	//��ʼ��P

	GPIO_Initure.Pin=GPIO_PIN_1;			//Pa1
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//�������
	GPIO_Initure.Pull=GPIO_NOPULL;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//����
	HAL_GPIO_Init(GPIOA,&GPIO_Initure); 	//��ʼ��P

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);	//��0��Ĭ�϶ϵ�
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);	//Pc1��1��Ĭ�ϲ���λ

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);	//��1����ߣ�Ĭ�ϲ�����

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);	//Pc1��0����ߣ�Ĭ�Ϲػ�

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);	//��0��Ĭ������RTS

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
-Description	: ���GSM	RING�Ƿ��ǵ͵�ƽ
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
-Description	: ʹ��POWERKEY���ϵ��Ͽ�����POWERKEY����Ϊ���������
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
	OSTimeDly(300);//��ʱ3s �ϵ�
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);	//Pc
	OSTimeDly(120);//����1s��0.2��ֹ��һ
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);	//Pc
	OSTimeDly(280);//��ʱ2.4s��0.4��ֹ��һ�������
}
/*****************************************************************************
-Fuction		: GsmPowerOffDev
-Description	: ʹ�öϵ�ػ�
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void GsmPowerOffDev(void)
{
	//�ػ���ʹ�ùػ����ڿ���Ҳ�����ʱ��ᵼ�²�������
	/*HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);	//Pc
	OSTimeDly(150);//����1.5s
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);	//Pc
	OSTimeDly(60);//��ʱ600ms�ػ����
	*/	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);	//��0��Ĭ�϶ϵ�
	OSTimeDly(300);//��ʱ3s
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
	OSTimeDly(12);//����120ms
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);	//
	OSTimeDly(300);//��ʱ2.6s��0.4��ֹ��һ��GSM��������
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
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);	//��0������RTS
	OSTimeDly(20);//����60ms��������200ms���л���
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
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);	//����RTS
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
		dwTimeout=i_Len+10;//һ���ֽڴ�Լ��Ҫ1ms�����10ms�ĳ�ʱ
		if(HAL_OK!=HAL_UART_Transmit(&UART2_Handler,(uint8_t*)i_buf,i_Len,dwTimeout))//���ͽ��յ�������
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
	
	/*HAL_UART_IRQHandler(&UART2_Handler);	//����HAL���жϴ����ú��� 
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
	/*HAL_UART_IRQHandler(&UART2_Handler);	//����HAL���жϴ����ú���	
    	if(HAL_UART_GetState(&UART2_Handler) != HAL_UART_STATE_READY)//�ȴ�����
	{
		
	}
	else
	{
		//ÿ�ν����жϽ���һ���ֽڣ��������¿����ж�
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


