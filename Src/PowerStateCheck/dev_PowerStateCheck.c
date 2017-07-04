/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_PowerStateCheck.c
* Description		: 	PowerStateCheck Drive operation center
* Created			: 	2016.09.25.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "stm32f4xx_hal.h"
#include "CBasicTools.h"
#include "Config.h"
#include "app_PowerStateCheck.h"
#include "main.h"

static u16 GetAdcAverageValue(u8 i_ucTimes);

static void AdcDevConfig(void);
static u8 AdcGetAverageValue(u8 i_ucTimes,float *o_pfAverPowerValue);

static u16 g_wAdcValue[GET_ADC_VAULE_NUM]={0};

static ADC_HandleTypeDef ADC1_Handler;//ADC���
static T_PowerStateCheckDevManage g_tPowerStateCheckDev ={
	.name="PowerStateCheckDev",
	.DevConfig			=AdcDevConfig,
	.DevGetAverageValue	=AdcGetAverageValue,
};

/*****************************************************************************
-Fuction		: PowerStateCheckDevInit
-Description	: PowerStateCheckDevInit
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/25	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void PowerStateCheckDevInit()
{
	RegisterPowerStateCheckDev(&g_tPowerStateCheckDev);
}
/*****************************************************************************
-Fuction		: HAL_ADC_MspInit
-Description	: HAL_ADC_MspInit
//ADC�ײ��������������ã�ʱ��ʹ��
//�˺����ᱻHAL_ADC_Init()����
//hadc:ADC���
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/25	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC1_CLK_ENABLE();            //ʹ��ADC1ʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOCʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_5;            //PC5
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}

/*****************************************************************************
-Fuction		: AdcDevConfig
-Description	: AdcDevConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/25	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void AdcDevConfig(void)
{
    ADC1_Handler.Instance=ADC1;
    ADC1_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   //4��Ƶ��ADCCLK=PCLK2/4=90/4=22.5MHZ
    ADC1_Handler.Init.Resolution=ADC_RESOLUTION_12B;             //12λģʽ
    ADC1_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //�Ҷ���
    ADC1_Handler.Init.ScanConvMode=DISABLE;                      //��ɨ��ģʽ
    ADC1_Handler.Init.EOCSelection=DISABLE;                      //�ر�EOC�ж�
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;                //�ر�����ת��
    ADC1_Handler.Init.NbrOfConversion=1;                         //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;             //��ֹ����������ģʽ
    ADC1_Handler.Init.NbrOfDiscConversion=0;                     //����������ͨ����Ϊ0
    ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //�������
    ADC1_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//ʹ���������
    ADC1_Handler.Init.DMAContinuousRequests=DISABLE;             //�ر�DMA����
    HAL_ADC_Init(&ADC1_Handler);                                 //��ʼ�� 
}
/*****************************************************************************
-Fuction		: AdcGetValue
-Description	: AdcGetValue
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/25	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 AdcGetValue(u16 *o_pwAdcValue)
{
	u8 ret=FALSE;
    	ADC_ChannelConfTypeDef ADC1_ChanConf;
    
	ADC1_ChanConf.Channel=ADC_CHANNEL_15;                                   //ͨ��
	ADC1_ChanConf.Rank=1;                                       //��1�����У�����1
    	ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES;        //����ʱ��
    	ADC1_ChanConf.Offset=0;                 
   	HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);        //ͨ������
	
 	HAL_ADC_Start(&ADC1_Handler);                               //����ADC
	
    	ret=HAL_ADC_PollForConversion(&ADC1_Handler,10);                //��ѯת��
 	if(HAL_OK!=ret)
 	{
		ret=FALSE;
	}
	else
	{
		*o_pwAdcValue=(u16)HAL_ADC_GetValue(&ADC1_Handler); 		//�������һ��ADC1�������ת�����
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: AdcGetAverageValue
-Description	: AdcGetAverageValue
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/25	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 AdcGetAverageValue(u8 i_ucTimes,float *o_pfAverPowerValue)
{
	u8 ret=FALSE;
	u16 wPowerValue=0;
	u32 dwValueSum=0;
	u8 uct;
	if(i_ucTimes>GET_ADC_VAULE_NUM)
	{
		DebugPrintf(ERR"AdcGetAverageValue err:%d\r\n",i_ucTimes);
	}
	else
	{
		for(uct=0;uct<i_ucTimes;uct++)
		{
			ret=AdcGetValue(&wPowerValue);
			if(FALSE==ret)
			{
				DebugPrintf(ERR"AdcGetAverageValue err\r\n");
				break;
			}
			else
			{
				dwValueSum+=wPowerValue;
				OSTimeDly(5);
			}
		}
		wPowerValue=dwValueSum/i_ucTimes;//ADC�����ֵ��һ��������, û�е�λ��.��Ҫת��
		*o_pfAverPowerValue=(float)(VBAT_RESISTOR_VAL/(float)ADC_RESISTOR_VAL)*wPowerValue*(ADC_VREF_VOL/ADC_FULL_VAL);
		HAL_ADC_Stop(&ADC1_Handler);							   //�ر�ADC
	}
	return ret;
}

/*****************************************************************************
-Fuction		: GetAdcAverageValue
-Description	: ȥ�������Сȡƽ��ֵ
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/25	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u16 GetAdcAverageValue(u8 i_ucTimes)
{
	u32 dwValueSum=0;
	u16 wPowerValue=0;
	u8 ucNum=0;
	u8 ucCount=0;

	for(ucNum=0;ucNum<i_ucTimes;ucNum++)
	{
		for(ucCount=0;ucCount<i_ucTimes-ucNum-1;ucCount++)
		{
			if(g_wAdcValue[ucCount]<g_wAdcValue[ucCount+1])
			{
			}
			else
			{
				wPowerValue=g_wAdcValue[ucCount+1];
				g_wAdcValue[ucCount+1]=g_wAdcValue[ucCount];
				g_wAdcValue[ucCount]=wPowerValue;
			}
		}
	}
	for(ucNum=1;ucNum<i_ucTimes-1;ucNum++)//ȥ�������С
	{
		dwValueSum+=g_wAdcValue[ucNum];
	}
	wPowerValue=dwValueSum/i_ucTimes;
	
	return wPowerValue;
}

