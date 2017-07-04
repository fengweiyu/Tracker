/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_I2C.c
* Description		: 	I2C Drive operation center
* Created			: 	2016.10.13.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "stm32f4xx_hal.h"
#include "CBasicTools.h"
#include "Config.h"
#include "dev_LightDistanceSensor.h"
#include "core_CM4.h"
#include "ucos_ii.h"

static void I2C_DevConfig(void);
static void I2C_Start();
static void I2C_SendByte(u8 i_ucData);
static u8 I2C_ReadByte(u8 i_ucAck);
static u8 I2C_WaitAck();
static void I2C_Stop();

static T_I2C_DevManage g_tI2C_Dev ={
	.name="I2C_Dev",
	.DevConfig		=I2C_DevConfig,
	.DevI2C_Start		=I2C_Start,	
	.DevI2C_SendByte	=I2C_SendByte,
	.DevI2C_ReadByte	=I2C_ReadByte,
	.DevI2C_WaitAck	=I2C_WaitAck,
	.DevI2C_Stop		=I2C_Stop,
};
//IO方向设置
#define SDA_IN()     {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9输入模式
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9输出模式

/*****************************************************************************
-Fuction		: I2C_DevInit
-Description	: I2C_DevInit
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void I2C_DevInit()
{
	RegisterI2C_Dev(&g_tI2C_Dev);
}

/*****************************************************************************
-Fuction		: DelayUs
-Description	: DelayUs
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void DelayUs(u8 i_ucTime)
 {
	u16 wTime = 75*i_ucTime;
	while(wTime--);
}
//延时nus
//nus:要延时的us数.	
//nus:0~190887435(最大值即2^32/fac_us@fac_us=22.5)	    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*84; 						//系统时钟晶振为84M晶振 
	OSSchedLock();					//阻止OS调度，防止打断us延时
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
	OSSchedUnlock();					//恢复OS调度											    
}

/*****************************************************************************
-Fuction		: I2C_SDA_SET
-Description	: I2C_SDA_SET
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 I2C_SDA_SET(u8 i_ucSetValue)
{
	u8 ret=FALSE;
	if(GPIO_PIN_SET==i_ucSetValue)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);	//对应引脚PB
		ret=TRUE;
	}
	else if(GPIO_PIN_RESET==i_ucSetValue)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);	//对应引脚PB		
		ret=TRUE;
	}
	else
	{
		ret=FALSE;
		DebugPrintf(ERR"I2C_SDA_SET format err\r\n");
	}
	return ret;
}
/*****************************************************************************
-Fuction		: I2C_SDA_READ
-Description	: I2C_SDA_READ
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 I2C_SDA_READ()
{
	u8 ret=0;
	ret=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9);
	return ret;
}
/*****************************************************************************
-Fuction		: I2C_SCL_SET
-Description	: I2C_SCL_SET
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 I2C_SCL_SET(u8 i_ucSetValue)
{
	u8 ret=FALSE;
	if(GPIO_PIN_SET==i_ucSetValue)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);	//对应引脚PB
		ret=TRUE;
	}
	else if(GPIO_PIN_RESET==i_ucSetValue)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);	//对应引脚PB		
		ret=TRUE;
	}
	else
	{
		ret=FALSE;
		DebugPrintf(ERR"I2C_SCL_SET format err\r\n");
	}
	return ret;
}

/*****************************************************************************
-Fuction		: I2C_DevConfig
-Description	: I2C_DevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void I2C_DevConfig(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();	//使能GPIOH时钟
	
	//PB8,9初始化设置
	GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9;
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_FAST; 	//快速
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	I2C_SDA_SET(1);
	I2C_SCL_SET(1);
}
/*****************************************************************************
-Fuction		: I2C_Start
-Description	: I2C_Start//产生IIC起始信号
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void I2C_Start()
{
	SDA_OUT();	   //sda线输出
	I2C_SDA_SET(1);
	I2C_SCL_SET(1);
	DelayUs(4);
	I2C_SDA_SET(0);//START:when CLK is high,DATA change form high to low 
	DelayUs(4);
	I2C_SCL_SET(0);//钳住I2C总线，准备发送或接收数据 
}
/*****************************************************************************
-Fuction		: I2C_Stop
-Description	: I2C_Stop//产生IIC起始信号
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void I2C_Stop()
{
	SDA_OUT();//sda线输出
	I2C_SCL_SET(0);
	I2C_SDA_SET(0);//STOP:when CLK is high DATA change form low to high
	DelayUs(4);
	I2C_SCL_SET(1); 
	I2C_SDA_SET(1);//发送I2C总线结束信号
	DelayUs(4);								
}
/*****************************************************************************
-Fuction		: I2C_AckGenerate
-Description	: I2C_AckGenerate////产生ACK应答
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void I2C_AckGenerate()
{
	I2C_SCL_SET(0);
	SDA_OUT();
	I2C_SDA_SET(0);
	DelayUs(2);
	I2C_SCL_SET(1);
	DelayUs(2);
	I2C_SCL_SET(0);
}
/*****************************************************************************
-Fuction		: I2C_AckGenerate
-Description	: I2C_AckGenerate//////不产生ACK应答
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void I2C_AckNoGenerate()
{
	I2C_SCL_SET(0);
	SDA_OUT();
	I2C_SDA_SET(1);
	DelayUs(2);
	I2C_SCL_SET(1);
	DelayUs(2);
	I2C_SCL_SET(0);
}
/*****************************************************************************
-Fuction		: I2C_SendByte
-Description	: I2C_SendByte////
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void I2C_SendByte(u8 i_ucData)
{
	u8 c;   
	SDA_OUT(); 	    
	I2C_SCL_SET(0);//拉低时钟开始数据传输
	for(c=0;c<8;c++)
	{              
		I2C_SDA_SET((i_ucData&0x80)>>7);
		i_ucData<<=1; 	  
		DelayUs(2);   //对TEA5767这三个延时都是必须的
		I2C_SCL_SET(1);
		DelayUs(2); 
		I2C_SCL_SET(0);	
		DelayUs(2);
	}
}
/*****************************************************************************
-Fuction		: I2C_OnDev
-Description	: I2C_OnDev
//等待应答信号到来
//返回值：0，接收应答失败
//        1，接收应答成功

-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 I2C_WaitAck()
{
	u8 ucErrTime=0;
	u8 ret=FALSE;
	SDA_IN();	   //SDA设置为输入	
	I2C_SDA_SET(1); 
	DelayUs(1);	   
	I2C_SCL_SET(1); 
	DelayUs(1);	 
	while(I2C_SDA_READ())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop();
			ret=FALSE;
			return ret;  
		}
	}
	I2C_SCL_SET(0);//时钟输出0	
	ret=TRUE;
	return ret;  
}

/*****************************************************************************
-Fuction		: I2C_ReadByte
-Description	: I2C_ReadByte////
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/13	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 I2C_ReadByte(u8 i_ucAck)
{
	u8 c;
	u8 ucReceive=0;
	SDA_IN();//SDA设置为输入
	for(c=0;c<8;c++ )
	{
		I2C_SCL_SET(0); 
		DelayUs(2);
		I2C_SCL_SET(1);
		ucReceive<<=1;
		if(I2C_SDA_READ())
		ucReceive++;   
		DelayUs(1); 
	}					 
	if (!i_ucAck)
	    I2C_AckNoGenerate();//发送nACK
	else
	    I2C_AckGenerate(); //发送ACK   
	return ucReceive;
}

