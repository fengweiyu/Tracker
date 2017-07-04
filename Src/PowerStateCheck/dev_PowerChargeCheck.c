/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_PowerChargeCheck.c
* Description		: 	PowerChargeCheck Drive operation center
* Created			: 	2016.09.26.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "stm32f4xx_hal.h"
#include "CBasicTools.h"
#include "Config.h"
#include "app_PowerChargeCheck.h"

static void PowerChargeCheckDevConfig(void);
static u8 GetPowerChargeStateDev(u8 *o_pucPowerChargeState);
static u8 SetRiseEXTIDev();
static u8 SetFallEXTIDev();

static T_PowerChargeCheckDevManage g_tPowerChargeCheckDev ={
	.name="PowerChargeCheckDev",
	.DevConfig				=PowerChargeCheckDevConfig,
	.DevGetPowerChargeState	=GetPowerChargeStateDev,
	.DevSetRiseEXTI			=SetRiseEXTIDev,
	.DevSetFallEXTI			=SetFallEXTIDev
};

/*****************************************************************************
-Fuction		: PowerChargeCheckDevInit
-Description	: PowerChargeCheckDevInit
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void PowerChargeCheckDevInit()
{
	RegisterPowerChargeCheckDev(&g_tPowerChargeCheckDev);
}
/*****************************************************************************
-Fuction		: PowerChargeCheckDevConfig
-Description	: PowerChargeCheckDevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void PowerChargeCheckDevConfig(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();           //开启GPIOC时钟
    
    GPIO_Initure.Pin=GPIO_PIN_4;           //PC4
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);                              //初始化 
}
/*****************************************************************************
-Fuction		: GetPowerChargeStateDev
-Description	: GetPowerChargeStateDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetPowerChargeStateDev(u8 *o_pucPowerChargeState)
{
	u8 ret=FALSE;
	*o_pucPowerChargeState=HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_4); //KEY2按键PC13
	ret=TRUE;
	return ret;
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
static u8 SetRiseEXTIDev()
{
	u8 ret=FALSE;
    	GPIO_InitTypeDef GPIO_Initure;    
  	GPIO_Initure.Pin=GPIO_PIN_4;               //PC13
    	GPIO_Initure.Mode=GPIO_MODE_IT_RISING;     //
    	GPIO_Initure.Pull=GPIO_PULLDOWN;
    	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

  	HAL_NVIC_SetPriority(EXTI4_IRQn,3,2);       //抢占优先级为3，子优先级为2
  	HAL_NVIC_EnableIRQ(EXTI4_IRQn);             //使能中断线4

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
static u8 SetFallEXTIDev()
{
	u8 ret=FALSE;
    	GPIO_InitTypeDef GPIO_Initure;    
  	GPIO_Initure.Pin=GPIO_PIN_4;               //PC4
    	GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    	GPIO_Initure.Pull=GPIO_PULLUP;
    	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

  	HAL_NVIC_SetPriority(EXTI4_IRQn,3,2);       //抢占优先级为3，子优先级为2
  	HAL_NVIC_EnableIRQ(EXTI4_IRQn);             //使能中断线4

	ret=TRUE;
	return ret;
}




