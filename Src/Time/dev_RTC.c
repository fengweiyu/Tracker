/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 dev_RTC.c
* Description		: 	RTC Drive operation center
* Created			: 	2016.08.29.
* Author			: 	Yu Weifeng
* Function List 		: 	RtcTimeInit
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "stm32f4xx_hal.h"
#include "cBasicTypeDefine.h"
#include "app_RTC.h"
#include "main.h"
static void RtcTimeDevConfig(void);
static u8 SetRtcTimeDev(datetime_t i_Datetime);
static u8 GetRtcTimeDev(datetime_t *o_Datetime);
static u8 RtcTimeAlarmDevConfig(datetime_t i_Datetime);
static void RtcTimeAlarmDevHandle();
static u8 RtcTimeWakeUpDevConfig(u32 i_dwSecondCount);
static void RtcTimeWakeUpDevHandle();
//static u8 g_ucBlueLedOnFlag=FALSE;
static RTC_HandleTypeDef RTC_Handler;  //RTC���
static T_RtcTimeDevManage g_tRtcTimeDev={
	.name="RtcTimeDev",
	.DevConfig		=RtcTimeDevConfig,
	.SetDateTime		=SetRtcTimeDev,
	.GetDateTime		=GetRtcTimeDev,
	.AlarmConfig		=RtcTimeAlarmDevConfig,
	.AlarmHandle		=RtcTimeAlarmDevHandle,
	.WakeUpConfig	=RtcTimeWakeUpDevConfig,
	.WakeUpHandle	=RtcTimeWakeUpDevHandle
};
/*****************************************************************************
-Fuction		: ProtocolFiFoPush
-Description	: ProtocolFiFoPush
-Input			: i_Com
-Output 		: o_ptProtocolFiFo
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RtcTimeDevInit()
{
	RegisterRtcTimeDev(&g_tRtcTimeDev);
}
/*****************************************************************************
-Fuction		: RtcTimeInit
-Description	: RtcTimeInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
//RTC�ײ�������ʱ������
//�˺����ᱻHAL_RTC_Init()����
//hrtc:RTC���
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	__HAL_RCC_PWR_CLK_ENABLE();//ʹ�ܵ�Դʱ��PWR
	HAL_PWR_EnableBkUpAccess();//ȡ����������д����
	
	RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_LSE;//LSE����
	RCC_OscInitStruct.PLL.PLLState=RCC_PLL_NONE;
	RCC_OscInitStruct.LSEState=RCC_LSE_ON;					//RTCʹ��LSE
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RTC;//����ΪRTC
	PeriphClkInitStruct.RTCClockSelection=RCC_RTCCLKSOURCE_LSE;//RTCʱ��ԴΪLSE
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
		
	__HAL_RCC_RTC_ENABLE();//RTCʱ��ʹ��
}
/*****************************************************************************
-Fuction		: RtcTimeInit
-Description	: RtcTimeInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/29	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void RtcTimeDevConfig(void)
{
    datetime_t tDataTime;
    RTC_Handler.Instance=RTC;
    RTC_Handler.Init.HourFormat=RTC_HOURFORMAT_24;//RTC����Ϊ24Сʱ��ʽ 
    RTC_Handler.Init.AsynchPrediv=0X7F;           //RTC�첽��Ƶϵ��(1~0X7F)
    RTC_Handler.Init.SynchPrediv=0XFF;            //RTCͬ����Ƶϵ��(0~7FFF)   
    RTC_Handler.Init.OutPut=RTC_OUTPUT_DISABLE;     
    RTC_Handler.Init.OutPutPolarity=RTC_OUTPUT_POLARITY_HIGH;
    RTC_Handler.Init.OutPutType=RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&RTC_Handler)!=HAL_OK) ;
      
    if(HAL_RTCEx_BKUPRead(&RTC_Handler,RTC_BKP_DR0)!=0X5050)//�Ƿ��һ������
    { 
    	tDataTime.date.year=16;
	tDataTime.date.month=10;
	tDataTime.date.day=9;
	tDataTime.time.hour=00;
	tDataTime.time.minute=00;
	tDataTime.time.second=00;
        SetRtcTimeDev(tDataTime);	        //����ʱ�� ,����ʵ��ʱ���޸�
        HAL_RTCEx_BKUPWrite(&RTC_Handler,RTC_BKP_DR0,0X5050);//����Ѿ���ʼ������
    }
}
/*****************************************************************************
-Fuction		: ProtocolFiFoPush
-Description	: ProtocolFiFoPush
-Input			: i_Com
-Output 		: o_ptProtocolFiFo
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SetRtcTimeDev(datetime_t i_Datetime)
{
	u8 ret=0;

	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	//if(i_Datetime.time.hour>12)
	{
	//	RTC_TimeStructure.TimeFormat=RTC_HOURFORMAT12_PM;
	//	RTC_TimeStructure.Hours=i_Datetime.time.hour-12;
	}
	//else
	{
	//	RTC_TimeStructure.TimeFormat=RTC_HOURFORMAT12_AM;
		RTC_TimeStructure.Hours=i_Datetime.time.hour;
	}

	RTC_TimeStructure.Minutes=i_Datetime.time.minute;
	RTC_TimeStructure.Seconds=i_Datetime.time.second;
	RTC_TimeStructure.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
  	RTC_TimeStructure.StoreOperation=RTC_STOREOPERATION_RESET;
	ret=HAL_RTC_SetTime(&RTC_Handler,&RTC_TimeStructure,RTC_FORMAT_BIN);	
	
    
	RTC_DateStructure.Date=i_Datetime.date.day;
	RTC_DateStructure.Month=i_Datetime.date.month;
	RTC_DateStructure.WeekDay=RTC_WEEKDAY_MONDAY;/////
	RTC_DateStructure.Year=i_Datetime.date.year;
	ret|=HAL_RTC_SetDate(&RTC_Handler,&RTC_DateStructure,RTC_FORMAT_BIN);

	if(ret!=HAL_OK)
	{
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: ProtocolFiFoPush
-Description	: ProtocolFiFoPush
-Input			: i_Com
-Output 		: o_ptProtocolFiFo
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetRtcTimeDev(datetime_t *o_Datetime)
{
	u8 ret=0;
	
	RTC_DateTypeDef RtcDate;
	RTC_TimeTypeDef RtcTime;
	if(HAL_RTC_GetState(&RTC_Handler) != HAL_RTC_STATE_READY)
	{
		ret=FALSE;
	}
	else
	{
		HAL_RTC_GetTime(&RTC_Handler,&RtcTime,RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&RTC_Handler,&RtcDate,RTC_FORMAT_BIN);
		o_Datetime->date.day=RtcDate.Date;
		o_Datetime->date.month=RtcDate.Month;
		o_Datetime->date.year=RtcDate.Year;
		//if(RtcTime.TimeFormat==RTC_HOURFORMAT12_PM)
		{
		//	RtcTime.Hours+=12;
		}
		//else
		{
		}
		o_Datetime->time.hour=RtcTime.Hours;
		o_Datetime->time.minute=RtcTime.Minutes;
		o_Datetime->time.second=RtcTime.Seconds;
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RtcTimeAlarmDevConfig
-Description	: RtcTimeAlarmDevConfig
-Input			: i_Com
-Output 		: o_ptProtocolFiFo
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 RtcTimeAlarmDevConfig(datetime_t i_Datetime)
{
    u8 ret=0;
    RTC_AlarmTypeDef RTC_AlarmSturuct;
    
    RTC_AlarmSturuct.AlarmTime.Hours==i_Datetime.time.hour;  //Сʱ
    RTC_AlarmSturuct.AlarmTime.Minutes=i_Datetime.time.minute; //����
    RTC_AlarmSturuct.AlarmTime.Seconds=i_Datetime.time.second; //��
    RTC_AlarmSturuct.AlarmTime.SubSeconds=0;
    //if(i_Datetime.time.hour>12)
    {
	//RTC_AlarmSturuct.AlarmTime.TimeFormat=RTC_HOURFORMAT12_PM;
	//RTC_AlarmSturuct.AlarmTime.Hours=i_Datetime.time.hour-12;
    }
   // else
    {
	//RTC_AlarmSturuct.AlarmTime.TimeFormat=RTC_HOURFORMAT12_AM;
	RTC_AlarmSturuct.AlarmTime.Hours=i_Datetime.time.hour;
    }
    RTC_AlarmSturuct.AlarmMask=RTC_ALARMMASK_NONE;//��ȷƥ�����ڣ�ʱ����
    RTC_AlarmSturuct.AlarmSubSecondMask=RTC_ALARMSUBSECONDMASK_NONE;
    RTC_AlarmSturuct.AlarmDateWeekDaySel=RTC_ALARMDATEWEEKDAYSEL_DATE;//������
    RTC_AlarmSturuct.AlarmDateWeekDay=i_Datetime.date.day; //����
    RTC_AlarmSturuct.Alarm=RTC_ALARM_A;     //����A
    HAL_RTC_SetAlarm_IT(&RTC_Handler,&RTC_AlarmSturuct,RTC_FORMAT_BIN);
    
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn,0x01,0x02); //��ռ���ȼ�1,�����ȼ�2
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);		

    ret=TRUE;
    return ret;
}
/*****************************************************************************
-Fuction		: RtcTimeWakeUpDevConfig
-Description	: RtcTimeWakeUpDevConfig
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 RtcTimeWakeUpDevConfig(u32 i_dwSecondCount)
{
	u8 ret=FALSE;
	u32 dwSecondCount=0;
	if(i_dwSecondCount<=0)
	{
		ret=FALSE;
	}
	else
	{
		dwSecondCount=i_dwSecondCount-1;	//0��1��	
		 __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handler, RTC_FLAG_WUTF);//���RTC WAKE UP�ı�־
		
		HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler,dwSecondCount,RTC_WAKEUPCLOCK_CK_SPRE_16BITS);			 //������װ��ֵ��ʱ�� 
		
		 HAL_NVIC_SetPriority(RTC_WKUP_IRQn,0x01,0x01); //��ռ���ȼ�1,�����ȼ�2
		HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: RtcTimeAlarmDevConfig
-Description	: RtcTimeAlarmDevConfig
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void RtcTimeAlarmDevHandle()
{
	u8 ucErr;
	HAL_RTC_AlarmIRQHandler(&RTC_Handler); 
	//OSTaskResume(FIXED_TIME_REPORT_TASK_PRIORITY);		
	//OSTaskResume(LED_INDICATE_TASK_PRIORITY);
	OSFlagPost(pstOsFlagForAppUse,BLUE_LED_LIGHT_ENABLE_FLAG,OS_FLAG_SET,&ucErr);
}
/*****************************************************************************
-Fuction		: RtcTimeWakeUpDevHandle
-Description	: RtcTimeWakeUpDevHandle
-Input			: 
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void RtcTimeWakeUpDevHandle()
{
	u8 ucErr;
    	HAL_RTCEx_WakeUpTimerIRQHandler(&RTC_Handler); 
	//OSTaskResume(LED_INDICATE_TASK_PRIORITY);
	
	//OSTaskResume(FIXED_TIME_REPORT_TASK_PRIORITY);		
	//OSSemPost(ptOsSemUsedForFixedTimeReportTask);
	
	OSFlagPost(pstOsFlagForAppUse, FIXED_TIME_REPORT_ENABLE_FLAG, OS_FLAG_SET, &ucErr);			
}

