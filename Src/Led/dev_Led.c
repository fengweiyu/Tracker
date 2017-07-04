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

TIM_HandleTypeDef TIM3_Handler;         //定时器3PWM句柄 
TIM_OC_InitTypeDef TIM3_CH4Handler;	    //定时器3通道4句柄

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
    __HAL_RCC_GPIOB_CLK_ENABLE();           //开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1; //PB1,0
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	//PB0置1，默认初始化后灯灭
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	//PB1置1，默认初始化后灯灭

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
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //对应引脚PB0
		ret=TRUE;
	}
	else if(BlueLed==i_eLedType)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //对应引脚PB1拉低，亮，等同于LED0(0)
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
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	//对应引脚PB0
		ret=TRUE;
	}
	else if(BlueLed==i_eLedType)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET); //对应引脚PB1高，灭

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
//定时器底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_PWM_Init()调用
//htim:定时器句柄
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
	__HAL_RCC_TIM3_CLK_ENABLE();			//使能定时器3
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟

	GPIO_Initure.Pin=GPIO_PIN_1;           	//PB1
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	GPIO_Initure.Alternate= GPIO_AF2_TIM3;	//PB1复用为TIM3_CH4
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
}
/*****************************************************************************
-Fuction		: HAL_TIM_PWM_MspInit
-Description	: HAL_TIM_PWM_MspInit
//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void TIM3_PWM_Init(u32 i_dwArr,u32 i_dwPsc)
{ 
	TIM3_Handler.Instance=TIM3;            //定时器3
	TIM3_Handler.Init.Prescaler=i_dwPsc;       //定时器分频
	TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//向上计数模式
	TIM3_Handler.Init.Period=i_dwArr;          //自动重装载值
	TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&TIM3_Handler);       //初始化PWM

	TIM3_CH4Handler.OCMode=TIM_OCMODE_PWM1; //模式选择PWM1
	TIM3_CH4Handler.Pulse=i_dwArr/21;            //设置比较值,此值用来确定占空比，默认比较值为0.5/10.5
	TIM3_CH4Handler.OCPolarity=TIM_OCPOLARITY_LOW; //输出比较极性为低 
	HAL_TIM_PWM_ConfigChannel(&TIM3_Handler,&TIM3_CH4Handler,TIM_CHANNEL_4);//配置TIM3通道4

	HAL_TIM_PWM_Start(&TIM3_Handler,TIM_CHANNEL_4);//开启PWM通道4
}
/*****************************************************************************
-Fuction		: TIM3CH4_SetCompare
-Description	: TIM3CH4_SetCompare
//设置TIM通道4的占空比
//compare:比较值
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
	//84M/2 *1000=1k计数频率，自动重装载为10500,1k/10500=1s*10.5=10.5s
		TIM3_PWM_Init(21000-1,(MCU_RUN_FREQUENCY/2*1000)-1);
		ret=TRUE;
	}
	else
	{
		ret=FALSE;
	}
	return ret;

}

