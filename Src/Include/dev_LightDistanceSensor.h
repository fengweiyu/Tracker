/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_LightDistanceSensor.h
* Description		: 	Motor application operation center
* Created			: 	2016.10.13.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _DEV_LIGHT_DISTANCE_SEMSOR_H
#define _DEV_LIGHT_DISTANCE_SEMSOR_H

#include "cBasicTypeDefine.h"

#define STK3311X_ADDR						(0x90)	//0x48 7位左移一位
#define STK3311X_STATE_REG_ADDR			(0x00)
#define STK3311X_PS_CTRL_REG_ADDR		(0x01)
#define STK3311X_ALS_CTRL_REG_ADDR		(0x02)
#define STK3311X_LED_REG_ADDR			(0x03)
#define STK3311X_INT_REG_ADDR				(0x04)
#define STK3311X_WAIT_REG_ADDR			(0x05)
#define STK3311X_THDH1_PS_REG_ADDR		(0x06)
#define STK3311X_THDH2_PS_REG_ADDR		(0x07)
#define STK3311X_THDL1_PS_REG_ADDR		(0x08)
#define STK3311X_THDL2_PS_REG_ADDR		(0x09)
#define STK3311X_FLAG_REG_ADDR			(0x10)
#define STK3311X_DATA1_PS_REG_ADDR		(0x11)
#define STK3311X_DATA2_PS_REG_ADDR		(0x12)
#define STK3311X_DATA1_ALS_REG_ADDR		(0x13)
#define STK3311X_DATA2_ALS_REG_ADDR		(0x14)
#define STK3311X_DATA1_IR_REG_ADDR		(0x17)
#define STK3311X_DATA2_IR_REG_ADDR		(0x18)
#define STK3311X_ID_REG_ADDR				(0x3E)
#define STK3311X_RESET_REG_ADDR			(0x80)

#define STK3311X_ALS_PS_NO_WAIT_ENABLE			(0X03)//开启ALS、PS无等待
#define STK3311X_PS_NO_WAIT_ENABLE				(0X01)//开启ALS、PS无等待
#define STK3311X_INT_MODE_ENABLE_PS_ONLY		(0x11)
#define STK3311X_INT_MODE_ENABLE				(0x93)
#define STK3311X_LED_LOWER_POWER_MODE		(0xFF)//ff 100ma,BF 50ma,7F 25ma,3f 12.5
#define STK3311X_IR_ALS_PS_NO_WAIT_ENABLE		(0X83)//开启ALS、PS+IR 无等待但中断各个模式就无效了
#define STK3311X_IR_ALS_PS_WAIT_ENABLE			(0X87)//开启ALS、PS+IR 等待
#define STK3311X_PS_WAIT_ENABLE					(0X05)//开启pS等待
#define STK3311X_ALS_PS_WAIT_ENABLE				(0X07)//开启ALS、PS等待
#define STK3311X_WAIT_UPDATE_DATA_TIME			(0X0E)//02 17.8ms  07 50ms,0E 100ms  1=6ms
#define STK3311X_PS_UPDATE_DATA_TIME			(0X31)//高2位是转换精度，低4位是刷新时间，
#define STK3311X_ID_INFO							(0x12)
#define STK3311X_POWER_STANDBY_MODE			(0x00)
#define STK3311X_PS_INT_MODE_ENABLE				(0x06)
#define STK3311X_PS_INT_FAR_MODE_ENABLE		(0x06)
#define STK3311X_PS_INT_NEAR_MODE_ENABLE		(0x05)
#define STK3311X_PS_INT_FLAG_NEAR_MODE_SET		(0x03)//02 远离低电平，03靠近低电平
#define STK3311X_PS_INT_FLAG_FAR_MODE_SET		(0x02)//02 远离低电平，03靠近低电平
#define STK3311X_PS_MAX_VALUE						(0x100)//100 2cm  b0 4cm
#define STK3311X_PS_INT_IS_VALID					(0x00)
#define STK3311X_PS_INT_IS_NOT_VALID				(0x01)

typedef struct I2C_DevManage
{
	char *name;
	void (*DevConfig)(void);
	void (*DevI2C_Start)();
	void (*DevI2C_SendByte)(u8 i_ucData);	
	u8 (*DevI2C_ReadByte)(u8 i_ucAck);
	u8 (*DevI2C_WaitAck)();
	void (*DevI2C_Stop)();
	struct I2C_DevManage *ptNext;
}T_I2C_DevManage,*PT_I2C_DevManage;

void RegisterI2C_Dev(PT_I2C_DevManage i_ptI2C_Dev);

void I2C_DevInit();


#endif



