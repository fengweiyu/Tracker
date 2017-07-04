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

	__HAL_RCC_GPIOC_CLK_ENABLE();               //����GPIOCʱ��
        
	GPIO_Initure.Pin=GPIO_PIN_0; //pc0
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;	//�������
	GPIO_Initure.Pull=GPIO_NOPULL;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	//����
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);	//P0��1��Ĭ��
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
    
    __HAL_RCC_GPIOC_CLK_ENABLE();               //����GPIOCʱ��
        
    GPIO_Initure.Pin=GPIO_PIN_2;               //PC2
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
        
    //�ж���2
    HAL_NVIC_SetPriority(EXTI2_IRQn,1,3);   //��ռ���ȼ�Ϊ2�������ȼ�Ϊ3
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);         //ʹ���ж���2
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
//IICдһ���ֽ�  
//reg:�Ĵ�����ַ
//data:Ҫд�������
//����ֵ:true,����
//    ����,�������
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
	I2C_SendByteDev(STK3311X_ADDR|0X00);//����������ַ+д����
	I2C_WaitAckDev(&ucAck);
	if(FALSE==ucAck)          //�ȴ�Ӧ��
	{
		I2C_StopDev();
		ret=FALSE;
	}
	else
	{
		I2C_SendByteDev(i_ucReg); 		//д�Ĵ�����ַ
		I2C_WaitAckDev(&ucAck); 			//�ȴ�Ӧ��
		I2C_SendByteDev(i_ucData);		//��������
		I2C_WaitAckDev(&ucAck);
		if(FALSE==ucAck)			//�ȴ�ACK
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
//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
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
	I2C_SendByteDev(STK3311X_ADDR|0X00); //����������ַ+д����
	I2C_WaitAckDev(&ucAck);             //�ȴ�Ӧ��
	I2C_SendByteDev(i_ucReg);         //д�Ĵ�����ַ
	I2C_WaitAckDev(&ucAck);             //�ȴ�Ӧ��
	I2C_StartDev();                
	I2C_SendByteDev(STK3311X_ADDR|0X01); //����������ַ+������
	I2C_WaitAckDev(&ucAck);             //�ȴ�Ӧ��
	//if(TRUE==ucAck)
	//OSTimeDly(1);
	ret=I2C_ReadByteDev(0,o_pucData);		//������,����nACK  
	I2C_StopDev();                 //����һ��ֹͣ����
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
	I2C_StopDev();//�ȷ���ֹͣ��ֹϵͳ��λ�˴��������ڹ���ûֹͣ������
	LightDistanceSensorSendByte(STK3311X_RESET_REG_ADDR,0X99);	//д���������ݸ�λ
	OSTimeDly(5);						//��λ����10ms   �������ﵼ�µĸ�λ��ʹ�ÿ����ʱ��ѭ������ʱ���Ǹ�λ
	LightDistanceSensorReadByte(STK3311X_ID_REG_ADDR,&temp);		//��ȡ�ո�д��ȥ��0X03
	if(temp!=STK3311X_ID_INFO)
	{
		ret=FALSE;			//ʧ��
		DebugPrintf(ERR"Init LightDistanceSensor err:%d\r\n",temp);
	}
	else
	{
		ret=TRUE;//����		
		DebugPrintf("LightDistanceSensor id :%xh\r\n",temp);
		temp=0;
	}	
	
	LightDistanceSensorSendByte(STK3311X_STATE_REG_ADDR,STK3311X_POWER_STANDBY_MODE);	//�����͹���ģʽ
	LightDistanceSensorSendByte(STK3311X_LED_REG_ADDR,STK3311X_LED_LOWER_POWER_MODE);	
	LightDistanceSensorSendByte(STK3311X_PS_CTRL_REG_ADDR,STK3311X_PS_UPDATE_DATA_TIME);
	//LightDistanceSensorSendByte(STK3311X_ALS_CTRL_REG_ADDR,0x07);
	
	LightDistanceSensorSendByte(STK3311X_STATE_REG_ADDR,STK3311X_PS_WAIT_ENABLE);	//�������е�������Ǻܿ�����
	LightDistanceSensorSendByte(STK3311X_WAIT_REG_ADDR,STK3311X_WAIT_UPDATE_DATA_TIME);//Ҫ�õȴ�ʱ��д�룬����Ҳ����ȴ�ģʽ����Ȼ��ȡ��0
	LightDistanceSensorSendByte(STK3311X_INT_REG_ADDR,STK3311X_PS_INT_FLAG_NEAR_MODE_SET);	//ʹ�ÿ����͵�ƽ���͹��ģ���Ϊһ��ʼ�Ǹߣ��͵�ƽ���ж�300uA
	//LightDistanceSensorSendByte(STK3311X_WAIT_REG_ADDR,STK3311X_WAIT_UPDATE_DATA_TIME);//������������һ�зź���\
	//LightDistanceSensorSendByte(STK3311X_STATE_REG_ADDR,STK3311X_ALS_PS_WAIT_ENABLE);	//��ȡ������ֵ����0,�ж�������Ǹ�
	OSTimeDlyHMSM(0,0,0,1);
	for(c=0;c<30;c++)//����ȡ30�����ݶ���
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
	OSTimeDly(2);//��ʱ20������������
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
	OSTimeDly(2);//��ʱ20������������
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

	//HAL_NVIC_SetPriority(EXTI2_IRQn,1,3);   //��ռ���ȼ�Ϊ2�������ȼ�Ϊ3
	//HAL_NVIC_EnableIRQ(EXTI2_IRQn);         //ʹ���ж���2
	LightDistanceSensorSendByte(STK3311X_INT_REG_ADDR,STK3311X_PS_INT_FLAG_NEAR_MODE_SET);	//�����ж�ģʽ
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
    	//GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
    	//GPIO_Initure.Pull=GPIO_PULLUP;
    	//HAL_GPIO_Init(GPIOC,&GPIO_Initure);

	//HAL_NVIC_SetPriority(EXTI2_IRQn,1,3);   //��ռ���ȼ�Ϊ2�������ȼ�Ϊ3
	//HAL_NVIC_EnableIRQ(EXTI2_IRQn);         //ʹ���ж���2
	LightDistanceSensorSendByte(STK3311X_INT_REG_ADDR,STK3311X_PS_INT_FLAG_FAR_MODE_SET);	//�����ж�ģʽ
	ret=TRUE;
	return ret;
}


/*****************************************************************************
-Fuction		: LightDistanceSensorReadData
-Description	: LightDistanceSensorReadData
//��ȡAP3216C������
//��ȡԭʼ���ݣ�����ALS,PS��IR
//ע�⣡���ͬʱ��ALS,IR+PS�Ļ��������ݶ�ȡ��ʱ����Ҫ����112.5ms
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
	* o_wPS=(u16)((ucBuf[0]<<8)|(ucBuf[1]));//��ȡPS������������  
	
	memset(ucBuf,0,sizeof(ucBuf));
	ret&=LightDistanceSensorReadByte(STK3311X_DATA1_ALS_REG_ADDR,&ucBuf[0]);
	ret&=LightDistanceSensorReadByte(STK3311X_DATA2_ALS_REG_ADDR,&ucBuf[1]);
	* o_wALS=(u16)((ucBuf[0]<<8)|(ucBuf[1]));//��ȡALS������������ 

	memset(ucBuf,0,sizeof(ucBuf));
	//ret&=LightDistanceSensorReadByte(STK3311X_DATA1_IR_REG_ADDR,&ucBuf[0]);
	ret&=LightDistanceSensorReadByte(STK3311X_FLAG_REG_ADDR,&ucBuf[1]);
	* o_wLedIR=(u16)(ucBuf[1]&0x01);//��ȡIR������������
	
	
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



