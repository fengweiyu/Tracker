/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_Led.c
* Description		: 	Led Drive operation center
* Created			: 	2016.09.23.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "stm32f4xx_hal.h"
#include "CBasicTools.h"
#include "Config.h"
#include "app_Led.h"

TIM_HandleTypeDef TIM3_Handler;         //��ʱ��3PWM��� 
TIM_OC_InitTypeDef TIM3_CH4Handler;	    //��ʱ��3ͨ��4���

static void LedDevConfig(void);
static u8 LedOnDev(E_LedType i_eLedType);
static u8 LedOffDev(E_LedType i_eLedType);
static u8 LedPwmConfigDev(E_LedType i_eLedType);

static T_LedDevManage g_tLedDev ={
	.name="LedDev",
	.DevConfig			=LedDevConfig,
	.DevLedOn			=LedOnDev,
	.DevLedOff			=LedOffDev,
	.DevLedPwmConfig		=LedPwmConfigDev
};

/*****************************************************************************
-Fuction		: LedDevInit
-Description	: LedDevInit
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void LedDevInit()
{
	RegisterLedDev(&g_tLedDev);
}

/*****************************************************************************
-Fuction		: LedDevConfig
-Description	: LedDevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void LedDevConfig(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1; //PB1,0
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	//PB0��1��Ĭ�ϳ�ʼ�������
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	//PB1��1��Ĭ�ϳ�ʼ�������

}
/*****************************************************************************
-Fuction		: SendDataDev
-Description	: SendDataDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LedOnDev(E_LedType i_eLedType)
{
	u8 ret=FALSE;
	if(RedLed==i_eLedType)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //��Ӧ����PB0
		ret=TRUE;
	}
	else if(BlueLed==i_eLedType)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //��Ӧ����PB1���ͣ�������ͬ��LED0(0)
		ret=TRUE;
	}
	else
	{
		ret=FALSE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: LedOff
-Description	: LedOff
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LedOffDev(E_LedType i_eLedType)
{
	u8 ret=FALSE;
	if(RedLed==i_eLedType)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	//��Ӧ����PB0
		ret=TRUE;
	}
	else if(BlueLed==i_eLedType)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET); //��Ӧ����PB1�ߣ���

		ret=TRUE;
	}
	else
	{
		ret=FALSE;
	}
	return ret;

}
/*****************************************************************************
-Fuction		: HAL_TIM_PWM_MspInit
-Description	: HAL_TIM_PWM_MspInit
//��ʱ���ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_PWM_Init()����
//htim:��ʱ�����
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM3_CLK_ENABLE();			//ʹ�ܶ�ʱ��3
	__HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��

	GPIO_Initure.Pin=GPIO_PIN_1;           	//PB1
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	GPIO_Initure.Alternate= GPIO_AF2_TIM3;	//PB1����ΪTIM3_CH4
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
}
/*****************************************************************************
-Fuction		: HAL_TIM_PWM_MspInit
-Description	: HAL_TIM_PWM_MspInit
//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void TIM3_PWM_Init(u32 i_dwArr,u32 i_dwPsc)
{ 
	TIM3_Handler.Instance=TIM3;            //��ʱ��3
	TIM3_Handler.Init.Prescaler=i_dwPsc;       //��ʱ����Ƶ
	TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//���ϼ���ģʽ
	TIM3_Handler.Init.Period=i_dwArr;          //�Զ���װ��ֵ
	TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&TIM3_Handler);       //��ʼ��PWM

	TIM3_CH4Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
	TIM3_CH4Handler.Pulse=i_dwArr/21;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ0.5/10.5
	TIM3_CH4Handler.OCPolarity=TIM_OCPOLARITY_LOW; //����Ƚϼ���Ϊ�� 
	HAL_TIM_PWM_ConfigChannel(&TIM3_Handler,&TIM3_CH4Handler,TIM_CHANNEL_4);//����TIM3ͨ��4

	HAL_TIM_PWM_Start(&TIM3_Handler,TIM_CHANNEL_4);//����PWMͨ��4
}
/*****************************************************************************
-Fuction		: TIM3CH4_SetCompare
-Description	: TIM3CH4_SetCompare
//����TIMͨ��4��ռ�ձ�
//compare:�Ƚ�ֵ
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void TIM3CH4_SetCompare(u32 i_dwCompare)
{
	TIM3->CCR4=i_dwCompare; 
}
/*****************************************************************************
-Fuction		: LedOff
-Description	: LedOff
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LedPwmConfigDev(E_LedType i_eLedType)
{
	u8 ret=FALSE;
	if(RedLed==i_eLedType)
	{		
		ret=TRUE;
	}
	else if(BlueLed==i_eLedType)
	{
	//84M/2 *1000=1k����Ƶ�ʣ��Զ���װ��Ϊ10500,1k/10500=1s*10.5=10.5s
		TIM3_PWM_Init(21000-1,(MCU_RUN_FREQUENCY/2*1000)-1);
		ret=TRUE;
	}
	else
	{
		ret=FALSE;
	}
	return ret;

}

