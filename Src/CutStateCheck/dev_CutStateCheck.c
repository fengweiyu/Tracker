/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_CutStateCheck.c
* Description		: 	CutStateCheck Drive operation center
* Created			: 	2016.10.20.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "stm32f4xx_hal.h"
#include "CBasicTools.h"
#include "Config.h"
#include "app_CutStateCheck.h"
#include "Alarm.h"
#include "Ucos_ii.h"
#include "main.h"

static void CutStateCheckDevConfig(void);
static u8 CutStateCheckSetAlarmDev(void);
static u8 CutStateCheckRelieveAlarmDev(void);
static u8 CutStateCheckSetRiseEXTIDev();
static u8 CutStateCheckSetFallEXTIDev();

static T_CutStateCheckDevManage g_tCutStateCheckDev ={
	.name="CutStateCheckDev",
	.DevConfig				=CutStateCheckDevConfig,
	.DevSetAlarm				=CutStateCheckSetAlarmDev,
	.DevRelieveAlarm			=CutStateCheckRelieveAlarmDev,
	.DevSetRiseEXTI			=CutStateCheckSetRiseEXTIDev,
	.DevSetFallEXTI			=CutStateCheckSetFallEXTIDev
};

/*****************************************************************************
-Fuction		: CutStateCheckDevInit
-Description	: CutStateCheckDevInit
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void CutStateCheckDevInit()
{
	RegisterCutStateCheckDev(&g_tCutStateCheckDev);
}
/*****************************************************************************
-Fuction		: CutStateCheckDevConfig
-Description	: CutStateCheckDevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void CutStateCheckDevConfig(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();           //开启GPIOC时钟
    
  	GPIO_Initure.Pin=GPIO_PIN_3;               //PC3
    	GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //
    	GPIO_Initure.Pull=GPIO_NOPULL;//由硬件决定不能加上下拉
    	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

  	HAL_NVIC_SetPriority(EXTI3_IRQn,1,0);       //抢占优先级为1，子优先级为0
  	HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //使能中断线3
}
/*****************************************************************************
-Fuction		: ReadCutStateDev
-Description	: ReadCutStateDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 ReadCutStateDev(u8 *o_pucCutState)
{
	u8 ret=FALSE;
	*o_pucCutState=HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_3); 
	ret=TRUE;
	return ret;
}
/*****************************************************************************
-Fuction		: CutStateCheckSetRiseEXTIDev
-Description	: CutStateCheckSetRiseEXTIDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 CutStateCheckSetRiseEXTIDev()
{
	u8 ret=FALSE;
    	GPIO_InitTypeDef GPIO_Initure;    
  	GPIO_Initure.Pin=GPIO_PIN_3;               //PC3
    	GPIO_Initure.Mode=GPIO_MODE_IT_RISING;     //
    	GPIO_Initure.Pull=GPIO_NOPULL;
    	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

  	HAL_NVIC_SetPriority(EXTI3_IRQn,1,0);       //抢占优先级为1，子优先级为0
  	HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //使能中断线3

	ret=TRUE;
	return ret;
}
/*****************************************************************************
-Fuction		: CutStateCheckSetFallEXTIDev
-Description	: CutStateCheckSetFallEXTIDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 CutStateCheckSetFallEXTIDev()
{
	u8 ret=FALSE;
    	GPIO_InitTypeDef GPIO_Initure;    
  	GPIO_Initure.Pin=GPIO_PIN_3;               //PC3
    	GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    	GPIO_Initure.Pull=GPIO_NOPULL;
    	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

  	HAL_NVIC_SetPriority(EXTI3_IRQn,1,0);       //抢占优先级为1，子优先级为0
  	HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //使能中断线3

	ret=TRUE;
	return ret;
}

/*****************************************************************************
-Fuction		: CutStateCheckSetAlarmDev
-Description	: CutStateCheckSetAlarmDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/20   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 CutStateCheckSetAlarmDev(void)
{
	u8 ret=FALSE;
	u8 ucExtiState=0;
	T_Alarm tAlarm={0};
	OSTimeDly(2);//延时20毫秒消除抖动
	ReadCutStateDev(&ucExtiState);
	if(CUT_TRACKER_STATE==ucExtiState)
	{
		ret=g_tAlarmOpr.GetState(&tAlarm);
		tAlarm.CutTrackerAlarm=1;
		ret&=g_tAlarmOpr.SetState(tAlarm);	
		OSSemPost(ptOsSemUsedForAlarmTask);
	}
	else
	{
		ret=FALSE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: CutStateCheckRelieveAlarmDev
-Description	: CutStateCheckRelieveAlarmDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/20   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 CutStateCheckRelieveAlarmDev(void)
{
	u8 ret=FALSE;
	u8 ucExtiState=0;
	T_Alarm tAlarm={0};
	OSTimeDly(2);//延时20毫秒消除抖动
	ReadCutStateDev(&ucExtiState);
	if(UN_CUT_TRACKER_STATE==ucExtiState)
	{
		ret=g_tAlarmOpr.GetState(&tAlarm);
		tAlarm.CutTrackerAlarm=0;
		ret&=g_tAlarmOpr.SetState(tAlarm);	
	}
	else
	{
		ret=FALSE;
	}
	return ret;
}





