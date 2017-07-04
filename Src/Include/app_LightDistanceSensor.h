/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 app_LightDistanceSensor.h
* Description		: 	Motor application operation center
* Created			: 	2016.10.13.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _APP_LIGHT_DISTANCE_SENSOR_H
#define _APP_LIGHT_DISTANCE_SENSOR_H

#include "cBasicTypeDefine.h"

typedef struct LightDistanceSensorDevManage
{
	char *name;
	void (*DevInit)(void);
	u8 (*DevConfig)(void);
	u8 (*DevSetAlarm)(void);	
	u8 (*DevRelieveAlarm)(void);
	u8 (*DevSetNearFlagMode)(void);	
	u8 (*DevSetFarFlagMode)(void);
	u8 (*DevReadData)(u16* o_wLedIR,u16* o_wPS,u16* o_wALS);
	struct LightDistanceSensorDevManage *ptNext;
}T_LightDistanceSensorDevManage,*PT_LightDistanceSensorDevManage;

void RegisterLightDistanceSensorDev(PT_LightDistanceSensorDevManage i_ptMotorDev);
void LightDistanceSensorDevInit();

typedef struct LightDistanceSensorOpr
{
	void (*Init)(void);
	u8 (*Config)(void);
	u8 (*CheckDropState)(void);
	u8 (*GetSensorData)(u16* o_wLedIR,u16* o_wPS,u16* o_wALS);
}T_LightDistanceSensorOpr,*PT_LightDistanceSensorOpr;

extern T_LightDistanceSensorOpr  const g_tLightDistanceSensor;

#endif




