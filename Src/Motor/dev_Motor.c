/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_Motor.c
* Description		: 	Motor Drive operation center
* Created			: 	2016.09.23.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "stm32f4xx_hal.h"
#include "CBasicTools.h"
#include "Config.h"
#include "app_Motor.h"

static void MotorDevConfig(void);
static u8 MotorOnDev();
static u8 MotorOffDev();

static T_MotorDevManage g_tMotorDev ={
	.name="MotorDev",
	.DevConfig	=MotorDevConfig,
	.DevMotorOn	=MotorOnDev,
	.DevMotorOff	=MotorOffDev,
};

/*****************************************************************************
-Fuction		: MotorDevInit
-Description	: MotorDevInit
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void MotorDevInit()
{
	RegisterMotorDev(&g_tMotorDev);
}
/*****************************************************************************
-Fuction		: MotorDevConfig
-Description	: MotorDevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void MotorDevConfig(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();           //开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_1; //Pc1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);	//PB10置0，默认停止震动
}
/*****************************************************************************
-Fuction		: MotorOnDev
-Description	: MotorOnDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 MotorOnDev()
{
	u8 ret=FALSE;
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET); //对应引脚高，打开震动
	ret=TRUE;
	return ret;
}
/*****************************************************************************
-Fuction		: MotorOffDev
-Description	: MotorOffDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 MotorOffDev()
{
	u8 ret=FALSE;
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);	//对应引脚PB10
	ret=TRUE;
	return ret;

}



