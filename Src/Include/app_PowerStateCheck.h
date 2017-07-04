/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 PowerStateCheck.c
* Description		: 	PowerStateCheck application operation center
* Created			: 	2016.09.23.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_POWER_STATE_CHECK_H
#define _APP_POWER_STATE_CHECK_H

#include "cBasicTypeDefine.h"

#define ADC_VREF_VOL 				(3.3)   /* 3.3V  *///基准电压为3.3V也就是最大量程是3.3
#define ADC_FULL_VAL 				4095   /* 0xFFF *///12位的ADC 最大的数字量是4095
#define ADC_RESISTOR_VAL			20//采样端分压电阻
#define VBAT_RESISTOR_VAL			(68+ADC_RESISTOR_VAL)//VBAT电阻

#define FULL_POWER_VALUE								(4.190)
#define NINETY_PERCENT_POWER_VALUE					(3.981)
#define EIGHTY_PERCENT_POWER_VALUE					(3.914)
#define SEVENTY_PERCENT_POWER_VALUE				(3.840)
#define SIXTY_PERCENT_POWER_VALUE					(3.794)
#define FIFTY_PERCENT_POWER_VALUE					(3.753)
#define FOURTY_PERCENT_POWER_VALUE				(3.710)
#define THIRTY_PERCENT_POWER_VALUE					(3.666)
#define TWENTY_PERCENT_POWER_VALUE				(3.624)
#define TEN_PERCENT_POWER_VALUE					(3.557)
#define FIVE_PERCENT_POWER_VALUE					(3.444)
#define ERROR_POWER_VALUE							(3.012)

#define REMAIN_PERCENT_POWER_VALUE_STEP			(5)//步进值

#define REMAIN_FULL_POWER_VALUE					(100)
#define REMAIN_NINETY_PERCENT_POWER_VALUE		(90)
#define REMAIN_EIGHTY_PERCENT_POWER_VALUE		(80)
#define REMAIN_SEVENTY_PERCENT_POWER_VALUE		(70)
#define REMAIN_SIXTY_PERCENT_POWER_VALUE			(60)
#define REMAIN_FIFTY_PERCENT_POWER_VALUE			(50)
#define REMAIN_FOURTY_PERCENT_POWER_VALUE		(40)
#define REMAIN_THIRTY_PERCENT_POWER_VALUE		(30)
#define REMAIN_TWENTY_PERCENT_POWER_VALUE		(20)
#define REMAIN_TEN_PERCENT_POWER_VALUE			(10)
#define REMAIN_FIVE_PERCENT_POWER_VALUE			(5)

#define GET_ADC_VAULE_NUM	20//电池采样次数

typedef enum PowerCheckStateType
{
	FullPowerState=0,
	NormalPowerState,
	FirstLowPowerState,
	SecondLowPowerState,
	ThirdLowPowerState,
	FourthLowPowerState,
}E_PowerCheckStateType;

typedef struct PowerStateCheckDevManage
{
	char *name;
	void (*DevConfig)(void);
	u8 (*DevGetAverageValue)(u8 i_ucTimes,float *o_pfAverPowerValue);
	struct PowerStateCheckDevManage *ptNext;
}T_PowerStateCheckDevManage,*PT_PowerStateCheckDevManage;

typedef struct PowerStateCheckOpr
{
	void (*Init)(void);
	u8 (*Config)(void);
	u8 (*GetPowerCheckState)(E_PowerCheckStateType *o_pePowerCheckState,u8 *o_ucRemainPowerValue);
}T_PowerStateCheckOpr,*PT_PowerStateCheckOpr;



void RegisterPowerStateCheckDev(PT_PowerStateCheckDevManage i_ptPowerStateCheckDev);
void PowerStateCheckDevInit();

extern T_PowerStateCheckOpr const g_tPowerStateCheck;

#endif


