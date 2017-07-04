/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_LightDistanceSensor.c
* Description		: 	LightDistanceSensor application operation center
* Created			: 	2016.10.13.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "CBasicTools.h"
#include "Config.h"
#include "dev_LightDistanceSensor.h"
#include "app_LightDistanceSensor.h"
#include "ucos_ii.h"
#include "main.h"
#include "Alarm.h"

static PT_I2C_DevManage g_ptI2C_DevHead=NULL;

static u8 I2C_ConfigDev(void);
static u8 I2C_StartDev(void);
static u8 I2C_SendByteDev(u8 i_ucData);
static u8 I2C_WaitAckDev(u8 *o_pucAck);
static u8 I2C_ReadByteDev(u8 i_ucAck,u8 *o_pucData);
static u8 I2C_StopDev(void);

static u8 LightDistanceSensorDevConfig(void);
static u8 LightDistanceSensorSetAlarmDev(void);
static u8 LightDistanceSensorRelieveAlarmDev(void);
static u8 LightDistanceSensorSetNearFlagModeDev();
static u8  LightDistanceSensorSetFarFlagModeDev();
static u8 LightDistanceSensorReadDataDev(u16* o_wLedIR,u16* o_wPS,u16* o_wALS);

static  T_LightDistanceSensorDevManage g_tLightDistanceSensorDev ={
	.name="LightDistanceSensorDev",
	.DevInit						=LightDistanceSensorDevInit,
	.DevConfig					=LightDistanceSensorDevConfig,
	.DevSetAlarm					=LightDistanceSensorSetAlarmDev,
	.DevRelieveAlarm				=LightDistanceSensorRelieveAlarmDev,
	.DevSetNearFlagMode			=LightDistanceSensorSetNearFlagModeDev,
	.DevSetFarFlagMode			=LightDistanceSensorSetFarFlagModeDev,
	.DevReadData					=LightDistanceSensorReadDataDev,
};
/*****************************************************************************
-Fuction		: LightDistanceSensorInit
-Description	: LightDistanceSensorInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void LightDistanceSensorDevInit()
{
	I2C_DevInit();
	RegisterLightDistanceSensorDev(&g_tLightDistanceSensorDev);
}
/*****************************************************************************
-Fuction		: LightDistanceSensorPowerInit
-Description	: LightDistanceSensorPowerInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void LightDistanceSensorPowerInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOC_CLK_ENABLE();               //开启GPIOC时钟
        
	GPIO_Initure.Pin=GPIO_PIN_0; //pc0
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_Initure.Pull=GPIO_NOPULL;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//高速
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);	//P0置1，默认
}

/*****************************************************************************
-Fuction		: LightDistanceSensorExtiInit
-Description	: LightDistanceSensorExtiInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void LightDistanceSensorExtiInit(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();               //开启GPIOC时钟
        
    GPIO_Initure.Pin=GPIO_PIN_2;               //PC2
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
        
    //中断线2
    HAL_NVIC_SetPriority(EXTI2_IRQn,1,3);   //抢占优先级为2，子优先级为3
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);         //使能中断线2
}
/*****************************************************************************
-Fuction		: LightDistanceSensorReadExtiState
-Description	: LightDistanceSensorReadExtiState
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorReadExtiState(u8 *o_pucExtiState)
{
	u8 ret=FALSE;
	*o_pucExtiState=HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_2); //
	ret=TRUE;
	return ret;
}

/*****************************************************************************
-Fuction		: LightDistanceSensorSendByte
-Description	: LightDistanceSensorSendByte
//IIC写一个字节  
//reg:寄存器地址
//data:要写入的数据
//返回值:true,正常
//    其他,错误代码
-Input			: i_ucReg i_ucData
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorSendByte(u8 i_ucReg,u8 i_ucData)
{
	u8 ret=FALSE;
	u8 ucAck=0;
	I2C_StartDev();
	I2C_SendByteDev(STK3311X_ADDR|0X00);//发送器件地址+写命令
	I2C_WaitAckDev(&ucAck);
	if(FALSE==ucAck)          //等待应答
	{
		I2C_StopDev();
		ret=FALSE;
	}
	else
	{
		I2C_SendByteDev(i_ucReg); 		//写寄存器地址
		I2C_WaitAckDev(&ucAck); 			//等待应答
		I2C_SendByteDev(i_ucData);		//发送数据
		I2C_WaitAckDev(&ucAck);
		if(FALSE==ucAck)			//等待ACK
		{
			I2C_StopDev();
			ret=FALSE;
		}
		else
		{
			I2C_StopDev();
			ret=TRUE;
		}
	}
	return ret;
}

/*****************************************************************************
-Fuction		: LightDistanceSensorReadByte
-Description	: LightDistanceSensorReadByte
//IIC读一个字节 
//reg:寄存器地址 
-Input			: i_ucReg i_ucData
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorReadByte(u8 i_ucReg,u8 *o_pucData)
{
	u8 ret=FALSE;
	u8 ucAck=0;
	I2C_StartDev();
	I2C_SendByteDev(STK3311X_ADDR|0X00); //发送器件地址+写命令
	I2C_WaitAckDev(&ucAck);             //等待应答
	I2C_SendByteDev(i_ucReg);         //写寄存器地址
	I2C_WaitAckDev(&ucAck);             //等待应答
	I2C_StartDev();                
	I2C_SendByteDev(STK3311X_ADDR|0X01); //发送器件地址+读命令
	I2C_WaitAckDev(&ucAck);             //等待应答
	//if(TRUE==ucAck)
	//OSTimeDly(1);
	ret=I2C_ReadByteDev(0,o_pucData);		//读数据,发送nACK  
	I2C_StopDev();                 //产生一个停止条件
	return ret;
}

/*****************************************************************************
-Fuction		: LightDistanceSensorConfig
-Description	: LightDistanceSensorConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorDevConfig(void)
{
	u8 ret=FALSE;
	u8 temp=0;	
	u8 ucBuf[2]={0};
	u16 wPsData=0;
	u16 wTHDH=0;
	u16 wTHDL=0;
	u8 c=0;
	I2C_ConfigDev();
	
	LightDistanceSensorPowerInit();
	I2C_StopDev();//先发个停止防止系统复位了传感器还在工作没停止的意外
	LightDistanceSensorSendByte(STK3311X_RESET_REG_ADDR,0X99);	//写入任意数据复位
	OSTimeDly(5);						//复位至少10ms   不是这里导致的复位，使用库的延时和循环的延时还是复位
	LightDistanceSensorReadByte(STK3311X_ID_REG_ADDR,&temp);		//读取刚刚写进去的0X03
	if(temp!=STK3311X_ID_INFO)
	{
		ret=FALSE;			//失败
		DebugPrintf(ERR"Init LightDistanceSensor err:%d\r\n",temp);
	}
	else
	{
		ret=TRUE;//正常		
		DebugPrintf("LightDistanceSensor id :%xh\r\n",temp);
		temp=0;
	}	
	
	LightDistanceSensorSendByte(STK3311X_STATE_REG_ADDR,STK3311X_POWER_STANDBY_MODE);	//开启低功耗模式
	LightDistanceSensorSendByte(STK3311X_LED_REG_ADDR,STK3311X_LED_LOWER_POWER_MODE);	
	LightDistanceSensorSendByte(STK3311X_PS_CTRL_REG_ADDR,STK3311X_PS_UPDATE_DATA_TIME);
	//LightDistanceSensorSendByte(STK3311X_ALS_CTRL_REG_ADDR,0x07);
	
	LightDistanceSensorSendByte(STK3311X_STATE_REG_ADDR,STK3311X_PS_WAIT_ENABLE);	//放上面有低输出但是很快上升
	LightDistanceSensorSendByte(STK3311X_WAIT_REG_ADDR,STK3311X_WAIT_UPDATE_DATA_TIME);//要用等待时间写入，上面也必须等待模式，不然读取都0
	LightDistanceSensorSendByte(STK3311X_INT_REG_ADDR,STK3311X_PS_INT_FLAG_NEAR_MODE_SET);	//使用靠近低电平降低功耗，因为一开始是高，低电平会有多300uA
	//LightDistanceSensorSendByte(STK3311X_WAIT_REG_ADDR,STK3311X_WAIT_UPDATE_DATA_TIME);//这里包括下面的一行放后面\
	//LightDistanceSensorSendByte(STK3311X_STATE_REG_ADDR,STK3311X_ALS_PS_WAIT_ENABLE);	//读取出来的值都是0,中断输出都是高
	OSTimeDlyHMSM(0,0,0,1);
	for(c=0;c<30;c++)//连续取30笔数据丢掉
	{
		LightDistanceSensorReadByte(STK3311X_DATA1_PS_REG_ADDR,&ucBuf[0]);
		LightDistanceSensorReadByte(STK3311X_DATA2_PS_REG_ADDR,&ucBuf[1]);		
		wPsData=(u16)((ucBuf[0]<<8)|(ucBuf[1]));
	}
	LightDistanceSensorReadByte(STK3311X_DATA1_PS_REG_ADDR,&ucBuf[0]);
	LightDistanceSensorReadByte(STK3311X_DATA2_PS_REG_ADDR,&ucBuf[1]);
	wPsData=(u16)((ucBuf[0]<<8)|(ucBuf[1]));
	
	wTHDL=wPsData+STK3311X_PS_MAX_VALUE;
	wTHDH=wPsData+STK3311X_PS_MAX_VALUE+20;
	ucBuf[0]=(wTHDL&0xff00)>>8;	
	ucBuf[1]=(wTHDL&0x00ff);	
	LightDistanceSensorSendByte(STK3311X_THDL1_PS_REG_ADDR,ucBuf[0]);	
	LightDistanceSensorSendByte(STK3311X_THDL2_PS_REG_ADDR,ucBuf[1]);	
	ucBuf[0]=(wTHDH&0xff00)>>8;	
	ucBuf[1]=(wTHDH&0x00ff);	
	LightDistanceSensorSendByte(STK3311X_THDH1_PS_REG_ADDR,ucBuf[0]);	
	LightDistanceSensorSendByte(STK3311X_THDH2_PS_REG_ADDR,ucBuf[1]);
	
	//LightDistanceSensorSendByte(STK3311X_WAIT_REG_ADDR,STK3311X_WAIT_UPDATE_DATA_TIME);
	//LightDistanceSensorSendByte(STK3311X_STATE_REG_ADDR,STK3311X_PS_WAIT_ENABLE);	//

	LightDistanceSensorExtiInit();

	return ret;
}

/*****************************************************************************
-Fuction		: LightDistanceSensorSetAlarmDev
-Description	: LightDistanceSensorSetAlarmDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/18   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorSetAlarmDev(void)
{
	u8 ret=FALSE;
	u8 ucExtiState=0;
	T_Alarm tAlarm={0};
	OSTimeDly(2);//延时20毫秒消除抖动
	LightDistanceSensorReadExtiState(&ucExtiState);
	if(STK3311X_PS_INT_IS_VALID==ucExtiState)
	{
		ret=g_tAlarmOpr.GetState(&tAlarm);
		tAlarm.DropTrackerAlarm=1;
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
-Fuction		: LightDistanceSensorRelieveAlarmDev
-Description	: LightDistanceSensorRelieveAlarmDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/18   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorRelieveAlarmDev(void)
{
	u8 ret=FALSE;
	u8 ucExtiState=0;
	T_Alarm tAlarm={0};
	OSTimeDly(2);//延时20毫秒消除抖动
	LightDistanceSensorReadExtiState(&ucExtiState);
	if(STK3311X_PS_INT_IS_VALID==ucExtiState)
	{
		ret=g_tAlarmOpr.GetState(&tAlarm);
		tAlarm.DropTrackerAlarm=0;
		ret&=g_tAlarmOpr.SetState(tAlarm);	
	}
	else
	{
		ret=FALSE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: LightDistanceSensorSetRiseEXTIDev
-Description	: LightDistanceSensorSetRiseEXTIDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorSetNearFlagModeDev()
{
	u8 ret=FALSE;
    	//GPIO_InitTypeDef GPIO_Initure;    
  	//GPIO_Initure.Pin=GPIO_PIN_2;               //PC3
    	//GPIO_Initure.Mode=GPIO_MODE_IT_RISING;     //
    	//GPIO_Initure.Pull=GPIO_PULLDOWN;
    	//HAL_GPIO_Init(GPIOC,&GPIO_Initure);

	//HAL_NVIC_SetPriority(EXTI2_IRQn,1,3);   //抢占优先级为2，子优先级为3
	//HAL_NVIC_EnableIRQ(EXTI2_IRQn);         //使能中断线2
	LightDistanceSensorSendByte(STK3311X_INT_REG_ADDR,STK3311X_PS_INT_FLAG_NEAR_MODE_SET);	//设置中断模式
	ret=TRUE;
	return ret;
}
/*****************************************************************************
-Fuction		: LightDistanceSensorSetFallEXTIDev
-Description	: LightDistanceSensorSetFallEXTIDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/26	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8  LightDistanceSensorSetFarFlagModeDev()
{
	u8 ret=FALSE;
    	//GPIO_InitTypeDef GPIO_Initure;    
  	//GPIO_Initure.Pin=GPIO_PIN_2;               //PC3
    	//GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    	//GPIO_Initure.Pull=GPIO_PULLUP;
    	//HAL_GPIO_Init(GPIOC,&GPIO_Initure);

	//HAL_NVIC_SetPriority(EXTI2_IRQn,1,3);   //抢占优先级为2，子优先级为3
	//HAL_NVIC_EnableIRQ(EXTI2_IRQn);         //使能中断线2
	LightDistanceSensorSendByte(STK3311X_INT_REG_ADDR,STK3311X_PS_INT_FLAG_FAR_MODE_SET);	//设置中断模式
	ret=TRUE;
	return ret;
}


/*****************************************************************************
-Fuction		: LightDistanceSensorReadData
-Description	: LightDistanceSensorReadData
//读取AP3216C的数据
//读取原始数据，包括ALS,PS和IR
//注意！如果同时打开ALS,IR+PS的话两次数据读取的时间间隔要大于112.5ms
-Input			:  
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 LightDistanceSensorReadDataDev(u16* o_wLedIR,u16* o_wPS,u16* o_wALS)
{
	u8 ret=FALSE;
	u8 ucBuf[2]={0};

	ret=LightDistanceSensorReadByte(STK3311X_DATA1_PS_REG_ADDR,&ucBuf[0]);
	ret&=LightDistanceSensorReadByte(STK3311X_DATA2_PS_REG_ADDR,&ucBuf[1]);
	* o_wPS=(u16)((ucBuf[0]<<8)|(ucBuf[1]));//读取PS传感器的数据  
	
	memset(ucBuf,0,sizeof(ucBuf));
	ret&=LightDistanceSensorReadByte(STK3311X_DATA1_ALS_REG_ADDR,&ucBuf[0]);
	ret&=LightDistanceSensorReadByte(STK3311X_DATA2_ALS_REG_ADDR,&ucBuf[1]);
	* o_wALS=(u16)((ucBuf[0]<<8)|(ucBuf[1]));//读取ALS传感器的数据 

	memset(ucBuf,0,sizeof(ucBuf));
	//ret&=LightDistanceSensorReadByte(STK3311X_DATA1_IR_REG_ADDR,&ucBuf[0]);
	ret&=LightDistanceSensorReadByte(STK3311X_FLAG_REG_ADDR,&ucBuf[1]);
	* o_wLedIR=(u16)(ucBuf[1]&0x01);//读取IR传感器的数据
	
	
	return ret;
}
/*****************************************************************************
-Fuction		: I2C_ConfigDev
-Description	: I2C_ConfigDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 I2C_ConfigDev(void)
{
	u8 ret=0;
	PT_I2C_DevManage ptTmp = g_ptI2C_DevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
		DebugPrintf(ERR"g_ptI2C_DevHead I2C_ConfigDev null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->DevConfig();
			ptTmp = ptTmp->ptNext;
		}	
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: I2C_StartDev
-Description	: I2C_StartDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 I2C_StartDev(void)
{
	u8 ret=0;
	PT_I2C_DevManage ptTmp = g_ptI2C_DevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
		DebugPrintf(ERR"g_ptI2C_DevHead I2C_StartDev null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->DevI2C_Start();
			ptTmp = ptTmp->ptNext;
		}	
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: I2C_SendByteDev
-Description	: I2C_SendByteDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 I2C_SendByteDev(u8 i_ucData)
{
	u8 ret=0;
	PT_I2C_DevManage ptTmp = g_ptI2C_DevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;		
		DebugPrintf(ERR"g_ptI2C_DevHead I2C_SendByteDev null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->DevI2C_SendByte(i_ucData);
			ptTmp = ptTmp->ptNext;
		}	
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: I2C_SendByteDev
-Description	: I2C_SendByteDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 I2C_WaitAckDev(u8 *o_pucAck)
{
	u8 ret=FALSE;
	PT_I2C_DevManage ptTmp = g_ptI2C_DevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;		
		DebugPrintf(ERR"g_ptI2C_DevHead I2C_WaitAckDev null\r\n");
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			*o_pucAck=ptTmp->DevI2C_WaitAck();
			ptTmp = ptTmp->ptNext;
		}		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: I2C_SendByteDev
-Description	: I2C_SendByteDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 I2C_ReadByteDev(u8 i_ucAck,u8 *o_pucData)
{
	u8 ret=FALSE;
	PT_I2C_DevManage ptTmp = g_ptI2C_DevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
		DebugPrintf(ERR"g_ptI2C_DevHead I2C_ReadByteDev null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			*o_pucData=ptTmp->DevI2C_ReadByte(i_ucAck);
			ptTmp = ptTmp->ptNext;
		}		
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: I2C_StopDev
-Description	: I2C_StopDev
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 I2C_StopDev(void)
{
	u8 ret=0;
	PT_I2C_DevManage ptTmp = g_ptI2C_DevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;		
		DebugPrintf(ERR"g_ptI2C_DevHead I2C_StopDev null\r\n");
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->DevI2C_Stop();
			ptTmp = ptTmp->ptNext;
		}	
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: PT_I2C_DevManage
-Description	: PT_I2C_DevManage
-Input			: i_ptI2C_Dev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterI2C_Dev(PT_I2C_DevManage i_ptI2C_Dev)
{
	PT_I2C_DevManage ptTmp;
	if (!g_ptI2C_DevHead)
	{
		g_ptI2C_DevHead   = i_ptI2C_Dev;
		g_ptI2C_DevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptI2C_DevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptI2C_Dev;
		i_ptI2C_Dev->ptNext = NULL;
	}
}



